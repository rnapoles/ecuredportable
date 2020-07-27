#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDesktopWidget>



WikiDbAnalizerMainWindow::WikiDbAnalizerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximized);

    setFixedSize(width(), height());

    ui->lineEditSource->installEventFilter(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Reparar");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    this->databaseInfo = new DatabaseInfo();
    this->dbm = NULL;

    this->adjustSize();
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
}


WikiDbAnalizerMainWindow::~WikiDbAnalizerMainWindow()
{
    delete ui;
}

void WikiDbAnalizerMainWindow::on_pushButtonSource_clicked()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)+ '/' ;
#ifdef linux
    QString fileName = QFileDialog::getOpenFileName(this,"Abrir",dir,"*");
#else
    QString fileName = QFileDialog::getOpenFileName(this,"Abrir",dir,"*.*");
#endif

    if(fileName.isEmpty()) return;

    QFileInfo finfo(fileName);
    if(QFile::exists(finfo.absoluteFilePath())){
        ui->lineEditSource->setText(fileName);

        //DatabaseManager dbm(finfo.absoluteFilePath());
        if(this->dbm!=NULL) delete this->dbm;
        this->dbm = new DatabaseManager(finfo.absoluteFilePath(),this);

        connect(this->dbm,SIGNAL(Error(int,QString,QString)),this,SLOT(onError(int,QString,QString)));

        if(this->dbm->OpenDB()){
            if(this->dbm->IsSqliteDb()){

                if(this->dbm->GetDbInfo(*this->databaseInfo)){


                    ui->lbfecha->setText(QString("Fecha: %1").arg(this->databaseInfo->Fecha()));

                    if(this->databaseInfo->Compressed()){
                        ui->lbcomp->setText("Comprimido: Sí");
                    } else {
                        ui->lbcomp->setText("Comprimido: No");
                    }

                    if(!this->databaseInfo->WikiCode()){
                        ui->lbhtml->setText("En formato HTML: Sí");
                    } else {
                        ui->lbhtml->setText("En formato HTML: No");
                    }


                    ui->lbcant->setText(QString("Cantidad de artículos: %1").arg(this->databaseInfo->NumArts()));

                } else {
                    return;
                }

            } else {
                return;
            }
        } else {
            return;
        }

    } else {
        //QMessageBox::critical(this,"Error","La fuente documental selecionada no existe.");
        return;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);


}

void WikiDbAnalizerMainWindow::onError(const int ErrType, const QString Msg, const QString OrgMsg)
{
    Q_UNUSED(ErrType);
    QMessageBox msgbox;
    msgbox.setText(Msg);
    msgbox.setDetailedText(OrgMsg);
    msgbox.setIcon(QMessageBox::Critical);
    msgbox.exec();

}

bool WikiDbAnalizerMainWindow::eventFilter(QObject *obj, QEvent *event)
{

    /*
    if (obj == ui->lineEditSource) {
        if (event->type() == QEvent::MouseButtonPress) {
            on_pushButtonSource_clicked();
        }
    }*/

    return QObject::eventFilter(obj, event);
}

void WikiDbAnalizerMainWindow::on_buttonBox_accepted()
{
    //if(this->databaseInfo->NumFields()<4 || this->databaseInfo->NumFields()==0){
        if(!this->dbm->SetDbInfo(false,true)){
            //QMessageBox::critical(this,"Error","No se pudo poner información de la fuente documental.");
            return;
        }
    //}

    this->dbm->isCompressed();

    if(this->dbm->GetDbInfo(*this->databaseInfo)){


        ui->lbfecha->setText(QString("Fecha: %1").arg(this->databaseInfo->Fecha()));

        if(this->databaseInfo->Compressed()){
            ui->lbcomp->setText("Comprimido: Sí");
        } else {
            ui->lbcomp->setText("Comprimido: No");
        }

        if(!this->databaseInfo->WikiCode()){
            ui->lbhtml->setText("En formato HTML: Sí");
        } else {
            ui->lbhtml->setText("En formato HTML: No");
        }


        ui->lbcant->setText(QString("Cantidad de artículos: %1").arg(this->databaseInfo->NumArts()));

    } else {
        return;
    }

    QMessageBox::information(this,"Información","Fuente documental reparada.");

}
