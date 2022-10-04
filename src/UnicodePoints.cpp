#include "UnicodePoints.h"

#include <QIODevice>
#include <QList>

QString UnicodePoints::codePointToString(quint32 cp, bool withU) {
    QString cpString = QString("%1").arg(cp, 0, 16).toUpper();

    if (cpString.size() == 2)
        return withU ? "U+00" + cpString : "00" + cpString;
    else if (cpString.size() == 3)
        return withU ? "U+0" + cpString : "0" + cpString;

    return withU ? "U+" + cpString : cpString;
}

bool UnicodePoints::load(QIODevice* file) {
    QByteArray signature = file->read(4);
    QByteArray cmp("UCP", 4);
    if (signature != cmp) {
        return false;
    }

    quint32 size;
    file->read((char*)&size, sizeof(size));
    m_fullTable.reserve(size);

    while (!file->atEnd()) {
        quint32 cp;
        if (file->read((char*)&cp, sizeof(cp)) != sizeof(cp)) {
            return false;
        }

        quint16 length;
        if (file->read((char*)&length, sizeof(length)) != sizeof(length)) {
            return false;
        }

        QString na = file->read(length);
        if (na.size() != length) {
            return false;
        }

        m_map[cp] = na;
        m_fullTable.append(cp);
    }

    return true;
}

const QString UnicodePoints::operator[](const int cp) const { return m_map.value(cp); }

// vector representing the full unicode table in sequential order for rendering
QList<quint32>& UnicodePoints::getFullTable() { return m_fullTable; }

qsizetype UnicodePoints::size() const { return m_map.size(); }

QMap<quint32, QString>::const_iterator UnicodePoints::cstart(quint32 cp) const {
    auto it = m_map.constFind(cp);
    while (it == m_map.constEnd()) {
        it = m_map.constFind(++cp);
    }
    return it;
}

QMap<quint32, QString>::const_iterator UnicodePoints::cend() const { return m_map.constEnd(); };
