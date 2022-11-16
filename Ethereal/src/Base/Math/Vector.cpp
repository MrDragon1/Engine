#include "Vector.h"

namespace Ethereal
{
    /***********************************************************************
    ******************************* Vector2 *******************************
    ***********************************************************************/
    const Vector2 Vector2::ZERO(0, 0);
    const Vector2 Vector2::UNIT_X(1, 0);
    const Vector2 Vector2::UNIT_Y(0, 1);
    const Vector2 Vector2::NEGATIVE_UNIT_X(-1, 0);
    const Vector2 Vector2::NEGATIVE_UNIT_Y(0, -1);
    const Vector2 Vector2::ONE(1, 1);

    Vector2::Vector2() { operator=(ZERO); }

    Vector2::Vector2(const Vector2& m) {
        x = m.x;
        y = m.y;
    }

    Vector2::Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector2::Vector2(float scalar) {
        x = scalar;
        y = scalar;
    }

    float& Vector2::operator[](int i) {
        assert(i < 2);
        switch (i) {
            default:
            case 0:
                return x;
            case 1:
                return y;
        }
    }

    const float& Vector2::operator[](int i) const {
        assert(i < 2);
        switch (i) {
            default:
            case 0:
                return x;
            case 1:
                return y;
        }
    }

    Vector2& Vector2::operator=(const Vector2& v) {
        this->x = v.x;
        this->y = v.y;
        return *this;
    }

    Vector2& Vector2::operator+=(const Vector2& v) {
        this->x += v.x;
        this->y += v.y;
        return *this;
    }


    Vector2& Vector2::operator+=(float v) {
        this->x += v;
        this->y += v;
        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& v) {
        this->x -= v.x;
        this->y -= v.y;
        return *this;
    }


    Vector2& Vector2::operator-=(float v) {
        this->x -= v;
        this->y -= v;
        return *this;
    }

    Vector2& Vector2::operator*=(const Vector2& v) {
        this->x *= v.x;
        this->y *= v.y;
        return *this;
    }


    Vector2& Vector2::operator*=(float v) {
        this->x *= v;
        this->y *= v;
        return *this;
    }

    Vector2& Vector2::operator/=(const Vector2& v) {
        this->x /= v.x;
        this->y /= v.y;
        return *this;
    }


    Vector2& Vector2::operator/=(float v) {
        this->x /= v;
        this->y /= v;
        return *this;
    }

    // -- Unary operators --
    Vector2 operator+(const Vector2& m) { return m; }

    Vector2 operator-(const Vector2& m) { return Vector2(0) -= m; }

    // -- Binary operators --
    Vector2 operator+(const Vector2& m, const Vector2& s) { return Vector2(m) += s; }


    Vector2 operator+(const float& s, const Vector2& m) {
        return Vector2(m) += s;
    }


    Vector2 operator+(const Vector2& m, const float& s) {
        return Vector2(m) += s;
    }

    Vector2 operator-(const Vector2& m, const Vector2& s) { return Vector2(m) -= s; }


    Vector2 operator-(const float& m, const Vector2& s) {
        return Vector2(m) -= s;
    }


    Vector2 operator-(const Vector2& s, const float& m) {
        return Vector2(m) -= s;
    }

    Vector2 operator*(const Vector2& m, const Vector2& s) { return Vector2(m) *= s; }


    Vector2 operator*(const float& m, const Vector2& s) {
        return Vector2(m) *= s;
    }


    Vector2 operator*(const Vector2& s, const float& m) {
        return Vector2(m) *= s;
    }

    Vector2 operator/(const Vector2& m, const Vector2& s) { return Vector2(m) /= s; }


    Vector2 operator/(const float& m, const Vector2& s) {
        return Vector2(m) /= s;
    }


    Vector2 operator/(const Vector2& s, const float& m) {
        return Vector2(s) /= Vector2(m);
    }

    // -- Boolean operators --
    bool operator==(const Vector2& m, const Vector2& s) { return (m.x == s.x) && (m.y == s.y); }

