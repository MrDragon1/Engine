#include "XmlParser.h"

namespace Ethereal::Xml {

void LoadDocument(DocumentPtr doc, const string& path) { ReadFromXmlFile(doc, path); }

void ReadFromXmlFile(DocumentPtr doc, const string& filename) {
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_file(filename.c_str());
    if (!result) {
        ET_CORE_WARN("Error loading file: " + filename);
        return;
    }
    DocumentFromXml(doc, xmlDoc);
}

void LoadLibrary(DocumentPtr lib, const string& path) {
    DocumentPtr libDoc = DocumentPtr::Create(nullptr, "lib");
    ReadFromXmlFile(libDoc, path);
    lib->ImportLibrary(libDoc);
}

void LoadLibraries(DocumentPtr lib) {
    for (auto& path : LibraryPath) {
        LoadLibrary(lib, path);
    }
}

void ElementFromXml(const xml_node& xmlNode, ElementPtr elem) {
    // Store attributes in element.
    for (const xml_attribute& xmlAttr : xmlNode.attributes()) {
        if (xmlAttr.name() != MaterialAttribute::NAME) {
            elem->SetAttribute(xmlAttr.name(), xmlAttr.value());
        }
    }

    // Create child elements and recurse.
    for (const xml_node& xmlChild : xmlNode.children()) {
        string category = xmlChild.name();
        string name;
        for (const xml_attribute& xmlAttr : xmlChild.attributes()) {
            if (xmlAttr.name() == MaterialAttribute::NAME) {
                name = xmlAttr.value();
                break;
            }
        }
        ElementPtr child = elem->AddChildOfType(category, name);

        ElementFromXml(xmlChild, child);
    }
}

void DocumentFromXml(DocumentPtr doc, const xml_document& xmlDoc) {
    xml_node xmlRoot = xmlDoc.child(MaterialElementType::DOCUMENT.c_str());
    if (xmlRoot) {
        ElementFromXml(xmlRoot, doc);
    }
}

}  // namespace Ethereal::Xml
