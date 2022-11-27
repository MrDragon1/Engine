#pragma once
#include "Base/Meta/Raw/Scene.h"

namespace Ethereal{
    class EntityRaw;
    class SceneRaw;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeEntityRawOperator{
    public:
        static const char* getClassName(){ return "EntityRaw";}
        static void* constructorWithYNode(const YNode& yaml_context){
            EntityRaw* ret_instance= new EntityRaw;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(EntityRaw*)instance);
        }
        // base class
        static int getEntityRawBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Name(){ return "Name";}
        static const char* getFieldTypeName_Name(){ return "std::string";}
        static void set_Name(void* instance, void* field_value){ static_cast<EntityRaw*>(instance)->Name = *static_cast<std::string*>(field_value);}
        static void* get_Name(void* instance){ return static_cast<void*>(&(static_cast<EntityRaw*>(instance)->Name));}
        static bool isArray_Name(){ return false; }
        static const char* getFieldName_Components(){ return "Components";}
        static const char* getFieldTypeName_Components(){ return "std::vector<Reflection::ReflectionPtr<Component>>";}
        static void set_Components(void* instance, void* field_value){ static_cast<EntityRaw*>(instance)->Components = *static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(field_value);}
        static void* get_Components(void* instance){ return static_cast<void*>(&(static_cast<EntityRaw*>(instance)->Components));}
        static bool isArray_Components(){ return true; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperatorMACRO
#define ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperatorMACRO
    class ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<Reflection::ReflectionPtr<Component>>";}
            static const char* getElementTypeName(){ return "Reflection::ReflectionPtr<Component>";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance))[index] = *static_cast<Reflection::ReflectionPtr<Component>*>(element_value);
            }
    };
#endif //ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_EntityRaw(){
        FieldFunctionTuple* f_field_function_tuple_Name=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeEntityRawOperator::set_Name,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::get_Name,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getFieldName_Name,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getFieldTypeName_Name,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::isArray_Name);
        REGISTER_FIELD_TO_MAP("EntityRaw", f_field_function_tuple_Name);
        FieldFunctionTuple* f_field_function_tuple_Components=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeEntityRawOperator::set_Components,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::get_Components,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getFieldName_Components,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getFieldTypeName_Components,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::isArray_Components);
        REGISTER_FIELD_TO_MAP("EntityRaw", f_field_function_tuple_Components);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLReflectionSSReflectionPtrLComponentRR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<Reflection::ReflectionPtr<Component>>", f_array_tuple_stdSSvectorLReflectionSSReflectionPtrLComponentRR);
        ClassFunctionTuple* f_class_function_tuple_EntityRaw=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeEntityRawOperator::getEntityRawBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeEntityRawOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("EntityRaw", f_class_function_tuple_EntityRaw);
    }namespace TypeFieldReflectionOparator{
    class TypeSceneRawOperator{
    public:
        static const char* getClassName(){ return "SceneRaw";}
        static void* constructorWithYNode(const YNode& yaml_context){
            SceneRaw* ret_instance= new SceneRaw;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(SceneRaw*)instance);
        }
        // base class
        static int getSceneRawBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Name(){ return "Name";}
        static const char* getFieldTypeName_Name(){ return "std::string";}
        static void set_Name(void* instance, void* field_value){ static_cast<SceneRaw*>(instance)->Name = *static_cast<std::string*>(field_value);}
        static void* get_Name(void* instance){ return static_cast<void*>(&(static_cast<SceneRaw*>(instance)->Name));}
        static bool isArray_Name(){ return false; }
        static const char* getFieldName_Environment(){ return "Environment";}
        static const char* getFieldTypeName_Environment(){ return "UUID";}
        static void set_Environment(void* instance, void* field_value){ static_cast<SceneRaw*>(instance)->Environment = *static_cast<UUID*>(field_value);}
        static void* get_Environment(void* instance){ return static_cast<void*>(&(static_cast<SceneRaw*>(instance)->Environment));}
        static bool isArray_Environment(){ return false; }
        static const char* getFieldName_Entities(){ return "Entities";}
        static const char* getFieldTypeName_Entities(){ return "std::vector<EntityRaw>";}
        static void set_Entities(void* instance, void* field_value){ static_cast<SceneRaw*>(instance)->Entities = *static_cast<std::vector<EntityRaw>*>(field_value);}
        static void* get_Entities(void* instance){ return static_cast<void*>(&(static_cast<SceneRaw*>(instance)->Entities));}
        static bool isArray_Entities(){ return true; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLEntityRawROperatorMACRO
#define ArraystdSSvectorLEntityRawROperatorMACRO
    class ArraystdSSvectorLEntityRawROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<EntityRaw>";}
            static const char* getElementTypeName(){ return "EntityRaw";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<EntityRaw>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<EntityRaw>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<EntityRaw>*>(instance))[index] = *static_cast<EntityRaw*>(element_value);
            }
    };
#endif //ArraystdSSvectorLEntityRawROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_SceneRaw(){
        FieldFunctionTuple* f_field_function_tuple_Name=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeSceneRawOperator::set_Name,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::get_Name,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getFieldName_Name,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getFieldTypeName_Name,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::isArray_Name);
        REGISTER_FIELD_TO_MAP("SceneRaw", f_field_function_tuple_Name);
        FieldFunctionTuple* f_field_function_tuple_Environment=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeSceneRawOperator::set_Environment,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::get_Environment,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getFieldName_Environment,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getFieldTypeName_Environment,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::isArray_Environment);
        REGISTER_FIELD_TO_MAP("SceneRaw", f_field_function_tuple_Environment);
        FieldFunctionTuple* f_field_function_tuple_Entities=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeSceneRawOperator::set_Entities,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::get_Entities,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getFieldName_Entities,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getFieldTypeName_Entities,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::isArray_Entities);
        REGISTER_FIELD_TO_MAP("SceneRaw", f_field_function_tuple_Entities);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLEntityRawR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLEntityRawROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLEntityRawROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLEntityRawROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLEntityRawROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLEntityRawROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<EntityRaw>", f_array_tuple_stdSSvectorLEntityRawR);
        ClassFunctionTuple* f_class_function_tuple_SceneRaw=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeSceneRawOperator::getSceneRawBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeSceneRawOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("SceneRaw", f_class_function_tuple_SceneRaw);
    }
namespace TypeWrappersRegister{
        void EntityRaw(){ TypeWrapperRegister_EntityRaw();}
        void SceneRaw(){ TypeWrapperRegister_SceneRaw();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