    bool operator!=(const Vector2& m, const Vector2& s) { return (m.x != s.x) || (m.y != s.y); }

    /***********************************************************************
     ******************************* Vector3 *******************************
     ***********************************************************************/
    const Vector3 Vector3::ZERO(0, 0, 0);
    const Vector3 Vector3::UNIT_X(1, 0, 0);
    const Vector3 Vector3::UNIT_Y(0, 1, 0);
    const Vector3 Vector3::UNIT_Z(0, 0, 1);
    const Vector3 Vector3::NEGATIVE_UNIT_X(-1, 0, 0);
    const Vector3 Vector3::NEGATIVE_UNIT_Y(0, -1, 0);
    const Vector3 Vector3::NEGATIVE_UNIT_Z(0, 0, -1);
    const Vector3 Vector3::ONE(1, 1, 1);

    Vector3::Vector3() { operator=(ZERO); }

    Vector3::Vector3(const Vector3& m) {
        x = m.x;
        y = m.y;
        z = m.z;
    }

    Vector3::Vector3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3::Vector3(float scalar) {
        x = scalar;
        y = scalar;
        z = scalar;
    }

    float& Vector3::operator[](int i) {
        assert(i < 3);
        switch (i) {
            default:
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
        }
    }

    const float& Vector3::operator[](int i) const {
        assert(i < 3);
        switch (i) {
            default:
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
        }
    }

    Vector3& Vector3::operator=(const Vector3& v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        return *this;
    }

    Vector3& Vector3::operator+=(const Vector3& v) {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }


    Vector3& Vector3::operator+=(float v) {
        this->x += v;
        this->y += v;
        this->z += v;
        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& v) {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }


    Vector3& Vector3::operator-=(float v) {
        this->x -= v;
        this->y -= v;
        this->z -= v;
        return *this;
    }

    Vector3& Vector3::operator*=(const Vector3& v) {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        return *this;
    }


    Vector3& Vector3::operator*=(float v) {
        this->x *= v;
        this->y *= v;
        this->z *= v;
        return *this;
    }

    Vector3& Vector3::operator/=(const Vector3& v) {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        return *this;
    }


    Vector3& Vector3::operator/=(float v) {
        this->x /= v;
        this->y /= v;
        this->z /= v;
        return *this;
    }

    // -- Unary operators --
    Vector3 operator+(const Vector3& m) { return m; }

    Vector3 operator-(const Vector3& m) { return Vector3(0) -= m; }

    // -- Binary operators --
    Vector3 operator+(const Vector3& m, const Vector3& s) { return Vector3(m) += s; }


    Vector3 operator+(const float& s, const Vector3& m) {
        return Vector3(m) += s;
    }


    Vector3 operator+(const Vector3& m, const float& s) {
        return Vector3(m) += s;
    }

    Vector3 operator-(const Vector3& m, const Vector3& s) { return Vector3(m) -= s; }


    Vector3 operator-(const float& m, const Vector3& s) {
        return Vector3(m) -= s;
    }


    Vector3 operator-(const Vector3& s, const float& m) {
        return Vector3(m) -= s;
    }

    Vector3 operator*(const Vector3& m, const Vector3& s) { return Vector3(m) *= s; }


    Vector3 operator*(const float& m, const Vector3& s) {
        return Vector3(m) *= s;
    }


    Vector3 operator*(const Vector3& s, const float& m) {
        return Vector3(m) *= s;
    }

    Vector3 operator/(const Vector3& m, const Vector3& s) { return Vector3(m) /= s; }


    Vector3 operator/(const float& m, const Vector3& s) {
        return Vector3(m) /= s;
    }


    Vector3 operator/(const Vector3& s, const float& m) {
        return Vector3(s) /= Vector3(m);
    }

    // -- Boolean operators --
    bool operator==(const Vector3& m, const Vector3& s) { return (m.x == s.x) && (m.y == s.y) && (m.z == s.z); }

    bool operator!=(const Vector3& m, const Vector3& s) { return (m.x != s.x) || (m.y != s.y) || (m.z != s.z); }

