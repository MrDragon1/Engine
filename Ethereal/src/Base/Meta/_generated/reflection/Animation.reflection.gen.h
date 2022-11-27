#pragma once
#include "Resource/Descriptor/Animation.h"

namespace Ethereal{
    class AnimPositionState;
    class AnimRotationState;
    class AnimScaleState;
    class AnimKeyClip;
    class AnimState;
    class AnimationDesc;
    class AnimatorDesc;
    class JointDesc;
    class SkeletonDesc;
namespace Reflection{
namespace TypeFieldReflectionOparator{
    class TypeAnimPositionStateOperator{
    public:
        static const char* getClassName(){ return "AnimPositionState";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimPositionState* ret_instance= new AnimPositionState;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimPositionState*)instance);
        }
        // base class
        static int getAnimPositionStateBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Position(){ return "Position";}
        static const char* getFieldTypeName_Position(){ return "Vector3";}
        static void set_Position(void* instance, void* field_value){ static_cast<AnimPositionState*>(instance)->Position = *static_cast<Vector3*>(field_value);}
        static void* get_Position(void* instance){ return static_cast<void*>(&(static_cast<AnimPositionState*>(instance)->Position));}
        static bool isArray_Position(){ return false; }
        static const char* getFieldName_TimeStamp(){ return "TimeStamp";}
        static const char* getFieldTypeName_TimeStamp(){ return "TimeStamp";}
        static void set_TimeStamp(void* instance, void* field_value){ static_cast<AnimPositionState*>(instance)->TimeStamp = *static_cast<TimeStamp*>(field_value);}
        static void* get_TimeStamp(void* instance){ return static_cast<void*>(&(static_cast<AnimPositionState*>(instance)->TimeStamp));}
        static bool isArray_TimeStamp(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_AnimPositionState(){
        FieldFunctionTuple* f_field_function_tuple_Position=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::set_Position,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::get_Position,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getFieldName_Position,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getFieldTypeName_Position,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::isArray_Position);
        REGISTER_FIELD_TO_MAP("AnimPositionState", f_field_function_tuple_Position);
        FieldFunctionTuple* f_field_function_tuple_TimeStamp=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::set_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::get_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getFieldName_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getFieldTypeName_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::isArray_TimeStamp);
        REGISTER_FIELD_TO_MAP("AnimPositionState", f_field_function_tuple_TimeStamp);
        
        
        ClassFunctionTuple* f_class_function_tuple_AnimPositionState=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::getAnimPositionStateBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimPositionStateOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimPositionState", f_class_function_tuple_AnimPositionState);
    }namespace TypeFieldReflectionOparator{
    class TypeAnimRotationStateOperator{
    public:
        static const char* getClassName(){ return "AnimRotationState";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimRotationState* ret_instance= new AnimRotationState;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimRotationState*)instance);
        }
        // base class
        static int getAnimRotationStateBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Rotation(){ return "Rotation";}
        static const char* getFieldTypeName_Rotation(){ return "Quaternion";}
        static void set_Rotation(void* instance, void* field_value){ static_cast<AnimRotationState*>(instance)->Rotation = *static_cast<Quaternion*>(field_value);}
        static void* get_Rotation(void* instance){ return static_cast<void*>(&(static_cast<AnimRotationState*>(instance)->Rotation));}
        static bool isArray_Rotation(){ return false; }
        static const char* getFieldName_TimeStamp(){ return "TimeStamp";}
        static const char* getFieldTypeName_TimeStamp(){ return "TimeStamp";}
        static void set_TimeStamp(void* instance, void* field_value){ static_cast<AnimRotationState*>(instance)->TimeStamp = *static_cast<TimeStamp*>(field_value);}
        static void* get_TimeStamp(void* instance){ return static_cast<void*>(&(static_cast<AnimRotationState*>(instance)->TimeStamp));}
        static bool isArray_TimeStamp(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_AnimRotationState(){
        FieldFunctionTuple* f_field_function_tuple_Rotation=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::set_Rotation,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::get_Rotation,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getFieldName_Rotation,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getFieldTypeName_Rotation,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::isArray_Rotation);
        REGISTER_FIELD_TO_MAP("AnimRotationState", f_field_function_tuple_Rotation);
        FieldFunctionTuple* f_field_function_tuple_TimeStamp=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::set_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::get_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getFieldName_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getFieldTypeName_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::isArray_TimeStamp);
        REGISTER_FIELD_TO_MAP("AnimRotationState", f_field_function_tuple_TimeStamp);
        
        
        ClassFunctionTuple* f_class_function_tuple_AnimRotationState=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::getAnimRotationStateBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimRotationStateOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimRotationState", f_class_function_tuple_AnimRotationState);
    }namespace TypeFieldReflectionOparator{
    class TypeAnimScaleStateOperator{
    public:
        static const char* getClassName(){ return "AnimScaleState";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimScaleState* ret_instance= new AnimScaleState;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimScaleState*)instance);
        }
        // base class
        static int getAnimScaleStateBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Scale(){ return "Scale";}
        static const char* getFieldTypeName_Scale(){ return "Vector3";}
        static void set_Scale(void* instance, void* field_value){ static_cast<AnimScaleState*>(instance)->Scale = *static_cast<Vector3*>(field_value);}
        static void* get_Scale(void* instance){ return static_cast<void*>(&(static_cast<AnimScaleState*>(instance)->Scale));}
        static bool isArray_Scale(){ return false; }
        static const char* getFieldName_TimeStamp(){ return "TimeStamp";}
        static const char* getFieldTypeName_TimeStamp(){ return "TimeStamp";}
        static void set_TimeStamp(void* instance, void* field_value){ static_cast<AnimScaleState*>(instance)->TimeStamp = *static_cast<TimeStamp*>(field_value);}
        static void* get_TimeStamp(void* instance){ return static_cast<void*>(&(static_cast<AnimScaleState*>(instance)->TimeStamp));}
        static bool isArray_TimeStamp(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_AnimScaleState(){
        FieldFunctionTuple* f_field_function_tuple_Scale=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::set_Scale,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::get_Scale,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getFieldName_Scale,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getFieldTypeName_Scale,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::isArray_Scale);
        REGISTER_FIELD_TO_MAP("AnimScaleState", f_field_function_tuple_Scale);
        FieldFunctionTuple* f_field_function_tuple_TimeStamp=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::set_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::get_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getFieldName_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getFieldTypeName_TimeStamp,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::isArray_TimeStamp);
        REGISTER_FIELD_TO_MAP("AnimScaleState", f_field_function_tuple_TimeStamp);
        
        
        ClassFunctionTuple* f_class_function_tuple_AnimScaleState=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::getAnimScaleStateBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimScaleStateOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimScaleState", f_class_function_tuple_AnimScaleState);
    }namespace TypeFieldReflectionOparator{
    class TypeAnimKeyClipOperator{
    public:
        static const char* getClassName(){ return "AnimKeyClip";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimKeyClip* ret_instance= new AnimKeyClip;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimKeyClip*)instance);
        }
        // base class
        static int getAnimKeyClipBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_PositionStates(){ return "PositionStates";}
        static const char* getFieldTypeName_PositionStates(){ return "std::vector<AnimPositionState>";}
        static void set_PositionStates(void* instance, void* field_value){ static_cast<AnimKeyClip*>(instance)->PositionStates = *static_cast<std::vector<AnimPositionState>*>(field_value);}
        static void* get_PositionStates(void* instance){ return static_cast<void*>(&(static_cast<AnimKeyClip*>(instance)->PositionStates));}
        static bool isArray_PositionStates(){ return true; }
        static const char* getFieldName_RotationStates(){ return "RotationStates";}
        static const char* getFieldTypeName_RotationStates(){ return "std::vector<AnimRotationState>";}
        static void set_RotationStates(void* instance, void* field_value){ static_cast<AnimKeyClip*>(instance)->RotationStates = *static_cast<std::vector<AnimRotationState>*>(field_value);}
        static void* get_RotationStates(void* instance){ return static_cast<void*>(&(static_cast<AnimKeyClip*>(instance)->RotationStates));}
        static bool isArray_RotationStates(){ return true; }
        static const char* getFieldName_ScaleStates(){ return "ScaleStates";}
        static const char* getFieldTypeName_ScaleStates(){ return "std::vector<AnimScaleState>";}
        static void set_ScaleStates(void* instance, void* field_value){ static_cast<AnimKeyClip*>(instance)->ScaleStates = *static_cast<std::vector<AnimScaleState>*>(field_value);}
        static void* get_ScaleStates(void* instance){ return static_cast<void*>(&(static_cast<AnimKeyClip*>(instance)->ScaleStates));}
        static bool isArray_ScaleStates(){ return true; }
        static const char* getFieldName_JointID(){ return "JointID";}
        static const char* getFieldTypeName_JointID(){ return "size_t";}
        static void set_JointID(void* instance, void* field_value){ static_cast<AnimKeyClip*>(instance)->JointID = *static_cast<size_t*>(field_value);}
        static void* get_JointID(void* instance){ return static_cast<void*>(&(static_cast<AnimKeyClip*>(instance)->JointID));}
        static bool isArray_JointID(){ return false; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLAnimPositionStateROperatorMACRO
#define ArraystdSSvectorLAnimPositionStateROperatorMACRO
    class ArraystdSSvectorLAnimPositionStateROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<AnimPositionState>";}
            static const char* getElementTypeName(){ return "AnimPositionState";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<AnimPositionState>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<AnimPositionState>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<AnimPositionState>*>(instance))[index] = *static_cast<AnimPositionState*>(element_value);
            }
    };
