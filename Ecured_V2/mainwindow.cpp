#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "page.h"
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QWindowStateChangeEvent>
#include <QTextStream>
#include <QWebFrame>
#include <QWebElement>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QTextBlockFormat>
#include <QTextCursor>
#include "dialogacercade.h"
#include "sleephelper.h"
#include <QDesktopServices>
#include <QMovie>
#include "import.h"
#include <QTextCodec>
#include <bookmarks/addbookmarkdialog.h>
#include <bookmarks/bookmarksdialog.h>
#include "indexgeneratorwizard.h"
#include "avancesearchdialog.h"
#include "ImportSource/importdocumentalsourcewizard.h"
#include <QDir>

#include <qobjecthelper.h>
#include <parser.h>

#include <QStringListModel>
#include "dialogecuredserverbrowser.h"

#include <qobjecthelper.h>
#include <parser.h>

#define AppInfo "http://ecured/EcuRed_Portable.html"
#define BlankPage "http://ecured/index"


EcuredMainWindow *EcuredMainWindow::s_instance = 0;

EcuredMainWindow::EcuredMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->appConfig = AppConfig::instance();
    this->databaseInfo=this->appConfig->getDatabaseInfo();
    connect(this->appConfig,SIGNAL(configchange()),this,SLOT(AppConfigChange()));
    this->AppConfigChange();



    timer = NULL;
    EnableUseFullMatch = false;
    showTextResult = false;
    SecondSearch = false;
    EditBuscar=NULL;

    lucene = CLuceneHelper::getInstance();

    connect(lucene,SIGNAL(indexContentStarted(int)),this,SLOT(indexContentStarted(int)));
    connect(lucene,SIGNAL(indexContentFinished()),this,SLOT(indexContentFinished()));
    connect(lucene,SIGNAL(Error(int,QString,QString)),this,SLOT(OnError(int,QString,QString)));




    settings = new QSettings(QSettings::IniFormat,QSettings::UserScope,"UCI","Ecured Portable");
    recentFileActionGroup = new QActionGroup(this);
    recentFileActionGroup->setExclusive(true);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        recentFileActs[i]->setCheckable(true);
        recentFileActionGroup->addAction(recentFileActs[i]);
        connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
    }

    for (int i = 0; i < MaxRecentFiles; ++i){
        ui->menuFuentes_documentales_recientes->addAction(recentFileActs[i]);
    }


    this->updateRecentFileActions();

    QStringList files = settings->value("recentFileList").toStringList();
    if(files.length()>0){
        this->setCurrentFile(files[0]);
        this->recentFileActs[0]->setChecked(true);
    } else if(QFile::exists("wikidb")) {
        this->setCurrentFile("wikidb");
    }

    //this->SetCurrentDocSource();


    connect(SearchEngine::instance(),SIGNAL(SearchStarted()),SLOT(page_loadStarted()));
    connect(SearchEngine::instance(),SIGNAL(SearchFinished(int,bool)),SLOT(SearchFinished(int,bool)));
    connect(SearchEngine::instance(),SIGNAL(OnTextCompletionReady(QList<QPair<QString,int> >,bool,double)),SLOT(slotTextCompletionReady(QList<QPair<QString,int> >,bool,double)));
    connect(SearchEngine::instance(),SIGNAL(OnSearchByTitleReady(QList<QSqlRecord>,bool,double)),SLOT(slotSearchByTitleReady(QList<QSqlRecord>,bool,double)));
    connect(SearchEngine::instance(),SIGNAL(OnSearchByTitleReady(QList<QPair<QString,int> >,double)),this,SLOT(slotSearchByTitleReady(QList<QPair<QString,int> >,double)));
    connect(SearchEngine::instance(),SIGNAL(Error(int,QString,QString)),this,SLOT(OnError(int,QString,QString)));



    movieLoading = new QMovie(this);
    movieLoading->setScaledSize(QSize(16,16));
    movieLoading->setFileName(":images/45.gif");
    movieLoading->start();



    ui->labelLoading->setMovie(movieLoading);
    ui->tabWidget->setTabsClosable(true);
    ui->actionAtras->setEnabled(false);
    ui->actionAdelante->setEnabled(false);

    ButtonBuscar = new QPushButton("Buscar",this);
    //CheckBoxAllMatch = true;

    EditBuscar= new QLineEdit(this);

    completer = new QCompleter();
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    EditBuscar->setCompleter(completer);
    EditBuscar->setEnabled(false);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), SLOT(triggerTextCompletion()));
    connect(EditBuscar, SIGNAL(textEdited(QString)), timer, SLOT(start()));

    connect(ButtonBuscar,SIGNAL(clicked()),this,SLOT(ButtonBuscar_clicked()));
    connect(EditBuscar,SIGNAL(returnPressed()),this,SLOT(ButtonBuscar_clicked()));

    ui->mainToolBar->addWidget(EditBuscar);
    ui->mainToolBar->addWidget(ButtonBuscar);


    ui->tabWidget->installEventFilter(this);
    ui->menu_Edici_n->installEventFilter(this);
    ui->menuB_squeda_avanzada->installEventFilter(this);


    createActions();
    createTrayIcon();
    trayIcon->show();

    installEventFilter(this);
    setMouseTracking(true);



    progressdialog = new QProgressDialog(this);
    progressdialog->setLabelText("<b>Creando índice de búsqueda</b>");
    progressdialog->setWindowModality(Qt::WindowModal);
    progressdialog->setCancelButton(0);
    progressdialog->hide();


#define Marcadores
#ifdef Marcadores
    bookmarksManager = new BookmarksManager(this);
    m_bookmarksMenu = new BookmarksMenuBarMenu(this);
    // menubookmark = new BookmarksMenu(this);
    // connect(menubookmark,SIGNAL(openUrl(Page*)),this,SLOT(BookmarkPageLoad(Page*)));
    ui->menuMarcadores->addMenu(m_bookmarksMenu);
    m_bookmarksMenu->setTitle("&Marcadores");
    m_bookmarksMenu->setBookmarksManager(bookmarksManager);
    //En la barra de menú
    //ui->menuBar->addMenu(m_bookmarksMenu);
