#ifndef DIALOGECUREDSERVERBROWSER_H
#define DIALOGECUREDSERVERBROWSER_H

#include <QDialog>
#include <QUdpSocket>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStandardItemModel>
#include <databasemanager.h>

namespace Ui {
class DialogEcuredServerBrowser;
}

class DialogEcuredServerBrowser : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogEcuredServerBrowser(QWidget *parent = 0);
    ~DialogEcuredServerBrowser();
    QString getServerInfo()const;
    QString getServerIP()const;
    QString getJson()const;
private:
    Ui::DialogEcuredServerBrowser *ui;
    QUdpSocket *udpSocket;
    QString ServerInfo;
    QString ServerIP;
    QString JsonCode;
    DatabaseInfo databaseInfo;
    QStandardItemModel *model;

    void InsertItem(const QString &ip,const QByteArray& jsonString);

private slots:
    void processPendingDatagrams();
    void on_ListaDeServidores_clicked(const QModelIndex &index);

public slots:
    virtual void accept();
};

#endif // DIALOGECUREDSERVERBROWSER_H
