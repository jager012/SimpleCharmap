#pragma once

#include <QIODevice>
#include <QList>
#include <QMap>
#include <QString>

class UnicodePoints {
    public:
        static QString codePointToString(quint32 cp, bool withU = true);

        bool load(QIODevice* file);

        const QString operator[](const int cp) const;  // returns the description of the cp codepoint

        QMap<quint32, QString>::const_iterator cstart(quint32 cp) const;
        QMap<quint32, QString>::const_iterator cend() const;

        QList<quint32>& getFullTable();

        qsizetype size() const;

    private:
        QMap<quint32, QString> m_map;
        QList<quint32> m_fullTable;  // only for fast lookup when constructing the complete table
};
