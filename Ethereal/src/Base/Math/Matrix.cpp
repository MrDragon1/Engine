#include "Matrix.h"

namespace Ethereal {
/***********************************************************************
 ******************************* Matrix3 *******************************
 ***********************************************************************/

const Matrix3 Matrix3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);
const Matrix3 Matrix3::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);

Matrix3::Matrix3() { operator=(IDENTITY); }

Matrix3::Matrix3(const Matrix3& m) {
    value0 = m[0];
    value1 = m[1];
    value2 = m[2];
}
Matrix3::Matrix3(float diag) {
    value0 = Vector3(diag, 0, 0);
    value1 = Vector3(0, diag, 0);
    value2 = Vector3(0, 0, diag);
}
Matrix3::Matrix3(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22) {
    value0 = Vector3(m00, m10, m20);
    value1 = Vector3(m01, m11, m21);
    value2 = Vector3(m02, m12, m22);
}

Matrix3::Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2) {
    value0 = col0;
    value1 = col1;
    value2 = col2;
}

Vector3& Matrix3::operator[](int i) {
    assert(i < 3);
    switch (i) {
        case 0:
            return value0;
        case 1:
            return value1;
        case 2:
            return value2;
    }
}

const Vector3& Matrix3::operator[](int i) const {
    assert(i < 3);
    switch (i) {
        case 0:
            return value0;
        case 1:
            return value1;
        case 2:
            return value2;
    }
}

Matrix3& Matrix3::operator=(const Matrix3& m) {
    value0 = m[0];
    value1 = m[1];
    value2 = m[2];
    return *this;
}

Matrix3& Matrix3::operator+=(const Matrix3& m) {
    value0 += m[0];
    value1 += m[1];
    value2 += m[2];
    return *this;
}

Matrix3& Matrix3::operator+=(float m) {
    value0 += m;
    value1 += m;
    value2 += m;
    return *this;
}

Matrix3& Matrix3::operator-=(const Matrix3& m) {
    value0 -= m[0];
    value1 -= m[1];
    value2 -= m[2];
    return *this;
}

Matrix3& Matrix3::operator-=(float m) {
    value0 -= m;
    value1 -= m;
    value2 -= m;
    return *this;
}

Matrix3& Matrix3::operator*=(const Matrix3& m) { return (*this = *this * m); }

Matrix3& Matrix3::operator*=(float m) {
    value0 *= m;
    value1 *= m;
    value2 *= m;
    return *this;
}

Matrix3& Matrix3::operator/=(const Matrix3& m) { return *this *= Math::Inverse(m); }

Matrix3& Matrix3::operator/=(float m) {
    value0 /= m;
    value1 /= m;
    value2 /= m;
    return *this;
}

Matrix3::operator Matrix4() const {
    return Matrix4(value0.x, value0.y, value0.z, 0, value1.x, value1.y, value1.z, 0, value2.x, value2.y, value2.z, 0, 0, 0, 0, 1);
}

// -- Unary operators --
Matrix3 operator+(const Matrix3& m) { return m; }

Matrix3 operator-(const Matrix3& m) { return Matrix3(-m[0], -m[1], -m[2]); }

// -- Binary operators --
Matrix3 operator+(const Matrix3& m, const Matrix3& s) { return Matrix3(m[0] + s[0], m[1] + s[1], m[2] + s[2]); }

Matrix3 operator+(const float& s, const Matrix3& m) { return Matrix3(s + m[0], s + m[1], s + m[2]); }

Matrix3 operator+(const Matrix3& m, const float& s) { return Matrix3(m[0] + s, m[1] + s, m[2] + s); }

Matrix3 operator-(const Matrix3& m, const Matrix3& s) { return Matrix3(m[0] - s[0], m[1] - s[1], m[2] - s[2]); }

Matrix3 operator-(const float& m, const Matrix3& s) { return Matrix3(m - s[0], m - s[1], m - s[2]); }

Matrix3 operator-(const Matrix3& s, const float& m) { return Matrix3(s[0] - m, s[1] - m, s[2] - m); }

