#pragma once
#include "Core/Scene/Components.h"

namespace Ethereal{
    class Component;
    class IDComponent;
    class TagComponent;
    class TransformComponent;
    class StaticMeshComponent;
    class MeshComponent;
    class CameraComponent;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeComponentOperator{
    public:
        static const char* getClassName(){ return "Component";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Component* ret_instance= new Component;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Component*)instance);
        }
        // base class
        static int getComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Component(){
        
        
        
        ClassFunctionTuple* f_class_function_tuple_Component=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeComponentOperator::getComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Component", f_class_function_tuple_Component);
    }namespace TypeFieldReflectionOparator{
    class TypeIDComponentOperator{
    public:
        static const char* getClassName(){ return "IDComponent";}
        static void* constructorWithYNode(const YNode& yaml_context){
            IDComponent* ret_instance= new IDComponent;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(IDComponent*)instance);
        }
        // base class
        static int getIDComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Component,static_cast<IDComponent*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_ID(){ return "ID";}
        static const char* getFieldTypeName_ID(){ return "UUID";}
        static void set_ID(void* instance, void* field_value){ static_cast<IDComponent*>(instance)->ID = *static_cast<UUID*>(field_value);}
        static void* get_ID(void* instance){ return static_cast<void*>(&(static_cast<IDComponent*>(instance)->ID));}
        static bool isArray_ID(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_IDComponent(){
        FieldFunctionTuple* f_field_function_tuple_ID=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeIDComponentOperator::set_ID,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::get_ID,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::getFieldName_ID,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::getFieldTypeName_ID,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::isArray_ID);
        REGISTER_FIELD_TO_MAP("IDComponent", f_field_function_tuple_ID);
        
        
        ClassFunctionTuple* f_class_function_tuple_IDComponent=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeIDComponentOperator::getIDComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeIDComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("IDComponent", f_class_function_tuple_IDComponent);
    }namespace TypeFieldReflectionOparator{
    class TypeTagComponentOperator{
    public:
        static const char* getClassName(){ return "TagComponent";}
        static void* constructorWithYNode(const YNode& yaml_context){
            TagComponent* ret_instance= new TagComponent;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(TagComponent*)instance);
        }
        // base class
        static int getTagComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Component,static_cast<TagComponent*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Tag(){ return "Tag";}
        static const char* getFieldTypeName_Tag(){ return "std::string";}
        static void set_Tag(void* instance, void* field_value){ static_cast<TagComponent*>(instance)->Tag = *static_cast<std::string*>(field_value);}
        static void* get_Tag(void* instance){ return static_cast<void*>(&(static_cast<TagComponent*>(instance)->Tag));}
        static bool isArray_Tag(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_TagComponent(){
        FieldFunctionTuple* f_field_function_tuple_Tag=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeTagComponentOperator::set_Tag,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::get_Tag,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::getFieldName_Tag,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::getFieldTypeName_Tag,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::isArray_Tag);
        REGISTER_FIELD_TO_MAP("TagComponent", f_field_function_tuple_Tag);
        
        
        ClassFunctionTuple* f_class_function_tuple_TagComponent=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeTagComponentOperator::getTagComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeTagComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("TagComponent", f_class_function_tuple_TagComponent);
    }namespace TypeFieldReflectionOparator{
    class TypeTransformComponentOperator{
    public:
        static const char* getClassName(){ return "TransformComponent";}
        static void* constructorWithYNode(const YNode& yaml_context){
            TransformComponent* ret_instance= new TransformComponent;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(TransformComponent*)instance);
        }
        // base class
        static int getTransformComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Component,static_cast<TransformComponent*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Position(){ return "Position";}
        static const char* getFieldTypeName_Position(){ return "Vector3";}
        static void set_Position(void* instance, void* field_value){ static_cast<TransformComponent*>(instance)->Position = *static_cast<Vector3*>(field_value);}
        static void* get_Position(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->Position));}
        static bool isArray_Position(){ return false; }
        static const char* getFieldName_Scale(){ return "Scale";}
        static const char* getFieldTypeName_Scale(){ return "Vector3";}
        static void set_Scale(void* instance, void* field_value){ static_cast<TransformComponent*>(instance)->Scale = *static_cast<Vector3*>(field_value);}
        static void* get_Scale(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->Scale));}
        static bool isArray_Scale(){ return false; }
        static const char* getFieldName_Rotation(){ return "Rotation";}
        static const char* getFieldTypeName_Rotation(){ return "Quaternion";}
        static void set_Rotation(void* instance, void* field_value){ static_cast<TransformComponent*>(instance)->Rotation = *static_cast<Quaternion*>(field_value);}
        static void* get_Rotation(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->Rotation));}
        static bool isArray_Rotation(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_TransformComponent(){
        FieldFunctionTuple* f_field_function_tuple_Position=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::set_Position,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::get_Position,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getFieldName_Position,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getFieldTypeName_Position,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::isArray_Position);
        REGISTER_FIELD_TO_MAP("TransformComponent", f_field_function_tuple_Position);
        FieldFunctionTuple* f_field_function_tuple_Scale=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::set_Scale,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::get_Scale,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getFieldName_Scale,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getFieldTypeName_Scale,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::isArray_Scale);
        REGISTER_FIELD_TO_MAP("TransformComponent", f_field_function_tuple_Scale);
        FieldFunctionTuple* f_field_function_tuple_Rotation=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::set_Rotation,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::get_Rotation,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getFieldName_Rotation,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getFieldTypeName_Rotation,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::isArray_Rotation);
        REGISTER_FIELD_TO_MAP("TransformComponent", f_field_function_tuple_Rotation);
        
        
        ClassFunctionTuple* f_class_function_tuple_TransformComponent=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::getTransformComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeTransformComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("TransformComponent", f_class_function_tuple_TransformComponent);
    }namespace TypeFieldReflectionOparator{
    class TypeStaticMeshComponentOperator{
    public:
        static const char* getClassName(){ return "StaticMeshComponent";}
        static void* constructorWithYNode(const YNode& yaml_context){
            StaticMeshComponent* ret_instance= new StaticMeshComponent;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(StaticMeshComponent*)instance);
        }
        // base class
        static int getStaticMeshComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Component,static_cast<StaticMeshComponent*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_StaticMeshHandle(){ return "StaticMeshHandle";}
        static const char* getFieldTypeName_StaticMeshHandle(){ return "AssetHandle";}
        static void set_StaticMeshHandle(void* instance, void* field_value){ static_cast<StaticMeshComponent*>(instance)->StaticMeshHandle = *static_cast<AssetHandle*>(field_value);}
        static void* get_StaticMeshHandle(void* instance){ return static_cast<void*>(&(static_cast<StaticMeshComponent*>(instance)->StaticMeshHandle));}
        static bool isArray_StaticMeshHandle(){ return false; }
        static const char* getFieldName_MaterialTableRaw(){ return "MaterialTableRaw";}
        static const char* getFieldTypeName_MaterialTableRaw(){ return "MaterialTableRaw";}
        static void set_MaterialTableRaw(void* instance, void* field_value){ static_cast<StaticMeshComponent*>(instance)->MaterialTableRaw = *static_cast<MaterialTableRaw*>(field_value);}
        static void* get_MaterialTableRaw(void* instance){ return static_cast<void*>(&(static_cast<StaticMeshComponent*>(instance)->MaterialTableRaw));}
        static bool isArray_MaterialTableRaw(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_StaticMeshComponent(){
        FieldFunctionTuple* f_field_function_tuple_StaticMeshHandle=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::set_StaticMeshHandle,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::get_StaticMeshHandle,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getFieldName_StaticMeshHandle,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getFieldTypeName_StaticMeshHandle,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::isArray_StaticMeshHandle);
        REGISTER_FIELD_TO_MAP("StaticMeshComponent", f_field_function_tuple_StaticMeshHandle);
        FieldFunctionTuple* f_field_function_tuple_MaterialTableRaw=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::set_MaterialTableRaw,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::get_MaterialTableRaw,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getFieldName_MaterialTableRaw,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getFieldTypeName_MaterialTableRaw,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::isArray_MaterialTableRaw);
        REGISTER_FIELD_TO_MAP("StaticMeshComponent", f_field_function_tuple_MaterialTableRaw);
        
        
        ClassFunctionTuple* f_class_function_tuple_StaticMeshComponent=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::getStaticMeshComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeStaticMeshComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("StaticMeshComponent", f_class_function_tuple_StaticMeshComponent);
    }namespace TypeFieldReflectionOparator{
    class TypeMeshComponentOperator{
    public:
        static const char* getClassName(){ return "MeshComponent";}
        static void* constructorWithYNode(const YNode& yaml_context){
            MeshComponent* ret_instance= new MeshComponent;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(MeshComponent*)instance);
        }
        // base class
        static int getMeshComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Component,static_cast<MeshComponent*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_MeshHandle(){ return "MeshHandle";}
        static const char* getFieldTypeName_MeshHandle(){ return "AssetHandle";}
        static void set_MeshHandle(void* instance, void* field_value){ static_cast<MeshComponent*>(instance)->MeshHandle = *static_cast<AssetHandle*>(field_value);}
        static void* get_MeshHandle(void* instance){ return static_cast<void*>(&(static_cast<MeshComponent*>(instance)->MeshHandle));}
        static bool isArray_MeshHandle(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_MeshComponent(){
        FieldFunctionTuple* f_field_function_tuple_MeshHandle=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::set_MeshHandle,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::get_MeshHandle,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::getFieldName_MeshHandle,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::getFieldTypeName_MeshHandle,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::isArray_MeshHandle);
        REGISTER_FIELD_TO_MAP("MeshComponent", f_field_function_tuple_MeshHandle);
        
        
        ClassFunctionTuple* f_class_function_tuple_MeshComponent=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::getMeshComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeMeshComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("MeshComponent", f_class_function_tuple_MeshComponent);
    }namespace TypeFieldReflectionOparator{
    class TypeCameraComponentOperator{
    public:
        static const char* getClassName(){ return "CameraComponent";}
        static void* constructorWithYNode(const YNode& yaml_context){
            CameraComponent* ret_instance= new CameraComponent;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(CameraComponent*)instance);
        }
        // base class
        static int getCameraComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Component,static_cast<CameraComponent*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Camera(){ return "Camera";}
        static const char* getFieldTypeName_Camera(){ return "CameraRaw";}
        static void set_Camera(void* instance, void* field_value){ static_cast<CameraComponent*>(instance)->Camera = *static_cast<CameraRaw*>(field_value);}
        static void* get_Camera(void* instance){ return static_cast<void*>(&(static_cast<CameraComponent*>(instance)->Camera));}
        static bool isArray_Camera(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_CameraComponent(){
        FieldFunctionTuple* f_field_function_tuple_Camera=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::set_Camera,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::get_Camera,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::getClassName,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::getFieldName_Camera,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::getFieldTypeName_Camera,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::isArray_Camera);
        REGISTER_FIELD_TO_MAP("CameraComponent", f_field_function_tuple_Camera);
        
        
        ClassFunctionTuple* f_class_function_tuple_CameraComponent=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::getCameraComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeCameraComponentOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("CameraComponent", f_class_function_tuple_CameraComponent);
    }
namespace TypeWrappersRegister{
        void Component(){ TypeWrapperRegister_Component();}
        void IDComponent(){ TypeWrapperRegister_IDComponent();}
        void TagComponent(){ TypeWrapperRegister_TagComponent();}
        void TransformComponent(){ TypeWrapperRegister_TransformComponent();}
        void StaticMeshComponent(){ TypeWrapperRegister_StaticMeshComponent();}
        void MeshComponent(){ TypeWrapperRegister_MeshComponent();}
        void CameraComponent(){ TypeWrapperRegister_CameraComponent();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

