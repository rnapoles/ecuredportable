#include "dialogacercade.h"
#include "ui_dialogacercade.h"
#include <QFile>
#include <QStandardItemModel>
#include <databasemanager.h>
#include <mainwindow.h>


void addLib(QAbstractItemView *view, const QString &biblioteca,const QString &version, const QString &Lenguaje, const QString &Licencia, const QString &Autor, const QString &Pag)
{
    QAbstractItemModel *model = view->model();
    model->insertRow(0);

    model->setData(model->index(0, 0), biblioteca);
    model->setData(model->index(0, 1), version);
    model->setData(model->index(0, 2), Lenguaje);
    model->setData(model->index(0, 3), Licencia);
    model->setData(model->index(0, 4), Autor);

    QLabel *lbl = new QLabel;
    lbl->setTextFormat(Qt::RichText);
    lbl->setText(Pag);
    view->setIndexWidget(model->index(0, 5) ,lbl);


}

DialogAcercaDe::DialogAcercaDe(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAcercaDe)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);




    QString TextoAcercaDe =  "<b>Ecured Portable versión "+ QCoreApplication::applicationVersion()+"</b><br/><br/>"

            "La creación de este producto responde a la necesidad de ofrecer el acceso a tan valiosa<br/>"
            "información a numerosos centros educacionales y organismos que aún no disponen de conexión<br/>"
            "a Internet, donde sí está disponible, accediendo mediante el sitio <a>www.ecured.cu</a>.<br/>"
            "Ecured Portable es una aplicación informática, que puede ser utilizada en cualquier ordenador<br/>"
            "y guardarse o ejecutarse directamente desde una memoria USB o DVD.<br/>"
            ;


        DatabaseInfo *dbinfo = AppConfig::instance()->getDatabaseInfo();

    //if(MainWindow::instance()->IsUsingRemoteServer()){
        TextoAcercaDe.append("<br/><b>Fecha de la fuente documental: "+ dbinfo->Fecha()+"</b>");
        QString str;
        str = QString("<br/><b>Cantidad de artículos: %1</b>").arg(dbinfo->NumArts());
        TextoAcercaDe.append(str);
    //}
    //DatabaseManager dbm(MainWindow::instance()->GetCurrentWikiDb());

    /*if(dbm.OpenDB()){
        if(dbm.IsSqliteDb()){
            if(dbm.GetDbInfo(dbinfo)){
                TextoAcercaDe.append("<br/><b>Fecha de la fuente documental: "+ dbinfo.Fecha()+"</b>");
                QString str;
                str = QString("<br/><b>Cantidad de artículos: %1</b>").arg(dbinfo.NumArts());
                TextoAcercaDe.append(str);
            }
        } else {
            TextoAcercaDe.append("<br/><b>La fuente documental no es valida o está dañada.</b>");
        }
        dbm.CloseDb();
    } else {
        TextoAcercaDe.append("<br/><b>La fuente documental no se pudo abrir</b>");
    }*/

    ui->LabelAcercaDe->setText(TextoAcercaDe);

    QStandardItemModel *model = new QStandardItemModel(0, 6, this);
    model->setHeaderData(0, Qt::Horizontal,"Biblioteca");
    model->setHeaderData(1, Qt::Horizontal,"Versión");
    model->setHeaderData(2, Qt::Horizontal,"Lenguaje");
    model->setHeaderData(3, Qt::Horizontal,"Licencia");
    model->setHeaderData(4, Qt::Horizontal,"Autor");
    model->setHeaderData(5, Qt::Horizontal,"Pág Web.");




    ui->treeView->setModel(model);

    addLib(ui->treeView,"jquery","1.5.2","Javascript","Dual MIT y GPLv2","John Resig","<a href='http://jquery.com/'>http://jquery.com/</a>");
    addLib(ui->treeView,"Live Preview","-","Javascript","-","Pilaf","<a href='http://es.wikipedia.org/wiki/Usuario:Pilaf/Live_Preview'>http://es.wikipedia.org/wiki/Usuario:Pilaf/Live_Preview</a>");
    addLib(ui->treeView,"QT","4.7.0","C++","GNU GPL v. 3.0","Corporación Nokia","<a href='http://qt.nokia.com'>http://qt.nokia.com</a>");
    addLib(ui->treeView,"Sqlite","3.7.6.2","C","Dominio público","D. Richard Hipp","<a href='http://www.sqlite.org/'>http://www.sqlite.org/</a>");
    addLib(ui->treeView,"Clucene","2.3.3.4","C++","LGPL","Proyecto CLucene","<a href='http://clucene.sourceforge.net/'>http://clucene.sourceforge.net/</a>");
    addLib(ui->treeView,"Zlib","1.2.5","C","Zlib License","Jean-loup Gailly y Mark Adler ","<a href='http://zlib.net/'>http://zlib.net/</a>");

    QHeaderView * header = ui->treeView->header();
    header->setDefaultSectionSize(70);
    QFile fichLicencia(":/info/gplv3.txt");
    fichLicencia.open(QIODevice::ReadOnly | QIODevice::Text);
    QString Licencia = fichLicencia.readAll();
    ui->textEdit->setText(Licencia);
    fichLicencia.close();
    ui->webView->setUrl(QUrl("qrc:/info/credits.html"));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

}

DialogAcercaDe::~DialogAcercaDe()
{
    delete ui;
}

void DialogAcercaDe::on_pushButton_clicked()
{
    close();
}



