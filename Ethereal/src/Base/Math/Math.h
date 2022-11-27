#pragma once
#include "Base/Math/Random.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace Ethereal
{
    class Vector2;
    class Vector3;
    class Vector4;
    class Matrix3;
    class Matrix4;
    class Quaternion;


    namespace Math{
        static const float POS_INFINITY = std::numeric_limits<float>::infinity();
        static const float NEG_INFINITY = -std::numeric_limits<float>::infinity();
        static const float PI = 3.14159265358979323846264338327950288f;
        static const float ONE_OVER_PI = 1.0f / PI;
        static const float TWO_PI = 2.0f * PI;
        static const float HALF_PI = 0.5f * PI;
        static const float fDeg2Rad = PI / 180.0f;
        static const float fRad2Deg = 180.0f / PI;
        static const float LOG2 = log(2.0f);
        static const float EPSILON = 1e-6f;

        static const float EPSILONF = FLT_EPSILON;
        static const float EPSILOND = DBL_EPSILON;

        float Radians(float degrees);
        Vector3 Radians(const Vector3& degrees);
        float Degrees(float radians);
        Vector3 Degrees(const Vector3& radians);

        float* Ptr(Vector2& v);
        const float* Ptr(const Vector2& v);
        float* Ptr(Vector3& v);
        const float* Ptr(const Vector3& v);
        float* Ptr(Vector4& v);
        const float* Ptr(const Vector4& v);
        float* Ptr(Matrix3& v);
        const float* Ptr(const Matrix3& v);
        float* Ptr(Matrix4& v);
        const float* Ptr(const Matrix4& v);

        float Sin(float v);
        Vector2 Sin(const Vector2& v);
        Vector3 Sin(const Vector3& v);
        Vector4 Sin(const Vector4& v);
        float Cos(float v);
        Vector2 Cos(const Vector2& v);
        Vector3 Cos(const Vector3& v);
        Vector4 Cos(const Vector4& v);
        float Tan(float v);
        Vector2 Tan(const Vector2& v);
        Vector3 Tan(const Vector3& v);
        Vector4 Tan(const Vector4& v);

        float Min(float a, float b);

        float Max(float a, float b);

        bool EpsilonEqual(float a, float b, float epsilon = EPSILONF);
        bool EpsilonEqual(const Vector2& a, const Vector2& b, float epsilon = EPSILONF);

        float Clamp(float v, float min, float max);

        /*********************************************************************
        ******************************* Vector *******************************
        **********************************************************************/

        float Dot(const Vector2& a,const Vector2& b);
        float Dot(const Vector3& a,const Vector3& b);
        float Dot(const Vector4& a,const Vector4& b);

        Vector2 Cross(const Vector2& a,const Vector2& b);
        Vector3 Cross(const Vector3& a,const Vector3& b);
        Vector4 Cross(const Vector4& a,const Vector4& b);

        Vector2 Normalize(const Vector2& v);
        Vector3 Normalize(const Vector3& v);
        Vector4 Normalize(const Vector4& v);

        float Length(const Vector2& v);
        float Length(const Vector3& v);
        float Length(const Vector4& v);

        float Mix(const float& a,const float& b,float t);
        Vector2 Mix(const Vector2& a,const Vector2& b,float t);
        Vector3 Mix(const Vector3& a,const Vector3& b,float t);
        Vector4 Mix(const Vector4& a,const Vector4& b,float t);

        Vector2 Scale(const Vector2& v,float length);
        Vector3 Scale(const Vector3& v,float length);
        Vector4 Scale(const Vector4& v,float length);

        Vector2 Combine(const Vector2& a,const Vector2& b,float fa,float fb);
        Vector3 Combine(const Vector3& a,const Vector3& b,float fa,float fb);
        Vector4 Combine(const Vector4& a,const Vector4& b,float fa,float fb);

        /*********************************************************************
        ******************************* Matrix *******************************
        **********************************************************************/

        Matrix3 Inverse(const Matrix3& m);
        Matrix4 Inverse(const Matrix4& m);

        Matrix3 Transpose(const Matrix3& m);
        Matrix4 Transpose(const Matrix4& m);

        float Determinant(const Matrix3& m);
        float Determinant(const Matrix4& m);

        Matrix4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
        Matrix4 Perspective(float fovy, float aspect, float zNear, float zFar);
        Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
        Matrix4 Translate(const Matrix4& m, const Vector3& v);
        Matrix4 Rotate(const Matrix4& m, float angle, const Vector3& v);
        Matrix4 Rotate(const Matrix4& m, const Quaternion& q);
        Vector3 Rotate(const Quaternion& q, const Vector3& v);
        Matrix4 Scale(const Matrix4& m, const Vector3& v);

        bool DecomposeTransformMatrix(const Matrix4& m, Vector3& translation, Quaternion& rotation, Vector3& scale, Vector3& skew, Vector4& perspective);

        /*********************************************************************
        ******************************* Quaternion ***************************
        **********************************************************************/
        float Dot(const Quaternion& a,const Quaternion& b);
        Quaternion Mix(const Quaternion& a, const Quaternion& b, float t);
        float Length(const Quaternion& q);
        Quaternion Normalize(const Quaternion& q);
        Quaternion Conjugate(const Quaternion& q);
        Quaternion Inverse(const Quaternion& q);

    }

}  // namespace Ethereal
