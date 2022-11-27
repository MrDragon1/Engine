#pragma once

#include "Base/Math/Math.h"
#include "Base/Math/Quaternion.h"
#include "Base/Math/Vector.h"

namespace Ethereal
{
    /***********************************************************************
     ******************************* Matrix3 *******************************
     ***********************************************************************/
    REFLECTION_TYPE(Matrix3)
    CLASS(Matrix3,Fields){
        REFLECTION_BODY(Matrix3);
        
      public:
        Vector3 value0;
        Vector3 value1;
        Vector3 value2;

      public:
        Matrix3();

        Matrix3(const Matrix3& m);
        Matrix3(float diag);
        Matrix3(float m00, float m10, float m20,
                float m01, float m11, float m21,
                float m02, float m12, float m22);
        Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2);

        Vector3& operator[](int i);
        const Vector3& operator[](int i) const;

        // -- Unary arithmetic operators --
        Matrix3& operator=(const Matrix3& m);
        Matrix3& operator+=(const Matrix3& m);
        
        Matrix3& operator+=(float m);
        Matrix3& operator-=(const Matrix3& m);
        
        Matrix3& operator-=(float m);
        Matrix3& operator*=(const Matrix3& m);
        
        Matrix3& operator*=(float m);
        Matrix3& operator/=(const Matrix3& m);
        
        Matrix3& operator/=(float m);

        operator Matrix4() const;

        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;
    };
    // -- Unary operators --
    Matrix3 operator+(const Matrix3& m);
    Matrix3 operator-(const Matrix3& m);
    // -- Binary operators --
    Matrix3 operator+(const Matrix3& m, const Matrix3& s);
    
    Matrix3 operator+(const float& s, const Matrix3& m);
    
    Matrix3 operator+(const Matrix3& m, const float& s);
    
    Matrix3 operator-(const Matrix3& m, const Matrix3& s);
    
    Matrix3 operator-(const float& m, const Matrix3& s);
    
    Matrix3 operator-(const Matrix3& s, const float& m);

    Matrix3 operator*(const Matrix3& m, const Matrix3& s);
    Vector3 operator*(const Matrix3& m, const Vector3& v);
    Vector3 operator*(const Vector3& v, const Matrix3& m);
    
    Matrix3 operator*(const float& m, const Matrix3& s);
    
    Matrix3 operator*(const Matrix3& s, const float& m);

    Matrix3 operator/(const Matrix3& m, const Matrix3& s);
    Vector3 operator/(const Matrix3& m, const Vector3& v);
    Vector3 operator/(const Vector3& v, const Matrix3& m);
    
    Matrix3 operator/(const float& m, const Matrix3& s);
    
    Matrix3 operator/(const Matrix3& s, const float& m);

    // -- Boolean operators --
    bool operator==(const Matrix3& m, const Matrix3& s);
    bool operator!=(const Matrix3& m, const Matrix3& s);


    /***********************************************************************
     ******************************* Matrix4 *******************************
     ***********************************************************************/
    REFLECTION_TYPE(Matrix4)
    CLASS(Matrix4, Fields) {
        REFLECTION_BODY(Matrix4);

      public:
        Vector4 value0;
        Vector4 value1;
        Vector4 value2;
        Vector4 value3;

      public:
        Matrix4();

        Matrix4(const Matrix4& m);
        Matrix4(float diag);
        Matrix4(float m00, float m10, float m20, float m30,
                float m01, float m11, float m21, float m31,
                float m02, float m12, float m22, float m32,
                float m03, float m13, float m23, float m33);
        Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3);

        Vector4& operator[](int i);
        const Vector4& operator[](int i) const;

        // -- Unary arithmetic operators --
        Matrix4& operator=(const Matrix4& m);
        Matrix4& operator+=(const Matrix4& m);
        
        Matrix4& operator+=(float m);
        Matrix4& operator-=(const Matrix4& m);
        
        Matrix4& operator-=(float m);
        Matrix4& operator*=(const Matrix4& m);
        
        Matrix4& operator*=(float m);
        Matrix4& operator/=(const Matrix4& m);
        
        Matrix4& operator/=(float m);

        operator Matrix3() const;

        static const Matrix4 ZERO;
        static const Matrix4 IDENTITY;
    };

    // -- Unary operators --
    Matrix4 operator+(const Matrix4& m);
    Matrix4 operator-(const Matrix4& m);
    // -- Binary operators --
    Matrix4 operator+(const Matrix4& m, const Matrix4& s);
    
    Matrix4 operator+(const float& s, const Matrix4& m);
    
    Matrix4 operator+(const Matrix4& m, const float& s);

    Matrix4 operator-(const Matrix4& m, const Matrix4& s);
    
    Matrix4 operator-(const float& m, const Matrix4& s);
    
    Matrix4 operator-(const Matrix4& s, const float& m);

    Matrix4 operator*(const Matrix4& m, const Matrix4& s);
    Vector4 operator*(const Matrix4& m, const Vector4& v);
    Vector4 operator*(const Vector4& v, const Matrix4& m);
    
    Matrix4 operator*(const float& m, const Matrix4& s);
    
    Matrix4 operator*(const Matrix4& s, const float& m);

    Matrix4 operator/(const Matrix4& m, const Matrix4& s);
    Vector4 operator/(const Matrix4& m, const Vector4& v);
    Vector4 operator/(const Vector4& v, const Matrix4& m);
    
    Matrix4 operator/(const float& m, const Matrix4& s);
    
    Matrix4 operator/(const Matrix4& s, const float& m);

    // -- Boolean operators --
    bool operator==(const Matrix4& m, const Matrix4& s);
    bool operator!=(const Matrix4& m, const Matrix4& s);
    
}  // namespace Ethereal
