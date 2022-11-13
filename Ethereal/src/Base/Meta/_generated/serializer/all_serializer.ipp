#pragma once
#include "Base/Meta/_generated/serializer/Vector.serializer.gen.h"
namespace Ethereal
{
    template <>
    YNode Serializer::write(const Vector2& instance) {
        YNode ret_context;

        ret_context["int"] = Serializer::write(instance.m_int);
        YNode m_int_vector_yaml;
        for (auto& item : instance.m_int_vector) {
            m_int_vector_yaml.push_back(Serializer::write(item));
        }
        ret_context["int_vector"] = m_int_vector_yaml;

        YNode m_double_vector_yaml;
        for (auto& item : instance.m_double_vector) {
            m_double_vector_yaml.push_back(Serializer::write(item));
        }
        ret_context["double_vector"] = m_double_vector_yaml;

        return ret_context;
    }
    template <>
    Vector2& Serializer::read(const YNode& yaml_context, Vector2& instance) {
        assert(!yaml_context.IsNull());

        if (!yaml_context["int"].IsNull()) {
            Serializer::read(yaml_context["int"], instance.m_int);
        }
        if (!yaml_context["int_vector"].IsNull()) {
            assert(yaml_context["int_vector"].IsSequence());
            YNode array_m_int_vector = yaml_context["int_vector"];
            instance.m_int_vector.resize(array_m_int_vector.size());
            for (size_t index = 0; index < array_m_int_vector.size(); ++index) {
                Serializer::read(array_m_int_vector[index], instance.m_int_vector[index]);
            }
        }
        if (!yaml_context["double_vector"].IsNull()) {
            assert(yaml_context["double_vector"].IsSequence());
            YNode array_m_double_vector = yaml_context["double_vector"];
            instance.m_double_vector.resize(array_m_double_vector.size());
            for (size_t index = 0; index < array_m_double_vector.size(); ++index) {
                Serializer::read(array_m_double_vector[index], instance.m_double_vector[index]);
            }
        }
        return instance;
    }

}  // namespace Ethereal
