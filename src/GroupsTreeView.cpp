#include "GroupsTreeView.h"

#include "GroupsTreeModel.h"

GroupsTreeView::GroupsTreeView(QWidget* parent) : QTreeView(parent) {}

GroupsTreeView::~GroupsTreeView() {}

void GroupsTreeView::setModel(QAbstractItemModel* m) {
    if (GroupsTreeModel* oldModel = dynamic_cast<GroupsTreeModel*>(model())) {
        disconnect(oldModel, &GroupsTreeModel::modelReset, this,
                   &GroupsTreeView::restoreExpandedItems);
        disconnect(this, &QTreeView::clicked, oldModel, &GroupsTreeModel::toggleItemCheck);
    }

    if (GroupsTreeModel* newModel = dynamic_cast<GroupsTreeModel*>(m)) {
        connect(newModel, &GroupsTreeModel::itemsFiltered, this, &GroupsTreeView::restoreExpandedItems);
        connect(this, &QTreeView::expanded, newModel, &GroupsTreeModel::itemExpanded);
        connect(this, &QTreeView::collapsed, newModel, &GroupsTreeModel::itemCollapsed);
        connect(this, &QTreeView::clicked, newModel, &GroupsTreeModel::toggleItemCheck);
    }

    QTreeView::setModel(m);
}

void GroupsTreeView::restoreExpandedItems() {
    QModelIndexList expanded = model()->match(model()->index(0,0), GroupsTreeModel::ExpandedRole, true, -1,
                                              Qt::MatchRecursive | Qt::MatchWrap | Qt::MatchExactly);
    for (auto it = expanded.begin(); it != expanded.end(); it++) {
        expand(*it);
    }
}
