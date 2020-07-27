#ifndef COMPLETERLINEEDIT_H
#define COMPLETERLINEEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>

class CompleterLineEdit : public QLineEdit
{
public:
    CompleterLineEdit(QWidget *parent);
    void setCompletionList(const QStringList &list);
    void complete();


private:
     QCompleter *completer;

protected:
  //  void keyReleaseEvent ( QKeyEvent * event );
};

#endif // COMPLETERLINEEDIT_H
