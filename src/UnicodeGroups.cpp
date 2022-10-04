#include "UnicodeGroups.h"

bool UnicodeGroups::load(QIODevice* file) {
    QXmlStreamReader xml(file);

    if (xml.readNextStartElement()) {
        if (xml.name() == QStringLiteral("groups"))
            return xmlParseGroup(xml, this);
        else
            return false;
    }

    return false;
}

bool UnicodeGroups::xmlParseGroup(QXmlStreamReader& xml, TreeNode<UnicodeGroup>* root) {
    while (xml.readNextStartElement()) {
        if (xml.name() == QStringLiteral("group") &&
            xml.attributes().hasAttribute(QStringLiteral("name"))) {
            QStringView name = xml.attributes().value(QStringLiteral("name"));

            if (xml.attributes().hasAttribute(QStringLiteral("from")) &&
                xml.attributes().hasAttribute(QStringLiteral("to"))) {
                bool ok;

                quint32 from = xml.attributes().value(QStringLiteral("from")).toUInt(&ok, 16);
                if (!ok) return false;

                quint32 to = xml.attributes().value(QStringLiteral("to")).toUInt(&ok, 16);
                if (!ok) return false;

                TreeNode<UnicodeGroup>* ptr = root->appendChild({name.toString(), from, to});
                // create a hashtable to quickly retrieve group nodes from the values saved inside an INI
                // file
                m_lookup.insert(from, ptr);
                xml.skipCurrentElement();
            } else {
                TreeNode<UnicodeGroup>* child = root->appendChild({name.toString(), 0, 0});
                xmlParseGroup(xml, child);
            }

            m_size++;
        } else
            return false;
    }

    return true;
}

TreeNode<UnicodeGroup>* UnicodeGroups::lookupGroup(quint32 start) const {
    return m_lookup.value(start);
}

qsizetype UnicodeGroups::size() const {
    return m_size;
}
