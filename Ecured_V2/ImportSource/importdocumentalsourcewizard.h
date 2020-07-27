#ifndef IMPORTDOCUMENTALSOURCEWIZARD_H
#define IMPORTDOCUMENTALSOURCEWIZARD_H

#include <QWizard>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QFutureWatcher>
#include <QtConcurrentRun>


class ImportDocumentalSourceWizard : public QWizard
{
    Q_OBJECT
public:
    enum {Page_SelectDocSource, Page_Config, Page_Progress , Page_Conclusion };
    explicit ImportDocumentalSourceWizard(QWidget *parent = 0);
    ~ImportDocumentalSourceWizard();

signals:
    void OnTerminate(bool ok,const QString &Dir,const QString &db);

public slots:
    
};

class DocSourceWizardPage : public QWizardPage
 {
     Q_OBJECT

 public:
     DocSourceWizardPage(QWidget *parent = 0);

     int nextId() const;

 private:
     QLabel *topLabel;
     QRadioButton *WikiDbRadioButton;
     QRadioButton *XmlBz2RadioButton;
     QRadioButton *WikiTaxiRadioButton;
 };

class ConfigWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    ConfigWizardPage(QWidget *parent = 0);
    int nextId() const;
    bool validatePage();

private:
    QLineEdit  *LineEditDocSource;
    QLineEdit  *LineEditProjDir;
    QLabel  *LabelDocSource;
    QLabel  *LabelProjDir;
    QLabel *topLabel;
private slots:
    void on_LineEditDocSource_clicked();
    void on_LineEditProjDir_clicked();
protected:
    bool eventFilter(QObject *obj, QEvent *event)    ;
};

class ProgressWizardPage : public QWizardPage
 {
     Q_OBJECT

 public:
     ProgressWizardPage(QWidget *parent = 0);
     void initializePage();
     int nextId() const;
     bool validatePage();

 private:
     QProgressBar *ProgressBar;
     QFutureWatcher <void> m_futureWatcher;
     QLabel *topLabel;

private slots:
    void Progress(int progress);
    void ReportMsg(const QString &Msg);
    void valueChanged ( int value );
    void Init();
    void Finished();
    void Error(const QString &Msg);
    void Error(const int ErrType,const QString &Msg,const QString &OrgMsg);
 };

class ConclusionWizardPage : public QWizardPage
 {
     Q_OBJECT

 public:
     ConclusionWizardPage(QWidget *parent = 0);
     void initializePage();
     int nextId() const;


 private:
     QLabel *bottomLabel;

 };

#endif // IMPORTDOCUMENTALSOURCEWIZARD_H
