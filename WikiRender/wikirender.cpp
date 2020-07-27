#include "wikirender.h"
#include "ui_wikirender.h"
#include <QFile>
#include <QWebFrame>
#include <QWebElement>
#include <QWebInspector>
#include <QDebug>

WikiRender::WikiRender(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WikiRender)
{
    ui->setupUi(this);

    QFile file;
    file.setFileName(":/recursos/default.html");
    file.open(QIODevice::ReadOnly);
    QString code  = file.readAll();
    //qDebug()<<code;
    ui->webView->setHtml(code);
    file.close();

    file.setFileName(":/recursos/monobook.js");
    file.open(QIODevice::ReadOnly);
    code = file.readAll();
    //qDebug()<<code;
    file.close();

    ui->webView->page()->mainFrame()->evaluateJavaScript(code);
    //end wikicode2html

    file.setFileName(":/recursos/jquery-latest.js");
    file.open(QIODevice::ReadOnly);
    code = file.readAll();
    //qDebug()<<code;
    file.close();

    ui->webView->page()->mainFrame()->evaluateJavaScript(code);

    QWebInspector *inspector = new QWebInspector;
    inspector->setPage(ui->webView->page());
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("textEdit", ui->textEdit);
    ui->textEdit->setFocus();
}

WikiRender::~WikiRender()
{
    delete ui;
}

void WikiRender::on_actionConvertir_triggered()
{
    QWebElement Contenedor = ui->webView->page()->mainFrame()->findFirstElement("#IdContenedor");
    Contenedor.setInnerXml("");

    ui->webView->page()->mainFrame()->evaluateJavaScript("$('#IdContenedor').html(wiki2html(textEdit.plainText))");
    ui->tabWidget->setCurrentIndex(1);
}

void WikiRender::populateJavaScriptWindowObject()
{
    qDebug()<<"ok";
    //ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("textEdit", ui->textEdit);
}

void WikiRender::on_tabWidget_currentChanged(int index)
{
    if (index==1){
        QWebElement Contenedor = ui->webView->page()->mainFrame()->findFirstElement("#IdContenedor");
        Contenedor.setInnerXml("");
        ui->webView->page()->mainFrame()->evaluateJavaScript("$('#IdContenedor').html(wiki2html(textEdit.plainText))");
    }
}

void WikiRender::on_actionEjemplo_triggered()
{
    QFile f(":/recursos/sample.wiki");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->textEdit->setText(f.readAll());
    }


}
