#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Math/Math.h"
#include "Base/Math/Quaternion.h"

namespace Ethereal
{
    /***********************************************************************
    ******************************* Vector2 *******************************
    ***********************************************************************/
    REFLECTION_TYPE(Vector2)
    CLASS(Vector2, Fields) {
        REFLECTION_BODY(Vector2);
        
      public:
        float x{0.f}, y{0.f};
        
      public:
        Vector2();

        Vector2(const Vector2& m);
        Vector2(float x, float y);
        Vector2(float scalar);

        float& operator[](int i);
        const float& operator[](int i) const;

        // -- Unary arithmetic operators --
        Vector2& operator=(const Vector2& v);
        Vector2& operator+=(const Vector2& v);
    
        Vector2& operator+=(float v);
        Vector2& operator-=(const Vector2& v);
    
        Vector2& operator-=(float v);
        Vector2& operator*=(const Vector2& v);
    
        Vector2& operator*=(float v);
        Vector2& operator/=(const Vector2& v);
    
        Vector2& operator/=(float v);

        static const Vector2 ZERO;
        static const Vector2 ONE;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        static const Vector2 NEGATIVE_UNIT_X;
        static const Vector2 NEGATIVE_UNIT_Y;
    };
    // -- Unary operators --
    Vector2 operator+(const Vector2& m);
    Vector2 operator-(const Vector2& m);
    // -- Binary operators --
    Vector2 operator+(const Vector2& m, const Vector2& s);

    Vector2 operator+(const float& s, const Vector2& m);

    Vector2 operator+(const Vector2& m, const float& s);
    
    Vector2 operator-(const Vector2& m, const Vector2& s);

    Vector2 operator-(const float& m, const Vector2& s);

    Vector2 operator-(const Vector2& s, const float& m);
    
    Vector2 operator*(const Vector2& m, const Vector2& s);

    Vector2 operator*(const float& m, const Vector2& s);

    Vector2 operator*(const Vector2& s, const float& m);

    Vector2 operator/(const Vector2& m, const Vector2& s);

    Vector2 operator/(const float& m, const Vector2& s);

    Vector2 operator/(const Vector2& s, const float& m);

    // -- Boolean operators --
    bool operator==(const Vector2& m, const Vector2& s);
    bool operator!=(const Vector2& m, const Vector2& s);

    /***********************************************************************
    ******************************* Vector3 *******************************
    ***********************************************************************/
    REFLECTION_TYPE(Vector3)
    CLASS(Vector3, Fields) {
        REFLECTION_BODY(Vector3);
        
      public:
        float x{0.f}, y{0.f}, z{0.f};

      public:
        Vector3();

        Vector3(const Vector3& m);
        Vector3(float x, float y, float z);
        Vector3(float scalar);

        float& operator[](int i);
        const float& operator[](int i) const;

        // -- Unary arithmetic operators --
        Vector3& operator=(const Vector3& v);
        Vector3& operator+=(const Vector3& v);
    
        Vector3& operator+=(float v);
        Vector3& operator-=(const Vector3& v);
    
        Vector3& operator-=(float v);
        Vector3& operator*=(const Vector3& v);
    
        Vector3& operator*=(float v);
        Vector3& operator/=(const Vector3& v);
    
        Vector3& operator/=(float v);

        static const Vector3 ZERO;
        static const Vector3 ONE;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 NEGATIVE_UNIT_X;
        static const Vector3 NEGATIVE_UNIT_Y;
        static const Vector3 NEGATIVE_UNIT_Z;
    };
    // -- Unary operators --
    Vector3 operator+(const Vector3& m);
    Vector3 operator-(const Vector3& m);
    // -- Binary operators --
    Vector3 operator+(const Vector3& m, const Vector3& s);

    Vector3 operator+(const float& s, const Vector3& m);

    Vector3 operator+(const Vector3& m, const float& s);
    
    Vector3 operator-(const Vector3& m, const Vector3& s);

    Vector3 operator-(const float& m, const Vector3& s);

    Vector3 operator-(const Vector3& s, const float& m);
    
    Vector3 operator*(const Vector3& m, const Vector3& s);

    Vector3 operator*(const float& m, const Vector3& s);

    Vector3 operator*(const Vector3& s, const float& m);

    Vector3 operator/(const Vector3& m, const Vector3& s);

    Vector3 operator/(const float& m, const Vector3& s);

    Vector3 operator/(const Vector3& s, const float& m);

    // -- Boolean operators --
    bool operator==(const Vector3& m, const Vector3& s);
    bool operator!=(const Vector3& m, const Vector3& s);

    /***********************************************************************
    ******************************* Vector4 *******************************
    ***********************************************************************/
    REFLECTION_TYPE(Vector4)
    CLASS(Vector4, Fields) {
        REFLECTION_BODY(Vector4);

      public:
        float x{0.f}, y{0.f}, z{0.f}, w{0.f};

      public:
        Vector4();

        Vector4(const Vector4& m);
        Vector4(float x, float y, float z, float w);
        Vector4(const Vector3& v, float w);
        Vector4(float scalar);

        float& operator[](int i);
        const float& operator[](int i) const;

        // -- Unary arithmetic operators --
        Vector4& operator=(const Vector4& v);
        Vector4& operator+=(const Vector4& v);
    
        Vector4& operator+=(float v);
        Vector4& operator-=(const Vector4& v);
    
        Vector4& operator-=(float v);
        Vector4& operator*=(const Vector4& v);
    
        Vector4& operator*=(float v);
        Vector4& operator/=(const Vector4& v);
    
        Vector4& operator/=(float v);

        operator Vector3() const;

        static const Vector4 ZERO;
        static const Vector4 ONE;
    };
    // -- Unary operators --
    Vector4 operator+(const Vector4& m);
    Vector4 operator-(const Vector4& m);
    // -- Binary operators --
    Vector4 operator+(const Vector4& m, const Vector4& s);
    Vector4 operator+(const float& s, const Vector4& m);
    Vector4 operator+(const Vector4& m, const float& s);
    
    Vector4 operator-(const Vector4& m, const Vector4& s);
    Vector4 operator-(const float& m, const Vector4& s);
    Vector4 operator-(const Vector4& s, const float& m);
    
    Vector4 operator*(const Vector4& m, const Vector4& s);
    Vector4 operator*(const float& m, const Vector4& s);
    Vector4 operator*(const Vector4& s, const float& m);

    Vector4 operator/(const Vector4& m, const Vector4& s);
    Vector4 operator/(const float& m, const Vector4& s);
    Vector4 operator/(const Vector4& s, const float& m);

    // -- Boolean operators --
    bool operator==(const Vector4& m, const Vector4& s);
    bool operator!=(const Vector4& m, const Vector4& s);
}  // namespace Ethereal