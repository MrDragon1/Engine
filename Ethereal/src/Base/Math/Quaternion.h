#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Math/Math.h"

namespace Ethereal
{
    class Matrix3;
    class Vector3;

    REFLECTION_TYPE(Quaternion)
    CLASS(Quaternion, Fields) {
        REFLECTION_BODY(Quaternion);

      public:
        float  x{0.f}, y{0.f}, z{0.f},w {1.0f};

      public:
        Quaternion();
        Quaternion(const Quaternion& m);
        Quaternion(float w, float x, float y, float z);
        Quaternion(const Vector3& eulerAngle);
        Quaternion(const Matrix3& m);
        Quaternion(const Matrix4& m);

        float& operator[](int i);
        const float& operator[](int i) const;

        // -- Unary arithmetic operators --
        Quaternion& operator=(const Quaternion& v);
        Quaternion& operator+=(const Quaternion& v);
        Quaternion& operator-=(const Quaternion& v);
        Quaternion& operator*=(const Quaternion& v);
        Quaternion& operator*=(float v);
        Quaternion& operator/=(float v);

        operator Matrix3() const;
        operator Matrix4() const;
        operator Vector3() const;

        float Roll() const;
        float Pitch() const;
        float Yaw() const;


        // special values
        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

      private:
        Quaternion Matrix3ToQuaternion(const Matrix3& m);
        Matrix3 QuaternionToMatrix3() const;
        Vector3 QuaternionToEuler() const;
    };
    
    // -- Unary operators --
    Quaternion operator+(const Quaternion& m);
    Quaternion operator-(const Quaternion& m);
    // -- Binary operators --
    Quaternion operator+(const Quaternion& m, const Quaternion& s);
    Quaternion operator-(const Quaternion& m, const Quaternion& s);

    Quaternion operator*(const Quaternion& m, const Quaternion& s);
    Quaternion operator*(const float& m, const Quaternion& s);
    Quaternion operator*(const Quaternion& s, const float& m);
    Vector3 operator*(const Quaternion& q, const Vector3& v);
    Vector3 operator*(const Vector3& v, const Quaternion& q);

    
    Quaternion operator/(const Quaternion& s, const float& m);

    // -- Boolean operators --
    bool operator==(const Quaternion& m, const Quaternion& s);
    bool operator!=(const Quaternion& m, const Quaternion& s);
    
}  // namespace Ethereal
