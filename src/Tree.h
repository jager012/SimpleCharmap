#pragma once

#include <QList>

template <class T>
class TreeNode {
    public:
        TreeNode(TreeNode* p = nullptr, TreeNode* ref = nullptr) : m_parent(p) {
            if (ref == nullptr)
                m_reference = this;
            else
                m_reference = ref;
        };

        TreeNode(TreeNode* p, const T& d, TreeNode* ref = nullptr) : m_parent(p), m_data(d) {
            if (ref == nullptr)
                m_reference = this;
            else
                m_reference = ref;
        };

        ~TreeNode() {
            for (auto it = m_children.begin(); it != m_children.end(); it++) {
                delete *it;
            }
        };

        TreeNode* appendChild(const T& c) {
            TreeNode* pc = new TreeNode(this, c);
            m_children.append(pc);
            return pc;
        };

        TreeNode* appendNode(TreeNode* n) {
            n->m_parent = this;
            m_children.append(n);
            return n;
        }

        void removeChild(int index) { m_children.remove(index); };

        bool isRoot() const { return !m_parent; }
        TreeNode<T>* root() const {
            if (m_parent->isRoot())
                return this;
            else
                return m_parent.root();
        }

        TreeNode* parent() const { return m_parent; };
        const QList<TreeNode*>& children() const { return m_children; };
        const QList<TreeNode*>& siblings() const { return m_parent->m_children; };

        const T& data() const { return m_data; }
        void setData(const T& d) { m_data = d; }

        // needed to keep selection tracking when a filtered tree is displayed
        // each node of the filtered tree should contain a reference to the corresponding
        // pointer inside the original tree that will be used for selection lookup.
        // original tree nodes will have a reference to themselves.
        TreeNode<T>* reference() { return m_reference; }
        void setReference(TreeNode<T>* ref) { m_reference = ref; };

    private:
        TreeNode<T>* m_parent;
        QList<TreeNode*> m_children;
        TreeNode<T>* m_reference;
        T m_data;
};

template <class T>
using Tree = TreeNode<T>;
