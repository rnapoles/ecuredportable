#include "dialogecuredserverbrowser.h"
#include "ui_dialogecuredserverbrowser.h"


#include <qobjecthelper.h>
#include <parser.h>

DialogEcuredServerBrowser::DialogEcuredServerBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEcuredServerBrowser)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(45454, QUdpSocket::ShareAddress);
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));

    model = new QStandardItemModel(0, 2, this);
    model->setHeaderData(0, Qt::Horizontal,"Servidor");
    model->setHeaderData(1, Qt::Horizontal,"Fecha de la fuente documental");
    ui->ListaDeServidores->setModel(model);

    QHeaderView * header = ui->ListaDeServidores->header();
    header->setDefaultSectionSize(100);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

}

DialogEcuredServerBrowser::~DialogEcuredServerBrowser()
{
    //qDebug()<<"ok";
    delete ui;

}

QString DialogEcuredServerBrowser::getServerInfo() const
{
    return this->ServerInfo;
}

QString DialogEcuredServerBrowser::getServerIP() const
{
    return this->ServerIP;
}

QString DialogEcuredServerBrowser::getJson() const
{
    return this->JsonCode;
}

void DialogEcuredServerBrowser::InsertItem(const QString &ip,const QByteArray& jsonString)
{

    QList<QStandardItem *> items = model->findItems(ip,Qt::MatchExactly);
    if(items.count()==0){
        //            QListWidgetItem *newItem = new QListWidgetItem;
        //            newItem->setText(ip);
        //            newItem->setData(Qt::UserRole,data);
        //            ui->ListaDeServidores->addItem(newItem);

        QJson::Parser parser;
        bool ok;
        QVariant variant = parser.parse(jsonString,&ok);
        if(ok){


            QJson::QObjectHelper::qvariant2qobject(variant.toMap(), &databaseInfo);

            QStandardItem *newItem = new QStandardItem(ip);
            newItem->setData(jsonString);
            model->insertRow(0,newItem);
            model->setData(model->index(0, 1),databaseInfo.Fecha());

        } else {
            QString info = QString("<b>Error al recibir la información del servidor: <span style=\"color:#f82b07;\">%1</span></b>").arg(ip);
            ui->ErrorLabel->setText(info);
        }



    }
}

void DialogEcuredServerBrowser::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(),&sender);
        this->InsertItem(sender.toString(),datagram);
        //qDebug()<<datagram;
    }
}

void DialogEcuredServerBrowser::accept()
{
    //    QListWidgetItem *item = ui->ListaDeServidores->currentItem();
        QModelIndex index = ui->ListaDeServidores->currentIndex();
        QStandardItem * item = model->itemFromIndex(index);


        //item( int row, int column = 0 )


        if(item!=NULL){

            int columna = index.column();

            if(columna>0){
                columna--;
            }

            QStandardItem *itemInfo = model->item(0,columna);
            if(itemInfo!=NULL){
                //qDebug()<<itemInfo->data();
                //qDebug()<<"Columna "<<index.column();
                //qDebug()<<"Fila "<<index.row();
                this->ServerInfo = itemInfo->data().toString();
                this->ServerIP = itemInfo->text();
            }

        }

    QDialog::accept();
}




void DialogEcuredServerBrowser::on_ListaDeServidores_clicked(const QModelIndex &index)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->ListaDeServidores->setCurrentIndex(index);
}
