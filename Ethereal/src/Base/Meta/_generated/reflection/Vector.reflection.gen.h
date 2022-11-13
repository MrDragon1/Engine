#pragma once
#include "Base/Math/Vector.h"

namespace Ethereal
{
    class Vector2;
    namespace Reflection
    {
        namespace TypeFieldReflectionOparator
        {
            class TypeVector2Operator {
              public:
                static const char* getClassName() { return "Vector2"; }
                static void* constructorWithYNode(const YNode& yaml_context) {
                    Vector2* ret_instance = new Vector2;
                    Serializer::read(yaml_context, *ret_instance);
                    return ret_instance;
                }
                static YNode writeByName(void* instance) { return Serializer::write(*(Vector2*)instance); }
                // base class
                static int getVector2BaseClassReflectionInstanceList(ReflectionInstance*& out_list, void* instance) {
                    int count = 0;

                    return count;
                }
                // fields
                static const char* getFieldName_m_int() { return "m_int"; }
                static const char* getFieldTypeName_m_int() { return "int"; }
                static void set_m_int(void* instance, void* field_value) { static_cast<Vector2*>(instance)->m_int = *static_cast<int*>(field_value); }
                static void* get_m_int(void* instance) { return static_cast<void*>(&(static_cast<Vector2*>(instance)->m_int)); }
                static bool isArray_m_int() { return false; }
                static const char* getFieldName_m_int_vector() { return "m_int_vector"; }
                static const char* getFieldTypeName_m_int_vector() { return "std::vector<int*>"; }
                static void set_m_int_vector(void* instance, void* field_value) {
                    static_cast<Vector2*>(instance)->m_int_vector = *static_cast<std::vector<int*>*>(field_value);
                }
                static void* get_m_int_vector(void* instance) { return static_cast<void*>(&(static_cast<Vector2*>(instance)->m_int_vector)); }
                static bool isArray_m_int_vector() { return true; }
                static const char* getFieldName_m_double_vector() { return "m_double_vector"; }
                static const char* getFieldTypeName_m_double_vector() { return "std::vector<double>"; }
                static void set_m_double_vector(void* instance, void* field_value) {
                    static_cast<Vector2*>(instance)->m_double_vector = *static_cast<std::vector<double>*>(field_value);
                }
                static void* get_m_double_vector(void* instance) { return static_cast<void*>(&(static_cast<Vector2*>(instance)->m_double_vector)); }
                static bool isArray_m_double_vector() { return true; }
            };
        }  // namespace TypeFieldReflectionOparator
        namespace ArrayReflectionOperator
        {
#ifndef ArraystdSSvectorLdoubleROperatorMACRO
#define ArraystdSSvectorLdoubleROperatorMACRO
            class ArraystdSSvectorLdoubleROperator {
              public:
                static const char* getArrayTypeName() { return "std::vector<double>"; }
                static const char* getElementTypeName() { return "double"; }
                static int getSize(void* instance) {
                    // todo: should check validation
                    return static_cast<int>(static_cast<std::vector<double>*>(instance)->size());
                }
                static void* get(int index, void* instance) {
                    // todo: should check validation
                    return static_cast<void*>(&((*static_cast<std::vector<double>*>(instance))[index]));
                }
                static void set(int index, void* instance, void* element_value) {
                    // todo: should check validation
                    (*static_cast<std::vector<double>*>(instance))[index] = *static_cast<double*>(element_value);
                }
            };
#endif  // ArraystdSSvectorLdoubleROperator
#ifndef ArraystdSSvectorLintPROperatorMACRO
#define ArraystdSSvectorLintPROperatorMACRO
            class ArraystdSSvectorLintPROperator {
              public:
                static const char* getArrayTypeName() { return "std::vector<int*>"; }
                static const char* getElementTypeName() { return "int*"; }
                static int getSize(void* instance) {
                    // todo: should check validation
                    return static_cast<int>(static_cast<std::vector<int*>*>(instance)->size());
                }
                static void* get(int index, void* instance) {
                    // todo: should check validation
                    return static_cast<void*>(&((*static_cast<std::vector<int*>*>(instance))[index]));
                }
                static void set(int index, void* instance, void* element_value) {
                    // todo: should check validation
                    (*static_cast<std::vector<int*>*>(instance))[index] = *static_cast<int**>(element_value);
                }
            };
#endif     // ArraystdSSvectorLintPROperator
        }  // namespace ArrayReflectionOperator

