#ifndef WIKIRENDER_H
#define WIKIRENDER_H

#include <QMainWindow>

namespace Ui {
class WikiRender;
}

class WikiRender : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WikiRender(QWidget *parent = 0);
    ~WikiRender();
    
private slots:
    void on_actionConvertir_triggered();
    void populateJavaScriptWindowObject();

    void on_tabWidget_currentChanged(int index);

    void on_actionEjemplo_triggered();

private:
    Ui::WikiRender *ui;
};

#endif // WIKIRENDER_H
