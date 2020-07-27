#ifndef AVANCESEARCHDIALOG_H
#define AVANCESEARCHDIALOG_H

#include <QDialog>
//#include "clucenehelper.h"
#include <searchengine.h>


namespace Ui {
    class AvanceSearchDialog;
}

class AvanceSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AvanceSearchDialog(QWidget *parent = 0);
    ~AvanceSearchDialog();

private slots:

    void on_label_5_linkActivated(const QString &link);

    void on_label_6_linkActivated(const QString &link);

    void on_label_9_linkActivated(const QString &link);

    void on_buttonBox_accepted();

    void on_LOperator1_currentIndexChanged(int index);

    void on_LOperator2_currentIndexChanged(int index);

    void on_QueryTerm1_textChanged(const QString &arg1);

    void on_QueryTerm2_textChanged(const QString &arg1);

    void on_MoreButton_clicked();

    void on_ExactQuery_textEdited(const QString &arg1);

private:
    bool IsValidQueryString(const QString);
    void MarkInvalid(QWidget *w,bool t = true);

    Ui::AvanceSearchDialog *ui;
    //CLuceneHelper *lucene;
    SearchEngine *searchEngine;
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // AVANCESEARCHDIALOG_H
