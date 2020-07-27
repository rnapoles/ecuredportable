#ifndef LUCENEDOCUMENT_H
#define LUCENEDOCUMENT_H

#include <QObject>
#include <QVariantList>


class LuceneDocument : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString SearchField READ SearchField WRITE setSearchField)
    Q_PROPERTY(QString LastQueryString READ LastQueryString WRITE setLastQueryString)
    Q_PROPERTY(bool MultiFieldSearch READ MultiFieldSearch WRITE setMultiFieldSearch)
    Q_PROPERTY(QString ErrorText READ ErrorText WRITE setErrorText)
    Q_PROPERTY(bool Error READ Error WRITE setError)
    Q_PROPERTY(QVariantList Arts READ Arts WRITE setArts)

public:
    explicit LuceneDocument(QObject *parent = 0);

    QString SearchField() const;
    void setSearchField(const QString& SearchField);

    QString LastQueryString() const;
    void setLastQueryString(const QString& LastQueryString);

    bool MultiFieldSearch() const;
    void setMultiFieldSearch(const bool &MultiFieldSearch);


    bool Error() const;
    void setError(const bool& Error);

    QString ErrorText() const;
    void setErrorText(const QString& ErrorText);

    QVariantList Arts() const;
    void setArts(const QVariantList  Arts);

private:
  QString p_SearchField;
  QString p_LastQueryString;
  bool p_MultiFieldSearch;
  QString p_ErrorText;
  bool p_Error;
  QVariantList p_Arts;
};

#endif // LUCENEDOCUMENT_H
