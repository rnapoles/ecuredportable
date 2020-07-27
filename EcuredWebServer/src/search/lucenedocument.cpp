#include "lucenedocument.h"

LuceneDocument::LuceneDocument(QObject *parent) :
    QObject(parent)
{
    this->p_Error = false;
}

QString LuceneDocument::SearchField() const
{
    return this->p_SearchField;
}

void LuceneDocument::setSearchField(const QString &SearchField)
{
    this->p_SearchField = SearchField;
}

void LuceneDocument::setLastQueryString(const QString &LastQueryString)
{
    this->p_LastQueryString = LastQueryString;
}

QString LuceneDocument::LastQueryString() const
{
    return this->p_LastQueryString;
}

void LuceneDocument::setMultiFieldSearch(const bool &MultiFieldSearch)
{
    this->p_MultiFieldSearch = MultiFieldSearch;
}

bool LuceneDocument::MultiFieldSearch() const
{
    return this->p_MultiFieldSearch;
}

bool LuceneDocument::Error() const
{
    return this->p_Error;
}

void LuceneDocument::setError(const bool &Error)
{
    this->p_Error = Error;
}

QString LuceneDocument::ErrorText() const
{
    return this->p_ErrorText;
}

void LuceneDocument::setErrorText(const QString &ErrorText)
{
    this->p_ErrorText = ErrorText;
}

QVariantList LuceneDocument::Arts() const
{
    return this->p_Arts;
}

void LuceneDocument::setArts(const QVariantList Arts)
{
    this->p_Arts = Arts;
}
