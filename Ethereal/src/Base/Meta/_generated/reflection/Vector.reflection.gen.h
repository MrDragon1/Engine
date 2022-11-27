#pragma once
#include "Base/Math/Vector.h"

namespace Ethereal{
    class Vector2;
    class Vector3;
    class Vector4;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeVector2Operator{
    public:
        static const char* getClassName(){ return "Vector2";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Vector2* ret_instance= new Vector2;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Vector2*)instance);
        }
        // base class
        static int getVector2BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_x(){ return "x";}
        static const char* getFieldTypeName_x(){ return "float";}
        static void set_x(void* instance, void* field_value){ static_cast<Vector2*>(instance)->x = *static_cast<float*>(field_value);}
        static void* get_x(void* instance){ return static_cast<void*>(&(static_cast<Vector2*>(instance)->x));}
        static bool isArray_x(){ return false; }
        static const char* getFieldName_y(){ return "y";}
        static const char* getFieldTypeName_y(){ return "float";}
        static void set_y(void* instance, void* field_value){ static_cast<Vector2*>(instance)->y = *static_cast<float*>(field_value);}
        static void* get_y(void* instance){ return static_cast<void*>(&(static_cast<Vector2*>(instance)->y));}
        static bool isArray_y(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Vector2(){
        FieldFunctionTuple* f_field_function_tuple_x=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector2Operator::set_x,
            &TypeFieldReflectionOparator::TypeVector2Operator::get_x,
            &TypeFieldReflectionOparator::TypeVector2Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector2Operator::getFieldName_x,
            &TypeFieldReflectionOparator::TypeVector2Operator::getFieldTypeName_x,
            &TypeFieldReflectionOparator::TypeVector2Operator::isArray_x);
        REGISTER_FIELD_TO_MAP("Vector2", f_field_function_tuple_x);
        FieldFunctionTuple* f_field_function_tuple_y=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector2Operator::set_y,
            &TypeFieldReflectionOparator::TypeVector2Operator::get_y,
            &TypeFieldReflectionOparator::TypeVector2Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector2Operator::getFieldName_y,
            &TypeFieldReflectionOparator::TypeVector2Operator::getFieldTypeName_y,
            &TypeFieldReflectionOparator::TypeVector2Operator::isArray_y);
        REGISTER_FIELD_TO_MAP("Vector2", f_field_function_tuple_y);
        
        
        ClassFunctionTuple* f_class_function_tuple_Vector2=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector2Operator::getVector2BaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeVector2Operator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeVector2Operator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Vector2", f_class_function_tuple_Vector2);
    }namespace TypeFieldReflectionOparator{
    class TypeVector3Operator{
    public:
        static const char* getClassName(){ return "Vector3";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Vector3* ret_instance= new Vector3;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Vector3*)instance);
        }
        // base class
        static int getVector3BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_x(){ return "x";}
        static const char* getFieldTypeName_x(){ return "float";}
        static void set_x(void* instance, void* field_value){ static_cast<Vector3*>(instance)->x = *static_cast<float*>(field_value);}
        static void* get_x(void* instance){ return static_cast<void*>(&(static_cast<Vector3*>(instance)->x));}
        static bool isArray_x(){ return false; }
        static const char* getFieldName_y(){ return "y";}
        static const char* getFieldTypeName_y(){ return "float";}
        static void set_y(void* instance, void* field_value){ static_cast<Vector3*>(instance)->y = *static_cast<float*>(field_value);}
        static void* get_y(void* instance){ return static_cast<void*>(&(static_cast<Vector3*>(instance)->y));}
        static bool isArray_y(){ return false; }
        static const char* getFieldName_z(){ return "z";}
        static const char* getFieldTypeName_z(){ return "float";}
        static void set_z(void* instance, void* field_value){ static_cast<Vector3*>(instance)->z = *static_cast<float*>(field_value);}
        static void* get_z(void* instance){ return static_cast<void*>(&(static_cast<Vector3*>(instance)->z));}
        static bool isArray_z(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Vector3(){
        FieldFunctionTuple* f_field_function_tuple_x=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector3Operator::set_x,
            &TypeFieldReflectionOparator::TypeVector3Operator::get_x,
            &TypeFieldReflectionOparator::TypeVector3Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector3Operator::getFieldName_x,
            &TypeFieldReflectionOparator::TypeVector3Operator::getFieldTypeName_x,
            &TypeFieldReflectionOparator::TypeVector3Operator::isArray_x);
        REGISTER_FIELD_TO_MAP("Vector3", f_field_function_tuple_x);
        FieldFunctionTuple* f_field_function_tuple_y=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector3Operator::set_y,
            &TypeFieldReflectionOparator::TypeVector3Operator::get_y,
            &TypeFieldReflectionOparator::TypeVector3Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector3Operator::getFieldName_y,
            &TypeFieldReflectionOparator::TypeVector3Operator::getFieldTypeName_y,
            &TypeFieldReflectionOparator::TypeVector3Operator::isArray_y);
        REGISTER_FIELD_TO_MAP("Vector3", f_field_function_tuple_y);
        FieldFunctionTuple* f_field_function_tuple_z=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector3Operator::set_z,
            &TypeFieldReflectionOparator::TypeVector3Operator::get_z,
            &TypeFieldReflectionOparator::TypeVector3Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector3Operator::getFieldName_z,
            &TypeFieldReflectionOparator::TypeVector3Operator::getFieldTypeName_z,
            &TypeFieldReflectionOparator::TypeVector3Operator::isArray_z);
        REGISTER_FIELD_TO_MAP("Vector3", f_field_function_tuple_z);
        
        
        ClassFunctionTuple* f_class_function_tuple_Vector3=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector3Operator::getVector3BaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeVector3Operator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeVector3Operator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Vector3", f_class_function_tuple_Vector3);
    }namespace TypeFieldReflectionOparator{
    class TypeVector4Operator{
    public:
        static const char* getClassName(){ return "Vector4";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Vector4* ret_instance= new Vector4;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Vector4*)instance);
        }
        // base class
        static int getVector4BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_x(){ return "x";}
        static const char* getFieldTypeName_x(){ return "float";}
        static void set_x(void* instance, void* field_value){ static_cast<Vector4*>(instance)->x = *static_cast<float*>(field_value);}
        static void* get_x(void* instance){ return static_cast<void*>(&(static_cast<Vector4*>(instance)->x));}
        static bool isArray_x(){ return false; }
        static const char* getFieldName_y(){ return "y";}
        static const char* getFieldTypeName_y(){ return "float";}
        static void set_y(void* instance, void* field_value){ static_cast<Vector4*>(instance)->y = *static_cast<float*>(field_value);}
        static void* get_y(void* instance){ return static_cast<void*>(&(static_cast<Vector4*>(instance)->y));}
        static bool isArray_y(){ return false; }
        static const char* getFieldName_z(){ return "z";}
        static const char* getFieldTypeName_z(){ return "float";}
        static void set_z(void* instance, void* field_value){ static_cast<Vector4*>(instance)->z = *static_cast<float*>(field_value);}
        static void* get_z(void* instance){ return static_cast<void*>(&(static_cast<Vector4*>(instance)->z));}
        static bool isArray_z(){ return false; }
        static const char* getFieldName_w(){ return "w";}
        static const char* getFieldTypeName_w(){ return "float";}
        static void set_w(void* instance, void* field_value){ static_cast<Vector4*>(instance)->w = *static_cast<float*>(field_value);}
        static void* get_w(void* instance){ return static_cast<void*>(&(static_cast<Vector4*>(instance)->w));}
        static bool isArray_w(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Vector4(){
        FieldFunctionTuple* f_field_function_tuple_x=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector4Operator::set_x,
            &TypeFieldReflectionOparator::TypeVector4Operator::get_x,
            &TypeFieldReflectionOparator::TypeVector4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldName_x,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldTypeName_x,
            &TypeFieldReflectionOparator::TypeVector4Operator::isArray_x);
        REGISTER_FIELD_TO_MAP("Vector4", f_field_function_tuple_x);
        FieldFunctionTuple* f_field_function_tuple_y=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector4Operator::set_y,
            &TypeFieldReflectionOparator::TypeVector4Operator::get_y,
            &TypeFieldReflectionOparator::TypeVector4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldName_y,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldTypeName_y,
            &TypeFieldReflectionOparator::TypeVector4Operator::isArray_y);
        REGISTER_FIELD_TO_MAP("Vector4", f_field_function_tuple_y);
        FieldFunctionTuple* f_field_function_tuple_z=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector4Operator::set_z,
            &TypeFieldReflectionOparator::TypeVector4Operator::get_z,
            &TypeFieldReflectionOparator::TypeVector4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldName_z,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldTypeName_z,
            &TypeFieldReflectionOparator::TypeVector4Operator::isArray_z);
        REGISTER_FIELD_TO_MAP("Vector4", f_field_function_tuple_z);
        FieldFunctionTuple* f_field_function_tuple_w=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector4Operator::set_w,
            &TypeFieldReflectionOparator::TypeVector4Operator::get_w,
            &TypeFieldReflectionOparator::TypeVector4Operator::getClassName,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldName_w,
            &TypeFieldReflectionOparator::TypeVector4Operator::getFieldTypeName_w,
            &TypeFieldReflectionOparator::TypeVector4Operator::isArray_w);
        REGISTER_FIELD_TO_MAP("Vector4", f_field_function_tuple_w);
        
        
        ClassFunctionTuple* f_class_function_tuple_Vector4=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeVector4Operator::getVector4BaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeVector4Operator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeVector4Operator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Vector4", f_class_function_tuple_Vector4);
    }
namespace TypeWrappersRegister{
        void Vector2(){ TypeWrapperRegister_Vector2();}
        void Vector3(){ TypeWrapperRegister_Vector3();}
        void Vector4(){ TypeWrapperRegister_Vector4();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

