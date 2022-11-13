#include "Serializer.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const char& instance) {
        return YNode(instance);
    }
    template <>
    char& Serializer::read(const YNode& yaml_context, char& instance) {
        assert(yaml_context.IsScalar());
        return instance = yaml_context.as<char>();
    }

    template <>
    YNode Serializer::write(const int& instance) {
        return YNode(instance);
    }
    template <>
    int& Serializer::read(const YNode& yaml_context, int& instance) {
        assert(yaml_context.IsScalar());
        return instance = static_cast<int>(yaml_context.as<int>());
    }

    template <>
    YNode Serializer::write(const unsigned int& instance) {
        return YNode(static_cast<int>(instance));
    }
    template <>
    unsigned int& Serializer::read(const YNode& yaml_context, unsigned int& instance) {
        assert(yaml_context.IsScalar());
        return instance = static_cast<unsigned int>(yaml_context.as<unsigned int>());
    }

    template <>
    YNode Serializer::write(const float& instance) {
        return YNode(instance);
    }
    template <>
    float& Serializer::read(const YNode& yaml_context, float& instance) {
        assert(yaml_context.IsScalar());
        return instance = static_cast<float>(yaml_context.as<float>());
    }

    template <>
    YNode Serializer::write(const double& instance) {
        return YNode(instance);
    }
    template <>
    double& Serializer::read(const YNode& yaml_context, double& instance) {
        assert(yaml_context.IsScalar());
        return instance = static_cast<float>(yaml_context.as<double>());
    }

    template <>
    YNode Serializer::write(const bool& instance) {
        return YNode(instance);
    }
    template <>
    bool& Serializer::read(const YNode& yaml_context, bool& instance) {
        assert(yaml_context.IsScalar());
        return instance = yaml_context.as<bool>();
    }

    template <>
    YNode Serializer::write(const std::string& instance) {
        return YNode(instance);
    }
    template <>
    std::string& Serializer::read(const YNode& yaml_context, std::string& instance) {
        assert(yaml_context.IsScalar());
        return instance = yaml_context.as<std::string>();
    }
}  // namespace Ethereal