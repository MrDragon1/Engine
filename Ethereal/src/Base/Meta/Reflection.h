#pragma once
#include "Utils/Yaml.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Ethereal {

#ifdef __REFLECTION_PARSER__
#define META(...) __attribute__((annotate(#__VA_ARGS__)))
#define CLASS(class_name, ...) class __attribute__((annotate(#__VA_ARGS__))) class_name
#define STRUCT(struct_name, ...) struct __attribute__((annotate(#__VA_ARGS__))) struct_name
#else
#define META(...)
#define CLASS(class_name, ...) class class_name
#define STRUCT(struct_name, ...) struct struct_name
#endif

#define REFLECTION_BODY(class_name)                                                   \
    friend class Reflection::TypeFieldReflectionOparator::Type##class_name##Operator; \
    friend class Serializer;

#define REFLECTION_TYPE(class_name)         \
    namespace Reflection {                  \
    namespace TypeFieldReflectionOparator { \
    class Type##class_name##Operator;       \
    }                                       \
    };

#define REGISTER_FIELD_TO_MAP(name, value) TypeMetaRegisterinterface::registerToFieldMap(name, value);
#define REGISTER_BASE_CLASS_TO_MAP(name, value) TypeMetaRegisterinterface::registerToClassMap(name, value);
#define REGISTER_ARRAY_TO_MAP(name, value) TypeMetaRegisterinterface::registerToArrayMap(name, value);
#define UNREGISTER_ALL TypeMetaRegisterinterface::unregisterAll();

#define PICCOLO_REFLECTION_NEW(name, ...) Reflection::ReflectionPtr(#name, new name(__VA_ARGS__));
#define PICCOLO_REFLECTION_DELETE(value)   \
    if (value) {                           \
        delete value.operator->();         \
        value.getPtrReference() = nullptr; \
    }
#define PICCOLO_REFLECTION_DEEP_COPY(type, dst_ptr, src_ptr) *static_cast<type *>(dst_ptr) = *static_cast<type *>(src_ptr.getPtr());

#define TypeMetaDef(class_name, ptr) Reflection::ReflectionInstance(Reflection::TypeMeta::newMetaFromName(#class_name), (class_name *)ptr)

