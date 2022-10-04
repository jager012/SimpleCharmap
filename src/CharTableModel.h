#pragma once

#include <QAbstractTableModel>

#include "UnicodeGroups.h"
#include "UnicodePoints.h"

class CharTableModel : public QAbstractTableModel {
        Q_OBJECT

    public:
        CharTableModel(UnicodePoints& codepoints, UnicodeGroups& groups, QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;

        int width() const;
        int cellSize() const;

        quint32 lookupIndex(const QModelIndex& index) const;
        QString statusHint(const QModelIndex& index) const;

    private:
        UnicodePoints& m_codePoints;
        UnicodeGroups& m_groupsTree;
        QList<quint32> m_selectionTable;

        QList<quint32>* m_table;  // reference to the table. just swap between
        // UnicodePoints::getFullTable and m_table

        int m_width;
        int m_cols;

        int m_cellSize;

    signals:
        void selectionUpdated();

    public slots:
        void setWidth(int w);
        void setCellSize(int size);
        void setSelection(QList<TreeNode<UnicodeGroup>*> selection);
};