#else
    delete ui->menuMarcadores;
#endif


    //se desactivo los menues de los toolbars
    QAction * accmainToolBar = ui->mainToolBar->toggleViewAction();
    accmainToolBar->setCheckable(false);
    accmainToolBar->setVisible(false);

    QAction * acctoolBarBusqueda = ui->toolBarBusqueda->toggleViewAction();
    acctoolBarBusqueda->setCheckable(false);
    acctoolBarBusqueda->setVisible(false);



    //restaura las pestañas abiertas
    /*
    if(this->IsSourceDocAvailable){
        QStringList TabList = settings->value("TabList").toStringList();
        int CantTabs= TabList.size();

        for (int i = 0; i < CantTabs; ++i){
            CreateTab(TabList.at(i),true);
        }
        if(CantTabs==0) CreateTab(BlankPage,true);
    } else {
        //setup tabbar
        CreateTab(BlankPage,true);
    }

    //setup tabbar
    CreateTab(BlankPage,true);

    QList<QTabBar *> Lista_widgets = ui->tabWidget->findChildren<QTabBar *>(); //obtenemos todos los QWebView en un QList
    int cant_widgets = Lista_widgets.count();
    if(cant_widgets > 0){
        TabBar = Lista_widgets.at(0);
        animationLabel(0,true);
    } else {
        TabBar = NULL;
    }
*/


    WidgetListaContainer = new QWidget(this);
    //WidgetListaContainer->resize(5,5);
    VBoxLayoutLista = new QVBoxLayout();


    splitter = new QSplitter(this);


    EditBuscarEnLista = new QLineEdit(WidgetListaContainer);
    EditBuscarEnLista->setFixedHeight(20);

    connect(EditBuscarEnLista,SIGNAL(textEdited(QString)),this,SLOT(EditBuscarEnLista_textEdited(QString)));

    //Lista = new QListWidget(WidgetListaContainer);
    Lista = new QListView(WidgetListaContainer);
    Lista->setContextMenuPolicy(Qt::CustomContextMenu);
    Lista->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ListaItemText.clear();
    ListaItemId = -1;
    WidgetListaContainer->hide();
    //connect(Lista,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(Lista_doubleClicked(QListWidgetItem*)));
    connect(Lista,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(Lista_doubleClicked(QModelIndex)));
    connect(Lista,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRequested(QPoint)));

    VBoxLayoutLista->addWidget(EditBuscarEnLista);
    VBoxLayoutLista->addWidget(Lista);
    WidgetListaContainer->setLayout(VBoxLayoutLista);

    splitter->addWidget(WidgetListaContainer);
    splitter->addWidget(ui->tabWidget);


    ui->verticalLayout->addWidget(splitter);
    EditBuscar->setFocus();

    proxyModel.setSourceModel(&ItemsModel);
    Lista->setModel(&proxyModel);

    /*if(this->IsSourceDocAvailable){
        QStringList TabList = settings->value("TabList").toStringList();
        int CantTabs= TabList.size();

        for (int i = 0; i < CantTabs; ++i){
            CreateTab(TabList.at(i),true);
        }
        if(CantTabs==0) CreateTab(BlankPage,true);
    } else {
        //setup tabbar
        CreateTab(BlankPage,true);
    }*/

    //setup tabbar
    CreateTab(AppInfo,true);

    QList<QTabBar *> Lista_widgets = ui->tabWidget->findChildren<QTabBar *>(); //obtenemos todos los QWebView en un QList
    int cant_widgets = Lista_widgets.count();
    if(cant_widgets > 0){
        TabBar = Lista_widgets.at(0);
        animationLabel(0,true);
    } else {
        TabBar = NULL;
    }

    findWidget = new FindWidget(this);

    ui->verticalLayout->addWidget(findWidget);
    findWidget->hide();

    connect(ui->actionBuscar,SIGNAL(triggered()),this,SLOT(showTextSearch()));
    connect(findWidget, SIGNAL(findNext()), this, SLOT(findNext()));
    connect(findWidget, SIGNAL(findPrevious()), this, SLOT(findPrevious()));
    connect(findWidget, SIGNAL(find(QString, bool)), this, SLOT(find(QString, bool)));
    connect(ui->actionBuscar_Siguiente, SIGNAL(triggered()), this, SLOT(findNext()));
    connect(ui->actionBuscar_Anterior, SIGNAL(triggered()), this, SLOT(findPrevious()));

    if(this->appConfig->IsSourceDocAvailable()){
        EditBuscar->setEnabled(true);
    }
}

EcuredMainWindow::~EcuredMainWindow()
{
    trayIcon->hide();
    delete ui;
}

void EcuredMainWindow::MenuMarcadoresClose()
{
    ui->menuMarcadores->close();
}

void EcuredMainWindow::AdicionarMarcador()
{
    QString url = QLatin1String(viewer->url().toEncoded());
    QString title = viewer->title();
    AddBookmarkDialog dialog(this,bookmarksManager);
    dialog.setUrl(url);
    dialog.setTitle(title);
    dialog.exec();
}

void EcuredMainWindow::CreateCurrentTab(const QString &Titulo ,bool IsUrl)
{
    CurrentTabPage->LoadUrl(QUrl(Titulo));
}

void EcuredMainWindow::CreateTab(const QString &Titulo,bool IsUrl){

    EcuredTabPage *p = new EcuredTabPage(Titulo,IsUrl);
    connect(p,SIGNAL(currentViewerChanged()),this,SLOT(updateNavigationItems()));
    connect(p,SIGNAL(TitleViewerChanged(QString)),this,SLOT(updateTabTitle(QString)));
    connect(p,SIGNAL(loadStarted()),this,SLOT(page_loadStarted()));
    connect(p,SIGNAL(loadFinished(bool)),this,SLOT(page_loadFinished(bool)));
    connect(p,SIGNAL(selectionChanged()),this,SLOT(PageSelectionChanged()));
    ui->tabWidget->addTab(p,Titulo);

}

