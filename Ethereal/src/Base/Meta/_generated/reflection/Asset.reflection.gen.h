#pragma once
#include "Core/Asset/Asset.h"

namespace Ethereal{
    class Asset;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeAssetOperator{
    public:
        static const char* getClassName(){ return "Asset";}
        static void* constructorWithYNode(const YNode& yaml_context){
            Asset* ret_instance= new Asset;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(Asset*)instance);
        }
        // base class
        static int getAssetBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::RefCounted,static_cast<Asset*>(instance));
            }
            return count;
        }
        // fields
        
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_Asset(){
        
        
        
        ClassFunctionTuple* f_class_function_tuple_Asset=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAssetOperator::getAssetBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAssetOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAssetOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("Asset", f_class_function_tuple_Asset);
    }
namespace TypeWrappersRegister{
        void Asset(){ TypeWrapperRegister_Asset();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

