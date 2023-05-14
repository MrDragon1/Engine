#pragma once

#include <pugixml.hpp>
#include "Core/Material/MaterialBase/MaterialDefine.h"
#include "Core/Material/MaterialBase/Document.h"
using namespace pugi;
using namespace Ethereal;

namespace Ethereal::Xml {
void LoadDocument(DocumentPtr doc, const string& path);

void ReadFromXmlFile(DocumentPtr doc, const string& filename);

void LoadLibrary(DocumentPtr lib, const string& path);

void LoadLibraries(DocumentPtr lib);

void ElementFromXml(const xml_node& xmlNode, ElementPtr elem);

void DocumentFromXml(DocumentPtr doc, const xml_document& xmlDoc);

}  // namespace Ethereal::Xml