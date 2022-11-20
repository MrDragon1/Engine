#pragma once
#include "Resource/Descriptor/Mesh.h"

namespace Ethereal{
    class StaticMeshDesc;
    class MeshDesc;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeStaticMeshDescOperator{
    public:
        static const char* getClassName(){ return "StaticMeshDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            StaticMeshDesc* ret_instance= new StaticMeshDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(StaticMeshDesc*)instance);
        }
        // base class
        static int getStaticMeshDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<StaticMeshDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Mesh(){ return "Mesh";}
        static const char* getFieldTypeName_Mesh(){ return "UUID";}
        static void set_Mesh(void* instance, void* field_value){ static_cast<StaticMeshDesc*>(instance)->Mesh = *static_cast<UUID*>(field_value);}
        static void* get_Mesh(void* instance){ return static_cast<void*>(&(static_cast<StaticMeshDesc*>(instance)->Mesh));}
        static bool isArray_Mesh(){ return false; }
        static const char* getFieldName_Materials(){ return "Materials";}
        static const char* getFieldTypeName_Materials(){ return "std::vector<UUID>";}
        static void set_Materials(void* instance, void* field_value){ static_cast<StaticMeshDesc*>(instance)->Materials = *static_cast<std::vector<UUID>*>(field_value);}
        static void* get_Materials(void* instance){ return static_cast<void*>(&(static_cast<StaticMeshDesc*>(instance)->Materials));}
        static bool isArray_Materials(){ return true; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLUUIDROperatorMACRO
#define ArraystdSSvectorLUUIDROperatorMACRO
    class ArraystdSSvectorLUUIDROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<UUID>";}
            static const char* getElementTypeName(){ return "UUID";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<UUID>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<UUID>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<UUID>*>(instance))[index] = *static_cast<UUID*>(element_value);
            }
    };
#endif //ArraystdSSvectorLUUIDROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_StaticMeshDesc(){
        FieldFunctionTuple* f_field_function_tuple_Mesh=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::set_Mesh,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::get_Mesh,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getFieldName_Mesh,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getFieldTypeName_Mesh,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::isArray_Mesh);
        REGISTER_FIELD_TO_MAP("StaticMeshDesc", f_field_function_tuple_Mesh);
        FieldFunctionTuple* f_field_function_tuple_Materials=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::set_Materials,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::get_Materials,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getFieldName_Materials,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getFieldTypeName_Materials,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::isArray_Materials);
        REGISTER_FIELD_TO_MAP("StaticMeshDesc", f_field_function_tuple_Materials);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLUUIDR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<UUID>", f_array_tuple_stdSSvectorLUUIDR);
        ClassFunctionTuple* f_class_function_tuple_StaticMeshDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::getStaticMeshDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeStaticMeshDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("StaticMeshDesc", f_class_function_tuple_StaticMeshDesc);
    }namespace TypeFieldReflectionOparator{
    class TypeMeshDescOperator{
    public:
        static const char* getClassName(){ return "MeshDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            MeshDesc* ret_instance= new MeshDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(MeshDesc*)instance);
        }
        // base class
        static int getMeshDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<MeshDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Mesh(){ return "Mesh";}
        static const char* getFieldTypeName_Mesh(){ return "UUID";}
        static void set_Mesh(void* instance, void* field_value){ static_cast<MeshDesc*>(instance)->Mesh = *static_cast<UUID*>(field_value);}
        static void* get_Mesh(void* instance){ return static_cast<void*>(&(static_cast<MeshDesc*>(instance)->Mesh));}
        static bool isArray_Mesh(){ return false; }
        static const char* getFieldName_Materials(){ return "Materials";}
        static const char* getFieldTypeName_Materials(){ return "std::vector<UUID>";}
        static void set_Materials(void* instance, void* field_value){ static_cast<MeshDesc*>(instance)->Materials = *static_cast<std::vector<UUID>*>(field_value);}
        static void* get_Materials(void* instance){ return static_cast<void*>(&(static_cast<MeshDesc*>(instance)->Materials));}
        static bool isArray_Materials(){ return true; }
        static const char* getFieldName_Animator(){ return "Animator";}
        static const char* getFieldTypeName_Animator(){ return "UUID";}
        static void set_Animator(void* instance, void* field_value){ static_cast<MeshDesc*>(instance)->Animator = *static_cast<UUID*>(field_value);}
        static void* get_Animator(void* instance){ return static_cast<void*>(&(static_cast<MeshDesc*>(instance)->Animator));}
        static bool isArray_Animator(){ return false; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLUUIDROperatorMACRO
#define ArraystdSSvectorLUUIDROperatorMACRO
    class ArraystdSSvectorLUUIDROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<UUID>";}
            static const char* getElementTypeName(){ return "UUID";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<UUID>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<UUID>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<UUID>*>(instance))[index] = *static_cast<UUID*>(element_value);
            }
    };
#endif //ArraystdSSvectorLUUIDROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_MeshDesc(){
        FieldFunctionTuple* f_field_function_tuple_Mesh=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMeshDescOperator::set_Mesh,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::get_Mesh,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getFieldName_Mesh,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getFieldTypeName_Mesh,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::isArray_Mesh);
        REGISTER_FIELD_TO_MAP("MeshDesc", f_field_function_tuple_Mesh);
        FieldFunctionTuple* f_field_function_tuple_Materials=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMeshDescOperator::set_Materials,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::get_Materials,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getFieldName_Materials,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getFieldTypeName_Materials,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::isArray_Materials);
        REGISTER_FIELD_TO_MAP("MeshDesc", f_field_function_tuple_Materials);
        FieldFunctionTuple* f_field_function_tuple_Animator=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMeshDescOperator::set_Animator,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::get_Animator,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getFieldName_Animator,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getFieldTypeName_Animator,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::isArray_Animator);
        REGISTER_FIELD_TO_MAP("MeshDesc", f_field_function_tuple_Animator);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLUUIDR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<UUID>", f_array_tuple_stdSSvectorLUUIDR);
        ClassFunctionTuple* f_class_function_tuple_MeshDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeMeshDescOperator::getMeshDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeMeshDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("MeshDesc", f_class_function_tuple_MeshDesc);
    }
namespace TypeWrappersRegister{
        void StaticMeshDesc(){ TypeWrapperRegister_StaticMeshDesc();}
        void MeshDesc(){ TypeWrapperRegister_MeshDesc();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

