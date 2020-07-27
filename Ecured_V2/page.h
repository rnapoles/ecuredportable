#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSpacerItem>
#include <QWebPage>
#include <QDockWidget>
#include <QSplitter>
#include <QLabel>
#include "webview.h"
#include "searchengine.h"

//class WebView;

class EcuredTabPage : public QWidget
{
    Q_OBJECT
public:
    explicit EcuredTabPage(const QString &texto = "",bool IsUrl=false,QWidget *parent = 0);
    ~EcuredTabPage();
    void search(const QString &str,bool useMatch=false);
    void search(const int &id);


    void scaleUp();
    void scaleDown();
    void resetScale();
    QString getSearchText()const;
    QString getUrl()const;


    EcuredWebView * getWebView() const {
        return HtmlViewer;
    }

signals:
    void currentViewerChanged();
    void TitleViewerChanged(const QString & title );
    void loadStarted();
    void loadFinished(bool ok);
    void selectionChanged();


private slots:

    void populateJavaScriptWindowObject();
    void on_HtmlViewer_loadStarted ();
    void on_HtmlViewer_loadFinished(bool);
    void on_HtmlViewer_linkClicked ( const QUrl & url );
    void on_HtmlViewer_urlChanged ( const QUrl & url );
    void on_HtmlViewer_titleChanged ( const QString & title );
    void on_HtmlViewer_selectionChanged();
    void openExternalUrl();
    void on_WebPage_linkHovered( const QString & link, const QString & title, const QString & textContent );
    void statusBarMessage ( const QString & text );
    void topLevelChanged ( bool topLevel );

public slots:
        void SearchPage(const QString &str);
        void openExternalUrl(const QString & url);
        void reconfigure();
        void LoadUrl(const QUrl & url);

private:
    QWebFrame *frame;
    QVBoxLayout *Layout ;
    QHBoxLayout *ProgressLayout;
    QSpacerItem *Spacer;
    EcuredWebView  *HtmlViewer;
    QWebPage *WebPage;
    QProgressBar *ProgressBar;
    QString TextoBusqueda;
    QUrl url;
    QLabel *LabelStatusBar;
    QDockWidget *InspectWindow;
    QSplitter *splitter;

protected:



};

#endif // PAGE_H
