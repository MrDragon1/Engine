#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Math/Math.h"
#include "Base/Math/Quaternion.h"

namespace Ethereal
{
    REFLECTION_TYPE(Vector2)
    CLASS(Vector2, Fields) {
        REFLECTION_BODY(Vector2);

      public:
        float x{0.f}, y{0.f};

      public:
        Vector2() = default;
        glm::vec2 toGLM()const
        {
            glm::vec2 m{x,y};
            return m;
        }

        Vector2(const glm::vec2& v)
        {
            x = v.x;
            y = v.y;
        }
        Vector2(float x_, float y_) : x(x_), y(y_) {}

        explicit Vector2(float scalar) : x(scalar), y(scalar) {}

        explicit Vector2(const float v[2]) : x(v[0]), y(v[1]) {}

        explicit Vector2(float* const r) : x(r[0]), y(r[1]) {}

        float* ptr() { return &x; }

        const float* ptr() const { return &x; }

        float operator[](size_t i) const {
            assert(i < 2);
            return (i == 0 ? x : y);
        }

        float& operator[](size_t i) {
            assert(i < 2);
            return (i == 0 ? x : y);
        }

        bool operator==(const Vector2& rhs) const { return (x == rhs.x && y == rhs.y); }

        bool operator!=(const Vector2& rhs) const { return (x != rhs.x || y != rhs.y); }

        // arithmetic operations
        Vector2 operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }

        Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }

        Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }

        Vector2 operator*(const Vector2& rhs) const { return Vector2(x * rhs.x, y * rhs.y); }

        Vector2 operator/(float scale) const {
            assert(scale != 0.0);

            float inv = 1.0f / scale;
            return Vector2(x * inv, y * inv);
        }

        Vector2 operator/(const Vector2& rhs) const { return Vector2(x / rhs.x, y / rhs.y); }

        const Vector2& operator+() const { return *this; }

        Vector2 operator-() const { return Vector2(-x, -y); }

        // overloaded operators to help Vector2
        friend Vector2 operator*(float scalar, const Vector2& rhs) { return Vector2(scalar * rhs.x, scalar * rhs.y); }

        friend Vector2 operator/(float fScalar, const Vector2& rhs) { return Vector2(fScalar / rhs.x, fScalar / rhs.y); }

        friend Vector2 operator+(const Vector2& lhs, float rhs) { return Vector2(lhs.x + rhs, lhs.y + rhs); }

        friend Vector2 operator+(float lhs, const Vector2& rhs) { return Vector2(lhs + rhs.x, lhs + rhs.y); }

        friend Vector2 operator-(const Vector2& lhs, float rhs) { return Vector2(lhs.x - rhs, lhs.y - rhs); }

        friend Vector2 operator-(float lhs, const Vector2& rhs) { return Vector2(lhs - rhs.x, lhs - rhs.y); }

        // arithmetic updates
        Vector2& operator+=(const Vector2& rhs) {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        Vector2& operator+=(float scalar) {
            x += scalar;
            y += scalar;

            return *this;
        }

        Vector2& operator-=(const Vector2& rhs) {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        Vector2& operator-=(float scalar) {
            x -= scalar;
            y -= scalar;

            return *this;
        }

        Vector2& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;

            return *this;
        }

        Vector2& operator*=(const Vector2& rhs) {
            x *= rhs.x;
            y *= rhs.y;

            return *this;
        }

        Vector2& operator/=(float scalar) {
            assert(scalar != 0.0);

            float inv = 1.0f / scalar;

            x *= inv;
            y *= inv;

            return *this;
        }

        Vector2& operator/=(const Vector2& rhs) {
            x /= rhs.x;
            y /= rhs.y;

            return *this;
        }

        /** Returns the length (magnitude) of the vector.
        @warning
        This operation requires a square root and is expensive in
        terms of CPU operations. If you don't need to know the exact
        length (e.g. for just comparing lengths) use squaredLength()
        instead.
        */
        float length() const { return std::hypot(x, y); }

        /** Returns the square of the length(magnitude) of the vector.
        @remarks
        This  method is for efficiency - calculating the actual
        length of a vector requires a square root, which is expensive
        in terms of the operations required. This method returns the
        square of the length of the vector, i.e. the same as the
        length but before the square root is taken. Use this if you
        want to find the longest / shortest vector without incurring
        the square root.
        */
        float squaredLength() const { return x * x + y * y; }

        /** Returns the distance to another vector.
        @warning
        This operation requires a square root and is expensive in
        terms of CPU operations. If you don't need to know the exact
        distance (e.g. for just comparing distances) use squaredDistance()
        instead.
        */
        float distance(const Vector2& rhs) const { return (*this - rhs).length(); }

        /** Returns the square of the distance to another vector.
        @remarks
        This method is for efficiency - calculating the actual
        distance to another vector requires a square root, which is
        expensive in terms of the operations required. This method
        returns the square of the distance to another vector, i.e.
        the same as the distance but before the square root is taken.
        Use this if you want to find the longest / shortest distance
        without incurring the square root.
        */
        float squaredDistance(const Vector2& rhs) const { return (*this - rhs).squaredLength(); }

        /** Calculates the dot (scalar) product of this vector with another.
        @remarks
        The dot product can be used to calculate the angle between 2
        vectors. If both are unit vectors, the dot product is the
        cosine of the angle; otherwise the dot product must be
        divided by the product of the lengths of both vectors to get
        the cosine of the angle. This result can further be used to
        calculate the distance of a point from a plane.
        @param
        vec Vector with which to calculate the dot product (together
        with this one).
        @returns
        A float representing the dot product value.
        */
        float dotProduct(const Vector2& vec) const { return x * vec.x + y * vec.y; }

        /** Normalizes the vector.
        @remarks
        This method normalizes the vector such that it's
        length / magnitude is 1. The result is called a unit vector.
        @note
        This function will not crash for zero-sized vectors, but there
        will be no changes made to their components.
        @returns The previous length of the vector.
        */

        float normalise() {
            float lengh = std::hypot(x, y);

            if (lengh > 0.0f) {
                float inv_length = 1.0f / lengh;
                x *= inv_length;
                y *= inv_length;
            }

            return lengh;
        }

        float getX() const { return x; }
        float getY() const { return y; }

        void setX(float value) { x = value; }
        void setY(float value) { y = value; }

        /** Returns a vector at a point half way between this and the passed
        in vector.
        */
        Vector2 midPoint(const Vector2& vec) const { return Vector2((x + vec.x) * 0.5f, (y + vec.y) * 0.5f); }

        /** Returns true if the vector's scalar components are all greater
        that the ones of the vector it is compared against.
        */
        bool operator<(const Vector2& rhs) const { return x < rhs.x && y < rhs.y; }

        /** Returns true if the vector's scalar components are all smaller
        that the ones of the vector it is compared against.
        */
        bool operator>(const Vector2& rhs) const { return x > rhs.x && y > rhs.y; }

        /** Sets this vector's components to the minimum of its own and the
        ones of the passed in vector.
        @remarks
        'Minimum' in this case means the combination of the lowest
        value of x, y and z from both vectors. Lowest is taken just
        numerically, not magnitude, so -1 < 0.
        */
        void makeFloor(const Vector2& cmp) {
            if (cmp.x < x) x = cmp.x;
            if (cmp.y < y) y = cmp.y;
        }

        /** Sets this vector's components to the maximum of its own and the
        ones of the passed in vector.
        @remarks
        'Maximum' in this case means the combination of the highest
        value of x, y and z from both vectors. Highest is taken just
        numerically, not magnitude, so 1 > -3.
        */
        void makeCeil(const Vector2& cmp) {
            if (cmp.x > x) x = cmp.x;
            if (cmp.y > y) y = cmp.y;
        }

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).
        @remarks
        This method will return a vector which is perpendicular to this
        vector. There are an infinite number of possibilities but this
        method will guarantee to generate one of them. If you need more
        control you should use the Quaternion class.
        */
        Vector2 perpendicular(void) const { return Vector2(-y, x); }

        /** Calculates the 2 dimensional cross-product of 2 vectors, which results
        in a single floating point value which is 2 times the area of the triangle.
        */

        float crossProduct(const Vector2& rhs) const { return x * rhs.y - y * rhs.x; }

        /** Returns true if this vector is zero length. */
        bool isZeroLength(void) const {
            float sqlen = (x * x) + (y * y);
            return (sqlen < (Float_EPSILON * Float_EPSILON));
        }

        /** As normalise, except that this vector is unaffected and the
        normalised vector is returned as a copy. */
        Vector2 normalisedCopy(void) const {
            Vector2 ret = *this;
            ret.normalise();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal .
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        Vector2 reflect(const Vector2& normal) const { return Vector2(*this - (2 * this->dotProduct(normal) * normal)); }

        /// Check whether this vector contains valid values
        bool isNaN() const { return Math::isNan(x) || Math::isNan(y); }

        static Vector2 lerp(const Vector2& lhs, const Vector2& rhs, float alpha) { return lhs + alpha * (rhs - lhs); }

        // special points
        static const Vector2 ZERO;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        static const Vector2 NEGATIVE_UNIT_X;
        static const Vector2 NEGATIVE_UNIT_Y;
        static const Vector2 UNIT_SCALE;
    };

    REFLECTION_TYPE(Vector3)
    CLASS(Vector3, Fields) {
        REFLECTION_BODY(Vector3);

      public:
        float x{0.f};
        float y{0.f};
        float z{0.f};

      public:
        glm::vec3 toGLM()const
        {
            glm::vec3 m{x,y,z};
            return m;
        }

        Vector3(const glm::vec3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        Vector3() = default;
        Vector3(float x_, float y_, float z_) : x{x_}, y{y_}, z{z_} {}
        explicit Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
        explicit Vector3(const float coords[3]) : x{coords[0]}, y{coords[1]}, z{coords[2]} {}

        float operator[](size_t i) const {
            assert(i < 3);
            return *(&x + i);
        }

        float& operator[](size_t i) {
            assert(i < 3);
            return *(&x + i);
        }
        /// Pointer accessor for direct copying
        float* ptr() { return &x; }
        /// Pointer accessor for direct copying
        const float* ptr() const { return &x; }

        bool operator==(const Vector3& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z); }

        bool operator!=(const Vector3& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

        // arithmetic operations
        Vector3 operator+(const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }

        Vector3 operator-(const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }

        Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

        Vector3 operator*(const Vector3& rhs) const { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }

        Vector3 operator/(float scalar) const {
            assert(scalar != 0.0);
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        Vector3 operator/(const Vector3& rhs) const {
            assert((rhs.x != 0 && rhs.y != 0 && rhs.z != 0));
            return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
        }

        const Vector3& operator+() const { return *this; }

        Vector3 operator-() const { return Vector3(-x, -y, -z); }

        // overloaded operators to help Vector3
        friend Vector3 operator*(float scalar, const Vector3& rhs) { return Vector3(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z); }

        friend Vector3 operator/(float scalar, const Vector3& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
            return Vector3(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
        }

        friend Vector3 operator+(const Vector3& lhs, float rhs) { return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs); }

        friend Vector3 operator+(float lhs, const Vector3& rhs) { return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z); }

        friend Vector3 operator-(const Vector3& lhs, float rhs) { return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs); }

        friend Vector3 operator-(float lhs, const Vector3& rhs) { return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z); }

        // arithmetic updates
        Vector3& operator+=(const Vector3& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vector3& operator+=(float scalar) {
            x += scalar;
            y += scalar;
            z += scalar;
            return *this;
        }

        Vector3& operator-=(const Vector3& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        Vector3& operator-=(float scalar) {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            return *this;
        }

        Vector3& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3& operator*=(const Vector3& rhs) {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            return *this;
        }

        Vector3& operator/=(float scalar) {
            assert(scalar != 0.0);
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        Vector3& operator/=(const Vector3& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            return *this;
        }

        /** Returns the length (magnitude) of the vector.
        @warning
        This operation requires a square root and is expensive in
        terms of CPU operations. If you don't need to know the exact
        length (e.g. for just comparing lengths) use squaredLength()
        instead.
        */

        float length() const { return std::hypot(x, y, z); }

        /** Returns the square of the length(magnitude) of the vector.
        @remarks
        This  method is for efficiency - calculating the actual
        length of a vector requires a square root, which is expensive
        in terms of the operations required. This method returns the
        square of the length of the vector, i.e. the same as the
        length but before the square root is taken. Use this if you
        want to find the longest / shortest vector without incurring
        the square root.
        */
        float squaredLength() const { return x * x + y * y + z * z; }

        /** Returns the distance to another vector.
        @warning
        This operation requires a square root and is expensive in
        terms of CPU operations. If you don't need to know the exact
        distance (e.g. for just comparing distances) use squaredDistance()
        instead.
        */

        float distance(const Vector3& rhs) const { return (*this - rhs).length(); }

        /** Returns the square of the distance to another vector.
        @remarks
        This method is for efficiency - calculating the actual
        distance to another vector requires a square root, which is
        expensive in terms of the operations required. This method
        returns the square of the distance to another vector, i.e.
        the same as the distance but before the square root is taken.
        Use this if you want to find the longest / shortest distance
        without incurring the square root.
        */

        float squaredDistance(const Vector3& rhs) const { return (*this - rhs).squaredLength(); }

        /** Calculates the dot (scalar) product of this vector with another.
        @remarks
        The dot product can be used to calculate the angle between 2
        vectors. If both are unit vectors, the dot product is the
        cosine of the angle; otherwise the dot product must be
        divided by the product of the lengths of both vectors to get
        the cosine of the angle. This result can further be used to
        calculate the distance of a point from a plane.
        @param
        vec Vector with which to calculate the dot product (together
        with this one).
        @returns
        A float representing the dot product value.
        */

        float dotProduct(const Vector3& vec) const { return x * vec.x + y * vec.y + z * vec.z; }

        /** Normalizes the vector.
        @remarks
        This method normalizes the vector such that it's
        length / magnitude is 1. The result is called a unit vector.
        @note
        This function will not crash for zero-sized vectors, but there
        will be no changes made to their components.
        @returns The previous length of the vector.
        */

        void normalise() {
            float length = std::hypot(x, y, z);
            if (length == 0.f) return;

            float inv_lengh = 1.0f / length;
            x *= inv_lengh;
            y *= inv_lengh;
            z *= inv_lengh;
        }

        /** Calculates the cross-product of 2 vectors, i.e. the vector that
        lies perpendicular to them both.
        @remarks
        The cross-product is normally used to calculate the normal
        vector of a plane, by calculating the cross-product of 2
        non-equivalent vectors which lie on the plane (e.g. 2 edges
        of a triangle).
        @param
        vec Vector which, together with this one, will be used to
        calculate the cross-product.
        @returns
        A vector which is the result of the cross-product. This
        vector will <b>NOT</b> be normalised, to maximize efficiency
        - call Vector3::normalise on the result if you wish this to
        be done. As for which side the resultant vector will be on, the
        returned vector will be on the side from which the arc from 'this'
        to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
        = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
        This is because CHAOS uses a right-handed coordinate system.
        @par
        For a clearer explanation, look a the left and the bottom edges
        of your monitor's screen. Assume that the first vector is the
        left edge and the second vector is the bottom edge, both of
        them starting from the lower-left corner of the screen. The
        resulting vector is going to be perpendicular to both of them
        and will go <i>inside</i> the screen, towards the cathode tube
        (assuming you're using a CRT monitor, of course).
        */

        Vector3 crossProduct(const Vector3& rhs) const { return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x); }

        /** Sets this vector's components to the minimum of its own and the
        ones of the passed in vector.
        @remarks
        'Minimum' in this case means the combination of the lowest
        value of x, y and z from both vectors. Lowest is taken just
        numerically, not magnitude, so -1 < 0.
        */
        void makeFloor(const Vector3& cmp) {
            if (cmp.x < x) x = cmp.x;
            if (cmp.y < y) y = cmp.y;
            if (cmp.z < z) z = cmp.z;
        }

        /** Sets this vector's components to the maximum of its own and the
        ones of the passed in vector.
        @remarks
        'Maximum' in this case means the combination of the highest
        value of x, y and z from both vectors. Highest is taken just
        numerically, not magnitude, so 1 > -3.
        */
        void makeCeil(const Vector3& cmp) {
            if (cmp.x > x) x = cmp.x;
            if (cmp.y > y) y = cmp.y;
            if (cmp.z > z) z = cmp.z;
        }

        /** Gets the angle between 2 vectors.
        @remarks
        Vectors do not have to be unit-length but must represent directions.
        */

        Radian angleBetween(const Vector3& dest) const {
            float len_product = length() * dest.length();

            // Divide by zero check
            if (len_product < 1e-6f) len_product = 1e-6f;

            float f = dotProduct(dest) / len_product;

            f = Math::clamp(f, (float)-1.0, (float)1.0);
            return Math::acos(f);
        }
        /** Gets the shortest arc quaternion to rotate this vector to the destination
        vector.
        @remarks
        If you call this with a dest vector that is close to the inverse
        of this vector, we will rotate 180 degrees around the 'fallbackAxis'
        (if specified, or a generated axis if not) since in this case
        ANY axis of rotation is valid.
        */

        Quaternion getRotationTo(const Vector3& dest, const Vector3& fallback_axis = Vector3::ZERO) const {
            // Based on Stan Melax's article in Game Programming Gems
            Quaternion q;
            // Copy, since cannot modify local
            Vector3 v0 = *this;
            Vector3 v1 = dest;
            v0.normalise();
            v1.normalise();

            float d = v0.dotProduct(v1);
            // If dot == 1, vectors are the same
            if (d >= 1.0f) {
                return Quaternion::IDENTITY;
            }
            if (d < (1e-6f - 1.0f)) {
                if (fallback_axis != Vector3::ZERO) {
                    // rotate 180 degrees about the fall back axis
                    q.fromAngleAxis(Radian(Math_PI), fallback_axis);
                } else {
                    // Generate an axis
                    Vector3 axis = Vector3::UNIT_X.crossProduct(*this);
                    if (axis.isZeroLength())  // pick another if collinear
                        axis = Vector3::UNIT_Y.crossProduct(*this);
                    axis.normalise();
                    q.fromAngleAxis(Radian(Math_PI), axis);
                }
            } else {
                float s = Math::sqrt((1 + d) * 2);
                float invs = 1 / s;

                Vector3 c = v0.crossProduct(v1);

                q.x = c.x * invs;
                q.y = c.y * invs;
                q.z = c.z * invs;
                q.w = s * 0.5f;
                q.normalise();
            }
            return q;
        }

        /** Returns true if this vector is zero length. */
        bool isZeroLength(void) const {
            float sqlen = (x * x) + (y * y) + (z * z);
            return (sqlen < (1e-06 * 1e-06));
        }

        bool isZero() const { return x == 0.f && y == 0.f && z == 0.f; }

        /** As normalise, except that this vector is unaffected and the
        normalised vector is returned as a copy. */

        Vector3 normalisedCopy(void) const {
            Vector3 ret = *this;
            ret.normalise();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal .
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        Vector3 reflect(const Vector3& normal) const { return Vector3(*this - (2 * this->dotProduct(normal) * normal)); }

        /** Calculates projection to a plane with the given normal
        @param normal The normal of given plane
        */
        Vector3 project(const Vector3& normal) const { return Vector3(*this - (this->dotProduct(normal) * normal)); }

        Vector3 absoluteCopy() const { return Vector3(fabsf(x), fabsf(y), fabsf(z)); }

        static Vector3 lerp(const Vector3& lhs, const Vector3& rhs, float alpha) { return lhs + alpha * (rhs - lhs); }

        static Vector3 clamp(const Vector3& v, const Vector3& min, const Vector3& max) {
            return Vector3(Math::clamp(v.x, min.x, max.x), Math::clamp(v.y, min.y, max.y), Math::clamp(v.z, min.z, max.z));
        }

        static float getMaxElement(const Vector3& v) { return Math::getMaxElement(v.x, v.y, v.z); }
        bool isNaN() const { return Math::isNan(x) || Math::isNan(y) || Math::isNan(z); }
        // special points
        static const Vector3 ZERO;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 NEGATIVE_UNIT_X;
        static const Vector3 NEGATIVE_UNIT_Y;
        static const Vector3 NEGATIVE_UNIT_Z;
        static const Vector3 UNIT_SCALE;
    };

    REFLECTION_TYPE(Vector4)
    CLASS(Vector4, Fields) {
        REFLECTION_BODY(Vector4);

      public:
        float x{0.f}, y{0.f}, z{0.f}, w{0.f};

      public:
        glm::vec4 toGLM()const
        {
            glm::vec4 m{x,y,z,w};
            return m;
        }

        Vector4(const glm::vec4& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
        }
        Vector4() = default;
        Vector4(float x_, float y_, float z_, float w_) : x{x_}, y{y_}, z{z_}, w{w_} {}
        Vector4(const Vector3& v3, float w_) : x{v3.x}, y{v3.y}, z{v3.z}, w{w_} {}
        explicit Vector4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        explicit Vector4(float coords[4]) : x{coords[0]}, y{coords[1]}, z{coords[2]}, w{coords[3]} {}

        float operator[](size_t i) const {
            assert(i < 4);
            return *(&x + i);
        }

        float& operator[](size_t i) {
            assert(i < 4);
            return *(&x + i);
        }

        /// Pointer accessor for direct copying
        float* ptr() { return &x; }
        /// Pointer accessor for direct copying
        const float* ptr() const { return &x; }

        Vector4& operator=(float scalar) {
            x = scalar;
            y = scalar;
            z = scalar;
            w = scalar;
            return *this;
        }

        bool operator==(const Vector4& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w); }

        bool operator!=(const Vector4& rhs) const { return !(rhs == *this); }

        Vector4 operator+(const Vector4& rhs) const { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
        Vector4 operator-(const Vector4& rhs) const { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
        Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
        Vector4 operator*(const Vector4& rhs) const { return Vector4(rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w); }
        Vector4 operator/(float scalar) const {
            assert(scalar != 0.0);
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }
        Vector4 operator/(const Vector4& rhs) const {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
            return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
        }

        const Vector4& operator+() const { return *this; }

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

        friend Vector4 operator*(float scalar, const Vector4& rhs) { return Vector4(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w); }

        friend Vector4 operator/(float scalar, const Vector4& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
            return Vector4(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
        }

        friend Vector4 operator+(const Vector4& lhs, float rhs) { return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs); }

        friend Vector4 operator+(float lhs, const Vector4& rhs) { return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w); }

        friend Vector4 operator-(const Vector4& lhs, float rhs) { return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs); }

        friend Vector4 operator-(float lhs, const Vector4& rhs) { return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w); }

        // arithmetic updates
        Vector4& operator+=(const Vector4& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }

        Vector4& operator-=(const Vector4& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }

        Vector4& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Vector4& operator+=(float scalar) {
            x += scalar;
            y += scalar;
            z += scalar;
            w += scalar;
            return *this;
        }

        Vector4& operator-=(float scalar) {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            w -= scalar;
            return *this;
        }

        Vector4& operator*=(const Vector4& rhs) {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            w *= rhs.w;
            return *this;
        }

        Vector4& operator/=(float scalar) {
            assert(scalar != 0.0);

            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        Vector4& operator/=(const Vector4& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            w /= rhs.w;
            return *this;
        }

        operator Vector3() { return Vector3{x, y, z}; }

        /** Calculates the dot (scalar) product of this vector with another.
        @param
        vec Vector with which to calculate the dot product (together
        with this one).
        @returns
        A float representing the dot product value.
        */
        float dotProduct(const Vector4& vec) const { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }

        /// Check whether this vector contains valid values
        bool isNaN() const { return Math::isNan(x) || Math::isNan(y) || Math::isNan(z) || Math::isNan(w); }

        // special
        static const Vector4 ZERO;
        static const Vector4 UNIT_SCALE;
    };

}  // namespace Ethereal