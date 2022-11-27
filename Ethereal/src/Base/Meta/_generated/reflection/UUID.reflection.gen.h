#pragma once
#include "Base/UUID.h"

namespace Ethereal{
    class UUID;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeUUIDOperator{
    public:
        static const char* getClassName(){ return "UUID";}
        static void* constructorWithYNode(const YNode& yaml_context){
            UUID* ret_instance= new UUID;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(UUID*)instance);
        }
        // base class
        static int getUUIDBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_m_UUID(){ return "m_UUID";}
        static const char* getFieldTypeName_m_UUID(){ return "uint64_t";}
        static void set_m_UUID(void* instance, void* field_value){ static_cast<UUID*>(instance)->m_UUID = *static_cast<uint64_t*>(field_value);}
        static void* get_m_UUID(void* instance){ return static_cast<void*>(&(static_cast<UUID*>(instance)->m_UUID));}
        static bool isArray_m_UUID(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_UUID(){
        FieldFunctionTuple* f_field_function_tuple_m_UUID=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeUUIDOperator::set_m_UUID,
            &TypeFieldReflectionOparator::TypeUUIDOperator::get_m_UUID,
            &TypeFieldReflectionOparator::TypeUUIDOperator::getClassName,
            &TypeFieldReflectionOparator::TypeUUIDOperator::getFieldName_m_UUID,
            &TypeFieldReflectionOparator::TypeUUIDOperator::getFieldTypeName_m_UUID,
            &TypeFieldReflectionOparator::TypeUUIDOperator::isArray_m_UUID);
        REGISTER_FIELD_TO_MAP("UUID", f_field_function_tuple_m_UUID);
        
        
        ClassFunctionTuple* f_class_function_tuple_UUID=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeUUIDOperator::getUUIDBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeUUIDOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeUUIDOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("UUID", f_class_function_tuple_UUID);
    }
namespace TypeWrappersRegister{
        void UUID(){ TypeWrapperRegister_UUID();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

