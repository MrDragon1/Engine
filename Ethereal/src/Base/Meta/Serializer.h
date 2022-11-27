#pragma once
#include "Base/Meta/Reflection.h"

#include <cassert>

namespace Ethereal
{
    template <typename...>
    inline constexpr bool always_false = false;

    class Serializer {
      public:
        template <typename T>
        static YNode writePointer(T* instance) {
            YNode node{};
            node["TypeName"] = "*";
            node["Context"] = Serializer::write(*instance);
            return node;
        }

        template <typename T>
        static T*& readPointer(const YNode& yaml_context, T*& instance) {
            assert(instance == nullptr);
            std::string type_name = yaml_context["TypeName"].as<std::string>();
            assert(!type_name.empty());
            if ('*' == type_name[0]) {
                instance = new T;
                read(yaml_context["Context"], *instance);
            } else {
                instance = static_cast<T*>(Reflection::TypeMeta::newFromNameAndYAML(type_name, yaml_context["Context"]).m_instance);
            }
            return instance;
        }

        template <typename T>
        static YNode write(const Reflection::ReflectionPtr<T>& instance) {
            T* instance_ptr = static_cast<T*>(instance.operator->());
            std::string type_name = instance.getTypeName();

            YNode node{};
            node["TypeName"] = type_name;
            node["Context"] = Reflection::TypeMeta::writeByName(type_name, instance_ptr);
            return node;
        }

        template <typename T>
        static T*& read(const YNode& yaml_context, Reflection::ReflectionPtr<T>& instance) {
            std::string type_name = yaml_context["TypeName"].as<std::string>();
            instance.setTypeName(type_name);
            return readPointer(yaml_context, instance.getPtrReference());
        }

        template <typename T>
        static YNode write(const T& instance) {
            if constexpr (std::is_pointer<T>::value) {
                return writePointer((T)instance);
            } else {
                static_assert(always_false<T>, "Serializer::write<T> has not been implemented yet!");
                return YNode();
            }
        }

        template <typename T>
        static T& read(const YNode& yaml_context, T& instance) {
            if constexpr (std::is_pointer<T>::value) {
                return readPointer(yaml_context, instance);
            } else {
                static_assert(always_false<T>, "Serializer::read<T> has not been implemented yet!");
                return instance;
            }
        }
    };

    // implementation of base types
    template <>
    YNode Serializer::write(const char& instance);
    template <>
    char& Serializer::read(const YNode& yaml_context, char& instance);

    template <>
    YNode Serializer::write(const int& instance);
    template <>
    int& Serializer::read(const YNode& yaml_context, int& instance);

    template <>
    YNode Serializer::write(const unsigned int& instance);
    template <>
    unsigned int& Serializer::read(const YNode& yaml_context, unsigned int& instance);

    template <>
    YNode Serializer::write(const float& instance);
    template <>
    float& Serializer::read(const YNode& yaml_context, float& instance);

    template <>
    YNode Serializer::write(const double& instance);
    template <>
    double& Serializer::read(const YNode& yaml_context, double& instance);

    template <>
    YNode Serializer::write(const bool& instance);
    template <>
    bool& Serializer::read(const YNode& yaml_context, bool& instance);

    template <>
    YNode Serializer::write(const std::string& instance);
    template <>
    std::string& Serializer::read(const YNode& yaml_context, std::string& instance);

    template <>
    YNode Serializer::write(const uint64_t& instance);
    template <>
    uint64_t& Serializer::read(const YNode& yaml_context, uint64_t& instance);

}  // namespace Ethereal