Matrix3 operator*(const Matrix3& m, const Matrix3& s) {
    const Vector3 SrcA0 = m[0];
    const Vector3 SrcA1 = m[1];
    const Vector3 SrcA2 = m[2];

    const Vector3 SrcB0 = s[0];
    const Vector3 SrcB1 = s[1];
    const Vector3 SrcB2 = s[2];

    Matrix3 Result;
    Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2];
    Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2];
    Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2];
    return Result;
}

Vector3 operator*(const Matrix3& m, const Vector3& v) {
    return Vector3(m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
                   m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
                   m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3]);
}

Vector3 operator*(const Vector3& v, const Matrix3& m) {
    return Vector3(m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
                   m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
                   m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3]);
}

Matrix3 operator*(const float& m, const Matrix3& s) { return Matrix3(m * s[0], m * s[1], m * s[2]); }

Matrix3 operator*(const Matrix3& s, const float& m) { return Matrix3(s[0] * m, s[1] * m, s[2] * m); }

Matrix3 operator/(const Matrix3& m, const Matrix3& s) {
    Matrix3 mc(m);
    return mc /= s;
}

Vector3 operator/(const Matrix3& m, const Vector3& v) { return Math::Inverse(m) * v; }

Vector3 operator/(const Vector3& v, const Matrix3& m) { return v * Math::Inverse(m); }

Matrix3 operator/(const float& m, const Matrix3& s) { return Matrix3(m / s[0], m / s[1], m / s[2]); }

Matrix3 operator/(const Matrix3& s, const float& m) { return Matrix3(s[0] / m, s[1] / m, s[2] / m); }

// -- Boolean operators --
bool operator==(const Matrix3& m, const Matrix3& s) { return (m[0] == s[0]) && (m[1] == s[1]) && (m[2] == s[2]); }
bool operator!=(const Matrix3& m, const Matrix3& s) { return (m[0] != s[0]) || (m[1] != s[1]) || (m[2] != s[2]); }

/***********************************************************************
 ******************************* Matrix4 *******************************
 ***********************************************************************/
const Matrix4 Matrix4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
const Matrix4 Matrix4::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

Matrix4::Matrix4() { operator=(IDENTITY); }

Matrix4::Matrix4(const Matrix4& m) {
    value0 = m[0];
    value1 = m[1];
    value2 = m[2];
    value3 = m[3];
}
Matrix4::Matrix4(float diag) {
    value0 = Vector4(diag, 0, 0, 0);
    value1 = Vector4(0, diag, 0, 0);
    value2 = Vector4(0, 0, diag, 0);
    value3 = Vector4(0, 0, 0, diag);
}
Matrix4::Matrix4(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32,
                 float m03, float m13, float m23, float m33) {
    value0 = Vector4(m00, m10, m20, m30);
    value1 = Vector4(m01, m11, m21, m31);
    value2 = Vector4(m02, m12, m22, m32);
    value3 = Vector4(m03, m13, m23, m33);
}

Matrix4::Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3) {
    value0 = col0;
    value1 = col1;
    value2 = col2;
    value3 = col3;
}

Vector4& Matrix4::operator[](int i) {
    assert(i < 4);
    switch (i) {
        case 0:
            return value0;
        case 1:
            return value1;
        case 2:
            return value2;
        case 3:
            return value3;
        default:
            return value0;
    }
}

const Vector4& Matrix4::operator[](int i) const {
    assert(i < 4);
    switch (i) {
        case 0:
            return value0;
        case 1:
            return value1;
        case 2:
            return value2;
        case 3:
            return value3;
        default:
            return value0;
    }
}

Matrix4& Matrix4::operator=(const Matrix4& m) {
    value0 = m[0];
    value1 = m[1];
    value2 = m[2];
    value3 = m[3];
    return *this;
}

Matrix4& Matrix4::operator+=(const Matrix4& m) {
    value0 += m[0];
    value1 += m[1];
    value2 += m[2];
    value3 += m[3];
    return *this;
}

Matrix4& Matrix4::operator+=(float m) {
    value0 += m;
    value1 += m;
    value2 += m;
    value3 += m;
    return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& m) {
    value0 -= m[0];
    value1 -= m[1];
    value2 -= m[2];
    value3 -= m[3];
    return *this;
}

Matrix4& Matrix4::operator-=(float m) {
    value0 -= m;
    value1 -= m;
    value2 -= m;
    value3 -= m;
    return *this;
}

