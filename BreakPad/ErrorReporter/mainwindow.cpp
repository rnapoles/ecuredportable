#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PushButtonRetartApp_clicked()
{
    QString path = QApplication::applicationDirPath()+"\\";
#if defined(Q_OS_WIN32)
    path.append("TestBreakPad.exe");
#else
    path.append("TestBreakPad");
#endif
    path =  QDir::toNativeSeparators(path);
    if(QFile::exists(path)){
        //QProcess::execute(path);
        QProcess::startDetached(path);
    } else {
        QString msg;
        msg.append(path);
        msg.append(" no se pudo ejecutar.");
        QMessageBox::information(this,"Información",msg);
    }
  close();
}

void MainWindow::on_PushButtonExit_clicked()
{
    close();
}
