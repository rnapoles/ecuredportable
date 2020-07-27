#ifndef INDEXGENERATORWIZARD_H
#define INDEXGENERATORWIZARD_H

#include <QWizard>

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QRegExpValidator>
#include <QProgressBar>
#include "clucenehelper.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QCloseEvent>

class IndexGeneratorWizard : public QWizard
{
    Q_OBJECT
public:
    enum { Page_Intro, Page_FirtsPage, Page_Progress , Page_Conclusion };

    explicit IndexGeneratorWizard(QWidget *parent = 0);
    ~IndexGeneratorWizard();
    bool CanClose;

    void accept();
signals:

public slots:

private slots:

protected:
    void closeEvent(QCloseEvent *event);
private:

};

class IntroPage : public QWizardPage
 {
     Q_OBJECT

 public:
     IntroPage(QWidget *parent = 0);

     int nextId() const;

 private:
     QLabel *topLabel;
 };


 class FirtsPage : public QWizardPage
 {
     Q_OBJECT

 public:
     FirtsPage(QWidget *parent = 0);
     void initializePage();

     int nextId() const;

 private:
     QCheckBox *TitleCheckbox;
     QCheckBox *ContentCheckbox;
 };

 class ProgressPage : public QWizardPage
 {
     Q_OBJECT

 public:
     ProgressPage(QWidget *parent = 0);
     void initializePage();
     int nextId() const;
     bool isComplete() const;
     void setVisible(bool visible);
 private:
    QProgressBar *ProgressBar;
    CLuceneHelper * lucene;
    QFutureWatcher <void> Watcher;
    QLabel *topLabel;

private slots:
    void indexContentStarted(int max);
    void indexContentProgress(int progress);
    void indexContentFinished();
    void valueChanged ( int value );

 };

 class ConclusionPage : public QWizardPage
 {
     Q_OBJECT

 public:
     ConclusionPage(QWidget *parent = 0);

     void initializePage();
     int nextId() const;
     void setVisible(bool visible);

 private slots:



 private:
     QLabel *bottomLabel;
 };

#endif // INDEXGENERATORWIZARD_H
