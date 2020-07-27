
#include "searchlineedit.h"
#include "clearbutton.h"
#include "searchbutton.h"

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : LineEdit(parent)
{
    init();
}

void SearchLineEdit::init()
{
    // search button on the left
    m_searchButton = new SearchButton(this);
    addWidget(m_searchButton, LeftSide);

    // clear button on the right
    m_clearButton = new ClearButton(this);
    connect(m_clearButton, SIGNAL(clicked()),
            this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)),
            m_clearButton, SLOT(textChanged(const QString&)));
    addWidget(m_clearButton, RightSide);
    m_clearButton->hide();

    updateTextMargins();
    setInactiveText("Buscar");
}

ClearButton *SearchLineEdit::clearButton() const
{
    return m_clearButton;
}

SearchButton *SearchLineEdit::searchButton() const
{
    return m_searchButton;
}

