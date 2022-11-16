#pragma once
#include "Base/Math/Matrix.h"

namespace Ethereal{
    class Matrix3;
    class Matrix4;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeMatrix3Operator{
    public:
        static const char* getClassName(){ return "Matrix3";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Matrix3* ret_instance= new Matrix3;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Matrix3*)instance);
        }
        // base class
        static int getMatrix3BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_value0(){ return "value0";}
        static const char* getFieldTypeName_value0(){ return "Vector3";}
        static void set_value0(void* instance, void* field_value){ static_cast<Matrix3*>(instance)->value0 = *static_cast<Vector3*>(field_value);}
        static void* get_value0(void* instance){ return static_cast<void*>(&(static_cast<Matrix3*>(instance)->value0));}
        static bool isArray_value0(){ return false; }
        static const char* getFieldName_value1(){ return "value1";}
        static const char* getFieldTypeName_value1(){ return "Vector3";}
        static void set_value1(void* instance, void* field_value){ static_cast<Matrix3*>(instance)->value1 = *static_cast<Vector3*>(field_value);}
        static void* get_value1(void* instance){ return static_cast<void*>(&(static_cast<Matrix3*>(instance)->value1));}
        static bool isArray_value1(){ return false; }
        static const char* getFieldName_value2(){ return "value2";}
        static const char* getFieldTypeName_value2(){ return "Vector3";}
        static void set_value2(void* instance, void* field_value){ static_cast<Matrix3*>(instance)->value2 = *static_cast<Vector3*>(field_value);}
        static void* get_value2(void* instance){ return static_cast<void*>(&(static_cast<Matrix3*>(instance)->value2));}
        static bool isArray_value2(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Matrix3(){
        FieldFunctionTuple* f_field_function_tuple_value0=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix3Operator::set_value0,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::get_value0,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getFieldName_value0,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getFieldTypeName_value0,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::isArray_value0);
        REGISTER_FIELD_TO_MAP("Matrix3", f_field_function_tuple_value0);
        FieldFunctionTuple* f_field_function_tuple_value1=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix3Operator::set_value1,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::get_value1,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getFieldName_value1,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getFieldTypeName_value1,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::isArray_value1);
        REGISTER_FIELD_TO_MAP("Matrix3", f_field_function_tuple_value1);
        FieldFunctionTuple* f_field_function_tuple_value2=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix3Operator::set_value2,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::get_value2,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getFieldName_value2,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getFieldTypeName_value2,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::isArray_value2);
        REGISTER_FIELD_TO_MAP("Matrix3", f_field_function_tuple_value2);
        
        
        ClassFunctionTuple* f_class_function_tuple_Matrix3=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix3Operator::getMatrix3BaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeMatrix3Operator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Matrix3", f_class_function_tuple_Matrix3);
    }namespace TypeFieldReflectionOparator{
    class TypeMatrix4Operator{
    public:
        static const char* getClassName(){ return "Matrix4";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Matrix4* ret_instance= new Matrix4;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Matrix4*)instance);
        }
        // base class
        static int getMatrix4BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_value0(){ return "value0";}
        static const char* getFieldTypeName_value0(){ return "Vector4";}
        static void set_value0(void* instance, void* field_value){ static_cast<Matrix4*>(instance)->value0 = *static_cast<Vector4*>(field_value);}
        static void* get_value0(void* instance){ return static_cast<void*>(&(static_cast<Matrix4*>(instance)->value0));}
        static bool isArray_value0(){ return false; }
        static const char* getFieldName_value1(){ return "value1";}
        static const char* getFieldTypeName_value1(){ return "Vector4";}
        static void set_value1(void* instance, void* field_value){ static_cast<Matrix4*>(instance)->value1 = *static_cast<Vector4*>(field_value);}
        static void* get_value1(void* instance){ return static_cast<void*>(&(static_cast<Matrix4*>(instance)->value1));}
        static bool isArray_value1(){ return false; }
        static const char* getFieldName_value2(){ return "value2";}
        static const char* getFieldTypeName_value2(){ return "Vector4";}
        static void set_value2(void* instance, void* field_value){ static_cast<Matrix4*>(instance)->value2 = *static_cast<Vector4*>(field_value);}
        static void* get_value2(void* instance){ return static_cast<void*>(&(static_cast<Matrix4*>(instance)->value2));}
        static bool isArray_value2(){ return false; }
        static const char* getFieldName_value3(){ return "value3";}
        static const char* getFieldTypeName_value3(){ return "Vector4";}
        static void set_value3(void* instance, void* field_value){ static_cast<Matrix4*>(instance)->value3 = *static_cast<Vector4*>(field_value);}
        static void* get_value3(void* instance){ return static_cast<void*>(&(static_cast<Matrix4*>(instance)->value3));}
        static bool isArray_value3(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Matrix4(){
        FieldFunctionTuple* f_field_function_tuple_value0=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix4Operator::set_value0,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::get_value0,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldName_value0,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldTypeName_value0,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::isArray_value0);
        REGISTER_FIELD_TO_MAP("Matrix4", f_field_function_tuple_value0);
        FieldFunctionTuple* f_field_function_tuple_value1=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix4Operator::set_value1,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::get_value1,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldName_value1,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldTypeName_value1,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::isArray_value1);
        REGISTER_FIELD_TO_MAP("Matrix4", f_field_function_tuple_value1);
        FieldFunctionTuple* f_field_function_tuple_value2=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix4Operator::set_value2,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::get_value2,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldName_value2,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldTypeName_value2,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::isArray_value2);
        REGISTER_FIELD_TO_MAP("Matrix4", f_field_function_tuple_value2);
        FieldFunctionTuple* f_field_function_tuple_value3=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix4Operator::set_value3,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::get_value3,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldName_value3,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getFieldTypeName_value3,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::isArray_value3);
        REGISTER_FIELD_TO_MAP("Matrix4", f_field_function_tuple_value3);
        
        
        ClassFunctionTuple* f_class_function_tuple_Matrix4=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeMatrix4Operator::getMatrix4BaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeMatrix4Operator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Matrix4", f_class_function_tuple_Matrix4);
    }
namespace TypeWrappersRegister{
        void Matrix3(){ TypeWrapperRegister_Matrix3();}
        void Matrix4(){ TypeWrapperRegister_Matrix4();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

