#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wiki/databasemanager.h"

namespace Ui {
class MainWindow;
}

class WikiDbAnalizerMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WikiDbAnalizerMainWindow(QWidget *parent = 0);
    ~WikiDbAnalizerMainWindow();
    
private slots:
    void on_pushButtonSource_clicked();
    void onError(const int ErrType,const QString Msg,const QString OrgMsg);
    void on_buttonBox_accepted();

private:
    Ui::MainWindow *ui;
    DatabaseInfo *databaseInfo;
    DatabaseManager *dbm;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
