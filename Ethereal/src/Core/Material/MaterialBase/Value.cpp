#include "Value.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
ValueBase::CreatorMap ValueBase::sCreatorMap;
std::unordered_map<string, ValueProperty> ValueBase::sValueMap;

template <class T>
void Save(const T& data, string& value) {}
template <class T>
void Load(const string& value, T& data) {}

template <>
void Save<string>(const string& data, string& value) {
    value = data;
}
template <>
void Load<string>(const string& value, string& data) {
    data = value;
}

template <>
void Save<int>(const int& data, string& value) {
    value = std::to_string(data);
}
template <>
void Load<int>(const string& value, int& data) {
    data = std::stoi(value);
}

template <>
void Save<float>(const float& data, string& value) {
    value = std::to_string(data);
}
template <>
void Load<float>(const string& value, float& data) {
    data = std::stof(value);
}

template <>
void Save<double>(const double& data, string& value) {
    value = std::to_string(data);
}
template <>
void Load<double>(const string& value, double& data) {
    data = std::stod(value);
}

template <>
void Save<bool>(const bool& data, string& value) {
    value = data ? "true" : "false";
}
template <>
void Load<bool>(const string& value, bool& data) {
    data = value == "true";
}

template <>
void Save<Vector2>(const Vector2& data, string& value) {
    value = std::to_string(data.x) + ", " + std::to_string(data.y);
}
template <>
void Load<Vector2>(const string& value, Vector2& data) {
    sscanf(value.c_str(), "%f, %f", &data.x, &data.y);
}

template <>
void Save<Vector3>(const Vector3& data, string& value) {
    value = std::to_string(data.x) + ", " + std::to_string(data.y) + ", " + std::to_string(data.z);
}
template <>
void Load<Vector3>(const string& value, Vector3& data) {
    sscanf(value.c_str(), "%f, %f, %f", &data.x, &data.y, &data.z);
}

template <>
void Save<Vector4>(const Vector4& data, string& value) {
    value = std::to_string(data.x) + ", " + std::to_string(data.y) + ", " + std::to_string(data.z) +
            ", " + std::to_string(data.w);
}
template <>
void Load<Vector4>(const string& value, Vector4& data) {
    sscanf(value.c_str(), "%f, %f, %f, %f", &data.x, &data.y, &data.z, &data.w);
}

template <>
void Save<Color3>(const Color3& data, string& value) {
    value = std::to_string(data.x) + ", " + std::to_string(data.y) + ", " + std::to_string(data.z);
}
template <>
void Load<Color3>(const string& value, Color3& data) {
    sscanf(value.c_str(), "%f, %f, %f", &data.x, &data.y, &data.z);
}

template <>
void Save<Color4>(const Color4& data, string& value) {
    value = std::to_string(data.x) + ", " + std::to_string(data.y) + ", " + std::to_string(data.z) +
            ", " + std::to_string(data.w);
}
template <>
void Load<Color4>(const string& value, Color4& data) {
    sscanf(value.c_str(), "%f, %f, %f, %f", &data.x, &data.y, &data.z, &data.w);
}

template <>
void Save<Matrix3>(const Matrix3& data, string& value) {
    value = "";
    string comma = "";
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            value += comma + std::to_string(data[i][j]);
            comma = ", ";
        }
    }
}
template <>
void Load<Matrix3>(const string& value, Matrix3& data) {
    sscanf(value.c_str(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", &data[0][0], &data[0][1],
           &data[0][2], &data[1][0], &data[1][1], &data[1][2], &data[2][0], &data[2][1],
           &data[2][2]);
}

template <>
void Save<Matrix4>(const Matrix4& data, string& value) {
    value = "";
    string comma = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            value += comma + std::to_string(data[i][j]);
            comma = ", ";
        }
    }
}
template <>
void Load<Matrix4>(const string& value, Matrix4& data) {
    sscanf(value.c_str(), "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f",
           &data[0][0], &data[0][1], &data[0][2], &data[0][3], &data[1][0], &data[1][1],
           &data[1][2], &data[1][3], &data[2][0], &data[2][1], &data[2][2], &data[2][3],
           &data[3][0], &data[3][1], &data[3][2], &data[3][3]);
}

template <>
void Save<BSDF>(const BSDF& data, string& value) {}
template <>
void Load<BSDF>(const string& value, BSDF& data) {}

template <>
void Save<surfaceshader>(const surfaceshader& data, string& value) {}
template <>
void Load<surfaceshader>(const string& value, surfaceshader& data) {}

template <>
void Save<material>(const material& data, string& value) {}
template <>
void Load<material>(const string& value, material& data) {}

ValueBasePtr ValueBase::CreateValueFromString(const string& value, const string& type) {
    CreatorMap::iterator it = sCreatorMap.find(type);
    if (it != sCreatorMap.end()) return it->second(value);

    return Value<string>::CreateFromString(value);
}

