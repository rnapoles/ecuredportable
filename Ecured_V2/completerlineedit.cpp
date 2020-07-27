#include "completerlineedit.h"
#include <QKeyEvent>
#include <QDebug>


CompleterLineEdit::CompleterLineEdit(QWidget *parent): QLineEdit(parent)
{

    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(completer);

}

void CompleterLineEdit::setCompletionList(const QStringList &list){

    QStringListModel *model = new QStringListModel();
    model->setStringList(list);
    completer->setModel(model);
}

void CompleterLineEdit::complete()
{
    completer->complete();
}



