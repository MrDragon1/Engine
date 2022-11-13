#pragma once

#include "Base/Meta/Reflection.h"

namespace Ethereal
{
    REFLECTION_TYPE(Vector2)

    CLASS(Vector2, Fields) {
        REFLECTION_BODY(Vector2);

      public:
        virtual void print() { std::cout << "BaseTest" << std::endl; }

      public:
        int m_int;
        std::vector<int *> m_int_vector;
        std::vector<double> m_double_vector;
    };

}  // namespace Ethereal