#endif //ArraystdSSvectorLAnimPositionStateROperator
#ifndef ArraystdSSvectorLAnimRotationStateROperatorMACRO
#define ArraystdSSvectorLAnimRotationStateROperatorMACRO
    class ArraystdSSvectorLAnimRotationStateROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<AnimRotationState>";}
            static const char* getElementTypeName(){ return "AnimRotationState";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<AnimRotationState>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<AnimRotationState>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<AnimRotationState>*>(instance))[index] = *static_cast<AnimRotationState*>(element_value);
            }
    };
#endif //ArraystdSSvectorLAnimRotationStateROperator
#ifndef ArraystdSSvectorLAnimScaleStateROperatorMACRO
#define ArraystdSSvectorLAnimScaleStateROperatorMACRO
    class ArraystdSSvectorLAnimScaleStateROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<AnimScaleState>";}
            static const char* getElementTypeName(){ return "AnimScaleState";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<AnimScaleState>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<AnimScaleState>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<AnimScaleState>*>(instance))[index] = *static_cast<AnimScaleState*>(element_value);
            }
    };
#endif //ArraystdSSvectorLAnimScaleStateROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_AnimKeyClip(){
        FieldFunctionTuple* f_field_function_tuple_PositionStates=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::set_PositionStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::get_PositionStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldName_PositionStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldTypeName_PositionStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::isArray_PositionStates);
        REGISTER_FIELD_TO_MAP("AnimKeyClip", f_field_function_tuple_PositionStates);
        FieldFunctionTuple* f_field_function_tuple_RotationStates=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::set_RotationStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::get_RotationStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldName_RotationStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldTypeName_RotationStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::isArray_RotationStates);
        REGISTER_FIELD_TO_MAP("AnimKeyClip", f_field_function_tuple_RotationStates);
        FieldFunctionTuple* f_field_function_tuple_ScaleStates=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::set_ScaleStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::get_ScaleStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldName_ScaleStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldTypeName_ScaleStates,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::isArray_ScaleStates);
        REGISTER_FIELD_TO_MAP("AnimKeyClip", f_field_function_tuple_ScaleStates);
        FieldFunctionTuple* f_field_function_tuple_JointID=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::set_JointID,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::get_JointID,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldName_JointID,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getFieldTypeName_JointID,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::isArray_JointID);
        REGISTER_FIELD_TO_MAP("AnimKeyClip", f_field_function_tuple_JointID);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLAnimPositionStateR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLAnimPositionStateROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimPositionStateROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimPositionStateROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimPositionStateROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimPositionStateROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<AnimPositionState>", f_array_tuple_stdSSvectorLAnimPositionStateR);
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLAnimRotationStateR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLAnimRotationStateROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimRotationStateROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimRotationStateROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimRotationStateROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimRotationStateROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<AnimRotationState>", f_array_tuple_stdSSvectorLAnimRotationStateR);
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLAnimScaleStateR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLAnimScaleStateROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimScaleStateROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimScaleStateROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimScaleStateROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimScaleStateROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<AnimScaleState>", f_array_tuple_stdSSvectorLAnimScaleStateR);
        ClassFunctionTuple* f_class_function_tuple_AnimKeyClip=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::getAnimKeyClipBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimKeyClipOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimKeyClip", f_class_function_tuple_AnimKeyClip);
    }namespace TypeFieldReflectionOparator{
    class TypeAnimStateOperator{
    public:
        static const char* getClassName(){ return "AnimState";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimState* ret_instance= new AnimState;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimState*)instance);
        }
        // base class
        static int getAnimStateBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_PositionState(){ return "PositionState";}
        static const char* getFieldTypeName_PositionState(){ return "AnimPositionState";}
        static void set_PositionState(void* instance, void* field_value){ static_cast<AnimState*>(instance)->PositionState = *static_cast<AnimPositionState*>(field_value);}
        static void* get_PositionState(void* instance){ return static_cast<void*>(&(static_cast<AnimState*>(instance)->PositionState));}
        static bool isArray_PositionState(){ return false; }
        static const char* getFieldName_RotationState(){ return "RotationState";}
        static const char* getFieldTypeName_RotationState(){ return "AnimRotationState";}
        static void set_RotationState(void* instance, void* field_value){ static_cast<AnimState*>(instance)->RotationState = *static_cast<AnimRotationState*>(field_value);}
        static void* get_RotationState(void* instance){ return static_cast<void*>(&(static_cast<AnimState*>(instance)->RotationState));}
        static bool isArray_RotationState(){ return false; }
        static const char* getFieldName_ScaleState(){ return "ScaleState";}
        static const char* getFieldTypeName_ScaleState(){ return "AnimScaleState";}
        static void set_ScaleState(void* instance, void* field_value){ static_cast<AnimState*>(instance)->ScaleState = *static_cast<AnimScaleState*>(field_value);}
        static void* get_ScaleState(void* instance){ return static_cast<void*>(&(static_cast<AnimState*>(instance)->ScaleState));}
        static bool isArray_ScaleState(){ return false; }
        static const char* getFieldName_JointID(){ return "JointID";}
        static const char* getFieldTypeName_JointID(){ return "size_t";}
        static void set_JointID(void* instance, void* field_value){ static_cast<AnimState*>(instance)->JointID = *static_cast<size_t*>(field_value);}
        static void* get_JointID(void* instance){ return static_cast<void*>(&(static_cast<AnimState*>(instance)->JointID));}
        static bool isArray_JointID(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_AnimState(){
        FieldFunctionTuple* f_field_function_tuple_PositionState=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimStateOperator::set_PositionState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::get_PositionState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldName_PositionState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldTypeName_PositionState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::isArray_PositionState);
        REGISTER_FIELD_TO_MAP("AnimState", f_field_function_tuple_PositionState);
        FieldFunctionTuple* f_field_function_tuple_RotationState=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimStateOperator::set_RotationState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::get_RotationState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldName_RotationState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldTypeName_RotationState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::isArray_RotationState);
        REGISTER_FIELD_TO_MAP("AnimState", f_field_function_tuple_RotationState);
        FieldFunctionTuple* f_field_function_tuple_ScaleState=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimStateOperator::set_ScaleState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::get_ScaleState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldName_ScaleState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldTypeName_ScaleState,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::isArray_ScaleState);
        REGISTER_FIELD_TO_MAP("AnimState", f_field_function_tuple_ScaleState);
        FieldFunctionTuple* f_field_function_tuple_JointID=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimStateOperator::set_JointID,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::get_JointID,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldName_JointID,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getFieldTypeName_JointID,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::isArray_JointID);
        REGISTER_FIELD_TO_MAP("AnimState", f_field_function_tuple_JointID);
        
        
        ClassFunctionTuple* f_class_function_tuple_AnimState=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimStateOperator::getAnimStateBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimStateOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimState", f_class_function_tuple_AnimState);
    }namespace TypeFieldReflectionOparator{
    class TypeAnimationDescOperator{
    public:
        static const char* getClassName(){ return "AnimationDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimationDesc* ret_instance= new AnimationDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimationDesc*)instance);
        }
        // base class
        static int getAnimationDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<AnimationDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Clips(){ return "Clips";}
        static const char* getFieldTypeName_Clips(){ return "std::vector<AnimKeyClip>";}
        static void set_Clips(void* instance, void* field_value){ static_cast<AnimationDesc*>(instance)->Clips = *static_cast<std::vector<AnimKeyClip>*>(field_value);}
        static void* get_Clips(void* instance){ return static_cast<void*>(&(static_cast<AnimationDesc*>(instance)->Clips));}
        static bool isArray_Clips(){ return true; }
        static const char* getFieldName_FramesPersecond(){ return "FramesPersecond";}
        static const char* getFieldTypeName_FramesPersecond(){ return "double";}
        static void set_FramesPersecond(void* instance, void* field_value){ static_cast<AnimationDesc*>(instance)->FramesPersecond = *static_cast<double*>(field_value);}
        static void* get_FramesPersecond(void* instance){ return static_cast<void*>(&(static_cast<AnimationDesc*>(instance)->FramesPersecond));}
        static bool isArray_FramesPersecond(){ return false; }
        static const char* getFieldName_Duration(){ return "Duration";}
        static const char* getFieldTypeName_Duration(){ return "TimeStamp";}
        static void set_Duration(void* instance, void* field_value){ static_cast<AnimationDesc*>(instance)->Duration = *static_cast<TimeStamp*>(field_value);}
        static void* get_Duration(void* instance){ return static_cast<void*>(&(static_cast<AnimationDesc*>(instance)->Duration));}
        static bool isArray_Duration(){ return false; }
        static const char* getFieldName_Name(){ return "Name";}
        static const char* getFieldTypeName_Name(){ return "std::string";}
        static void set_Name(void* instance, void* field_value){ static_cast<AnimationDesc*>(instance)->Name = *static_cast<std::string*>(field_value);}
        static void* get_Name(void* instance){ return static_cast<void*>(&(static_cast<AnimationDesc*>(instance)->Name));}
        static bool isArray_Name(){ return false; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLAnimKeyClipROperatorMACRO
#define ArraystdSSvectorLAnimKeyClipROperatorMACRO
    class ArraystdSSvectorLAnimKeyClipROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<AnimKeyClip>";}
            static const char* getElementTypeName(){ return "AnimKeyClip";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<AnimKeyClip>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<AnimKeyClip>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<AnimKeyClip>*>(instance))[index] = *static_cast<AnimKeyClip*>(element_value);
            }
    };
#endif //ArraystdSSvectorLAnimKeyClipROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_AnimationDesc(){
        FieldFunctionTuple* f_field_function_tuple_Clips=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::set_Clips,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::get_Clips,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldName_Clips,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldTypeName_Clips,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::isArray_Clips);
        REGISTER_FIELD_TO_MAP("AnimationDesc", f_field_function_tuple_Clips);
        FieldFunctionTuple* f_field_function_tuple_FramesPersecond=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::set_FramesPersecond,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::get_FramesPersecond,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldName_FramesPersecond,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldTypeName_FramesPersecond,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::isArray_FramesPersecond);
        REGISTER_FIELD_TO_MAP("AnimationDesc", f_field_function_tuple_FramesPersecond);
        FieldFunctionTuple* f_field_function_tuple_Duration=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::set_Duration,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::get_Duration,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldName_Duration,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldTypeName_Duration,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::isArray_Duration);
        REGISTER_FIELD_TO_MAP("AnimationDesc", f_field_function_tuple_Duration);
        FieldFunctionTuple* f_field_function_tuple_Name=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::set_Name,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::get_Name,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldName_Name,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getFieldTypeName_Name,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::isArray_Name);
        REGISTER_FIELD_TO_MAP("AnimationDesc", f_field_function_tuple_Name);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLAnimKeyClipR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLAnimKeyClipROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimKeyClipROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimKeyClipROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimKeyClipROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLAnimKeyClipROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<AnimKeyClip>", f_array_tuple_stdSSvectorLAnimKeyClipR);
        ClassFunctionTuple* f_class_function_tuple_AnimationDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::getAnimationDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimationDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimationDesc", f_class_function_tuple_AnimationDesc);
    }namespace TypeFieldReflectionOparator{
    class TypeAnimatorDescOperator{
    public:
        static const char* getClassName(){ return "AnimatorDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            AnimatorDesc* ret_instance= new AnimatorDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(AnimatorDesc*)instance);
        }
        // base class
        static int getAnimatorDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<AnimatorDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_Animation(){ return "Animation";}
        static const char* getFieldTypeName_Animation(){ return "UUID";}
        static void set_Animation(void* instance, void* field_value){ static_cast<AnimatorDesc*>(instance)->Animation = *static_cast<UUID*>(field_value);}
        static void* get_Animation(void* instance){ return static_cast<void*>(&(static_cast<AnimatorDesc*>(instance)->Animation));}
        static bool isArray_Animation(){ return false; }
        static const char* getFieldName_Skeleton(){ return "Skeleton";}
        static const char* getFieldTypeName_Skeleton(){ return "UUID";}
        static void set_Skeleton(void* instance, void* field_value){ static_cast<AnimatorDesc*>(instance)->Skeleton = *static_cast<UUID*>(field_value);}
        static void* get_Skeleton(void* instance){ return static_cast<void*>(&(static_cast<AnimatorDesc*>(instance)->Skeleton));}
        static bool isArray_Skeleton(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_AnimatorDesc(){
        FieldFunctionTuple* f_field_function_tuple_Animation=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::set_Animation,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::get_Animation,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getFieldName_Animation,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getFieldTypeName_Animation,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::isArray_Animation);
        REGISTER_FIELD_TO_MAP("AnimatorDesc", f_field_function_tuple_Animation);
        FieldFunctionTuple* f_field_function_tuple_Skeleton=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::set_Skeleton,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::get_Skeleton,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getFieldName_Skeleton,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getFieldTypeName_Skeleton,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::isArray_Skeleton);
        REGISTER_FIELD_TO_MAP("AnimatorDesc", f_field_function_tuple_Skeleton);
        
        
        ClassFunctionTuple* f_class_function_tuple_AnimatorDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::getAnimatorDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeAnimatorDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("AnimatorDesc", f_class_function_tuple_AnimatorDesc);
    }namespace TypeFieldReflectionOparator{
    class TypeJointDescOperator{
    public:
        static const char* getClassName(){ return "JointDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            JointDesc* ret_instance= new JointDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(JointDesc*)instance);
        }
        // base class
        static int getJointDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 0;
            
            return count;
        }
        // fields
        static const char* getFieldName_Children(){ return "Children";}
        static const char* getFieldTypeName_Children(){ return "std::vector<JointDesc>";}
        static void set_Children(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->Children = *static_cast<std::vector<JointDesc>*>(field_value);}
        static void* get_Children(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->Children));}
        static bool isArray_Children(){ return true; }
        static const char* getFieldName_Name(){ return "Name";}
        static const char* getFieldTypeName_Name(){ return "std::string";}
        static void set_Name(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->Name = *static_cast<std::string*>(field_value);}
        static void* get_Name(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->Name));}
        static bool isArray_Name(){ return false; }
        static const char* getFieldName_ID(){ return "ID";}
        static const char* getFieldTypeName_ID(){ return "size_t";}
        static void set_ID(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->ID = *static_cast<size_t*>(field_value);}
        static void* get_ID(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->ID));}
        static bool isArray_ID(){ return false; }
        static const char* getFieldName_InitialPosition(){ return "InitialPosition";}
        static const char* getFieldTypeName_InitialPosition(){ return "Vector3";}
        static void set_InitialPosition(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->InitialPosition = *static_cast<Vector3*>(field_value);}
        static void* get_InitialPosition(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->InitialPosition));}
        static bool isArray_InitialPosition(){ return false; }
        static const char* getFieldName_InitialRotation(){ return "InitialRotation";}
        static const char* getFieldTypeName_InitialRotation(){ return "Quaternion";}
        static void set_InitialRotation(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->InitialRotation = *static_cast<Quaternion*>(field_value);}
        static void* get_InitialRotation(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->InitialRotation));}
        static bool isArray_InitialRotation(){ return false; }
        static const char* getFieldName_InitialScale(){ return "InitialScale";}
        static const char* getFieldTypeName_InitialScale(){ return "Vector3";}
        static void set_InitialScale(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->InitialScale = *static_cast<Vector3*>(field_value);}
        static void* get_InitialScale(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->InitialScale));}
        static bool isArray_InitialScale(){ return false; }
        static const char* getFieldName_OffsetMatrix(){ return "OffsetMatrix";}
        static const char* getFieldTypeName_OffsetMatrix(){ return "Matrix4";}
        static void set_OffsetMatrix(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->OffsetMatrix = *static_cast<Matrix4*>(field_value);}
        static void* get_OffsetMatrix(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->OffsetMatrix));}
        static bool isArray_OffsetMatrix(){ return false; }
        static const char* getFieldName_InverseOffsetMatrix(){ return "InverseOffsetMatrix";}
        static const char* getFieldTypeName_InverseOffsetMatrix(){ return "Matrix4";}
        static void set_InverseOffsetMatrix(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->InverseOffsetMatrix = *static_cast<Matrix4*>(field_value);}
        static void* get_InverseOffsetMatrix(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->InverseOffsetMatrix));}
        static bool isArray_InverseOffsetMatrix(){ return false; }
        static const char* getFieldName_LocalTransform(){ return "LocalTransform";}
        static const char* getFieldTypeName_LocalTransform(){ return "Matrix4";}
        static void set_LocalTransform(void* instance, void* field_value){ static_cast<JointDesc*>(instance)->LocalTransform = *static_cast<Matrix4*>(field_value);}
        static void* get_LocalTransform(void* instance){ return static_cast<void*>(&(static_cast<JointDesc*>(instance)->LocalTransform));}
        static bool isArray_LocalTransform(){ return false; }
    };
}//namespace TypeFieldReflectionOparator
namespace ArrayReflectionOperator{
#ifndef ArraystdSSvectorLJointDescROperatorMACRO
#define ArraystdSSvectorLJointDescROperatorMACRO
    class ArraystdSSvectorLJointDescROperator{
        public:
            static const char* getArrayTypeName(){ return "std::vector<JointDesc>";}
            static const char* getElementTypeName(){ return "JointDesc";}
            static int getSize(void* instance){
                //todo: should check validation
                return static_cast<int>(static_cast<std::vector<JointDesc>*>(instance)->size());
            }
            static void* get(int index,void* instance){
                //todo: should check validation
                return static_cast<void*>(&((*static_cast<std::vector<JointDesc>*>(instance))[index]));
            }
            static void set(int index, void* instance, void* element_value){
                //todo: should check validation
                (*static_cast<std::vector<JointDesc>*>(instance))[index] = *static_cast<JointDesc*>(element_value);
            }
    };
#endif //ArraystdSSvectorLJointDescROperator
}//namespace ArrayReflectionOperator

    void TypeWrapperRegister_JointDesc(){
        FieldFunctionTuple* f_field_function_tuple_Children=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_Children,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_Children,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_Children,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_Children,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_Children);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_Children);
        FieldFunctionTuple* f_field_function_tuple_Name=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_Name,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_Name,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_Name,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_Name,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_Name);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_Name);
        FieldFunctionTuple* f_field_function_tuple_ID=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_ID,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_ID,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_ID,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_ID,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_ID);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_ID);
        FieldFunctionTuple* f_field_function_tuple_InitialPosition=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_InitialPosition,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_InitialPosition,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_InitialPosition,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_InitialPosition,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_InitialPosition);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_InitialPosition);
        FieldFunctionTuple* f_field_function_tuple_InitialRotation=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_InitialRotation,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_InitialRotation,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_InitialRotation,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_InitialRotation,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_InitialRotation);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_InitialRotation);
        FieldFunctionTuple* f_field_function_tuple_InitialScale=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_InitialScale,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_InitialScale,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_InitialScale,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_InitialScale,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_InitialScale);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_InitialScale);
        FieldFunctionTuple* f_field_function_tuple_OffsetMatrix=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_OffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_OffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_OffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_OffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_OffsetMatrix);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_OffsetMatrix);
        FieldFunctionTuple* f_field_function_tuple_InverseOffsetMatrix=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_InverseOffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_InverseOffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_InverseOffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_InverseOffsetMatrix,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_InverseOffsetMatrix);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_InverseOffsetMatrix);
        FieldFunctionTuple* f_field_function_tuple_LocalTransform=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::set_LocalTransform,
            &TypeFieldReflectionOparator::TypeJointDescOperator::get_LocalTransform,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldName_LocalTransform,
            &TypeFieldReflectionOparator::TypeJointDescOperator::getFieldTypeName_LocalTransform,
            &TypeFieldReflectionOparator::TypeJointDescOperator::isArray_LocalTransform);
        REGISTER_FIELD_TO_MAP("JointDesc", f_field_function_tuple_LocalTransform);
        
        ArrayFunctionTuple* f_array_tuple_stdSSvectorLJointDescR = new  ArrayFunctionTuple(
            &ArrayReflectionOperator::ArraystdSSvectorLJointDescROperator::set,
            &ArrayReflectionOperator::ArraystdSSvectorLJointDescROperator::get,
            &ArrayReflectionOperator::ArraystdSSvectorLJointDescROperator::getSize,
            &ArrayReflectionOperator::ArraystdSSvectorLJointDescROperator::getArrayTypeName,
            &ArrayReflectionOperator::ArraystdSSvectorLJointDescROperator::getElementTypeName);
        REGISTER_ARRAY_TO_MAP("std::vector<JointDesc>", f_array_tuple_stdSSvectorLJointDescR);
        ClassFunctionTuple* f_class_function_tuple_JointDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeJointDescOperator::getJointDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeJointDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeJointDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("JointDesc", f_class_function_tuple_JointDesc);
    }namespace TypeFieldReflectionOparator{
    class TypeSkeletonDescOperator{
    public:
        static const char* getClassName(){ return "SkeletonDesc";}
        static void* constructorWithYNode(const YNode& yaml_context){
            SkeletonDesc* ret_instance= new SkeletonDesc;
            Serializer::read(yaml_context, *ret_instance);
            return ret_instance;
        }
        static YNode writeByName(void* instance){
            return Serializer::write(*(SkeletonDesc*)instance);
        }
        // base class
        static int getSkeletonDescBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance){
            int count = 1;
            out_list = new ReflectionInstance[count];
            for (int i=0;i<count;++i){
               out_list[i] = TypeMetaDef(Ethereal::Asset,static_cast<SkeletonDesc*>(instance));
            }
            return count;
        }
        // fields
        static const char* getFieldName_RootJoint(){ return "RootJoint";}
        static const char* getFieldTypeName_RootJoint(){ return "JointDesc";}
        static void set_RootJoint(void* instance, void* field_value){ static_cast<SkeletonDesc*>(instance)->RootJoint = *static_cast<JointDesc*>(field_value);}
        static void* get_RootJoint(void* instance){ return static_cast<void*>(&(static_cast<SkeletonDesc*>(instance)->RootJoint));}
        static bool isArray_RootJoint(){ return false; }
        static const char* getFieldName_Name(){ return "Name";}
        static const char* getFieldTypeName_Name(){ return "std::string";}
        static void set_Name(void* instance, void* field_value){ static_cast<SkeletonDesc*>(instance)->Name = *static_cast<std::string*>(field_value);}
        static void* get_Name(void* instance){ return static_cast<void*>(&(static_cast<SkeletonDesc*>(instance)->Name));}
        static bool isArray_Name(){ return false; }
    };
}//namespace TypeFieldReflectionOparator


    void TypeWrapperRegister_SkeletonDesc(){
        FieldFunctionTuple* f_field_function_tuple_RootJoint=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::set_RootJoint,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::get_RootJoint,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getFieldName_RootJoint,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getFieldTypeName_RootJoint,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::isArray_RootJoint);
        REGISTER_FIELD_TO_MAP("SkeletonDesc", f_field_function_tuple_RootJoint);
        FieldFunctionTuple* f_field_function_tuple_Name=new FieldFunctionTuple(
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::set_Name,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::get_Name,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getClassName,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getFieldName_Name,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getFieldTypeName_Name,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::isArray_Name);
        REGISTER_FIELD_TO_MAP("SkeletonDesc", f_field_function_tuple_Name);
        
        
        ClassFunctionTuple* f_class_function_tuple_SkeletonDesc=new ClassFunctionTuple(
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::getSkeletonDescBaseClassReflectionInstanceList,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::constructorWithYNode,
            &TypeFieldReflectionOparator::TypeSkeletonDescOperator::writeByName);
        REGISTER_BASE_CLASS_TO_MAP("SkeletonDesc", f_class_function_tuple_SkeletonDesc);
    }
namespace TypeWrappersRegister{
        void AnimPositionState(){ TypeWrapperRegister_AnimPositionState();}
        void AnimRotationState(){ TypeWrapperRegister_AnimRotationState();}
        void AnimScaleState(){ TypeWrapperRegister_AnimScaleState();}
        void AnimKeyClip(){ TypeWrapperRegister_AnimKeyClip();}
        void AnimState(){ TypeWrapperRegister_AnimState();}
        void AnimationDesc(){ TypeWrapperRegister_AnimationDesc();}
        void AnimatorDesc(){ TypeWrapperRegister_AnimatorDesc();}
        void JointDesc(){ TypeWrapperRegister_JointDesc();}
        void SkeletonDesc(){ TypeWrapperRegister_SkeletonDesc();}
}//namespace TypeWrappersRegister

}//namespace Reflection
}//namespace Ethereal

