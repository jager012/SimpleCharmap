#include "GroupsTreeModel.h"

#include <algorithm>

#include "UnicodePoints.h"

QModelIndex GroupsTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    const TreeNode<UnicodeGroup>* parentPtr;
    if (!parent.isValid())
        parentPtr = m_tree;
    else
        parentPtr = static_cast<TreeNode<UnicodeGroup>*>(parent.internalPointer());

    TreeNode<UnicodeGroup>* child = parentPtr->children().at(row);
    if (child)
        return createIndex(row, column, child);
    else
        return QModelIndex();
}

QModelIndex GroupsTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    TreeNode<UnicodeGroup>* child = static_cast<TreeNode<UnicodeGroup>*>(index.internalPointer());
    TreeNode<UnicodeGroup>* parent = child->parent();

    if (parent->isRoot()) return QModelIndex();

    return createIndex(parent->siblings().indexOf(parent), 0, parent);
}

int GroupsTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0) return 0;

    const TreeNode<UnicodeGroup>* ptr = m_tree;
    if (!parent.isValid())
        ptr = m_tree;
    else
        ptr = static_cast<TreeNode<UnicodeGroup>*>(parent.internalPointer());

    return ptr->children().count();
}

int GroupsTreeModel::columnCount(const QModelIndex& parent) const { return 1; }

QVariant GroupsTreeModel::data(const QModelIndex& index, int role) const {
    TreeNode<UnicodeGroup>* ptr = static_cast<TreeNode<UnicodeGroup>*>(index.internalPointer());

    switch (role) {
        case Qt::CheckStateRole:
            return m_selection->value(ptr);

        case Qt::StatusTipRole: {
            UnicodeGroup group = ptr->data();
            if (group.to != 0)
                return group.name + " (" + UnicodePoints::codePointToString(group.from, false) + "-" +
                        UnicodePoints::codePointToString(group.to, false) + ")";
            else
                return group.name;
        }

        case Qt::DisplayRole:
            if (!index.isValid()) return QVariant();

            return ptr->data().name;

        case GroupsTreeModel::ExpandedRole:
            if (m_expandedItems.contains(ptr->reference()))
                return true;
            else
                return false;

        default:
            return QVariant();
    }
}

Qt::ItemFlags GroupsTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::ItemFlags();

    return Qt::ItemIsEnabled;
}

void GroupsTreeModel::_setItemChecked(TreeNode<UnicodeGroup>* ptr, Qt::CheckState check,
                                      QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>* selection) {
    if (check == Qt::Unchecked)
        selection->remove(ptr);
    else
        selection->insert(ptr, check);

    if (ptr->children().count() == 0)
        return;
    else {
        for (auto it = ptr->children().begin(); it != ptr->children().end(); it++)
            _setItemChecked(*it, check, selection);

        if (!ptr->isRoot())
            emit dataChanged(createIndex(0, 0, ptr), createIndex(ptr->siblings().count() - 1, 0, ptr));
    }
}

void GroupsTreeModel::_setParentCheckStatus(
        TreeNode<UnicodeGroup>* ptr, Qt::CheckState check,
        QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>* selection) {
    if (ptr->isRoot()) return;

    bool parentPartiallyChecked = false;
    for (auto it = ptr->siblings().begin(); it != ptr->siblings().end(); it++) {
        if (selection->value(*it) != check) {
            parentPartiallyChecked = true;
            break;
        }
    }
    if (parentPartiallyChecked) {
        selection->insert(ptr->parent(), Qt::PartiallyChecked);
    } else {
        if (check == Qt::Unchecked)
            selection->remove(ptr->parent());
        else
            selection->insert(ptr->parent(), Qt::Checked);
    }

    emit dataChanged(createIndex(0, 0, ptr), createIndex(ptr->siblings().count() - 1, 0, ptr));

    _setParentCheckStatus(ptr->parent(), check, selection);
}

void GroupsTreeModel::toggleItemCheck(const QModelIndex& index) {
    TreeNode<UnicodeGroup>* ptr = static_cast<TreeNode<UnicodeGroup>*>(index.internalPointer());
    Qt::CheckState check = m_selection->value(ptr) == Qt::Checked ? Qt::Unchecked : Qt::Checked;

    // if currently filtering, pass the actual ptr value and update selection of the filtered tree
    if (m_selection == &m_filteredSelection) {
        _setItemChecked(ptr, check, &m_filteredSelection);
        _setParentCheckStatus(ptr, check, &m_filteredSelection);
    }

    // filter the real stuff
    // regardless of filtering, we will update the current value in the actual tree selection
    _setItemChecked(ptr->reference(), check, &m_treeSelection);
    _setParentCheckStatus(ptr->reference(), check, &m_treeSelection);

    auto selectionList = m_treeSelection.keys();
    std::sort(selectionList.begin(), selectionList.end(),
              [](TreeNode<UnicodeGroup>* a, TreeNode<UnicodeGroup>* b) -> bool {
        return a->data().from < b->data().from;
    });

    emit selectionChanged(selectionList);
}