void EcuredMainWindow::on_actionPDF_triggered()
{
    QString PageTitle = this->getNormalizeFileName();
    PageTitle = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)+PageTitle;
    QString fileName = QFileDialog::getSaveFileName(this,"Guardar",PageTitle,"Pdf (*.pdf)");

    if(fileName.isNull()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    viewer->print(&printer);
}

void EcuredMainWindow::showTextSearch()
{
    if(!viewer->selectedText().isEmpty()) findWidget->setText(viewer->selectedText());
    findWidget->show();
}

void EcuredMainWindow::keyPressEvent(QKeyEvent *e)
{
    const QString &text = e->text();
    if ((e->key()==Qt::Key_F) && (e->modifiers()==Qt::ControlModifier)){
        if (!findWidget->isVisible()) {
            findWidget->showAndClear();
            if(!viewer->selectedText().isEmpty()) findWidget->setText(viewer->selectedText());
        } else {
            if(!viewer->selectedText().isEmpty()) findWidget->setText(viewer->selectedText());
            findWidget->show();
        }
    }

    if(e->key()==Qt::Key_F3){
        findNext();
    }

    if(e->key()==Qt::Key_F4){
        findPrevious();
    }

    if(e->key()==Qt::Key_Escape){
        findWidget->hide();
    }

    if (text.startsWith(QLatin1Char('/'))) {
        if (!findWidget->isVisible()) {
            findWidget->showAndClear();
            if(!viewer->selectedText().isEmpty()) findWidget->setText(viewer->selectedText());
        } else {
            if(!viewer->selectedText().isEmpty()) findWidget->setText(viewer->selectedText());
            findWidget->show();
        }
    } else {
        QWidget::keyPressEvent(e);
    }
}

void EcuredMainWindow::findNext()
{

    find(findWidget->text(), true);
}

void EcuredMainWindow::findPrevious()
{
    find(findWidget->text(), false);
}

void EcuredMainWindow::find(const QString &ttf, bool forward)
{

    bool found = false;

    found = findInWebPage(ttf, forward);


    if (!found && ttf.isEmpty())
        found = true;   // the line edit is empty, no need to mark it red...

    if (!findWidget->isVisible())
        findWidget->show();
    findWidget->setPalette(found);
}

bool EcuredMainWindow::findInWebPage(const QString &ttf, bool forward)
{
    if (viewer) {
        bool found = false;
        QWebPage::FindFlags options;
        if (!ttf.isEmpty()) {
            if (!forward)
                options |= QWebPage::FindBackward;

            if (findWidget->caseSensitive())
                options |= QWebPage::FindCaseSensitively;

            found = viewer->findText(ttf, options);
            findWidget->setTextWrappedVisible(false);

            if (!found) {
                options |= QWebPage::FindWrapsAroundDocument;
                found = viewer->findText(ttf, options);
                if (found)
                    findWidget->setTextWrappedVisible(true);
            }
        }
        // force highlighting of all other matches, also when empty (clear)
        options = QWebPage::HighlightAllOccurrences;

        if (findWidget->caseSensitive())
            options |= QWebPage::FindCaseSensitively;
        viewer->findText(QLatin1String(""), options);
        viewer->findText(ttf, options);
        return found;
    }

    return false;
}




void EcuredMainWindow::activateTab(bool onlyHelpViewer)
{

}

void EcuredMainWindow::on_tabWidget_currentChanged(int index)
{
    CurrentTabPage = qobject_cast<EcuredTabPage*>(ui->tabWidget->currentWidget());
    QList<QWebView*> WebViewList = CurrentTabPage->findChildren<QWebView*>(); //obtenemos todos los QWebView en un QList
    viewer = WebViewList.at(0);
    updateNavigationItems();
}

void EcuredMainWindow::on_actionZoomIn_triggered()
{
    CurrentTabPage->scaleUp();
}

void EcuredMainWindow::on_actionZoomOut_triggered()
{
    CurrentTabPage->scaleDown();
}

void EcuredMainWindow::on_actionNormalZoom_triggered()
{

    CurrentTabPage->resetScale();
}


void EcuredMainWindow::on_actionNewTab_triggered()
{
    if(this->appConfig->IsUsingRemoteServer()){
        CreateTab(this->appConfig->getServerURL(),true);
    } else {
        CreateTab(AppInfo,true);
    }
}

void EcuredMainWindow::updateNavigationItems()
{

    if(viewer->pageAction(QWebPage::Forward)->isEnabled()){
        ui->actionAdelante->setEnabled(true);
    } else {
        ui->actionAdelante->setEnabled(false);
    }

    if(viewer->pageAction(QWebPage::Back)->isEnabled()){
        ui->actionAtras->setEnabled(true);
    } else {
        ui->actionAtras->setEnabled(false);
    }
}

void EcuredMainWindow::updateTabTitle(const QString & title){


    QWidget *page = qobject_cast<QWidget*>(sender());

    int index = ui->tabWidget->indexOf(page);

    if (-1 == index)
        return;

    ui->tabWidget->setTabText(index,title);
    updateNavigationItems();

}

void EcuredMainWindow::page_loadStarted(){

    if(SearchEngine::instance() == sender()){
        if(!SearchEngine::instance()->getSearchTitle().isEmpty()){
            ui->ResultadosPara->setText("Buscando resultados para <b>"+SearchEngine::instance()->getSearchTitle()+"</b>");
        }
    }


    QWidget *page = qobject_cast<QWidget*>(sender());
    int index = ui->tabWidget->indexOf(page);

    if(index != -1){
        animationLabel(index,true);
    }
    setMovieLoading();


}


