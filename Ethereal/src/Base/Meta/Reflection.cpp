#include "Reflection.h"
#include <cstring>
#include <map>

namespace Ethereal {
namespace Reflection {
const char* k_unknown_type = "UnknownType";
const char* k_unknown = "Unknown";

static std::map<std::string, ClassFunctionTuple*> mclass_map;
static std::multimap<std::string, FieldFunctionTuple*> mfield_map;
static std::map<std::string, ArrayFunctionTuple*> marray_map;

void TypeMetaRegisterinterface::registerToFieldMap(const char* name, FieldFunctionTuple* value) { mfield_map.insert(std::make_pair(name, value)); }

void TypeMetaRegisterinterface::registerToArrayMap(const char* name, ArrayFunctionTuple* value) {
    if (marray_map.find(name) == marray_map.end()) {
        marray_map.insert(std::make_pair(name, value));
    } else {
        delete value;
    }
}

void TypeMetaRegisterinterface::registerToClassMap(const char* name, ClassFunctionTuple* value) {
    if (mclass_map.find(name) == mclass_map.end()) {
        mclass_map.insert(std::make_pair(name, value));
    } else {
        delete value;
    }
}

void TypeMetaRegisterinterface::unregisterAll() {
    for (const auto& itr : mfield_map) {
        delete itr.second;
    }
    mfield_map.clear();
    for (const auto& itr : mclass_map) {
        delete itr.second;
    }
    mclass_map.clear();
    for (const auto& itr : marray_map) {
        delete itr.second;
    }
    marray_map.clear();
}

TypeMeta::TypeMeta(std::string type_name) : mTypeName(type_name) {
    mis_valid = false;
    mfields.clear();

    auto fileds_iter = mfield_map.equal_range(type_name);
    while (fileds_iter.first != fileds_iter.second) {
        FieldAccessor f_field(fileds_iter.first->second);
        mfields.emplace_back(f_field);
        mis_valid = true;

        ++fileds_iter.first;
    }
}

TypeMeta::TypeMeta() : mTypeName(k_unknown_type), mis_valid(false) { mfields.clear(); }

TypeMeta TypeMeta::newMetaFromName(std::string type_name) {
    TypeMeta f_type(type_name);
    return f_type;
}

bool TypeMeta::newArrayAccessorFromName(std::string array_type_name, ArrayAccessor& accessor) {
    auto iter = marray_map.find(array_type_name);

    if (iter != marray_map.end()) {
        ArrayAccessor new_accessor(iter->second);
        accessor = new_accessor;
        return true;
    }

    return false;
}

ReflectionInstance TypeMeta::newFromNameAndYAML(std::string type_name, const YNode& yaml_context) {
    auto iter = mclass_map.find(type_name);

    if (iter != mclass_map.end()) {
        return {TypeMeta(type_name), (std::get<1>(*iter->second)(yaml_context))};
    }
    return {};
}

YNode TypeMeta::writeByName(std::string type_name, void* instance) {
    auto iter = mclass_map.find(type_name);

    if (iter != mclass_map.end()) {
        return std::get<2>(*iter->second)(instance);
    }
    return YNode();
}

std::string TypeMeta::getTypeName() { return mTypeName; }

int TypeMeta::getFieldsList(FieldAccessor*& out_list) {
    int count = mfields.size();
    out_list = new FieldAccessor[count];
    for (int i = 0; i < count; ++i) {
        out_list[i] = mfields[i];
    }
    return count;
}

int TypeMeta::getBaseClassReflectionInstanceList(ReflectionInstance*& out_list, void* instance) {
    auto iter = mclass_map.find(mTypeName);

    if (iter != mclass_map.end()) {
        return (std::get<0>(*iter->second))(out_list, instance);
    }

    return 0;
}

FieldAccessor TypeMeta::getFieldByName(const char* name) {
    const auto it = std::find_if(mfields.begin(), mfields.end(), [&](const auto& i) { return std::strcmp(i.getFieldName(), name) == 0; });
    if (it != mfields.end()) return *it;
    return FieldAccessor(nullptr);
}

TypeMeta& TypeMeta::operator=(const TypeMeta& dest) {
    if (this == &dest) {
        return *this;
    }
    mfields.clear();
    mfields = dest.mfields;

    mTypeName = dest.mTypeName;
    mis_valid = dest.mis_valid;

    return *this;
}
FieldAccessor::FieldAccessor() {
    mfield_type_name = k_unknown_type;
    mfield_name = k_unknown;
    mFunctions = nullptr;
}

FieldAccessor::FieldAccessor(FieldFunctionTuple* functions) : mFunctions(functions) {
    mfield_type_name = k_unknown_type;
    mfield_name = k_unknown;
    if (mFunctions == nullptr) {
        return;
    }

    mfield_type_name = (std::get<4>(*mFunctions))();
    mfield_name = (std::get<3>(*mFunctions))();
}

void* FieldAccessor::get(void* instance) {
    // todo: should check validation
    return static_cast<void*>((std::get<1>(*mFunctions))(instance));
}

void FieldAccessor::set(void* instance, void* value) {
    // todo: should check validation
    (std::get<0>(*mFunctions))(instance, value);
}

TypeMeta FieldAccessor::getOwnerTypeMeta() {
    // todo: should check validation
    TypeMeta f_type((std::get<2>(*mFunctions))());
    return f_type;
}

bool FieldAccessor::getTypeMeta(TypeMeta& field_type) {
    TypeMeta f_type(mfield_type_name);
    field_type = f_type;
    return f_type.mis_valid;
}

const char* FieldAccessor::getFieldName() const { return mfield_name; }
const char* FieldAccessor::getFieldTypeName() { return mfield_type_name; }

bool FieldAccessor::isArrayType() {
    // todo: should check validation
    return (std::get<5>(*mFunctions))();
}

FieldAccessor& FieldAccessor::operator=(const FieldAccessor& dest) {
    if (this == &dest) {
        return *this;
    }
    mFunctions = dest.mFunctions;
    mfield_name = dest.mfield_name;
    mfield_type_name = dest.mfield_type_name;
    return *this;
}

ArrayAccessor::ArrayAccessor() : mFunc(nullptr), mArrayTypeName("UnKnownType"), mElementTypeName("UnKnownType") {}

ArrayAccessor::ArrayAccessor(ArrayFunctionTuple* array_func) : mFunc(array_func) {
    mArrayTypeName = k_unknown_type;
    mElementTypeName = k_unknown_type;
    if (mFunc == nullptr) {
        return;
    }

    mArrayTypeName = std::get<3>(*mFunc)();
    mElementTypeName = std::get<4>(*mFunc)();
}
const char* ArrayAccessor::getArrayTypeName() { return mArrayTypeName; }
const char* ArrayAccessor::getElementTypeName() { return mElementTypeName; }
void ArrayAccessor::set(int index, void* instance, void* element_value) {
    // todo: should check validation
    size_t count = getSize(instance);
    // todo: should check validation(index < count)
    std::get<0> (*mFunc)(index, instance, element_value);
}

void* ArrayAccessor::get(int index, void* instance) {
    // todo: should check validation
    size_t count = getSize(instance);
    // todo: should check validation(index < count)
    return std::get<1>(*mFunc)(index, instance);
}

int ArrayAccessor::getSize(void* instance) {
    // todo: should check validation
    return std::get<2>(*mFunc)(instance);
}

ArrayAccessor& ArrayAccessor::operator=(ArrayAccessor& dest) {
    if (this == &dest) {
        return *this;
    }
    mFunc = dest.mFunc;
    mArrayTypeName = dest.mArrayTypeName;
    mElementTypeName = dest.mElementTypeName;
    return *this;
}

ReflectionInstance& ReflectionInstance::operator=(ReflectionInstance& dest) {
    if (this == &dest) {
        return *this;
    }
    mInstance = dest.mInstance;
    mMeta = dest.mMeta;

    return *this;
}

ReflectionInstance& ReflectionInstance::operator=(ReflectionInstance&& dest) {
    if (this == &dest) {
        return *this;
    }
    mInstance = dest.mInstance;
    mMeta = dest.mMeta;

    return *this;
}
}  // namespace Reflection
}  // namespace Ethereal