#ifndef ECUREDQCOMPLETER_H
#define ECUREDQCOMPLETER_H

#include <QCompleter>

class EcuredQCompleter : public QCompleter
{
    Q_OBJECT

public:
    inline EcuredQCompleter(const QStringList& words, QObject * parent) :
        QCompleter(parent), m_list(words), m_model()
    {
        setModel(&m_model);
    }

    inline void update(QString word)
    {
        // Do any filtering you like.
        // Here we just include all items that contain word.
        QStringList filtered = m_list.filter(word, caseSensitivity());
        m_model.setStringList(filtered);
        m_word = word;
        complete();
    }

    inline QString word()
    {
        return m_word;
    }

private:
    QStringList m_list;
    QStringListModel m_model;
    QString m_word;
};

#endif // ECUREDQCOMPLETER_H
