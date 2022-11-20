#pragma once
#include "Resource/Descriptor/Environment.h"

namespace Ethereal{
    class EnvironmentDesc;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeEnvironmentDescOperator{
    public:
        static const char* getClassName(){ return "EnvironmentDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            EnvironmentDesc* ret_instance= new EnvironmentDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(EnvironmentDesc*)instance);
        }
        // base class
        static int getEnvironmentDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<EnvironmentDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_EnvironmentMap(){ return "EnvironmentMap";}
        static const char* getFieldTypeName_EnvironmentMap(){ return "UUID";}
        static void set_EnvironmentMap(void* instance, void* field_value){ static_cast<EnvironmentDesc*>(instance)->EnvironmentMap = *static_cast<UUID*>(field_value);}
        static void* get_EnvironmentMap(void* instance){ return static_cast<void*>(&(static_cast<EnvironmentDesc*>(instance)->EnvironmentMap));}
        static bool isArray_EnvironmentMap(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_EnvironmentDesc(){
        FieldFunctionTuple* f_field_function_tuple_EnvironmentMap=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::set_EnvironmentMap,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::get_EnvironmentMap,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::getFieldName_EnvironmentMap,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::getFieldTypeName_EnvironmentMap,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::isArray_EnvironmentMap);
        REGISTER_FIELD_TO_MAP("EnvironmentDesc", f_field_function_tuple_EnvironmentMap);
        
        
        ClassFunctionTuple* f_class_function_tuple_EnvironmentDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::getEnvironmentDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeEnvironmentDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("EnvironmentDesc", f_class_function_tuple_EnvironmentDesc);
    }
namespace TypeWrappersRegister{
        void EnvironmentDesc(){ TypeWrapperRegister_EnvironmentDesc();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

