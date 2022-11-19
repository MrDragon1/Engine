#pragma once
#include "Base/Meta/Raw/Mesh.h"

namespace Ethereal{
    class MaterialTableRaw;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeMaterialTableRawOperator{
    public:
        static const char* getClassName(){ return "MaterialTableRaw";}
        static void* constructorWithYNode(const YNode& yaml_context){
            MaterialTableRaw* ret_instance= new MaterialTableRaw;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(MaterialTableRaw*)instance);
        }
        // base class
        static int getMaterialTableRawBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Materials(){ return "Materials";}
        static const char* getFieldTypeName_Materials(){ return "std::vector<UUID>";}
        static void set_Materials(void* instance, void* field_value){ static_cast<MaterialTableRaw*>(instance)->Materials = *static_cast<std::vector<UUID>*>(field_value);}
        static void* get_Materials(void* instance){ return static_cast<void*>(&(static_cast<MaterialTableRaw*>(instance)->Materials));}
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

    void TypeWrapperRegister_MaterialTableRaw(){
        FieldFunctionTuple* f_field_function_tuple_Materials=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::set_Materials,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::get_Materials,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::getFieldName_Materials,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::getFieldTypeName_Materials,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::isArray_Materials);
        REGISTER_FIELD_TO_MAP("MaterialTableRaw", f_field_function_tuple_Materials);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLUUIDR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLUUIDROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<UUID>", f_array_tuple_stdSSvectorLUUIDR);
        ClassFunctionTuple* f_class_function_tuple_MaterialTableRaw=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::getMaterialTableRawBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeMaterialTableRawOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("MaterialTableRaw", f_class_function_tuple_MaterialTableRaw);
    }
namespace TypeWrappersRegister{
        void MaterialTableRaw(){ TypeWrapperRegister_MaterialTableRaw();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

