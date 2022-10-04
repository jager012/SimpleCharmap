#pragma once

#include <QAbstractItemModel>
#include <QHash>

#include "UnicodeGroups.h"

class GroupsTreeModel : public QAbstractItemModel {
        Q_OBJECT

    public:
        static const int ExpandedRole = Qt::UserRole;

        GroupsTreeModel(UnicodeGroups& g, QObject* parent = nullptr)
            : QAbstractItemModel(parent), m_groups(g), m_tree(&g), m_selection(&m_treeSelection) {}

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role) const override;

        const QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>& selection() const;
        void restoreSelection(QList<quint32> restore);

    private:
        UnicodeGroups& m_groups;

        TreeNode<UnicodeGroup>* m_tree;
        QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>*
        m_selection;  // reference to the current selection hashtable

        QHash<TreeNode<UnicodeGroup>*, Qt::CheckState> m_treeSelection;
        QHash<TreeNode<UnicodeGroup>*, Qt::CheckState> m_filteredSelection;

        QSet<TreeNode<UnicodeGroup>*> m_expandedItems;

        void _setItemChecked(TreeNode<UnicodeGroup>* ptr, Qt::CheckState check,
                             QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>* selection);
        void _setParentCheckStatus(TreeNode<UnicodeGroup>* ptr, Qt::CheckState check,
                                   QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>* selection);

        void _setCheckStatusTopDown(TreeNode<UnicodeGroup>* ptr,
                                    QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>* selection);
        TreeNode<UnicodeGroup>* _filterTree(QString query, TreeNode<UnicodeGroup>* ptr);

    signals:
        void selectionChanged(QList<TreeNode<UnicodeGroup>*>);
        void itemsFiltered();  // just to expand the view

    public slots:
        void toggleItemCheck(const QModelIndex& index);
        void filterItems(const QString& query);

        void itemExpanded(const QModelIndex& i);
        void itemCollapsed(const QModelIndex& i);
};