ValueBasePtr ValueBase::CreateValueWithType(const string& type) {
    CreatorMap::iterator it = sCreatorMap.find(type);
    if (it != sCreatorMap.end()) return it->second(sValueMap[type].DefaultValue);

    return Value<string>::CreateFromString(sValueMap[type].DefaultValue);
}

void ValueBase::EmitTypeDefine(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto [name, type] : sValueMap) {
            if (!type.Definition.empty()) {
                stage->EmitLine(type.Definition);
            }
        }
        stage->EmitLine();
    }
}

template <class T>
const string& GetTypeString() {
    return Value<T>::sType;
}

template <class T>
string ToValueString(const T& data) {
    string value;
    Save<T>(data, value);
    return value;
}

template <class T>
T FromValueString(const string& value) {
    T data;
    if (value.empty())
        Load<T>(Value<T>::sDefaultValue, data);
    else
        Load<T>(value, data);
    return data;
}

template <class T>
class ValueRegistry {
   public:
    ValueRegistry() {
        if (!ValueBase::sCreatorMap.count(Value<T>::sType)) {
            ValueBase::sCreatorMap[Value<T>::sType] = Value<T>::CreateFromString;
            ValueBase::sValueMap[Value<T>::sType] =
                ValueProperty{Value<T>::sDefaultValue, Value<T>::sSyntax,
                              Value<T>::sSyntaxInitilize, Value<T>::sDefinition};
        }
    }
    ~ValueRegistry() {}
};

//
// Template instantiations
//

#define INSTANTIATE_TYPE(T, name, dv, syntax, syntaxinit, def) \
    template <>                                                \
    const string Value<T>::sType = name;                       \
    const string Value<T>::sDefaultValue = dv;                 \
    const string Value<T>::sSyntax = syntax;                   \
    const string Value<T>::sSyntaxInitilize = syntaxinit;      \
    const string Value<T>::sDefinition = def;                  \
    template <>                                                \
    string Value<T>::GetValueString() const {                  \
        return ToValueString<T>(mData);                        \
    }                                                          \
    template <>                                                \
    string Value<T>::GetDefaultValueString() const {           \
        return Value<T>::sDefaultValue;                        \
    }                                                          \
    template <>                                                \
    string Value<T>::GetSyntaxString() const {                 \
        return Value<T>::sSyntax;                              \
    }                                                          \
    template <>                                                \
    string Value<T>::GetSyntaxInitilizeString() const {        \
        return Value<T>::sSyntaxInitilize;                     \
    }                                                          \
    template <>                                                \
    string Value<T>::GetDefinitionString() const {             \
        return Value<T>::sDefinition;                          \
    }                                                          \
    template bool ValueBase::Is<T>() const;                    \
    template T* ValueBase::GetPtr<T>();                        \
    template void ValueBase::SetData<T>(const T& value);       \
    template T ValueBase::GetData<T>();                        \
    template const string& GetTypeString<T>();                 \
    template string ToValueString(const T& data);              \
    template T FromValueString(const string& value);           \
    ValueRegistry<T> registry##T;

// Base types
INSTANTIATE_TYPE(int, "integer", "0", "int", "0", "")
INSTANTIATE_TYPE(bool, "boolean", "false", "bool", "false", "")
INSTANTIATE_TYPE(float, "float", "0.0", "float", "0.0", "")
INSTANTIATE_TYPE(Vector2, "float2", "0.0", "vec2", "0.0", "")
INSTANTIATE_TYPE(Vector3, "float3", "0.0, 0.0, 0.0", "vec3", "0.0", "")
INSTANTIATE_TYPE(Vector4, "float4", "0.0, 0.0, 0.0, 0.0", "vec4", "0.0", "")
INSTANTIATE_TYPE(Color3, "color3", "1.0, 1.0, 1.0", "vec3", "1.0", "")
INSTANTIATE_TYPE(Color4, "color4", "1.0, 1.0, 1.0, 1.0", "vec4", "1.0", "")
INSTANTIATE_TYPE(Matrix3, "matrix3", "1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0", "mat3", "1.0",
                 "")
INSTANTIATE_TYPE(Matrix4, "matrix4",
                 "1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0", "mat4", "1.0",
                 "")
INSTANTIATE_TYPE(string, "string", "", "", "", "")

INSTANTIATE_TYPE(BSDF, "bsdf", "", "BSDF", "vec3(0.0),vec3(1.0), 0.0, 0.0",
                 "struct BSDF { vec3 response; vec3 throughput; float thickness; float ior; };")
INSTANTIATE_TYPE(surfaceshader, "surfaceshader", "", "surfaceshader", "vec3(0.0),vec3(0.0)",
                 "struct surfaceshader { vec3 color; vec3 transparency; };")
INSTANTIATE_TYPE(material, "material", "", "material", "vec3(0.0),vec3(0.0)",
                 "#define material surfaceshader")

// Alias types
INSTANTIATE_TYPE(long, "integer", "0", "int", "0", "")
INSTANTIATE_TYPE(double, "float", "0.0", "float", "0.0", "")
}  // namespace Ethereal