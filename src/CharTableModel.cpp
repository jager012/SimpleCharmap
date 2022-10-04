#include "CharTableModel.h"

#include <QSize>

CharTableModel::CharTableModel(UnicodePoints& codepoints, UnicodeGroups& groups, QObject* parent)
    : QAbstractTableModel(parent),
      m_codePoints(codepoints),
      m_groupsTree(groups),
      m_width(0),
      m_cols(16),
      m_cellSize(35) {
    m_table = &codepoints.getFullTable();
}

int CharTableModel::rowCount(const QModelIndex& parent) const {
    return m_table->size() / m_cols + (m_table->size() % m_cols != 0);
}

int CharTableModel::columnCount(const QModelIndex& parent) const { return m_cols; }

QVariant CharTableModel::data(const QModelIndex& index, int role) const {
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::StatusTipRole:
            return statusHint(index);

        case Qt::SizeHintRole:
            return QSize(m_width / m_cols + (index.column() < m_width % m_cols ? 0 : -1), m_cellSize);

        case Qt::DisplayRole: {
            if (!index.isValid()) return QVariant();

            quint32 cp = lookupIndex(index);
            if (cp)
                return QString::fromUcs4(reinterpret_cast<char32_t*>(&cp), 1);
            else
                return QVariant();
        }

        default:
            return QVariant();
    }
}

QVariant CharTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

quint32 CharTableModel::lookupIndex(const QModelIndex& index) const {
    int idx = index.row() * m_cols + index.column();
    if (idx < m_table->size())
        return m_table->at(idx);
    else
        return 0;
}

int CharTableModel::width() const { return m_width; }

void CharTableModel::setWidth(int w) {
    if (w <= 0) return;

    int c = (w / m_cellSize > 0 ? w / m_cellSize : 1);
    m_width = w;
    if (c != m_cols) {
        beginResetModel();
        m_cols = c;
        endResetModel();
    }
}

int CharTableModel::cellSize() const { return m_cellSize; }

void CharTableModel::setCellSize(int size) {
    if (size != m_cellSize) {
        m_cellSize = size;
        setWidth(m_width);
    }
}

void CharTableModel::setSelection(QList<TreeNode<UnicodeGroup>*> selection) {
    if ((selection.count() == 0 || selection.count() == m_groupsTree.size()) && m_table == &m_codePoints.getFullTable()) {
        return;
    }

    beginResetModel();
    if (selection.count() == 0 || selection.count() == m_groupsTree.size()) {
        m_table = &m_codePoints.getFullTable();
    } else {
        m_table = &m_selectionTable;
        m_selectionTable.clear();
        for (auto it = selection.begin(); it != selection.end(); it++) {
            if ((*it)->data().to != 0) {
                for (auto it2 = m_codePoints.cstart((*it)->data().from);
                     it2 != m_codePoints.cend() && it2.key() <= (*it)->data().to; it2++)
                    m_selectionTable.append(it2.key());
            }
        }
    }

    endResetModel();
    emit selectionUpdated();
}

QString CharTableModel::statusHint(const QModelIndex& index) const {
    quint32 cp = lookupIndex(index);
    if (!cp) return QString();

    QString name = m_codePoints[cp];
    QString out;
    QTextStream ts(&out);
    ts << UnicodePoints::codePointToString(cp) << " ";

    auto tokens = name.split(" ");
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
        ts << (*it)[0].toUpper() << (*it).right((*it).length() - 1).toLower() << " ";
    }

    return out;
}
