#include <QtGui/QApplication>
#include "wikirender.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WikiRender w;
    w.showMaximized();
    
    return a.exec();
}
