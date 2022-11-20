#pragma once
#include "Base/Ref.h"

namespace Ethereal{
    class RefCounted;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeRefCountedOperator{
    public:
        static const char* getClassName(){ return "RefCounted";}
        static void* constructorWithYNode(const YNode& yaml_context){
            RefCounted* ret_instance= new RefCounted;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(RefCounted*)instance);
        }
        // base class
        static int getRefCountedBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_RefCounted(){
        
        
        
        ClassFunctionTuple* f_class_function_tuple_RefCounted=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeRefCountedOperator::getRefCountedBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeRefCountedOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeRefCountedOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("RefCounted", f_class_function_tuple_RefCounted);
    }
namespace TypeWrappersRegister{
        void RefCounted(){ TypeWrapperRegister_RefCounted();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