    /***********************************************************************
     ******************************* Vector4 *******************************
     ***********************************************************************/
    const Vector4 Vector4::ZERO(0, 0, 0, 0);
    const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);

    Vector4::Vector4() { operator=(ZERO); }

    Vector4::Vector4(const Vector4& m) {
        x = m.x;
        y = m.y;
        z = m.z;
        w = m.w;
    }

    Vector4::Vector4(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Vector4::Vector4(const Vector3& v, float w){
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = w;
    }

    Vector4::Vector4(float scalar) {
        x = scalar;
        y = scalar;
        z = scalar;
        w = scalar;
    }

    float& Vector4::operator[](int i) {
        assert(i < 4);
        switch (i) {
            default:
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
    }

    const float& Vector4::operator[](int i) const {
        assert(i < 4);
        switch (i) {
            default:
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
    }

    Vector4& Vector4::operator=(const Vector4& v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = v.w;
        return *this;
    }

    Vector4& Vector4::operator+=(const Vector4& v) {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        this->w += v.w;
        return *this;
    }


    Vector4& Vector4::operator+=(float v) {
        this->x += v;
        this->y += v;
        this->z += v;
        this->w += v;
        return *this;
    }

    Vector4& Vector4::operator-=(const Vector4& v) {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        this->w -= v.w;
        return *this;
    }


    Vector4& Vector4::operator-=(float v) {
        this->x -= v;
        this->y -= v;
        this->z -= v;
        this->w -= v;
        return *this;
    }

    Vector4& Vector4::operator*=(const Vector4& v) {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        this->w *= v.w;
        return *this;
    }


    Vector4& Vector4::operator*=(float v) {
        this->x *= v;
        this->y *= v;
        this->z *= v;
        this->w *= v;
        return *this;
    }

    Vector4& Vector4::operator/=(const Vector4& v) {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        this->w /= v.w;
        return *this;
    }


    Vector4& Vector4::operator/=(float v) {
        this->x /= v;
        this->y /= v;
        this->z /= v;
        this->w /= v;
        return *this;
    }

    Vector4::operator Vector3() const {
        return Vector3(x, y, z);
    }


    // -- Unary operators --
    Vector4 operator+(const Vector4& m) { return m; }

    Vector4 operator-(const Vector4& m) { return Vector4(0) -= m; }

    // -- Binary operators --
    Vector4 operator+(const Vector4& m, const Vector4& s) { return Vector4(m) += s; }


    Vector4 operator+(const float& s, const Vector4& m) {
        return Vector4(m) += s;
    }


    Vector4 operator+(const Vector4& m, const float& s) {
        return Vector4(m) += s;
    }

    Vector4 operator-(const Vector4& m, const Vector4& s) { return Vector4(m) -= s; }


    Vector4 operator-(const float& m, const Vector4& s) {
        return Vector4(m) -= s;
    }


    Vector4 operator-(const Vector4& s, const float& m) {
        return Vector4(m) -= s;
    }

    Vector4 operator*(const Vector4& m, const Vector4& s) { return Vector4(m) *= s; }


    Vector4 operator*(const float& m, const Vector4& s) {
        return Vector4(m) *= s;
    }


    Vector4 operator*(const Vector4& s, const float& m) {
        return Vector4(m) *= s;
    }

    Vector4 operator/(const Vector4& m, const Vector4& s) { return Vector4(m) /= s; }


    Vector4 operator/(const float& m, const Vector4& s) {
        return Vector4(m) /= s;
    }

    Vector4 operator/(const Vector4& s, const float& m) {
        return Vector4(s) /= Vector4(m);
    }

    // -- Boolean operators --
    bool operator==(const Vector4& m, const Vector4& s) { return (m.x == s.x) && (m.y == s.y) && (m.z == s.z) && (m.w == s.w); }

    bool operator!=(const Vector4& m, const Vector4& s) { return (m.x != s.x) || (m.y != s.y) || (m.z != s.z) || (m.w != s.w); }

}  // namespace Ethereal
