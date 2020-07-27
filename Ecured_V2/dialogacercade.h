#ifndef DIALOGACERCADE_H
#define DIALOGACERCADE_H

#include <QDialog>
#include <QTableWidgetItem>


namespace Ui {
    class DialogAcercaDe;
}

class DialogAcercaDe : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAcercaDe(QWidget *parent = 0);
    ~DialogAcercaDe();

private:
    Ui::DialogAcercaDe *ui;

protected:
private slots:
    void on_pushButton_clicked();
};

#endif // DIALOGACERCADE_H