#define TypeMetaDefPtr(class_name, ptr) \
    new Piccolo::Reflection::ReflectionInstance(Piccolo::Reflection::TypeMeta::newMetaFromName(#class_name), (class_name *)ptr)

template <typename T, typename U, typename = void>
struct is_safely_castable : std::false_type {};

template <typename T, typename U>
struct is_safely_castable<T, U, std::void_t<decltype(static_cast<U>(std::declval<T>()))>> : std::true_type {};

namespace Reflection {
class TypeMeta;

class FieldAccessor;

class ArrayAccessor;

class ReflectionInstance;
}  // namespace Reflection
typedef std::function<void(void *, void *)> SetFuncion;
typedef std::function<void *(void *)> GetFuncion;
typedef std::function<const char *()> GetNameFuncion;
typedef std::function<void(int, void *, void *)> SetArrayFunc;
typedef std::function<void *(int, void *)> GetArrayFunc;
typedef std::function<int(void *)> GetSizeFunc;
typedef std::function<bool()> GetBoolFunc;

typedef std::function<void *(const YNode &)> ConstructorWithYAML;
typedef std::function<YNode(void *)> WriteYAMLByName;
typedef std::function<int(Reflection::ReflectionInstance *&, void *)> GetBaseClassReflectionInstanceListFunc;

typedef std::tuple<SetFuncion, GetFuncion, GetNameFuncion, GetNameFuncion, GetNameFuncion, GetBoolFunc> FieldFunctionTuple;
typedef std::tuple<GetBaseClassReflectionInstanceListFunc, ConstructorWithYAML, WriteYAMLByName> ClassFunctionTuple;
typedef std::tuple<SetArrayFunc, GetArrayFunc, GetSizeFunc, GetNameFuncion, GetNameFuncion> ArrayFunctionTuple;

namespace Reflection {
class TypeMetaRegisterinterface {
   public:
    static void registerToClassMap(const char *name, ClassFunctionTuple *value);

    static void registerToFieldMap(const char *name, FieldFunctionTuple *value);

    static void registerToArrayMap(const char *name, ArrayFunctionTuple *value);

    static void unregisterAll();
};

class TypeMeta {
    friend class FieldAccessor;

    friend class ArrayAccessor;

    friend class TypeMetaRegisterinterface;

   public:
    TypeMeta();

    // static void Register();

    static TypeMeta newMetaFromName(std::string type_name);

    static bool newArrayAccessorFromName(std::string array_type_name, ArrayAccessor &accessor);

    static ReflectionInstance newFromNameAndYAML(std::string type_name, const YNode &yaml_context);

    static YNode writeByName(std::string type_name, void *instance);

    std::string getTypeName();

    int getFieldsList(FieldAccessor *&out_list);

    int getBaseClassReflectionInstanceList(ReflectionInstance *&out_list, void *instance);

    FieldAccessor getFieldByName(const char *name);

    bool isValid() { return mis_valid; }

    TypeMeta &operator=(const TypeMeta &dest);

   private:
    TypeMeta(std::string type_name);

   private:
    std::vector<FieldAccessor, std::allocator<FieldAccessor>> mfields;

    std::string mTypeName;

    bool mis_valid;
};

class FieldAccessor {
    friend class TypeMeta;

   public:
    FieldAccessor();

    void *get(void *instance);

    void set(void *instance, void *value);

    TypeMeta getOwnerTypeMeta();

    /**
     * param: TypeMeta out_type
     *        a reference of TypeMeta
     *
     * return: bool value
     *        true: it's a reflection type
     *        false: it's not a reflection type
     */
    bool getTypeMeta(TypeMeta &field_type);

    const char *getFieldName() const;

    const char *getFieldTypeName();

    bool isArrayType();

    FieldAccessor &operator=(const FieldAccessor &dest);

   private:
    FieldAccessor(FieldFunctionTuple *functions);

   private:
    FieldFunctionTuple *mFunctions;
    const char *mfield_name;
    const char *mfield_type_name;
};

/**
 *  Function reflection is not implemented, so use this as an std::vector accessor
 */
class ArrayAccessor {
    friend class TypeMeta;

   public:
    ArrayAccessor();

    const char *getArrayTypeName();

    const char *getElementTypeName();

    void set(int index, void *instance, void *element_value);

    void *get(int index, void *instance);

    int getSize(void *instance);

    ArrayAccessor &operator=(ArrayAccessor &dest);

   private:
    ArrayAccessor(ArrayFunctionTuple *array_func);

   private:
    ArrayFunctionTuple *mFunc;
    const char *mArrayTypeName;
    const char *mElementTypeName;
};

class ReflectionInstance {
   public:
    ReflectionInstance(TypeMeta meta, void *instance) : mMeta(meta), mInstance(instance) {}

    ReflectionInstance() : mMeta(), mInstance(nullptr) {}

    ReflectionInstance &operator=(ReflectionInstance &dest);

    ReflectionInstance &operator=(ReflectionInstance &&dest);

   public:
    TypeMeta mMeta;
    void *mInstance;
};

template <typename T>
class ReflectionPtr {
    template <typename U>
    friend class ReflectionPtr;

   public:
    ReflectionPtr(std::string type_name, T *instance) : mTypeName(type_name), mInstance(instance) {}

    ReflectionPtr() : mTypeName(), mInstance(nullptr) {}

    ReflectionPtr(const ReflectionPtr &dest) : mTypeName(dest.mTypeName), mInstance(dest.mInstance) {}

    template <typename U /*, typename = typename std::enable_if<std::is_safely_castable<T*, U*>::value>::type */>
    ReflectionPtr<T> &operator=(const ReflectionPtr<U> &dest) {
        if (this == static_cast<void *>(&dest)) {
            return *this;
        }
        mTypeName = dest.mTypeName;
        mInstance = static_cast<T *>(dest.mInstance);
        return *this;
    }

    template <typename U /*, typename = typename std::enable_if<std::is_safely_castable<T*, U*>::value>::type*/>
    ReflectionPtr<T> &operator=(ReflectionPtr<U> &&dest) {
        if (this == static_cast<void *>(&dest)) {
            return *this;
        }
        mTypeName = dest.mTypeName;
        mInstance = static_cast<T *>(dest.mInstance);
        return *this;
    }

    ReflectionPtr<T> &operator=(const ReflectionPtr<T> &dest) {
        if (this == &dest) {
            return *this;
        }
        mTypeName = dest.mTypeName;
        mInstance = dest.mInstance;
        return *this;
    }

    ReflectionPtr<T> &operator=(ReflectionPtr<T> &&dest) {
        if (this == &dest) {
            return *this;
        }
        mTypeName = dest.mTypeName;
        mInstance = dest.mInstance;
        return *this;
    }

    std::string getTypeName() const { return mTypeName; }

    void setTypeName(std::string name) { mTypeName = name; }

    bool operator==(const T *ptr) const { return (mInstance == ptr); }

    bool operator!=(const T *ptr) const { return (mInstance != ptr); }

    bool operator==(const ReflectionPtr<T> &rhs_ptr) const { return (mInstance == rhs_ptr.mInstance); }

    bool operator!=(const ReflectionPtr<T> &rhs_ptr) const { return (mInstance != rhs_ptr.mInstance); }

    template <typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    explicit operator T1 *() {
        return static_cast<T1 *>(mInstance);
    }

    template <typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    operator ReflectionPtr<T1>() {
        return ReflectionPtr<T1>(mTypeName, (T1 *)(mInstance));
    }

    template <typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    explicit operator const T1 *() const {
        return static_cast<T1 *>(mInstance);
    }

    template <typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    operator const ReflectionPtr<T1>() const {
        return ReflectionPtr<T1>(mTypeName, (T1 *)(mInstance));
    }

    T *operator->() { return mInstance; }

    T *operator->() const { return mInstance; }

    T &operator*() { return *(mInstance); }

    T *getPtr() { return mInstance; }

    T *getPtr() const { return mInstance; }

    const T &operator*() const { return *(static_cast<const T *>(mInstance)); }

    T *&getPtrReference() { return mInstance; }

    operator bool() const { return (mInstance != nullptr); }

   private:
    std::string mTypeName{""};
    typedef T mType;
    T *mInstance{nullptr};
};

}  // namespace Reflection

}  // namespace Ethereal