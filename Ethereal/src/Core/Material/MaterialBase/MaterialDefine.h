#pragma once

#include "pch.h"
namespace Ethereal {

using std::map;
using std::string;
using std::unordered_map;
using std::vector;

using StringMap = std::unordered_map<string, string>;

using IntVec = vector<int>;
using BoolVec = vector<bool>;
using FloatVec = vector<float>;
using StringVec = vector<string>;

const string EMPTY_STRING = "";

const StringVec LibraryPath = {
    "assets/materials/lib/def.mtlx",
    "assets/materials/lib/impl.mtlx",
    "assets/materials/lib/pbrdef.mtlx",
};

namespace MaterialElementType {

const string ELEMENT = "element";
const string DOCUMENT = "materialx";
const string NODEGRAPH = "nodegraph";
const string NODEDEFINE = "nodedef";
const string NODEIMPL = "implementation";
const string NODEINSTANCE = "node";
const string INPUT = "input";
const string OUTPUT = "output";

}  // namespace MaterialElementType

namespace MaterialAttribute {

const string NAME = "name";
const string CONNECTOR = "connector";
const string PORT = "output";
const string NODEDEF = "nodedef";
const string TYPE = "type";

const string IMPLGRAPH = "implgraph";
const string SOURCECODE = "sourcecode";
const string FILE = "file";
const string FUNCTION = "function";

}  // namespace MaterialAttribute

}  // namespace Ethereal
