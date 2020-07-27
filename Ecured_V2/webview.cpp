#include <QWebHitTestResult>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QWebInspector>
#include <QMetaObject>
#include "webview.h"
#include "mainwindow.h"
#include "qurlhelper.h"


EcuredWebView::EcuredWebView(QWidget *parent,QWidget *InspectorParent) :
    QWebView(parent)
  , m_currentZoom(100)
{
      TextLink= "";

    //MainWindow *mainWindow = MainWindow::instance();
    //this->OldAccessManager = page()->networkAccessManager();

    //if(!mainWindow->IsUsingRemoteServer()){
        AccessManager =  new EcuredNetworkAccessManager(this);
        page()->setNetworkAccessManager(AccessManager);
    //}

    //settings
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebInspector *inspector = new QWebInspector(InspectorParent);
    QDockWidget *dockWidget = qobject_cast<QDockWidget*>(InspectorParent);
    dockWidget->setWidget(inspector);
    inspector->setPage(page());
    settings()->setDefaultTextEncoding("utf-8");

    //actions

    pageAction(QWebPage::InspectElement)->setText("Inspeccionar  elemento");
    pageAction(QWebPage::DownloadImageToDisk)->setVisible(false);
    pageAction(QWebPage::OpenLink)->setText("Abrir enlace");
    pageAction(QWebPage::OpenLinkInNewWindow)->setText("Abrir enlace en una pestaña");
    connect(pageAction(QWebPage::OpenLinkInNewWindow),SIGNAL(triggered()),this,SLOT(openLinkInNewTab()));
    pageAction(QWebPage::Reload)->setText("Recargar");
    pageAction(QWebPage::Copy)->setText("Copiar");
    pageAction(QWebPage::CopyLinkToClipboard)->setText("Copiar enlace");
    pageAction(QWebPage::CopyImageToClipboard)->setText("Copiar imagen");
    pageAction(QWebPage::DownloadLinkToDisk)->setVisible(false);
    pageAction(QWebPage::Back)->setText("Ir a la página anterior");
    pageAction(QWebPage::Forward)->setText("Ir a la página siguiente");

    connect(pageAction(QWebPage::InspectElement),SIGNAL(triggered()),this,SLOT(InspectElement()));

    // the zoom values (in percent) are chosen to be like in Mozilla Firefox 3
    m_zoomLevels << 30 << 50 << 67 << 80 << 90;
    m_zoomLevels << 100;
    m_zoomLevels << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;

    pageAction(QWebPage::OpenImageInNewWindow)->setText("Adicionar marcador ");
    connect(pageAction(QWebPage::OpenImageInNewWindow),SIGNAL(triggered()),this,SLOT(addbookmark()));
    pageAction(QWebPage::CopyImageToClipboard)->setVisible(false);
    pageAction(QWebPage::DownloadLinkToDisk)->setVisible(false);

}

void EcuredWebView::addbookmark()
{
    EcuredMainWindow *mainWindow = EcuredMainWindow::instance();
    mainWindow->AdicionarMarcador();
}

void EcuredWebView::contextMenuEvent(QContextMenuEvent *event)
{

    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
    QMenu menu(this);

    if(AppConfig::instance()->IsSourceDocAvailable())
    if (!r.linkUrl().isEmpty()) {
        QUrlHelper urlhelper(r.linkUrl().path());

        TextLink = urlhelper.obtenerParametro();

        menu.addAction("Abrir en una nueva pestaña", this, SLOT(openLinkInNewTab()));
        menu.addSeparator();
        menu.addAction(pageAction(QWebPage::OpenLink));
        menu.addAction("Copiar Enlace", this, SLOT(CopyLinkToClipboard()));


        menu.exec(mapToGlobal(event->pos()));
        return;
    }

    QWebView::contextMenuEvent(event);

}

void EcuredWebView::mousePressEvent(QMouseEvent *event)
{
    if (event->isAccepted() && (event->button() & Qt::MidButton)) {

        QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
        if (!r.linkUrl().isEmpty()) {
            QUrlHelper urlhelper(r.linkUrl().path());
            TextLink = urlhelper.obtenerParametro();
            openLinkInNewTab();
        }
        return;
    }



    QWebView::mouseReleaseEvent(event);
}


void EcuredWebView::openLinkInNewTab()
{
    if(!TextLink.isEmpty()){

        EcuredMainWindow *mainWindow = EcuredMainWindow::instance();
        mainWindow->CreateTab(TextLink);
        TextLink.clear();
    }
    TextLink.clear();
}

void EcuredWebView::CopyLinkToClipboard()
{

    pageAction(QWebPage::CopyLinkToClipboard)->trigger();

}

void EcuredWebView::CopyToClipboard()
{
    pageAction(QWebPage::Copy)->trigger();
}


void EcuredWebView::InspectElement(){
    QMetaObject::invokeMethod(parent()->parent(),"reconfigure",Qt::QueuedConnection);

}


void EcuredWebView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        m_currentZoom = m_currentZoom + numSteps * 10;
        applyZoom();
        event->accept();
        return;
    }
    QWebView::wheelEvent(event);
}

int EcuredWebView::levelForZoom(int zoom)
{
    int i;

    i = m_zoomLevels.indexOf(zoom);
    if (i >= 0)
        return i;

    for (i = 0 ; i < m_zoomLevels.count(); ++i)
        if (zoom <= m_zoomLevels[i])
            break;

    if (i == m_zoomLevels.count())
        return i - 1;
    if (i == 0)
        return i;

    if (zoom - m_zoomLevels[i-1] > m_zoomLevels[i] - zoom)
        return i;
    else
        return i-1;
}

void EcuredWebView::applyZoom()
{
    setZoomFactor(qreal(m_currentZoom) / 100.0);
}

void EcuredWebView::zoomIn()
{
    int i = levelForZoom(m_currentZoom);

    if (i < m_zoomLevels.count() - 1)
        m_currentZoom = m_zoomLevels[i + 1];
    applyZoom();
}

void EcuredWebView::zoomOut()
{
    int i = levelForZoom(m_currentZoom);

    if (i > 0)
        m_currentZoom = m_zoomLevels[i - 1];
    applyZoom();
}

void EcuredWebView::resetZoom()
{
    m_currentZoom = 100;
    applyZoom();
}

