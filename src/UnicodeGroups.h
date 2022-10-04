#pragma once

#include <QHash>
#include <QIODevice>
#include <QXmlStreamReader>

#include "Tree.h"

struct UnicodeGroup {
        QString name = nullptr;
        quint32 from = 0;
        quint32 to = 0;
};

class UnicodeGroups : public Tree<UnicodeGroup> {
    public:
        UnicodeGroups() : m_size(1) {}; //accounting for root node

        bool load(QIODevice* file);
        // use internal lookup hashtable to lookup a group address basing on its starting point
        TreeNode<UnicodeGroup>* lookupGroup(quint32 start) const;

        qsizetype size() const;

    private:
        bool xmlParseGroup(QXmlStreamReader& xml, TreeNode<UnicodeGroup>* root);

        QHash<quint32, TreeNode<UnicodeGroup>*> m_lookup;
        qsizetype m_size;
};
