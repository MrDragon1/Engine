#pragma once

#include "pch.h"
namespace Ethereal {

struct BSDF {
    Vector3 response;
    Vector3 throughput;
    float thickness;
    float ior;
};

struct surfaceshader {
    Vector3 color;
    Vector3 transparency;
};
struct material : public surfaceshader {};

const string EMPTY_STRING = "";

const StringVec LibraryPath = {
    "assets/materials/lib/def.mtlx",         "assets/materials/lib/impl.mtlx",
    "assets/materials/lib/pbrdef.mtlx",      "assets/materials/lib/pbrlib.mtlx",
    "assets/materials/lib/pbrlib_impl.mtlx",
};

namespace Stage {
const string VERTEX = "vertex";
const string PIXEL = "pixel";
}  // namespace Stage

namespace ShaderBuildInVariable {
// variable block name
const string OUTPUT = "output";
const string INPUT = "input";
const string VERTEXDATA = "vertexData";
const string VSPUBUNIFORM = "vspubuniform";
const string PSPUBUNIFORM = "pspubuniform";
const string PRVUNIFORM = "prvuniform";

/// Shader build in variable
const string IN_POSITION = "i_position";
const string IN_NORMAL = "i_normal";
const string IN_TANGENT = "i_tangent";
const string IN_BITANGENT = "i_bitangent";
const string IN_TEXCOORD = "i_texcoord";
const string IN_GEOMPROP = "i_geomprop";
const string IN_COLOR = "i_color";
const string POSITION_WORLD = "positionWorld";
const string NORMAL_WORLD = "normalWorld";
const string TANGENT_WORLD = "tangentWorld";
const string BITANGENT_WORLD = "bitangentWorld";
const string POSITION_OBJECT = "positionObject";
const string NORMAL_OBJECT = "normalObject";
const string TANGENT_OBJECT = "tangentObject";
const string BITANGENT_OBJECT = "bitangentObject";
const string TEXCOORD = "texcoord";
const string COLOR = "color";
const string MODEL_MATRIX = "u_modelMatrix";
const string MODEL_INVERSE_MATRIX = "u_modelInverseMatrix";
const string MODEL_TRANSPOSE_MATRIX = "u_modelTransposeMatrix";
const string MODEL_INVERSE_TRANSPOSE_MATRIX = "u_modelInverseTransposeMatrix";
const string VIEW_MATRIX = "u_viewMatrix";
const string VIEW_INVERSE_MATRIX = "u_viewInverseMatrix";
const string VIEW_TRANSPOSE_MATRIX = "u_viewTransposeMatrix";
const string VIEW_INVERSE_TRANSPOSE_MATRIX = "u_viewInverseTransposeMatrix";
const string PROJ_MATRIX = "u_projectionMatrix";
const string PROJ_INVERSE_MATRIX = "u_projectionInverseMatrix";
const string PROJ_TRANSPOSE_MATRIX = "u_projectionTransposeMatrix";
const string PROJ_INVERSE_TRANSPOSE_MATRIX = "u_projectionInverseTransposeMatrix";
const string WORLD_VIEW_MATRIX = "u_worldViewMatrix";
const string VIEW_PROJECTION_MATRIX = "u_viewProjectionMatrix";
const string WORLD_VIEW_PROJECTION_MATRIX = "u_worldViewProjectionMatrix";
const string VIEW_POSITION = "u_viewPosition";
const string LIGHT_POSITION = "u_lightPosition";  // Temp
const string FRAME = "u_frame";
const string TIME = "u_time";
}  // namespace ShaderBuildInVariable

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
const string VALUE = "value";
const string NODEGROUP = "nodegroup";

const string IMPLGRAPH = "implgraph";
const string SOURCECODE = "sourcecode";
const string FILE = "file";
const string FUNCTION = "function";
const string CHANNELS = "channels";
const string UNIFORM = "uniform";
const string GEOM = "geom";
}  // namespace MaterialAttribute

}  // namespace Ethereal
