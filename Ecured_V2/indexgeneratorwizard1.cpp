#include "indexgeneratorwizard.h"
#include <QVariant>
#include <QFile>

IndexGeneratorWizard::IndexGeneratorWizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage(this));
    setPage(Page_FirtsPage, new FirtsPage(this));
    setPage(Page_Progress, new ProgressPage(this));
    setPage(Page_Conclusion, new ConclusionPage(this));
    setStartId(Page_Intro);

    setAttribute(Qt::WA_DeleteOnClose);

    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/wizard/logo.png"));

    QList<QWizard::WizardButton> layout;
    layout << QWizard::Stretch <<QWizard::BackButton<< QWizard::NextButton << QWizard::FinishButton;
    //layout << QWizard::Stretch << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    setButtonText(QWizard::NextButton,"Siguiente >");
    setButtonText(QWizard::BackButton,"< Anterior");
    setButtonText(QWizard::FinishButton,"Finalizar");

    setWindowTitle("Crear índice de búsqueda");
    CanClose = true;
    setWizardStyle(QWizard::ModernStyle);
}

IndexGeneratorWizard::~IndexGeneratorWizard(){
}

void IndexGeneratorWizard::accept(){
     QDialog::accept();
}


IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introducción"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/wizard/watermark.png"));

    topLabel = new QLabel("Este asistente le guiará durante el proceso de indexación."
                          " Este proceso puede demorar varios minutos, por lo que es recomendable hacerlo"
                          " en equipos con buenas prestaciones. Una vez iniciado este proceso no debe ser detenido"
                          ", pues provocaría que la aplicación dejase de funcionar."
                          );
    topLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

int IntroPage::nextId() const
{
       return IndexGeneratorWizard::Page_FirtsPage;
}

FirtsPage::FirtsPage(QWidget *parent)
    : QWizardPage(parent)
{


    setTitle("Selecione los campos que desea indexar");
    setSubTitle("Es recomendable que si su equipo no es muy rápido, solo se haga la indexación  por título.");

    TitleCheckbox = new QCheckBox("Indexar título",this);
    ContentCheckbox = new QCheckBox("Indexar contenido",this);



    registerField("index.title*", TitleCheckbox);
    registerField("index.content", ContentCheckbox);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(TitleCheckbox, 0, 0);
    layout->addWidget(ContentCheckbox, 1, 0);
    setLayout(layout);
}

int FirtsPage::nextId() const
{
    return IndexGeneratorWizard::Page_Progress;
}

void FirtsPage::initializePage(){
    wizard()->button(QWizard::CancelButton)->setEnabled(false);
}

ProgressPage::ProgressPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Indexando fuente documental");
    setSubTitle("Iniciando ");

    lucene = CLuceneHelper::getInstance();

    connect(lucene,SIGNAL(indexContentStarted(int)),this,SLOT(indexContentStarted(int)));
    connect(lucene,SIGNAL(indexContentProgress(int)),this,SLOT(indexContentProgress(int)));
    connect(lucene,SIGNAL(indexContentFinished()),this,SLOT(indexContentFinished()));

    topLabel = new QLabel(" ");
    QFile fich(":/info/indexacion0.txt");
    fich.open(QIODevice::ReadOnly | QIODevice::Text);

    QString text = fich.readAll();

    topLabel->setText(text);

    topLabel->setWordWrap(true);
    topLabel->setMinimumWidth(200);

    ProgressBar = new QProgressBar(this);
    ProgressBar->setMinimum(0);
    ProgressBar->setValue(0);

    ProgressBar->setAlignment(Qt::AlignCenter);
    connect(ProgressBar,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(ProgressBar);
    setLayout(layout);

}

void ProgressPage::initializePage(){

    wizard()->button(QWizard::BackButton)->setEnabled(false);
    wizard()->button(QWizard::NextButton)->setEnabled(false);

  //  wizard()->button(QWizard::BackButton)->setEnabled(false);

    bool IndexContent = field("index.content").toBool();
    QFuture <void> f = QtConcurrent::run(lucene,&CLuceneHelper::indexContent,IndexContent);
    Watcher.setFuture(f);
}

void ProgressPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);

    if (visible) {
        wizard()->button(QWizard::BackButton)->setEnabled(false);
    } else {

    }
}

int ProgressPage::nextId() const
{
    return IndexGeneratorWizard::Page_Conclusion;
}

bool ProgressPage::isComplete() const
{

    return ProgressBar->value() == ProgressBar->maximum();

}

void ProgressPage::indexContentStarted(int max){
    wizard()->button(QWizard::BackButton)->setEnabled(false);
    ProgressBar->setMaximum(max);
}

void ProgressPage::indexContentProgress(int progress){
    ProgressBar->setValue(progress);
}


void ProgressPage::indexContentFinished(){

    setSubTitle("Finalizado ...");
   // wizard()->button(QWizard::NextButton)->setEnabled(true);
    wizard()->button(QWizard::NextButton)->setEnabled(true);
    wizard()->button(QWizard::BackButton)->setEnabled(true);
}

void ProgressPage::valueChanged (int value){

    static int counter;

    if(ProgressBar->text() == "50%"){
        QFile fich(":/info/motor0.txt");
        fich.open(QIODevice::ReadOnly | QIODevice::Text);
        QString text = fich.readAll();
        topLabel->setText(text);
    }

    if(counter == 0){
        setSubTitle("Procesando .");
    } else if (counter == 1)  {
        setSubTitle("Procesando ..");
    } else if (counter == 2)  {
        setSubTitle("Procesando ...");
    } else if (counter == 3)  {
        setSubTitle("Procesando ....");
    } else if (counter == 4)  {
        setSubTitle("Procesando .....");
        counter = 0;
    }

    counter++;
}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Finalizar");
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/wizard/watermark.png"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(bottomLabel);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{
    return -1;
}

void ConclusionPage::initializePage()
{
    wizard()->button(QWizard::BackButton)->setEnabled(false);
    QString text = "El proceso ha concluido satisfactoriamente.";
    bottomLabel->setText(text);
}


void ConclusionPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);

}

void IndexGeneratorWizard::closeEvent(QCloseEvent *event)
{
    event->accept();
}