void EcuredMainWindow::page_loadFinished(bool ok){

    EcuredTabPage *TabPage = qobject_cast<EcuredTabPage*>(sender());
    int index = ui->tabWidget->indexOf(TabPage);
    if(index != -1) {


        if(!ok){
            QFile notFoundErrorFile(QLatin1String(":/info/networkerror.html"));

            if (!notFoundErrorFile.open(QIODevice::ReadOnly))
                return;

            QString html = QLatin1String(notFoundErrorFile.readAll());
            html = html.arg(this->appConfig->getServerURL(),this->appConfig->getServerURL());
            TabPage->getWebView()->page()->mainFrame()->setHtml(html);

        }

        int CantPag = ui->tabWidget->count();
        if(CantPag>0){
            QStringList TabList;
            for(int i=0;i<CantPag;i++){
                EcuredTabPage *cpage = qobject_cast<EcuredTabPage*>(ui->tabWidget->widget(i));
                TabList.append(cpage->getUrl());
            }
            settings->setValue("TabList", TabList);

        }
        animationLabel(index,false);
    }
    setMovieStop();
    ButtonBuscar->setEnabled(true);

    if(TabPage!=NULL){
        TabPage->getWebView()->page()->mainFrame()->addToJavaScriptWindowObject("EcuredMainWindow",this);
    }
}

void EcuredMainWindow::on_actionInicio_triggered()
{
    if(this->appConfig->IsUsingRemoteServer()){
        CurrentTabPage->LoadUrl(QUrl(BlankPage));
    } else {
        CurrentTabPage->LoadUrl(QUrl(AppInfo));
    }
}

void EcuredMainWindow::on_actionGuardar_triggered()
{
    QString PageTitle = this->getNormalizeFileName();
    QString fileName = QFileDialog::getSaveFileName(this,"Guardar",PageTitle,"HTML (*.html)");
    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        out.setCodec("UTF-8");
        out << viewer->page()->mainFrame()->toHtml() ;
    }
}

void EcuredMainWindow::ButtonBuscar_clicked()
{

    TextoBusqueda = EditBuscar->text();
    EnableUseFullMatch = true;


    if(!TextoBusqueda.isNull())
        if(!TextoBusqueda.isEmpty()){
            ButtonBuscar->setEnabled(false);
            if((hashTextCompletion.contains(TextoBusqueda))/* && (CheckBoxAllMatch==false)*/){
                int id = hashTextCompletion.value(TextoBusqueda);
//                if(this->appConfig->IsUsingRemoteServer()){
//                    SearchEngine::instance()->SearchById(id);
//                } else {
//                    CurrentPage->search(id);
//                }
                CurrentTabPage->search(id);
            } else {
                if(this->appConfig->IsUsingRemoteServer()){
                    SearchEngine::instance()->SearchByTitle(TextoBusqueda,true);
                } else {
                    CurrentTabPage->search(TextoBusqueda,true);
                }
            }
        }

    viewer->setFocus();
    timer->stop();
}

void EcuredMainWindow::BookmarkPageLoad(EcuredTabPage *bookmarkpage){

    CurrentTabPage = bookmarkpage;
    viewer->setFocus();
    timer->stop();
}

void EcuredMainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (ui->tabWidget->count() > 1) {
        QWidget* tabPage = ui->tabWidget->widget(index);
        ui->tabWidget->removeTab(index);
        delete tabPage;
    }
}


void EcuredMainWindow::slotTextCompletionReady(const QList<QSqlRecord>& records,double tiempo){

    int size = records.size();
    QString str;
    if(size != 1){
        ui->ResultadosPara->setText("Encontrados <b>"+str.number(size)+" resultados</b> en "+str.number(tiempo) +" sec.");
    } else{
        ui->ResultadosPara->setText("Encontrado <b>"+str.number(size)+" resultado</b> en "+str.number(tiempo) +" sec.");
    }

    setMovieStop();
    QStringList ListaResultados;
    hashTextCompletion.clear();

    for (int i = 0; i < records.size(); ++i) {
        QString texto =  records.at(i).value(0).toString();

        ListaResultados.append(texto);
        hashTextCompletion.insert(texto,records.at(i).value(1).toInt());
    }

    QStringListModel *model = new QStringListModel();
    model->setParent(completer);
    model->setStringList(ListaResultados);
    CacheCompletionList.clear();
    CacheCompletionList.append(ListaResultados);
    //delete completer->model();
    completer->setModel(model);
    completer->complete();


}

void EcuredMainWindow::slotTextCompletionReady(const QList<QPair<QString, int> >& results,bool lucene,double tiempo){

    int size = results.size();

    QString str;
    if(size != 1){
        ui->ResultadosPara->setText("Encontrados <b>"+str.number(size)+" resultados</b> en "+str.number(tiempo) +" sec.");
    } else{
        ui->ResultadosPara->setText("Encontrado <b>"+str.number(size)+" resultado</b> en "+str.number(tiempo) +" sec.");
    }

    setMovieStop();
    QStringList ListaResultados;
    hashTextCompletion.clear();


    for (int i = 0; i < size; i++) {
        QString texto =  results.at(i).first;
        ListaResultados.append(texto);
        hashTextCompletion.insert(texto,results.at(i).second);
    }


    QStringListModel *model = new QStringListModel();
    model->setStringList(ListaResultados);
    CacheCompletionList.clear();
    CacheCompletionList.append(ListaResultados);
    completer->setModel(model);
    completer->complete();
}


void EcuredMainWindow::slotSearchByTitleReady(const QList<QSqlRecord>& records ,bool useMatch,double tiempo){

    //this->EditBuscar->setText(lucene->LastQuery());

    ButtonBuscar->setEnabled(true);
    setMovieStop();
    int size = records.size();

    if(!showTextResult){
        if(!SecondSearch){
            QString str;
            if(size != 1){
                ui->ResultadosPara->setText("Encontrados <b>"+str.number(size)+" resultados</b> en "+str.number(tiempo) +" sec.");
            } else{
                ui->ResultadosPara->setText("Encontrado <b>"+str.number(size)+" resultado</b> en "+str.number(tiempo) +" sec.");
            }
        } else {
            SecondSearch = false;
        }

    } else {
        showTextResult =false;
    }


    if(EnableUseFullMatch){
        EnableUseFullMatch = false;
        ItemsModel.clear();
        for (int i = 0; i < size; ++i) {
            QString texto =  records.at(i).value(0).toString();
            int id =  records.at(i).value(1).toInt();
            //QListWidgetItem *newItem = new QListWidgetItem;
            //newItem->setText(texto);
            //newItem->setData(Qt::UserRole,id);
            //Lista->addItem(newItem);
            QStandardItem *item = new QStandardItem(texto);
            item->setData(id,Qt::UserRole);
            ItemsModel.appendRow(item);
        }


        if(size>0){
            showTextResult =true;
            int id =  records.at(0).value(1).toInt();
            CurrentTabPage->search(id);
        }
    }

    if(ItemsModel.rowCount() == 0) WidgetListaContainer->hide();
    else if (ItemsModel.rowCount() >= 2){
        ui->actionOcultarLista->setIcon(QIcon(":/images/win/previous.png"));
        if(showTextResult == true){
            WidgetListaContainer->show();
        }
    }

    static int counter;
    if(size == 0)
    {

        if(counter == 0){
            if(!TextoBusqueda.isEmpty()){
                SearchEngine::instance()->SearchByTitle(TextoBusqueda,true);
                EnableUseFullMatch = true;
                counter++;
            }
        } else {
            counter=0;
        }


    }



}