        void TypeWrapperRegister_Vector2() {
            FieldFunctionTuple* f_field_function_tuple_m_int = new FieldFunctionTuple(
                &TypeFieldReflectionOparator::TypeVector2Operator::set_m_int, &TypeFieldReflectionOparator::TypeVector2Operator::get_m_int,
                &TypeFieldReflectionOparator::TypeVector2Operator::getClassName,
                &TypeFieldReflectionOparator::TypeVector2Operator::getFieldName_m_int,
                &TypeFieldReflectionOparator::TypeVector2Operator::getFieldTypeName_m_int,
                &TypeFieldReflectionOparator::TypeVector2Operator::isArray_m_int);
            REGISTER_FIELD_TO_MAP("Vector2", f_field_function_tuple_m_int);
            FieldFunctionTuple* f_field_function_tuple_m_int_vector = new FieldFunctionTuple(
                &TypeFieldReflectionOparator::TypeVector2Operator::set_m_int_vector,
                &TypeFieldReflectionOparator::TypeVector2Operator::get_m_int_vector, &TypeFieldReflectionOparator::TypeVector2Operator::getClassName,
                &TypeFieldReflectionOparator::TypeVector2Operator::getFieldName_m_int_vector,
                &TypeFieldReflectionOparator::TypeVector2Operator::getFieldTypeName_m_int_vector,
                &TypeFieldReflectionOparator::TypeVector2Operator::isArray_m_int_vector);
            REGISTER_FIELD_TO_MAP("Vector2", f_field_function_tuple_m_int_vector);
            FieldFunctionTuple* f_field_function_tuple_m_double_vector =
                new FieldFunctionTuple(&TypeFieldReflectionOparator::TypeVector2Operator::set_m_double_vector,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::get_m_double_vector,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::getClassName,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::getFieldName_m_double_vector,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::getFieldTypeName_m_double_vector,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::isArray_m_double_vector);
            REGISTER_FIELD_TO_MAP("Vector2", f_field_function_tuple_m_double_vector);

            ArrayFunctionTuple* f_array_tuple_stdSSvectorLdoubleR = new ArrayFunctionTuple(
                &ArrayReflectionOperator::ArraystdSSvectorLdoubleROperator::set, &ArrayReflectionOperator::ArraystdSSvectorLdoubleROperator::get,
                &ArrayReflectionOperator::ArraystdSSvectorLdoubleROperator::getSize,
                &ArrayReflectionOperator::ArraystdSSvectorLdoubleROperator::getArrayTypeName,
                &ArrayReflectionOperator::ArraystdSSvectorLdoubleROperator::getElementTypeName);
            REGISTER_ARRAY_TO_MAP("std::vector<double>", f_array_tuple_stdSSvectorLdoubleR);
            ArrayFunctionTuple* f_array_tuple_stdSSvectorLintPR = new ArrayFunctionTuple(
                &ArrayReflectionOperator::ArraystdSSvectorLintPROperator::set, &ArrayReflectionOperator::ArraystdSSvectorLintPROperator::get,
                &ArrayReflectionOperator::ArraystdSSvectorLintPROperator::getSize,
                &ArrayReflectionOperator::ArraystdSSvectorLintPROperator::getArrayTypeName,
                &ArrayReflectionOperator::ArraystdSSvectorLintPROperator::getElementTypeName);
            REGISTER_ARRAY_TO_MAP("std::vector<int*>", f_array_tuple_stdSSvectorLintPR);
            ClassFunctionTuple* f_class_function_tuple_Vector2 =
                new ClassFunctionTuple(&TypeFieldReflectionOparator::TypeVector2Operator::getVector2BaseClassReflectionInstanceList,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::constructorWithYNode,
                                       &TypeFieldReflectionOparator::TypeVector2Operator::writeByName);
            REGISTER_BASE_CLASS_TO_MAP("Vector2", f_class_function_tuple_Vector2);
        }
        namespace TypeWrappersRegister
        {
            void Vector2() { TypeWrapperRegister_Vector2(); }
        }  // namespace TypeWrappersRegister

    }  // namespace Reflection
}  // namespace Ethereal
