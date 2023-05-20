#pragma once
#include "Base/Base.h"
#include "Core/Material/MaterialBase/MaterialDefine.h"

namespace Ethereal {

using ValueBasePtr = Ref<class ValueBase>;

template <class T>
class Value;

class ValueBase : public RefCounted {
   public:
    ValueBase() {}
    virtual ~ValueBase() {}

    template <class T>
    static ValueBasePtr CreateValue(const T& data) {
        return Ref<Value<T>>::Create(data);
    }

    static ValueBasePtr CreateValueFromString(const string& value, const string& type);

    virtual ValueBasePtr Copy() const = 0;

    virtual string GetValueString() const = 0;

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
    Value() : data{} {}

    explicit Value(const T& value) : data(value) {}

    virtual ~Value() {}

    ValueBasePtr Copy() const override { return ValueBase::CreateValue<T>(data); }

    void SetData(const T& value) { data = value; }

    void SetData(const Value<T>& value) { data = value.data; }

    T GetData() const { return data; }

    T* GetDataPtr() { return &data; }

    static ValueBasePtr CreateFromString(const string& value);

    string GetValueString() const override;

   public:
    static const string TYPE;

   private:
    T data;
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