void EcuredMainWindow::slotSearchByTitleReady(const QList<QPair<QString, int> >& results,double tiempo){

    ButtonBuscar->setEnabled(true);
    int size = results.size();
    EnableUseFullMatch = false;
    setMovieStop();
    ItemsModel.clear();

    for (int i = 0; i < size ; ++i) {

        //        QListWidgetItem *newItem = new QListWidgetItem;
        //        newItem->setText(results.at(i).first);
        //        newItem->setData(Qt::UserRole,results.at(i).second);
        //        Lista->addItem(newItem);
        QStandardItem *item = new QStandardItem(results.at(i).first);
        item->setData(results.at(i).second,Qt::UserRole);
        ItemsModel.appendRow(item);
    }


    if(size>0){
        showTextResult =true;
        SecondSearch = true;
        int id = results.at(0).second;
        CurrentTabPage->search(id);
    }


    if(ItemsModel.rowCount() == 0) WidgetListaContainer->hide();
    else if (ItemsModel.rowCount() >= 2){
        ui->actionOcultarLista->setIcon(QIcon(":/images/win/previous.png"));
        WidgetListaContainer->show();
    }



    QString str;
    if(size != 1){
        ui->ResultadosPara->setText("Encontrados <b>"+str.number(size)+" resultados</b> en "+str.number(tiempo) +" sec.");
    } else{
        ui->ResultadosPara->setText("Encontrado <b>"+str.number(size)+" resultado</b> en "+str.number(tiempo) +" sec.");
    }

}

void  EcuredMainWindow::triggerTextCompletion(){



    static QString last;

    if(EditBuscar->hasFocus()) {
        QString s1 = QRegExp::escape("+-&|!(){}[]^\"~*?:\\");
        QRegExp rx("["+s1+"]");

        if(rx.indexIn(EditBuscar->text()) != -1){
            return;
        }

        if(!EditBuscar->text().isEmpty()){


            if(!last.startsWith(EditBuscar->text(),Qt::CaseInsensitive)){

                last = EditBuscar->text();

                bool encontrada = false;

                foreach (QString key, CacheCompletionList)
                    //                     if (key.startsWith(last,Qt::CaseInsensitive)){
                    //                        encontrada = true;
                    //                        break;
                    //                    }
                    if(key.contains(last,Qt::CaseInsensitive)){
                        encontrada = true;
                        break;
                    }

                if(!encontrada){
                    SearchEngine::instance()->TextCompletion(last);
                } else {
                    completer->complete();
                }

            } else {
                completer->complete();
            }

        }
    }

}


void EcuredMainWindow::resetTimer(){
    timer->setInterval(timer->interval()+100);
}


void EcuredMainWindow::showEvent(QShowEvent *event) {

    if(timer == NULL){
        timer = new QTimer(this);

        connect(timer, SIGNAL(timeout()), this, SLOT(triggerTextCompletion()));
        timer->start(500);
    }

    QMainWindow::showEvent(event);
}



void EcuredMainWindow::closeEvent(QCloseEvent *event)
{
    /*static int  Counter;

    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }

    if(Counter == 0){
        trayIcon->showMessage("EcuRed Portable","Click derecho para mostrar un menú de acciones",QSystemTrayIcon::Information,5000);
        Counter++;
    }*/

    trayIcon->hide();
    QMainWindow::closeEvent(event);


}


void EcuredMainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if(event->type() == QEvent::WindowStateChange) {
        if(isMinimized())
            QTimer::singleShot(0, this, SLOT(ToSystray()));
    }
}


