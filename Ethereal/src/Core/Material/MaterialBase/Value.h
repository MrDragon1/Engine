#pragma once
#include "Base/Base.h"
#include "Core/Material/MaterialBase/MaterialDefine.h"

namespace Ethereal {
using ShaderContextPtr = Ref<class ShaderContext>;
using ShaderStagePtr = Ref<class ShaderStage>;

using ValueBasePtr = Ref<class ValueBase>;

template <class T>
class Value;
struct ValueProperty {
    string DefaultValue;
    string Syntax;
    string SyntaxInitilize;
    string Definition;
};

class ValueBase : public RefCounted {
   public:
    ValueBase() {}
    virtual ~ValueBase() {}

    template <class T>
    static ValueBasePtr CreateValue(const T& data) {
        return Ref<Value<T>>::Create(data);
    }

    static ValueBasePtr CreateValueFromString(const string& value, const string& type);
    static ValueBasePtr CreateValueWithType(const string& type);
    static void EmitTypeDefine(ShaderContextPtr context, ShaderStagePtr stage);

    virtual ValueBasePtr Copy() const = 0;

    virtual string GetValueString() const = 0;
    virtual string GetTypeString() const = 0;

    virtual string GetDefaultValueString() const = 0;
    virtual string GetSyntaxString() const = 0;
    virtual string GetSyntaxInitilizeString() const = 0;
    virtual string GetDefinitionString() const = 0;

    template <class T>
    bool Is() const;

    template <class T>
    T* GetPtr();

    template <class T>
    void SetData(const T& value) {
        Value<T>* typedVal = dynamic_cast<Value<T>*>(this);
        typedVal->SetData(value);
    }

    template <typename T>
    T GetData() {
        Value<T>* typedVal = dynamic_cast<Value<T>*>(this);
        return typedVal->GetData();
    }

   protected:
    template <class T>
    friend class ValueRegistry;

    using CreatorFunction = ValueBasePtr (*)(const string&);
    using CreatorMap = std::unordered_map<string, CreatorFunction>;

   private:
    static CreatorMap sCreatorMap;
    static std::unordered_map<string, ValueProperty> sValueMap;
};

template <class T>
T* ValueBase::GetPtr() {
    Value<T>* typedVal = dynamic_cast<Value<T>*>(this);
    return typedVal->GetDataPtr();
}

template <class T>
bool ValueBase::Is() const {
    return dynamic_cast<const Value<T>*>(this) != nullptr;
}

template <class T>
class Value : public ValueBase {
   public:
    Value() : mData{} {}

    explicit Value(const T& value) : mData(value) {}

    virtual ~Value() {}

    ValueBasePtr Copy() const override { return ValueBase::CreateValue<T>(mData); }

    void SetData(const T& value) { mData = value; }

    void SetData(const Value<T>& value) { mData = value.data; }

    T GetData() const { return mData; }

    T* GetDataPtr() { return &mData; }

    static ValueBasePtr CreateFromString(const string& value);

    string GetTypeString() const override;
    string GetValueString() const override;

    string GetDefaultValueString() const override;
    string GetSyntaxString() const override;
    string GetSyntaxInitilizeString() const override;
    string GetDefinitionString() const override;

   public:
    static const string sType;
    static const string sDefaultValue;
    static const string sSyntax;
    static const string sSyntaxInitilize;
    static const string sDefinition;

   private:
    T mData;
};

template <class T>
ValueBasePtr Value<T>::CreateFromString(const string& value) {
    return ValueBase::CreateValue<T>(FromValueString<T>(value));
}

}  // namespace Ethereal

template <class T>
const std::string& GetTypeString();

template <class T>
std::string ToValueString(const T& data);

template <class T>
T FromValueString(const std::string& value);
