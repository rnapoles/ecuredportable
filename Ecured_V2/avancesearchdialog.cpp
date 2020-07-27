#include "avancesearchdialog.h"
#include "ui_avancesearchdialog.h" 
#include "qballoontip.h"
#include "searchengine.h"
#include <QKeyEvent>
#include "appconfig.h"
#include "mainwindow.h"

AvanceSearchDialog::AvanceSearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AvanceSearchDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    ui->LOperator1->addItem("-");
    ui->LOperator1->addItem("y");
    ui->LOperator1->addItem("o");

    ui->LOperator2->addItem("-");
    ui->LOperator2->addItem("y");
    ui->LOperator2->addItem("o");

    QList<QAbstractButton *>botones = ui->buttonBox->buttons ();
    botones.at(0)->setText("&Aceptar");
    botones.at(1)->setText("&Cerrar");


    //lucene = CLuceneHelper::getInstance();
    searchEngine=SearchEngine::instance();
    QStringList FieldNames = AppConfig::instance()->getDatabaseInfo()->FieldNames();


    int CFieldNames = FieldNames.size();
    ui->Fields->addItem("todos los campos");
    for (int i = 0; i < CFieldNames; ++i){
        ui->Fields->addItem(FieldNames.at(i));
    }

    installEventFilter(this);
    ui->ExactQuery->installEventFilter(this);

    ui->QueryTerm1->installEventFilter(this);
    ui->QueryTerm2->installEventFilter(this);
    ui->QueryTerm3->installEventFilter(this);
    ui->RejectTermQuery->installEventFilter(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

bool AvanceSearchDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this)
    {
        if(event->type() == QEvent::Show)
        {
            on_MoreButton_clicked();
        }
    }

    if( QString::compare(obj->metaObject()->className(),"QLineEdit") == 0 ){     
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(!IsValidQueryString(keyEvent->text())){
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);

}

AvanceSearchDialog::~AvanceSearchDialog()
{
    delete ui;
}

void AvanceSearchDialog::on_label_5_linkActivated(const QString &link)
{
    QPoint pos = QCursor::pos();
    QBalloonTip::showBalloon(QSystemTrayIcon::Information,"Información","<b>En la búsqueda estándar</b><br> \"entrecomilla la frase\".",pos,5000,true);
}

void AvanceSearchDialog::on_label_6_linkActivated(const QString &link)
{    
    QPoint pos = QCursor::pos();
    QBalloonTip::showBalloon(QSystemTrayIcon::Information,"Información","<b>En la búsqueda estándar</b><br> escribe OR o AND entre los términos sinónimos.",pos,5000,true);
}

void AvanceSearchDialog::on_label_9_linkActivated(const QString &link)
{
    QPoint pos = QCursor::pos();    
    QBalloonTip::showBalloon(QSystemTrayIcon::Information,"Información","<b>En la búsqueda estánda</b>r<br>añade el - (signo menos) delante del término de búsqueda del que no deseas obtener resultados.",pos,5000,true);
}

void AvanceSearchDialog::on_buttonBox_accepted()
{
    QString query = "";
    if(ui->MainQuery->isEnabled()){
        query = ui->MainQuery->text();
    }
        else {
        QString QueryTerm = ui->ExactQuery->text();
        if(QueryTerm.length()!=0){
            query = "\"" + ui->ExactQuery->text() + "\"";            
        }


        QueryTerm =  ui->QueryTerm1->text().simplified();
        if(QueryTerm.length()!=0){
            query = query +   " " + ui->QueryTerm1->text();
        }

        QueryTerm =  ui->QueryTerm2->text().simplified();
        if(QueryTerm.length()!=0){
            if(ui->LOperator1->currentText().contains("y")){
                query =  QString("%1 AND %2").arg(query).arg(ui->QueryTerm2->text());
            } else {
                query =  QString("%1 OR %2").arg(query).arg(ui->QueryTerm2->text());
            }
        }


        QueryTerm =  ui->QueryTerm3->text().simplified();
        if(QueryTerm.length()!=0){
            if(ui->LOperator2->currentText().contains("y")){
                query =  QString("%1 AND %2").arg(query).arg(ui->QueryTerm3->text());
            } else {
                query =  QString("%1 OR %2").arg(query).arg(ui->QueryTerm3->text());
            }
        }

        QueryTerm = ui->RejectTermQuery->text().simplified();

        if(QueryTerm.length()!=0){
            query = query + "  -\"" + ui->RejectTermQuery->text()+"\"";
        }


    }


    if(ui->Fields->currentIndex() == 0){
        //lucene->UseMultiFieldSearch();
        searchEngine->UseMultiFieldSearch();
    } else {
        //lucene->SetSearchField(ui->Fields->currentText());
        searchEngine->SetSearchField(ui->Fields->currentText());
    }


    if(!query.simplified().isEmpty()){
        //query = "\""+query+"\"";
        SearchEngine::instance()->SearchByTitle(query,true);
        EcuredMainWindow::instance()->SetQuery(query);
    }

}