bool EcuredMainWindow::eventFilter(QObject *obj, QEvent *event)
{

    if(obj == ui->menu_Edici_n)
        if(event->type() == QEvent::Paint){
            if(viewer->selectedText().trimmed().isEmpty()){
                ui->actionCopiar->setEnabled(false);
            } else {
                ui->actionCopiar->setEnabled(true);
            }
        }

    if(obj == ui->menuB_squeda_avanzada)
        if(event->type() == QEvent::MouseButtonRelease){
            static int MouseClickCounter ;
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            if(me->button() == Qt::LeftButton){
                if(MouseClickCounter %2 == 0){
                    AvanceSearchDialog *AvanceSearch = new AvanceSearchDialog(this);
                    AvanceSearch->show();
                }
                MouseClickCounter++;
            }
        }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(Qt::Key_F11 == keyEvent->key())
            on_actionPantalla_Completa_triggered(false);

    }

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *MouseEvent = static_cast<QMouseEvent*>(event);

        if(windowState() & Qt::WindowFullScreen){
            if(MouseEvent->y()<=0){
                ui->mainToolBar->setVisible(true);
            } else if(MouseEvent->y()>=40) {
                ui->mainToolBar->setVisible(false);
            }
        }
    }

    if(obj == ui->tabWidget){
        if (event->type() == QEvent::MouseButtonDblClick) {
            //QMutex m;
            //m.lock();
            static int ccont = 0;
            if (ccont % 2 != 0){

                if(this->appConfig->IsUsingRemoteServer()){
                    CreateTab(this->appConfig->getServerURL(),true);
                } else {
                    CreateTab(AppInfo,true);
                }

            }
            ccont++;
            //m.unlock();
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void EcuredMainWindow::on_actionAtras_triggered()
{
    viewer->back();
}

void EcuredMainWindow::on_actionAdelante_triggered()
{
    viewer->forward();
}

void EcuredMainWindow::on_actionAcerca_de_Ecured_Portable_triggered()
{

    DialogAcercaDe  *AcercaDe = new DialogAcercaDe();
    AcercaDe->show();
}

void EcuredMainWindow::on_actionOcultarLista_triggered()
{
    this->ListaShow();
}



void EcuredMainWindow::Lista_doubleClicked(QModelIndex item ){

    int id = item.data(Qt::UserRole).toInt();
    CurrentTabPage->search(id);
}

void EcuredMainWindow::contextMenuRequested (const QPoint & pos){


    //QListWidgetItem * item = Lista->itemAt(pos);
    QModelIndex item = Lista->indexAt(pos);

    if(item.isValid()){
        ListaItemText = item.data(Qt::UserRole).toString();
        QMenu menu(this);
        menu.addAction("Abrir en una nueva pestaña", this, SLOT(Lista_OpenItem()));
        menu.exec(Lista->mapToGlobal(pos));
    }

}

void EcuredMainWindow::Lista_OpenItem(){

    if(!ListaItemText.isEmpty()){
        CreateTab(QString("http://ecured/index?id="+ListaItemText),true);
        ListaItemText = "";
    }
}



void EcuredMainWindow::createActions()
{
    minimizeAction = new QAction("Mi&nimizar", this);
    minimizeAction->setIcon(QIcon(":/images/minimize.png"));
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction("Ma&ximizar", this);
    maximizeAction->setIcon(QIcon(":/images/maximize.png"));
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction("&Restaurar", this);
    restoreAction->setIcon(QIcon(":/images/restore.png"));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));

    quitAction = new QAction("&Salir", this);
    quitAction->setIcon(QIcon(":/images/exit.png"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void EcuredMainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/Logotipo.png"));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(activated(QSystemTrayIcon::ActivationReason)));
}

void EcuredMainWindow::ListaShow(){
    if(WidgetListaContainer->isHidden()){
        WidgetListaContainer->show();
        ui->actionOcultarLista->setIcon(QIcon(":/images/win/previous.png"));
    }else{
        WidgetListaContainer->hide();
        ui->actionOcultarLista->setIcon(QIcon(":/images/win/next.png"));
    }
}

void EcuredMainWindow::SetCurrentDocSource()
{
    lucene->SetDataBaseName(this->appConfig->getWikiDb());
    lucene->SetDirIndex(this->appConfig->getDirIndex());
}

void EcuredMainWindow::activated(QSystemTrayIcon::ActivationReason reason){
    //    if(reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger ){
    //        show();
    //    }
    if(reason == QSystemTrayIcon::Trigger ){
        //contextMenu
        if(isVisible()){
            hide();
        }
    }


    if (reason == QSystemTrayIcon::DoubleClick )
    {
        if ( isVisible() )
        {
            hide();
        } else {
            show();
            raise();
        }
    }
}


void EcuredMainWindow::PageSelectionChanged(){
}

void EcuredMainWindow::on_actionCopiar_triggered()
{
    if(!viewer->selectedText().isEmpty()){
        viewer->pageAction(QWebPage::Copy)->trigger();
    }
}

void EcuredMainWindow::SearchFinished(int size,bool isTextCompletion){
    setMovieLoading();
    ButtonBuscar->setEnabled(true);
}

void EcuredMainWindow::on_actionSalir_triggered()
{
    qApp->quit();
}

void  EcuredMainWindow::EditBuscarEnLista_textEdited (QString SearchText){

    /*QList<QListWidgetItem *> items = Lista->findItems(EditBuscarEnLista->text(),Qt::MatchStartsWith);

    if(items.size()>0){
        items.at(0)->setSelected(true);
        Lista->scrollToItem(items.at(0));
    }*/

    if(SearchText.length()>0){
        QRegExp regExp(QString("*%1*").arg(SearchText), Qt::CaseInsensitive, QRegExp::Wildcard);
        proxyModel.setFilterRegExp(regExp);
    } else {
        QRegExp regExp("*", Qt::CaseInsensitive, QRegExp::Wildcard);
        proxyModel.setFilterRegExp(regExp);
    }

}

void EcuredMainWindow::setMovieLoading(){

    ui->labelLoading->show();
}

void EcuredMainWindow::setMovieStop(){
    ui->labelLoading->hide();
}


QLabel *EcuredMainWindow::animationLabel(int index, bool addMovie)
{

    if (-1 == index)
        return NULL;

    QTabBar::ButtonPosition side = QTabBar::LeftSide;
    TabBar->tabButton(index, side);
    QLabel *loadingAnimation = qobject_cast<QLabel*>(TabBar->tabButton(index, side));


    if (!loadingAnimation) {
        loadingAnimation = new QLabel(this);
        loadingAnimation->setText("");
    }

    if(!addMovie) {
        loadingAnimation->hide();
        return loadingAnimation;
    }

    if (loadingAnimation->movie()) {
        loadingAnimation->show();
        QMovie *movie = new QMovie(":images/89.gif", QByteArray(), loadingAnimation);
        movie->setScaledSize(QSize(16,16));
        movie->start();
    } else {
        QMovie *movie = new QMovie(":images/89.gif", QByteArray(), loadingAnimation);
        movie->setScaledSize(QSize(16,16));
        loadingAnimation->setMovie(movie);
        movie->start();
    }

    TabBar->setTabButton(index, side, 0);
    TabBar->setTabButton(index, side, loadingAnimation);

    return loadingAnimation;
}




void EcuredMainWindow::on_actionImportar_triggered()
{
    ImportDocumentalSourceWizard *Idsw = new ImportDocumentalSourceWizard(this);
    connect(Idsw,SIGNAL(OnTerminate(bool,QString,QString)),this,SLOT(WizardTerminate(bool,QString,QString)));
    Idsw->show();

}

void EcuredMainWindow::on_actionCrear_ndice_de_Busqueda_triggered()
{
    IndexGeneratorWizard *wizard = new IndexGeneratorWizard(this);
    wizard->show();
}


void EcuredMainWindow::indexContentStarted(int max){

}

void  EcuredMainWindow::indexContentFinished(){


}

void EcuredMainWindow::OnError(const int ErrType,const QString Msg,const QString OrgMsg){

    static int counter = 0;

    if(counter==0){
        counter++;
        return;
    }

    Q_UNUSED(ErrType);
    QMessageBox msgbox;
    msgbox.setText(Msg);
    msgbox.setDetailedText(OrgMsg);
    msgbox.setIcon(QMessageBox::Critical);
    msgbox.exec();

}

void EcuredMainWindow::on_actionAyuda_de_Ecured_triggered()
{
    CreateTab("http://ecured/ayuda/manual.html",true);
}

void EcuredMainWindow::on_actionPantalla_Completa_triggered(bool checked)
{
    if(checked){
        setWindowState(windowState() | Qt::WindowFullScreen);
        menuBar()->hide();
        statusBar()->hide();
        ui->mainToolBar->hide();
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
        menuBar()->setVisible(true);
        statusBar()->setVisible(true);
        ui->mainToolBar->setVisible(true);
        ui->actionPantalla_Completa->setChecked(false);

    }

}

void EcuredMainWindow::codec_triggered(){

    QAction *action = qobject_cast<QAction*>(sender());
    QMessageBox msg;
    msg.setText(viewer->settings()->defaultTextEncoding());
    msg.exec();
    viewer->settings()->setDefaultTextEncoding(action->text());
}




void EcuredMainWindow::on_actionImprimir_triggered()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle("Imprimir artículo");
    if (dialog.exec() != QDialog::Accepted)
        return;
    viewer->print(&printer);
}

void EcuredMainWindow::on_actionVista_previa_de_impresi_n_triggered()
{

    QPrinter printer( QPrinter ::HighResolution);
    QPrintPreviewDialog dialog(&printer, this );
    dialog.resize( 700 , 700 );
    dialog.setWindowTitle(ui->tabWidget->tabText(ui->tabWidget->currentIndex()));
    connect(&dialog, SIGNAL (paintRequested( QPrinter *)), viewer, SLOT (print( QPrinter *)));
    dialog.exec();




    //http://doc.qt.nokia.com/4.6/demos-textedit-textedit-cpp.html
    /*   QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog dialog(&printer,this);

    dialog.setWindowTitle(ui->tabWidget->tabText(ui->tabWidget->currentIndex()));
    dialog.resize(700,700);
    connect(&dialog, SIGNAL(paintRequested(QPrinter *)),
            viewer, SLOT(print(QPrinter *)));
    dialog.exec();*/
}

void EcuredMainWindow::on_actionA_adir_marcador_triggered()
{
    //se optimizo el metodo para agregar los marcadores.
    //int *i = reinterpret_cast<int*>(0x45);
    //*i = 5;  // crash! test

    QString url = QLatin1String(viewer->url().toEncoded());
    QString title = viewer->title();
    AddBookmarkDialog dialog(this,bookmarksManager);
    dialog.setUrl(url);
    dialog.setTitle(title);
    dialog.exec();
}



void EcuredMainWindow::on_actionMostrar_todos_loas_macadores_triggered()
{
    BookmarksDialog *dialog = new BookmarksDialog(this,bookmarksManager);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

}


void EcuredMainWindow::on_actionSolo_ampliar_texto_toggled(bool textOnly)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::ZoomTextOnly, textOnly);
    on_actionNormalZoom_triggered();
}

