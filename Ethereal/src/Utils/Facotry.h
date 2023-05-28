#pragma once
#include "pch.h"

namespace Ethereal {
template <class T>
class Factory {
   public:
    using Ptr = Ref<T>;
    using CreatorFunction = Ptr (*)();
    using CreatorMap = unordered_map<string, CreatorFunction>;

    /// Register a new class given a unique type name
    /// and a creator function for the class.
    void registerClass(const string& typeName, CreatorFunction f) { _creatorMap[typeName] = f; }

    /// Determine if a class has been registered for a type name
    bool classRegistered(const string& typeName) const {
        return _creatorMap.find(typeName) != _creatorMap.end();
    }

    /// Unregister a registered class
    void unregisterClass(const string& typeName) {
        auto it = _creatorMap.find(typeName);
        if (it != _creatorMap.end()) {
            _creatorMap.erase(it);
        }
    }

    /// Create a new instance of the class with given type name.
    /// Returns nullptr if no class with given name is registered.
    Ptr create(const string& typeName) const {
        auto it = _creatorMap.find(typeName);
        return (it != _creatorMap.end() ? it->second() : nullptr);
    }

   private:
    CreatorMap _creatorMap;
};
}  // namespace Ethereal
