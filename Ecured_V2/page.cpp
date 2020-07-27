#include "page.h"
#include <QMessageBox>
#include <QtGui/qevent.h>
#include <QFile>
#include <QWebFrame>
#include <QWebElement>
#include "appconfig.h"
#include "qurlhelper.h"
#include "sleephelper.h"
#include <QTextStream>
#include <QTextCodec>
#include <QDesktopServices>
#include <QTimer>
#include <QLineEdit>
#include <QApplication>
#include "appconfig.h"

EcuredTabPage::EcuredTabPage(const QString &texto,bool IsUrl,QWidget *parent): QWidget(parent)
{


    Layout = new QVBoxLayout(this);
    ProgressLayout = new QHBoxLayout;

    InspectWindow = new QDockWidget(this);
    InspectWindow->hide();

    HtmlViewer = new EcuredWebView(this,InspectWindow);

    Spacer = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    LabelStatusBar = new QLabel(this);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(HtmlViewer);
    splitter->addWidget(InspectWindow);

    ProgressBar = new QProgressBar(this);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ProgressBar->sizePolicy().hasHeightForWidth());

    LabelStatusBar->setText("");
    ProgressLayout->addWidget(LabelStatusBar);

    ProgressBar->setSizePolicy(sizePolicy);
    ProgressLayout->addItem(Spacer);
    ProgressLayout->addWidget(ProgressBar);


    connect(HtmlViewer,SIGNAL(loadProgress(int)),ProgressBar,SLOT(setValue(int)));

    setAttribute(Qt::WA_DeleteOnClose);
    Layout->addWidget(splitter);
    Layout->addItem(ProgressLayout);


    connect(HtmlViewer->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));


    connect(HtmlViewer, SIGNAL(loadStarted()),this, SLOT(on_HtmlViewer_loadStarted()));

    connect(HtmlViewer, SIGNAL(loadFinished(bool)),this, SLOT(on_HtmlViewer_loadFinished(bool)));

    connect(HtmlViewer, SIGNAL(linkClicked(QUrl)), this, SLOT(on_HtmlViewer_linkClicked(QUrl)));
    connect(HtmlViewer, SIGNAL(urlChanged(QUrl)), this, SLOT(on_HtmlViewer_urlChanged(QUrl)));
    connect(HtmlViewer, SIGNAL(titleChanged(QString)), this, SLOT(on_HtmlViewer_titleChanged(QString)));
    connect(HtmlViewer,SIGNAL(selectionChanged()),this,SLOT(on_HtmlViewer_selectionChanged()));
    HtmlViewer->page()->setLinkDelegationPolicy(QWebPage::DontDelegateLinks);
    WebPage = HtmlViewer->page();
    frame = WebPage->mainFrame();
    connect(HtmlViewer,SIGNAL(statusBarMessage(QString)),this,SLOT(statusBarMessage(QString)));
    connect(WebPage,SIGNAL(linkHovered(QString,QString,QString)),this,SLOT(on_WebPage_linkHovered(QString,QString,QString)));
    connect(InspectWindow,SIGNAL(topLevelChanged(bool)),this,SLOT(topLevelChanged(bool)));


    TextoBusqueda = texto;
    if(IsUrl){
        LoadUrl(QUrl(texto));
    } else {
        search(texto);
    }
}

void EcuredTabPage::topLevelChanged ( bool topLevel ){
    if(topLevel){
        InspectWindow->setParent(0);
        InspectWindow->show();
    }
}

void EcuredTabPage::reconfigure(){
    if(InspectWindow->isHidden()){
        splitter->addWidget(InspectWindow);
        InspectWindow->show();
    }
}

EcuredTabPage::~EcuredTabPage()
{
    delete HtmlViewer;
    delete Layout ;
    delete ProgressBar;
    delete LabelStatusBar;
}


void EcuredTabPage::SearchPage(const QString &str){
    search(str,false);
}



