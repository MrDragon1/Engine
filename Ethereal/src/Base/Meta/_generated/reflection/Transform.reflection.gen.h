#pragma once
#include "Base/Math/Transform.h"

namespace Ethereal {
class Transform;
namespace Reflection {
namespace TypeFieldReflectionOparator {
class TypeTransformOperator {
   public:
    static const char* getClassName() { return "Transform"; }
    static void* constructorWithYNode(const YNode& yaml_context) {
        Transform* ret_instance = new Transform;
        Serializer::read(yaml_context, *ret_instance);
        return ret_instance;
    }
    static YNode writeByName(void* instance) { return Serializer::write(*(Transform*)instance); }
    // base class
    static int getTransformBaseClassReflectionInstanceList(ReflectionInstance*& out_list, void* instance) {
        int count = 0;

        return count;
    }
    // fields
    static const char* getFieldName_m_position() { return "m_position"; }
    static const char* getFieldTypeName_m_position() { return "Vector3"; }
    static void set_m_position(void* instance, void* field_value) {
        static_cast<Transform*>(instance)->mPosition = *static_cast<Vector3*>(field_value);
    }
    static void* get_m_position(void* instance) { return static_cast<void*>(&(static_cast<Transform*>(instance)->mPosition)); }
    static bool isArray_m_position() { return false; }
    static const char* getFieldName_m_scale() { return "m_scale"; }
    static const char* getFieldTypeName_m_scale() { return "Vector3"; }
    static void set_m_scale(void* instance, void* field_value) { static_cast<Transform*>(instance)->mScale = *static_cast<Vector3*>(field_value); }
    static void* get_m_scale(void* instance) { return static_cast<void*>(&(static_cast<Transform*>(instance)->mScale)); }
    static bool isArray_m_scale() { return false; }
    static const char* getFieldName_m_rotation() { return "m_rotation"; }
    static const char* getFieldTypeName_m_rotation() { return "Quaternion"; }
    static void set_m_rotation(void* instance, void* field_value) {
        static_cast<Transform*>(instance)->mRotation = *static_cast<Quaternion*>(field_value);
    }
    static void* get_m_rotation(void* instance) { return static_cast<void*>(&(static_cast<Transform*>(instance)->mRotation)); }
    static bool isArray_m_rotation() { return false; }
};
}  // namespace TypeFieldReflectionOparator

void TypeWrapperRegister_Transform() {
    FieldFunctionTuple* f_field_function_tuple_m_position = new FieldFunctionTuple(
        &TypeFieldReflectionOparator::TypeTransformOperator::set_m_position, &TypeFieldReflectionOparator::TypeTransformOperator::get_m_position,
        &TypeFieldReflectionOparator::TypeTransformOperator::getClassName,
        &TypeFieldReflectionOparator::TypeTransformOperator::getFieldName_m_position,
        &TypeFieldReflectionOparator::TypeTransformOperator::getFieldTypeName_m_position,
        &TypeFieldReflectionOparator::TypeTransformOperator::isArray_m_position);
    REGISTER_FIELD_TO_MAP("Transform", f_field_function_tuple_m_position);
    FieldFunctionTuple* f_field_function_tuple_m_scale = new FieldFunctionTuple(
        &TypeFieldReflectionOparator::TypeTransformOperator::set_m_scale, &TypeFieldReflectionOparator::TypeTransformOperator::get_m_scale,
        &TypeFieldReflectionOparator::TypeTransformOperator::getClassName, &TypeFieldReflectionOparator::TypeTransformOperator::getFieldName_m_scale,
        &TypeFieldReflectionOparator::TypeTransformOperator::getFieldTypeName_m_scale,
        &TypeFieldReflectionOparator::TypeTransformOperator::isArray_m_scale);
    REGISTER_FIELD_TO_MAP("Transform", f_field_function_tuple_m_scale);
    FieldFunctionTuple* f_field_function_tuple_m_rotation = new FieldFunctionTuple(
        &TypeFieldReflectionOparator::TypeTransformOperator::set_m_rotation, &TypeFieldReflectionOparator::TypeTransformOperator::get_m_rotation,
        &TypeFieldReflectionOparator::TypeTransformOperator::getClassName,
        &TypeFieldReflectionOparator::TypeTransformOperator::getFieldName_m_rotation,
        &TypeFieldReflectionOparator::TypeTransformOperator::getFieldTypeName_m_rotation,
        &TypeFieldReflectionOparator::TypeTransformOperator::isArray_m_rotation);
    REGISTER_FIELD_TO_MAP("Transform", f_field_function_tuple_m_rotation);

    ClassFunctionTuple* f_class_function_tuple_Transform = new ClassFunctionTuple(
        &TypeFieldReflectionOparator::TypeTransformOperator::getTransformBaseClassReflectionInstanceList,
        &TypeFieldReflectionOparator::TypeTransformOperator::constructorWithYNode, &TypeFieldReflectionOparator::TypeTransformOperator::writeByName);
    REGISTER_BASE_CLASS_TO_MAP("Transform", f_class_function_tuple_Transform);
}
namespace TypeWrappersRegister {
void Transform() { TypeWrapperRegister_Transform(); }
}  // namespace TypeWrappersRegister

}  // namespace Reflection
}  // namespace Ethereal