void AvanceSearchDialog::on_LOperator1_currentIndexChanged(int index)
{
    if(index != 0){
        ui->QueryTerm2->setEnabled(true);
        ui->LOperator2->setEnabled(true);
    } else {

        ui->QueryTerm2->setEnabled(false);
        ui->QueryTerm2->setText("");
        ui->LOperator2->setEnabled(false);
        ui->LOperator2->setCurrentIndex(0);
    }
}

void AvanceSearchDialog::on_LOperator2_currentIndexChanged(int index)
{
    if(index != 0){
        ui->QueryTerm3->setEnabled(true);
    } else {
        ui->QueryTerm3->setEnabled(false);
        ui->QueryTerm3->setText("");
    }
}

void AvanceSearchDialog::on_QueryTerm1_textChanged(const QString &arg1)
{
    if(ui->QueryTerm1->text().simplified().length()==0){
        ui->LOperator1->setCurrentIndex(0);
        ui->LOperator1->setEnabled(false);
    }else{
        ui->LOperator1->setEnabled(true);
    }
}

void AvanceSearchDialog::on_QueryTerm2_textChanged(const QString &arg1)
{
    if(ui->QueryTerm2->text().simplified().length()==0){
        ui->LOperator2->setCurrentIndex(0);
        ui->LOperator2->setEnabled(false);
    } else {
        ui->LOperator2->setEnabled(true);
    }
}

/*void AvanceSearchDialog::on_groupBox_toggled(bool show)
{
    //ui->gridLayout->set

    QLayoutItem *item = 0;
    QWidget *widget = 0;
    QGridLayout *layout= ui->gridLayout;

    for(int i = 0; i < layout->rowCount(); ++i)
    {
        for(int j = 0; j < layout->columnCount(); ++j)
        {
            item = layout->itemAtPosition(i,j);
            widget=item?item->widget():0;
            if(widget)
                widget->setVisible(show);
        }
    }

    if(show){
       // ui->groupBox->resize(625,128);
        //ui->groupBox->setMaximumHeight(128);
    } else {
        //ui->groupBox->resize(625,10);
        //ui->groupBox->setMaximumHeight(20);
    }


}*/

void AvanceSearchDialog::on_MoreButton_clicked()
{

    if(ui->frame1->isVisible()){
        ui->frame1->setVisible(false);
        ui->MoreButton->setIcon(QIcon(":/images/folding/chevron-expand.png"));
        ui->MainQuery->setEnabled(true);
    } else {
        ui->frame1->setVisible(true);
        ui->MoreButton->setIcon(QIcon(":/images/folding/chevron.png"));
        ui->MainQuery->setEnabled(false);
    }
    adjustSize();
}

void AvanceSearchDialog::on_ExactQuery_textEdited(const QString &arg1)
{
    /*if(IsValidQueryString(ui->ExactQuery->text())){
        MarkInvalid(ui->ExactQuery);
    } else {
        MarkInvalid(ui->ExactQuery,false);
    }*/
}



bool AvanceSearchDialog::IsValidQueryString(const QString s){
    QString s1 = QRegExp::escape("+-&|!(){}[]^\"~*?:\\");
    QRegExp rx("["+s1+"]");

    if(rx.indexIn(s) != -1){
        return false;
    }

    return true;
}

void AvanceSearchDialog::MarkInvalid(QWidget *w,bool t){
    QPalette palette = w->palette();
    palette.setColor(QPalette::Active, QPalette::Base, t ? Qt::white
                                                             : QColor(255, 102, 102));
    w->setPalette(palette);
}