Matrix4& Matrix4::operator*=(const Matrix4& m) { return (*this = *this * m); }

Matrix4& Matrix4::operator*=(float m) {
    value0 *= m;
    value1 *= m;
    value2 *= m;
    value3 *= m;
    return *this;
}

Matrix4& Matrix4::operator/=(const Matrix4& m) { return *this *= Math::Inverse(m); }

Matrix4& Matrix4::operator/=(float m) {
    value0 /= m;
    value1 /= m;
    value2 /= m;
    value3 /= m;
    return *this;
}

Matrix4::operator Matrix3() const {
    Matrix3 m;
    m[0] = Vector3(value0[0], value0[1], value0[2]);
    m[1] = Vector3(value1[0], value1[1], value1[2]);
    m[2] = Vector3(value2[0], value2[1], value2[2]);
    return m;
}

// -- Unary operators --
Matrix4 operator+(const Matrix4& m) { return m; }

Matrix4 operator-(const Matrix4& m) { return Matrix4(-m[0], -m[1], -m[2], -m[3]); }

// -- Binary operators --
Matrix4 operator+(const Matrix4& m, const Matrix4& s) { return Matrix4(m[0] + s[0], m[1] + s[1], m[2] + s[2], m[3] + s[3]); }

Matrix4 operator+(const float& s, const Matrix4& m) { return Matrix4(s + m[0], s + m[1], s + m[2], s + m[3]); }

Matrix4 operator+(const Matrix4& m, const float& s) { return Matrix4(m[0] + s, m[1] + s, m[2] + s, m[3] + s); }

Matrix4 operator-(const Matrix4& m, const Matrix4& s) { return Matrix4(m[0] - s[0], m[1] - s[1], m[2] - s[2], m[3] - s[3]); }

Matrix4 operator-(const float& m, const Matrix4& s) { return Matrix4(m - s[0], m - s[1], m - s[2], m - s[3]); }

Matrix4 operator-(const Matrix4& s, const float& m) { return Matrix4(s[0] - m, s[1] - m, s[2] - m, s[3] - m); }

Matrix4 operator*(const Matrix4& m, const Matrix4& s) {
    const Vector4 SrcA0 = m[0];
    const Vector4 SrcA1 = m[1];
    const Vector4 SrcA2 = m[2];
    const Vector4 SrcA3 = m[3];

    const Vector4 SrcB0 = s[0];
    const Vector4 SrcB1 = s[1];
    const Vector4 SrcB2 = s[2];
    const Vector4 SrcB3 = s[3];

    Matrix4 Result;
    Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
    Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
    Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
    Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
    return Result;
}

Vector4 operator*(const Matrix4& m, const Vector4& v) {
    return Vector4(
        m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3], m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
        m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3], m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]);
}

Vector4 operator*(const Vector4& v, const Matrix4& m) {
    return Vector4(
        m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3], m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
        m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3], m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]);
}

Matrix4 operator*(const float& m, const Matrix4& s) { return Matrix4(m * s[0], m * s[1], m * s[2], m * s[3]); }

Matrix4 operator*(const Matrix4& s, const float& m) { return Matrix4(s[0] * m, s[1] * m, s[2] * m, s[3] * m); }

Matrix4 operator/(const Matrix4& m, const Matrix4& s) {
    Matrix4 mc(m);
    return mc /= s;
}

Vector4 operator/(const Matrix4& m, const Vector4& v) { return Math::Inverse(m) * v; }

Vector4 operator/(const Vector4& v, const Matrix4& m) { return v * Math::Inverse(m); }

Matrix4 operator/(const float& m, const Matrix4& s) { return Matrix4(m / s[0], m / s[1], m / s[2], m / s[3]); }

Matrix4 operator/(const Matrix4& s, const float& m) { return Matrix4(s[0] / m, s[1] / m, s[2] / m, s[3] / m); }

// -- Boolean operators --
bool operator==(const Matrix4& m, const Matrix4& s) { return (m[0] == s[0]) && (m[1] == s[1]) && (m[2] == s[2]) && (m[3] == s[3]); }
bool operator!=(const Matrix4& m, const Matrix4& s) { return (m[0] != s[0]) || (m[1] != s[1]) || (m[2] != s[2]) || (m[3] != s[3]); }

}  // namespace Ethereal