#include "Value.h"

namespace Ethereal {
ValueBase::CreatorMap ValueBase::sCreatorMap;

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
ValueBasePtr ValueBase::CreateValueFromString(const string& value, const string& type) {
    CreatorMap::iterator it = sCreatorMap.find(type);
    if (it != sCreatorMap.end()) return it->second(value);

    return Value<string>::CreateFromString(value);
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
        }
    }
    ~ValueRegistry() {}
};

//
// Template instantiations
//

#define INSTANTIATE_TYPE(T, name, dv)                    \
    template <>                                          \
    const string Value<T>::sType = name;                 \
    const string Value<T>::sDefaultValue = dv;           \
    template <>                                          \
    string Value<T>::GetValueString() const {            \
        return ToValueString<T>(mData);                  \
    }                                                    \
    template bool ValueBase::Is<T>() const;              \
    template T* ValueBase::GetPtr<T>();                  \
    template void ValueBase::SetData<T>(const T& value); \
    template T ValueBase::GetData<T>();                  \
    template const string& GetTypeString<T>();           \
    template string ToValueString(const T& data);        \
    template T FromValueString(const string& value);     \
    ValueRegistry<T> registry##T;

// Base types
INSTANTIATE_TYPE(int, "integer", "0")
INSTANTIATE_TYPE(bool, "boolean", "false")
INSTANTIATE_TYPE(float, "float", "0.0")
INSTANTIATE_TYPE(Vector2, "float2", "0.0, 0.0")
INSTANTIATE_TYPE(Vector3, "float3", "0.0, 0.0, 0.0")
INSTANTIATE_TYPE(Vector4, "float4", "0.0, 0.0, 0.0, 0.0")
INSTANTIATE_TYPE(Color3, "color3", "1.0, 1.0, 1.0")
INSTANTIATE_TYPE(Color4, "color4", "1.0, 1.0, 1.0, 1.0")
INSTANTIATE_TYPE(string, "string", "")

// Alias types
INSTANTIATE_TYPE(long, "integer", "0")
INSTANTIATE_TYPE(double, "float", "0.0")
}  // namespace Ethereal