void EcuredMainWindow::on_actionExportar_a_ODF_triggered()
{
    QString PageTitle = this->getNormalizeFileName();
    PageTitle = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + PageTitle;
    QString fileName = QFileDialog::getSaveFileName(this,"Guardar",PageTitle,"Documento de texto (*.odf)");

    if(fileName.isNull()) return;

    QTextDocument document ;
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(Qt::AlignLeft);
    QTextCursor cursor(&document);
    cursor.movePosition(QTextCursor::Start);
    cursor.insertBlock();
    cursor.insertHtml(viewer->page()->mainFrame()->toHtml());
    cursor.setBlockFormat(blockFmt);
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(blockFmt);
    //document.setHtml(viewer->page()->mainFrame()->toHtml());
    QTextDocumentWriter writer(fileName);
    writer.setFormat("odf");
    writer.write(&document);

}

QString EcuredMainWindow::getNormalizeFileName() const{
    QString PageTitle = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    PageTitle.replace(QRegExp("[^a-zA-Z0-9._-]"), "_");
    PageTitle=QDir::separator()+PageTitle;
    return PageTitle;
}

void EcuredMainWindow::on_actionExportar_a_txt_triggered()
{
    QString PageTitle = this->getNormalizeFileName();
    QString fileName = QFileDialog::getSaveFileName(this,"Guardar",PageTitle,"TXT (*.txt)");
    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        out.setCodec("UTF-8");
        out << viewer->page()->mainFrame()->toPlainText() ;

    }
}

void EcuredMainWindow::WizardTerminate(bool ok, const QString &Dir, const QString &db)
{
    if(ok){
        setCurrentFile(db);
        on_actionCrear_ndice_de_Busqueda_triggered();
    }
}

void EcuredMainWindow::SetQuery(const QString &query){
    this->EditBuscar->setText(query);
}



void EcuredMainWindow::on_actionAbrir_fuente_documental_triggered()
{

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)+ '/' ;
#ifdef linux
    QString fileName = QFileDialog::getOpenFileName(this,"Abrir",dir,"*");
#else
    QString fileName = QFileDialog::getOpenFileName(this,"Abrir",dir,"*.*");
#endif

    if(!(fileName.isEmpty() && fileName.isNull())){

        setCurrentFile(fileName);
    }
}

void EcuredMainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){
        this->setCurrentFile(action->data().toString());
    }
}

