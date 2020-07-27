#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebView>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QWebView>
#include <QSplitter>
#include <QCheckBox>
#include <QMovie>
#include <QSystemTrayIcon>
#include <ctime>
#include <QCursor>
#include <QToolTip>
#include <QTabBar>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QProgressDialog>
#include <bookmarks/bookmarksmanager.h>
#include <bookmarks/bookmarksmenu.h>
#include <QMouseEvent>
#include <QSettings>
#include <QCompleter>
#include "findwidget.h"
#include "page.h"
//#include "completerlineedit.h"
#include "searchengine.h"
#include "clucenehelper.h"
#include <databasemanager.h>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "appconfig.h"

namespace Ui {
class MainWindow;
}



class EcuredMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~EcuredMainWindow();
    static EcuredMainWindow *instance()
    {
        if (!s_instance)
            s_instance = new EcuredMainWindow;
        return s_instance;
    }

    bool findInWebPage(const QString &ttf, bool forward);
    void setCompletionList(const QStringList &list);
    void SetQuery(const QString &query);


    void BuildUrlAndReloadPage();


private:
    EcuredMainWindow(QWidget *parent = 0);
    void resetTimer();
    void createActions();
    void createTrayIcon();
    void ListaShow();
    void SetCurrentDocSource();
    void setMovieLoading();
    void setMovieStop();
    QString getNormalizeFileName() const;


    QLabel *animationLabel(int index, bool addMovie);



    static EcuredMainWindow *s_instance;


    Ui::MainWindow *ui;
    FindWidget *findWidget;
    QWebView *viewer;
    EcuredTabPage *CurrentTabPage;
    QLineEdit *EditBuscar;
    QCompleter *completer;
    QStringList CacheCompletionList;
    QPushButton *ButtonBuscar;
    QTimer *timer;

    //QListWidget *Lista;
    QListView *Lista;
    QStandardItemModel ItemsModel;
    QSortFilterProxyModel proxyModel;

    QSplitter *splitter;
    //bool CheckBoxAllMatch;
    QLineEdit *EditBuscarEnLista;
    QVBoxLayout *VBoxLayoutLista;
    QWidget *WidgetListaContainer;



    QHash<QString, int> hashTextCompletion;

    QString ListaItemText;
    int ListaItemId;



    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QString TextoBusqueda;
    bool EnableUseFullMatch ;
    bool showTextResult ;


    QTabBar *TabBar;
    QMovie *movieLoading;
    QFutureWatcher <void> Watcher;
    QProgressDialog *progressdialog;
    CLuceneHelper * lucene;

    BookmarksManager * bookmarksManager;
    BookmarksMenuBarMenu *m_bookmarksMenu;

    BookmarksMenu* menubookmark;

    bool SecondSearch;

    //recent files
    QSettings *settings;
    QString curFile;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    QActionGroup  *recentFileActionGroup;

    AppConfig *appConfig;
    DatabaseInfo *databaseInfo;
public slots:
    void CreateTab(const QString &Titulo = "",bool IsUrl=false);
    void showTextSearch();
    void BookmarkPageLoad(EcuredTabPage *bookmarkpage);

    void CreateCurrentTab(const QString &Titulo = "",bool IsUrl=false);

    void findNext();
    void findPrevious();
    void find(const QString &text, bool forward);

    void MenuMarcadoresClose();
    void AdicionarMarcador();

    void activateTab(bool onlyHelpViewer = false);
    void triggerTextCompletion();
    void contextMenuRequested (const QPoint & pos);

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();

    void handleMessage(const QString& message);

    void on_actionAbrir_fuente_documental_triggered();




private slots:
    void on_actionSalir_triggered();
    void on_actionCopiar_triggered();
    void on_actionOcultarLista_triggered();
    void on_actionAcerca_de_Ecured_Portable_triggered();
    void on_actionAdelante_triggered();
    void on_actionAtras_triggered();
    void on_tabWidget_tabCloseRequested(int index);

    void ButtonBuscar_clicked();

    void on_actionGuardar_triggered();
    void on_actionInicio_triggered();
    void on_actionNewTab_triggered();
    void on_actionNormalZoom_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomIn_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_actionPDF_triggered();
    void updateNavigationItems();
    void updateTabTitle(const QString & title);

    void slotTextCompletionReady(const QList<QSqlRecord>& records,double tiempo);
    void slotTextCompletionReady(const QList<QPair<QString, int> >& results,bool lucene,double tiempo);
    void slotSearchByTitleReady(const QList<QSqlRecord>& records ,bool useMatch,double tiempo);
    void slotSearchByTitleReady(const QList<QPair<QString, int> >& results,double tiempo);

    void Lista_doubleClicked(QModelIndex item );
    void Lista_OpenItem();

    void page_loadStarted ();
    void page_loadFinished(bool ok);

    void PageSelectionChanged();

    void activated(QSystemTrayIcon::ActivationReason reason);

    void SearchFinished(int size,bool isTextCompletion);

    void  EditBuscarEnLista_textEdited(QString SearchText);
    void on_actionImportar_triggered();
    void on_actionCrear_ndice_de_Busqueda_triggered();

    void indexContentStarted(int max);
    void indexContentFinished();
    void OnError(const int ErrType,const QString Msg,const QString OrgMsg);


    void on_actionAyuda_de_Ecured_triggered();

    void on_actionPantalla_Completa_triggered(bool checked);

    void codec_triggered();

    void on_actionA_adir_marcador_triggered();

    void on_actionImprimir_triggered();

    void on_actionVista_previa_de_impresi_n_triggered();

    void on_actionMostrar_todos_loas_macadores_triggered();

    void on_actionSolo_ampliar_texto_toggled(bool arg1);

    void on_actionExportar_a_ODF_triggered();

    void on_actionExportar_a_txt_triggered();

    void WizardTerminate(bool ok,const QString &Dir,const QString &db);

    void openRecentFile();

    void on_actionGestionar_Marcadores_triggered();

    void on_actionA_adir_carpeta_2_triggered();

    void ToSystray();
    void on_actionAbrir_funete_documental_remota_triggered();

    void AppConfigChange();
protected:
    void keyPressEvent(QKeyEvent *);
    bool eventFilter(QObject *obj, QEvent *ev);
    void showEvent(QShowEvent *event) ;
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);


};

#endif // MAINWINDOW_H