void EcuredTabPage::populateJavaScriptWindowObject()
{
    HtmlViewer->page()->mainFrame()->addToJavaScriptWindowObject("Page", this);
    //HtmlViewer->page()->mainFrame()->addToJavaScriptWindowObject("MainWindow", EcuredMainWindow::instance());
    HtmlViewer->page()->mainFrame()->addToJavaScriptWindowObject("AppConfig", AppConfig::instance());
    HtmlViewer->page()->mainFrame()->addToJavaScriptWindowObject("QApp",qApp);
}


void EcuredTabPage::search(const QString &text,bool useMatch){
    emit loadStarted();
    TextoBusqueda = text;

    QString UrlString = AppConfig::instance()->getServerURL();


    if(AppConfig::instance()->IsUsingRemoteServer()){
        if(useMatch){
            //http://localhost:8080/Lucene?buscar=cuba
            UrlString = UrlString + QString("Lucene?buscar=%1").arg(text);
            QUrl ServerUrl(UrlString);
            HtmlViewer->load(ServerUrl);
        } else {
            //http://localhost:8080/index/Ceaden
            UrlString = UrlString + QString("index/%1").arg(text);
            QUrl ServerUrl(UrlString);
            HtmlViewer->load(ServerUrl);
        }
    } else {
        if(useMatch){
            //http://localhost:8080/Lucene?buscar=cuba
            HtmlViewer->load(QUrl("http://ecured/index?buscar="+text+"&useMatch=T"));
        } else {
            //http://localhost:8080/index/Ceaden
            HtmlViewer->load(QUrl("http://ecured/index?buscar="+text));
        }
    }


}

void EcuredTabPage::search(const int &id){
    emit loadStarted();
    TextoBusqueda = "";
    QString idstr;
    idstr.setNum(id);
    //http://localhost:8080/index/?id=800
    HtmlViewer->load(QUrl("http://ecured/index?id="+idstr));
}

void EcuredTabPage::LoadUrl(const QUrl & url){
    HtmlViewer->load(url);
}

void EcuredTabPage::scaleUp()
{
    HtmlViewer->zoomIn();
}

void EcuredTabPage::scaleDown()
{
    HtmlViewer->zoomOut();
}

void EcuredTabPage::resetScale()
{
    HtmlViewer->resetZoom();
}

void EcuredTabPage::on_HtmlViewer_loadStarted(){
    emit loadStarted();
    ProgressBar->show();
}


void EcuredTabPage::on_HtmlViewer_loadFinished(bool ok)
{
    /*QFile file;
    file.setFileName("wiki2html.js");
    file.open(QIODevice::ReadOnly);
    file.close();
    file.setFileName("Parser.js");
    file.open(QIODevice::ReadOnly);
    file.close();*/
    emit loadFinished(ok);
    QTimer::singleShot(2000,ProgressBar,SLOT(hide()));
}

void EcuredTabPage::on_HtmlViewer_linkClicked ( const QUrl & lurl ){

    emit loadStarted();
}

void EcuredTabPage::on_HtmlViewer_urlChanged ( const QUrl & url ){
    emit currentViewerChanged();
}


void EcuredTabPage::on_HtmlViewer_titleChanged ( const QString & title ){
    emit TitleViewerChanged(title);
    parentWidget()->setWindowTitle(title);

}


void EcuredTabPage::on_HtmlViewer_selectionChanged(){
    emit selectionChanged();
}

void EcuredTabPage::openExternalUrl(){
    QDesktopServices::openUrl(this->url);
}

void EcuredTabPage::openExternalUrl(const QString & url){

//   bool f = QDesktopServices::openUrl(QUrl(url));
//   QMessageBox msg;
//   if(f){
//           msg.setText("ok");
//   } else {
//       msg.setText("err");
//   }
//       msg.exec();
    QtConcurrent::run(QDesktopServices::openUrl,QUrl(url));

}

QString EcuredTabPage::getSearchText()const {
    return TextoBusqueda;
}

QString EcuredTabPage::getUrl() const
{
    return HtmlViewer->url().toString();
}


void EcuredTabPage::on_WebPage_linkHovered( const QString & link, const QString & title, const QString & textContent ){
    LabelStatusBar->setText(link);
}


void EcuredTabPage::statusBarMessage ( const QString & text ){
    LabelStatusBar->setText(text);
}


