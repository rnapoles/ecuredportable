#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>
#include <QProgressDialog>
#include "clucenehelper.h"

namespace Ui {
    class Import;
}

class Import : public QDialog
{
    Q_OBJECT

public:
    explicit Import(QWidget *parent = 0);
    bool copyFile(const QString &source, QString &destination, bool overwrite, bool move,const QString &msgstr);
    ~Import();

private slots:
    void on_pushButtonSource_clicked();
    void on_pushButtonBackup_clicked();
    void on_pushButtonAceptar_clicked();
    void on_pushButtonCancelar_clicked();
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Import *ui;

protected:
    bool eventFilter(QObject *obj, QEvent *event)    ;

public slots:
};

#endif // IMPORT_H
