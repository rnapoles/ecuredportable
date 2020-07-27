
#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "lineedit.h"

class ClearButton;
class SearchButton;
class SearchLineEdit : public LineEdit
{
    Q_OBJECT

public:
    SearchLineEdit(QWidget *parent = 0);

    ClearButton *clearButton() const;
    SearchButton *searchButton() const;

private:
    void init();
    ClearButton *m_clearButton;
    SearchButton *m_searchButton;

};

#endif // SEARCHLINEEDIT_H

