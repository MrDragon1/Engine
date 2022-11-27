#pragma once
#include "Resource/Descriptor/Material.h"

namespace Ethereal{
    class MaterialTableRaw;
    class MaterialDesc;
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
    }namespace TypeFieldReflectionOparator{
    class TypeMaterialDescOperator{
    public:
        static const char* getClassName(){ return "MaterialDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            MaterialDesc* ret_instance= new MaterialDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(MaterialDesc*)instance);
        }
        // base class
        static int getMaterialDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<MaterialDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_AlbedoMap(){ return "AlbedoMap";}
        static const char* getFieldTypeName_AlbedoMap(){ return "UUID";}
        static void set_AlbedoMap(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->AlbedoMap = *static_cast<UUID*>(field_value);}
        static void* get_AlbedoMap(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->AlbedoMap));}
        static bool isArray_AlbedoMap(){ return false; }
        static const char* getFieldName_NormalMap(){ return "NormalMap";}
        static const char* getFieldTypeName_NormalMap(){ return "UUID";}
        static void set_NormalMap(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->NormalMap = *static_cast<UUID*>(field_value);}
        static void* get_NormalMap(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->NormalMap));}
        static bool isArray_NormalMap(){ return false; }
        static const char* getFieldName_MetallicMap(){ return "MetallicMap";}
        static const char* getFieldTypeName_MetallicMap(){ return "UUID";}
        static void set_MetallicMap(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->MetallicMap = *static_cast<UUID*>(field_value);}
        static void* get_MetallicMap(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->MetallicMap));}
        static bool isArray_MetallicMap(){ return false; }
        static const char* getFieldName_RoughnessMap(){ return "RoughnessMap";}
        static const char* getFieldTypeName_RoughnessMap(){ return "UUID";}
        static void set_RoughnessMap(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->RoughnessMap = *static_cast<UUID*>(field_value);}
        static void* get_RoughnessMap(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->RoughnessMap));}
        static bool isArray_RoughnessMap(){ return false; }
        static const char* getFieldName_OcclusionMap(){ return "OcclusionMap";}
        static const char* getFieldTypeName_OcclusionMap(){ return "UUID";}
        static void set_OcclusionMap(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->OcclusionMap = *static_cast<UUID*>(field_value);}
        static void* get_OcclusionMap(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->OcclusionMap));}
        static bool isArray_OcclusionMap(){ return false; }
        static const char* getFieldName_Albedo(){ return "Albedo";}
        static const char* getFieldTypeName_Albedo(){ return "Vector3";}
        static void set_Albedo(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->Albedo = *static_cast<Vector3*>(field_value);}
        static void* get_Albedo(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->Albedo));}
        static bool isArray_Albedo(){ return false; }
        static const char* getFieldName_Metallic(){ return "Metallic";}
        static const char* getFieldTypeName_Metallic(){ return "float";}
        static void set_Metallic(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->Metallic = *static_cast<float*>(field_value);}
        static void* get_Metallic(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->Metallic));}
        static bool isArray_Metallic(){ return false; }
        static const char* getFieldName_Roughness(){ return "Roughness";}
        static const char* getFieldTypeName_Roughness(){ return "float";}
        static void set_Roughness(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->Roughness = *static_cast<float*>(field_value);}
        static void* get_Roughness(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->Roughness));}
        static bool isArray_Roughness(){ return false; }
        static const char* getFieldName_Emisstion(){ return "Emisstion";}
        static const char* getFieldTypeName_Emisstion(){ return "float";}
        static void set_Emisstion(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->Emisstion = *static_cast<float*>(field_value);}
        static void* get_Emisstion(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->Emisstion));}
        static bool isArray_Emisstion(){ return false; }
        static const char* getFieldName_Transparency(){ return "Transparency";}
        static const char* getFieldTypeName_Transparency(){ return "float";}
        static void set_Transparency(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->Transparency = *static_cast<float*>(field_value);}
        static void* get_Transparency(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->Transparency));}
        static bool isArray_Transparency(){ return false; }
        static const char* getFieldName_IsAlbedo(){ return "IsAlbedo";}
        static const char* getFieldTypeName_IsAlbedo(){ return "bool";}
        static void set_IsAlbedo(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->IsAlbedo = *static_cast<bool*>(field_value);}
        static void* get_IsAlbedo(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->IsAlbedo));}
        static bool isArray_IsAlbedo(){ return false; }
        static const char* getFieldName_IsMetallic(){ return "IsMetallic";}
        static const char* getFieldTypeName_IsMetallic(){ return "bool";}
        static void set_IsMetallic(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->IsMetallic = *static_cast<bool*>(field_value);}
        static void* get_IsMetallic(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->IsMetallic));}
        static bool isArray_IsMetallic(){ return false; }
        static const char* getFieldName_IsRoughness(){ return "IsRoughness";}
        static const char* getFieldTypeName_IsRoughness(){ return "bool";}
        static void set_IsRoughness(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->IsRoughness = *static_cast<bool*>(field_value);}
        static void* get_IsRoughness(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->IsRoughness));}
        static bool isArray_IsRoughness(){ return false; }
        static const char* getFieldName_IsNormal(){ return "IsNormal";}
        static const char* getFieldTypeName_IsNormal(){ return "bool";}
        static void set_IsNormal(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->IsNormal = *static_cast<bool*>(field_value);}
        static void* get_IsNormal(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->IsNormal));}
        static bool isArray_IsNormal(){ return false; }
        static const char* getFieldName_IsOcclusion(){ return "IsOcclusion";}
        static const char* getFieldTypeName_IsOcclusion(){ return "bool";}
        static void set_IsOcclusion(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->IsOcclusion = *static_cast<bool*>(field_value);}
        static void* get_IsOcclusion(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->IsOcclusion));}
        static bool isArray_IsOcclusion(){ return false; }
        static const char* getFieldName_IsTransparent(){ return "IsTransparent";}
        static const char* getFieldTypeName_IsTransparent(){ return "bool";}
        static void set_IsTransparent(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->IsTransparent = *static_cast<bool*>(field_value);}
        static void* get_IsTransparent(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->IsTransparent));}
        static bool isArray_IsTransparent(){ return false; }
        static const char* getFieldName_Name(){ return "Name";}
        static const char* getFieldTypeName_Name(){ return "std::string";}
        static void set_Name(void* instance, void* field_value){ static_cast<MaterialDesc*>(instance)->Name = *static_cast<std::string*>(field_value);}
        static void* get_Name(void* instance){ return static_cast<void*>(&(static_cast<MaterialDesc*>(instance)->Name));}
        static bool isArray_Name(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_MaterialDesc(){
        FieldFunctionTuple* f_field_function_tuple_AlbedoMap=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_AlbedoMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_AlbedoMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_AlbedoMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_AlbedoMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_AlbedoMap);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_AlbedoMap);
        FieldFunctionTuple* f_field_function_tuple_NormalMap=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_NormalMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_NormalMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_NormalMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_NormalMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_NormalMap);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_NormalMap);
        FieldFunctionTuple* f_field_function_tuple_MetallicMap=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_MetallicMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_MetallicMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_MetallicMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_MetallicMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_MetallicMap);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_MetallicMap);
        FieldFunctionTuple* f_field_function_tuple_RoughnessMap=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_RoughnessMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_RoughnessMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_RoughnessMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_RoughnessMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_RoughnessMap);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_RoughnessMap);
        FieldFunctionTuple* f_field_function_tuple_OcclusionMap=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_OcclusionMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_OcclusionMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_OcclusionMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_OcclusionMap,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_OcclusionMap);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_OcclusionMap);
        FieldFunctionTuple* f_field_function_tuple_Albedo=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_Albedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_Albedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_Albedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_Albedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_Albedo);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_Albedo);
        FieldFunctionTuple* f_field_function_tuple_Metallic=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_Metallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_Metallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_Metallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_Metallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_Metallic);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_Metallic);
        FieldFunctionTuple* f_field_function_tuple_Roughness=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_Roughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_Roughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_Roughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_Roughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_Roughness);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_Roughness);
        FieldFunctionTuple* f_field_function_tuple_Emisstion=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_Emisstion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_Emisstion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_Emisstion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_Emisstion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_Emisstion);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_Emisstion);
        FieldFunctionTuple* f_field_function_tuple_Transparency=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_Transparency,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_Transparency,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_Transparency,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_Transparency,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_Transparency);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_Transparency);
        FieldFunctionTuple* f_field_function_tuple_IsAlbedo=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_IsAlbedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_IsAlbedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_IsAlbedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_IsAlbedo,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_IsAlbedo);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_IsAlbedo);
        FieldFunctionTuple* f_field_function_tuple_IsMetallic=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_IsMetallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_IsMetallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_IsMetallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_IsMetallic,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_IsMetallic);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_IsMetallic);
        FieldFunctionTuple* f_field_function_tuple_IsRoughness=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_IsRoughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_IsRoughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_IsRoughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_IsRoughness,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_IsRoughness);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_IsRoughness);
        FieldFunctionTuple* f_field_function_tuple_IsNormal=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_IsNormal,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_IsNormal,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_IsNormal,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_IsNormal,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_IsNormal);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_IsNormal);
        FieldFunctionTuple* f_field_function_tuple_IsOcclusion=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_IsOcclusion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_IsOcclusion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_IsOcclusion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_IsOcclusion,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_IsOcclusion);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_IsOcclusion);
        FieldFunctionTuple* f_field_function_tuple_IsTransparent=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_IsTransparent,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_IsTransparent,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_IsTransparent,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_IsTransparent,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_IsTransparent);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_IsTransparent);
        FieldFunctionTuple* f_field_function_tuple_Name=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::set_Name,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::get_Name,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldName_Name,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getFieldTypeName_Name,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::isArray_Name);
        REGISTER_FIELD_TO_MAP("MaterialDesc", f_field_function_tuple_Name);
        
        
        ClassFunctionTuple* f_class_function_tuple_MaterialDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::getMaterialDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeMaterialDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("MaterialDesc", f_class_function_tuple_MaterialDesc);
    }
namespace TypeWrappersRegister{
        void MaterialTableRaw(){ TypeWrapperRegister_MaterialTableRaw();}
        void MaterialDesc(){ TypeWrapperRegister_MaterialDesc();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

