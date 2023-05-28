#include "MaterialManager.h"
#include "Utils/XmlParser.h"
namespace Ethereal {
DocumentPtr MaterialManager::mLibrary = nullptr;
MaterialCorePtr MaterialManager::LoadMaterial(const string& path) {
    if (!mLibrary) LoadLibrary();

    MaterialCorePtr mat = MaterialCorePtr::Create();
    DocumentPtr Doc = DocumentPtr::Create(nullptr, "doc1");
    Xml::LoadDocument(Doc, path);
    Doc->ImportLibrary(mLibrary);
    Doc->Validate();

    mat->SetDocument(Doc);
    mat->Compile();
    return mat;
}

void MaterialManager::LoadLibrary() {
    mLibrary = DocumentPtr::Create(nullptr, "lib");
    for (auto& path : LibraryPath) {
        DocumentPtr libDoc = DocumentPtr::Create(nullptr, "sublib");
        Xml::ReadFromXmlFile(libDoc, path);
        mLibrary->ImportLibrary(libDoc);
    }
}

}  // namespace Ethereal
