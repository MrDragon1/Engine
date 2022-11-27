#pragma once
#include "Base/Meta/Raw/Camera.h"

namespace Ethereal{
    class CameraRaw;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeCameraRawOperator{
    public:
        static const char* getClassName(){ return "CameraRaw";}
        static void* constructorWithYNode(const YNode& yaml_context){
            CameraRaw* ret_instance= new CameraRaw;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(CameraRaw*)instance);
        }
        // base class
        static int getCameraRawBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Primary(){ return "Primary";}
        static const char* getFieldTypeName_Primary(){ return "bool";}
        static void set_Primary(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->Primary = *static_cast<bool*>(field_value);}
        static void* get_Primary(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->Primary));}
        static bool isArray_Primary(){ return false; }
        static const char* getFieldName_FixedAspectRatio(){ return "FixedAspectRatio";}
        static const char* getFieldTypeName_FixedAspectRatio(){ return "bool";}
        static void set_FixedAspectRatio(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->FixedAspectRatio = *static_cast<bool*>(field_value);}
        static void* get_FixedAspectRatio(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->FixedAspectRatio));}
        static bool isArray_FixedAspectRatio(){ return false; }
        static const char* getFieldName_Perspective(){ return "Perspective";}
        static const char* getFieldTypeName_Perspective(){ return "bool";}
        static void set_Perspective(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->Perspective = *static_cast<bool*>(field_value);}
        static void* get_Perspective(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->Perspective));}
        static bool isArray_Perspective(){ return false; }
        static const char* getFieldName_Fov(){ return "Fov";}
        static const char* getFieldTypeName_Fov(){ return "float";}
        static void set_Fov(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->Fov = *static_cast<float*>(field_value);}
        static void* get_Fov(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->Fov));}
        static bool isArray_Fov(){ return false; }
        static const char* getFieldName_Near(){ return "Near";}
        static const char* getFieldTypeName_Near(){ return "float";}
        static void set_Near(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->Near = *static_cast<float*>(field_value);}
        static void* get_Near(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->Near));}
        static bool isArray_Near(){ return false; }
        static const char* getFieldName_Far(){ return "Far";}
        static const char* getFieldTypeName_Far(){ return "float";}
        static void set_Far(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->Far = *static_cast<float*>(field_value);}
        static void* get_Far(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->Far));}
        static bool isArray_Far(){ return false; }
        static const char* getFieldName_AspectRatio(){ return "AspectRatio";}
        static const char* getFieldTypeName_AspectRatio(){ return "float";}
        static void set_AspectRatio(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->AspectRatio = *static_cast<float*>(field_value);}
        static void* get_AspectRatio(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->AspectRatio));}
        static bool isArray_AspectRatio(){ return false; }
        static const char* getFieldName_OrthographicSize(){ return "OrthographicSize";}
        static const char* getFieldTypeName_OrthographicSize(){ return "float";}
        static void set_OrthographicSize(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->OrthographicSize = *static_cast<float*>(field_value);}
        static void* get_OrthographicSize(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->OrthographicSize));}
        static bool isArray_OrthographicSize(){ return false; }
        static const char* getFieldName_OrthographicNear(){ return "OrthographicNear";}
        static const char* getFieldTypeName_OrthographicNear(){ return "float";}
        static void set_OrthographicNear(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->OrthographicNear = *static_cast<float*>(field_value);}
        static void* get_OrthographicNear(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->OrthographicNear));}
        static bool isArray_OrthographicNear(){ return false; }
        static const char* getFieldName_OrthographicFar(){ return "OrthographicFar";}
        static const char* getFieldTypeName_OrthographicFar(){ return "float";}
        static void set_OrthographicFar(void* instance, void* field_value){ static_cast<CameraRaw*>(instance)->OrthographicFar = *static_cast<float*>(field_value);}
        static void* get_OrthographicFar(void* instance){ return static_cast<void*>(&(static_cast<CameraRaw*>(instance)->OrthographicFar));}
        static bool isArray_OrthographicFar(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_CameraRaw(){
        FieldFunctionTuple* f_field_function_tuple_Primary=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_Primary,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_Primary,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_Primary,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_Primary,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_Primary);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_Primary);
        FieldFunctionTuple* f_field_function_tuple_FixedAspectRatio=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_FixedAspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_FixedAspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_FixedAspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_FixedAspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_FixedAspectRatio);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_FixedAspectRatio);
        FieldFunctionTuple* f_field_function_tuple_Perspective=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_Perspective,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_Perspective,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_Perspective,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_Perspective,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_Perspective);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_Perspective);
        FieldFunctionTuple* f_field_function_tuple_Fov=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_Fov,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_Fov,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_Fov,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_Fov,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_Fov);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_Fov);
        FieldFunctionTuple* f_field_function_tuple_Near=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_Near,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_Near,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_Near,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_Near,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_Near);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_Near);
        FieldFunctionTuple* f_field_function_tuple_Far=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_Far,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_Far,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_Far,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_Far,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_Far);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_Far);
        FieldFunctionTuple* f_field_function_tuple_AspectRatio=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_AspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_AspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_AspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_AspectRatio,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_AspectRatio);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_AspectRatio);
        FieldFunctionTuple* f_field_function_tuple_OrthographicSize=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_OrthographicSize,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_OrthographicSize,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_OrthographicSize,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_OrthographicSize,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_OrthographicSize);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_OrthographicSize);
        FieldFunctionTuple* f_field_function_tuple_OrthographicNear=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_OrthographicNear,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_OrthographicNear,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_OrthographicNear,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_OrthographicNear,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_OrthographicNear);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_OrthographicNear);
        FieldFunctionTuple* f_field_function_tuple_OrthographicFar=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::set_OrthographicFar,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::get_OrthographicFar,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldName_OrthographicFar,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getFieldTypeName_OrthographicFar,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::isArray_OrthographicFar);
        REGISTER_FIELD_TO_MAP("CameraRaw", f_field_function_tuple_OrthographicFar);
        
        
        ClassFunctionTuple* f_class_function_tuple_CameraRaw=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraRawOperator::getCameraRawBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeCameraRawOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("CameraRaw", f_class_function_tuple_CameraRaw);
    }
namespace TypeWrappersRegister{
        void CameraRaw(){ TypeWrapperRegister_CameraRaw();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