void EcuredMainWindow::setCurrentFile(const QString &fileName)
{
    if(EditBuscar!=NULL){
        EditBuscar->setEnabled(false);
    }

    this->appConfig->setUsingRemoteServer(false);
    QFileInfo finfo(fileName);
    if(QFile::exists(finfo.absoluteFilePath())){

        DatabaseManager dbm(finfo.absoluteFilePath());
        connect(&dbm,SIGNAL(Error(int,QString,QString)),this,SLOT(OnError(int,QString,QString)));

        if(dbm.OpenDB()){
            if(dbm.IsSqliteDb()){

                if(dbm.GetDbInfo(*this->databaseInfo)){
                    if(this->databaseInfo->NumFields()<4 || this->databaseInfo->NumFields()==0){
                        if(!dbm.SetDbInfo(false,true)){
                            //QMessageBox::critical(this,"Error","No se pudo poner información de la fuente documental.");
                            this->appConfig->setSourceDocAvailable(false);
                            return ;
                        }
                    }
                } else {
                    //QMessageBox::critical(this,"Error",dbm.LastErrorStr());
                    this->appConfig->setSourceDocAvailable(false);
                    return ;
                }

                QDir d;
                this->appConfig->setWikiDb(finfo.absoluteFilePath());
                this->appConfig->setDirIndex(finfo.absoluteDir().absolutePath() + d.separator());
                this->SetCurrentDocSource();

            } else {
                //QMessageBox::critical(this,"Error","La fuente documental selecionada no es válida.");
                this->appConfig->setSourceDocAvailable(false);
                return;
            }
        } else {
            //QMessageBox::critical(this,"Error",dbm.LastErrorStr());
            this->appConfig->setSourceDocAvailable(false);
            return ;
        }

    } else {
        static unsigned char init = 0;


        if(init!=0){
            QMessageBox::critical(this,"Error","La fuente documental selecionada no existe.");
            init++;
        }
          this->appConfig->setSourceDocAvailable(false);
        return ;
    }

    curFile = fileName;
    setWindowFilePath(curFile);



    QStringList files = settings->value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings->setValue("recentFileList", files);

    this->updateRecentFileActions();
    this->appConfig->setSourceDocAvailable(true);
    this->appConfig->setUsingRemoteServer(false);

    if(EditBuscar!=NULL){
        EditBuscar->setEnabled(true);
        viewer->page()->mainFrame()->evaluateJavaScript("$(\"#idOpenDocSource\").html('')");
    }

}

void EcuredMainWindow::updateRecentFileActions()
{    
    QStringList files = settings->value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {

        QFileInfo f(files[i]);
        if(f.exists()){
            QString text = tr("&%1 %2").arg(i + 1).arg(f.absoluteFilePath());
            recentFileActs[i]->setText(text);
            recentFileActs[i]->setData(files[i]);
            recentFileActs[i]->setVisible(true);
            recentFileActs[i]->setIcon(QIcon(":/images/accept-database-icon.png"));
        }

    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j){
        recentFileActs[j]->setVisible(false);
    }


    if(files.length()>0){
        ui->menuFuentes_documentales_recientes->setEnabled(true);
    } else {
        ui->menuFuentes_documentales_recientes->setEnabled(false);
    }
}

void EcuredMainWindow::handleMessage(const QString &message)
{
    showMaximized();
    raise();
}


void EcuredMainWindow::on_actionGestionar_Marcadores_triggered()
{
    //se optimizo el metodo para la gestion de los marcadores
    BookmarksDialog *dialog = new BookmarksDialog(this,bookmarksManager);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();


}

void EcuredMainWindow::on_actionA_adir_carpeta_2_triggered()
{

    AddBookmarkDialog dialog(this,bookmarksManager);
    BookmarkNode *menu = bookmarksManager->menu();
    QModelIndex index = bookmarksManager->bookmarksModel()->index(menu);
    dialog.setFolder(true);
    dialog.exec();
}

void EcuredMainWindow::ToSystray()
{
    static int  Counter;

    if (trayIcon->isVisible()) {
        hide();
    }

    if(Counter == 0){
        trayIcon->showMessage("EcuRed Portable","Click derecho para mostrar un menú de acciones.",QSystemTrayIcon::Information,5000);
        Counter++;
    }
}

void EcuredMainWindow::on_actionAbrir_funete_documental_remota_triggered()
{
    DialogEcuredServerBrowser dialog(this);
    if(dialog.exec() == QDialog::Accepted){

        this->appConfig->setUsingRemoteServer(false);
        this->appConfig->setSourceDocAvailable(false);            


        //this->p_ServerInfo = dialog.getServerInfo();
        this->appConfig->setServerInfo(dialog.getServerInfo());
        //this->p_ServerIP = dialog.getServerIP();
        this->appConfig->setServerIP(dialog.getServerIP());

        QJson::Parser parser;
        bool ok;
        QVariant variant = parser.parse(this->appConfig->getServerInfo().toUtf8(),&ok);
        if(ok){
            QJson::QObjectHelper::qvariant2qobject(variant.toMap(), this->databaseInfo);
            this->appConfig->setServerPort(this->databaseInfo->WebServerPort());

            //qDebug()<<this->p_ServerInfo;
            //qDebug()<<this->p_ServerIP;

            this->appConfig->setUsingRemoteServer(true);
            this->appConfig->setSourceDocAvailable(true);  
            this->BuildUrlAndReloadPage();          
        } else {
            this->OnError(5,"Los datos provenientes del servidor contienen un formato inválido.",parser.errorString());
        }

    }

}

void EcuredMainWindow::AppConfigChange()
{
    //qDebug()<<"AppConfigChange";
    if(this->appConfig->IsSourceDocAvailable()){
        ui->actionCrear_ndice_de_Busqueda->setEnabled(true);
        ui->menuB_squeda_avanzada->setEnabled(true);
    } else {
        ui->actionCrear_ndice_de_Busqueda->setEnabled(false);
        ui->menuB_squeda_avanzada->setEnabled(false);
    }
}

void EcuredMainWindow::BuildUrlAndReloadPage()
{

    if(this->appConfig->IsUsingRemoteServer()){
        QUrl url(this->appConfig->getServerURL());
        viewer->load(url);
    }

}