void GroupsTreeModel::restoreSelection(QList<quint32> restore) {
    beginResetModel();
    // all the elements there don't have children
    for (auto it = restore.begin(); it != restore.end(); it++) {
        auto ptr = m_groups.lookupGroup(*it);
        m_selection->insert(ptr, Qt::Checked);
    }

    auto selectionList = m_selection->keys();
    std::sort(selectionList.begin(), selectionList.end(),
              [](TreeNode<UnicodeGroup>* a, TreeNode<UnicodeGroup>* b) -> bool {
        return a->data().from < b->data().from;
    });

    for (auto it = m_groups.children().begin(); it != m_groups.children().end(); it++)
        _setCheckStatusTopDown((*it), m_selection);

    endResetModel();

    emit selectionChanged(selectionList);
}

const QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>& GroupsTreeModel::selection() const {
    return *m_selection;
}

// should never be called on root node
void GroupsTreeModel::_setCheckStatusTopDown(
        TreeNode<UnicodeGroup>* ptr, QHash<TreeNode<UnicodeGroup>*, Qt::CheckState>* selection) {
    int checkedCount = 0;
    for (auto it = ptr->children().begin(); it != ptr->children().end(); it++) {
        if (!(*it)->children().isEmpty()) _setCheckStatusTopDown(*it, selection);

        if (selection->contains(*it)) checkedCount++;
    }

    if (checkedCount == 0)
        selection->remove(ptr);
    else if (checkedCount == ptr->children().count())
        selection->insert(ptr, Qt::Checked);
    else
        selection->insert(ptr, Qt::PartiallyChecked);
}

TreeNode<UnicodeGroup>* GroupsTreeModel::_filterTree(QString query, TreeNode<UnicodeGroup>* ptr) {
    if (ptr->children().isEmpty()) {
        if (ptr->data().name.contains(query, Qt::CaseInsensitive)) {
            auto node = new TreeNode<UnicodeGroup>(nullptr, ptr);
            node->setData(ptr->data());

            if (m_treeSelection.value(ptr, Qt::Unchecked) == Qt::Checked)
                m_filteredSelection.insert(node, Qt::Checked);

            return node;
        } else
            return nullptr;
    }
    TreeNode<UnicodeGroup>* node = new TreeNode<UnicodeGroup>(nullptr, ptr);
    for (auto it = ptr->children().begin(); it != ptr->children().end(); it++) {
        TreeNode<UnicodeGroup>* result = _filterTree(query, *it);
        if (result) node->appendNode(result);
    }

    if (node->children().count()) {
        node->setData(ptr->data());
        return node;
    } else {
        delete node;
        return nullptr;
    }
}

void GroupsTreeModel::filterItems(const QString& query) {
    beginResetModel();
    if (query.length() == 0) {
        if (m_tree != &m_groups) {
            delete m_tree;
            m_tree = &m_groups;
            m_selection = &m_treeSelection;  // restore original selection
        }
    } else {
        // reset previous selection tree
        m_filteredSelection.clear();
        auto ptr = _filterTree(query, &m_groups);
        if (ptr) {
            if (m_tree != &m_groups) delete m_tree;       // delete previous search results
            // selection tree has been filtered too.
            m_selection = &m_filteredSelection;
            _setCheckStatusTopDown(ptr, &m_filteredSelection);

            m_tree = ptr;
        } else
            m_tree = new TreeNode<UnicodeGroup>;
    }
    endResetModel();
    emit itemsFiltered();
}

void GroupsTreeModel::itemExpanded(const QModelIndex& i) {
    TreeNode<UnicodeGroup>* ptr = static_cast<TreeNode<UnicodeGroup>*>(i.internalPointer());
    m_expandedItems.insert(ptr->reference());
}

void GroupsTreeModel::itemCollapsed(const QModelIndex& i) {
    TreeNode<UnicodeGroup>* ptr = static_cast<TreeNode<UnicodeGroup>*>(i.internalPointer());
    m_expandedItems.remove(ptr->reference());
}
