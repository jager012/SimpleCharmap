#pragma once

#include <QTreeView>

#include "Tree.h"
#include "UnicodePoints.h"

class GroupsTreeView : public QTreeView {
        Q_OBJECT

    public:
        GroupsTreeView(QWidget* parent);
        ~GroupsTreeView();

        void setModel(QAbstractItemModel* m);

    public slots:
        void restoreExpandedItems();

    private:
        QSet<QModelIndex> m_expandedItems;
};
