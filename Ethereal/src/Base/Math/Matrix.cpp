#include "Matrix.h"

namespace Ethereal
{
    const Matrix3x3 Matrix3x3::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);
    const Matrix3x3 Matrix3x3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

    //-----------------------------------------------------------------------
    void Matrix3x3::setColumn(size_t col_index, const Vector3& vec)
    {
        m_mat[0][col_index] = vec.x;
        m_mat[1][col_index] = vec.y;
        m_mat[2][col_index] = vec.z;
    }
    //-----------------------------------------------------------------------
    void Matrix3x3::fromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis)
    {
        setColumn(0, x_axis);
        setColumn(1, y_axis);
        setColumn(2, z_axis);
    }

    void Matrix3x3::calculateQDUDecomposition(Matrix3x3& out_Q, Vector3& out_D, Vector3& out_U) const
    {
        // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
        // and U is upper triangular with ones on its diagonal.  Algorithm uses
        // Gram-Schmidt orthogonalization (the QR algorithm).
        //
        // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.  The matrix R has entries
        //
        //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
        //   r10 = 0      r11 = q1*m1  r12 = q1*m2
        //   r20 = 0      r21 = 0      r22 = q2*m2
        //
        // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
        // u02 = r02/r00, and u12 = r12/r11.

        // Q = rotation
        // D = scaling
        // U = shear

        // D stores the three diagonal entries r00, r11, r22
        // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

        // build orthogonal matrix Q
        float inv_length = m_mat[0][0] * m_mat[0][0] + m_mat[1][0] * m_mat[1][0] + m_mat[2][0] * m_mat[2][0];
        if (!Math::realEqual(inv_length, 0))
            inv_length = Math::invSqrt(inv_length);

        out_Q[0][0] = m_mat[0][0] * inv_length;
        out_Q[1][0] = m_mat[1][0] * inv_length;
        out_Q[2][0] = m_mat[2][0] * inv_length;

        float dot   = out_Q[0][0] * m_mat[0][1] + out_Q[1][0] * m_mat[1][1] + out_Q[2][0] * m_mat[2][1];
        out_Q[0][1] = m_mat[0][1] - dot * out_Q[0][0];
        out_Q[1][1] = m_mat[1][1] - dot * out_Q[1][0];
        out_Q[2][1] = m_mat[2][1] - dot * out_Q[2][0];
        inv_length  = out_Q[0][1] * out_Q[0][1] + out_Q[1][1] * out_Q[1][1] + out_Q[2][1] * out_Q[2][1];
        if (!Math::realEqual(inv_length, 0))
            inv_length = Math::invSqrt(inv_length);

        out_Q[0][1] *= inv_length;
        out_Q[1][1] *= inv_length;
        out_Q[2][1] *= inv_length;

        dot         = out_Q[0][0] * m_mat[0][2] + out_Q[1][0] * m_mat[1][2] + out_Q[2][0] * m_mat[2][2];
        out_Q[0][2] = m_mat[0][2] - dot * out_Q[0][0];
        out_Q[1][2] = m_mat[1][2] - dot * out_Q[1][0];
        out_Q[2][2] = m_mat[2][2] - dot * out_Q[2][0];
        dot         = out_Q[0][1] * m_mat[0][2] + out_Q[1][1] * m_mat[1][2] + out_Q[2][1] * m_mat[2][2];
        out_Q[0][2] -= dot * out_Q[0][1];
        out_Q[1][2] -= dot * out_Q[1][1];
        out_Q[2][2] -= dot * out_Q[2][1];
        inv_length = out_Q[0][2] * out_Q[0][2] + out_Q[1][2] * out_Q[1][2] + out_Q[2][2] * out_Q[2][2];
        if (!Math::realEqual(inv_length, 0))
            inv_length = Math::invSqrt(inv_length);

        out_Q[0][2] *= inv_length;
        out_Q[1][2] *= inv_length;
        out_Q[2][2] *= inv_length;

        // guarantee that orthogonal matrix has determinant 1 (no reflections)
        float det = out_Q[0][0] * out_Q[1][1] * out_Q[2][2] + out_Q[0][1] * out_Q[1][2] * out_Q[2][0] +
                    out_Q[0][2] * out_Q[1][0] * out_Q[2][1] - out_Q[0][2] * out_Q[1][1] * out_Q[2][0] -
                    out_Q[0][1] * out_Q[1][0] * out_Q[2][2] - out_Q[0][0] * out_Q[1][2] * out_Q[2][1];

        if (det < 0.0)
        {
            for (size_t row_index = 0; row_index < 3; row_index++)
                for (size_t rol_index = 0; rol_index < 3; rol_index++)
                    out_Q[row_index][rol_index] = -out_Q[row_index][rol_index];
        }

        // build "right" matrix R
        Matrix3x3 R;
        R[0][0] = out_Q[0][0] * m_mat[0][0] + out_Q[1][0] * m_mat[1][0] + out_Q[2][0] * m_mat[2][0];
        R[0][1] = out_Q[0][0] * m_mat[0][1] + out_Q[1][0] * m_mat[1][1] + out_Q[2][0] * m_mat[2][1];
        R[1][1] = out_Q[0][1] * m_mat[0][1] + out_Q[1][1] * m_mat[1][1] + out_Q[2][1] * m_mat[2][1];
        R[0][2] = out_Q[0][0] * m_mat[0][2] + out_Q[1][0] * m_mat[1][2] + out_Q[2][0] * m_mat[2][2];
        R[1][2] = out_Q[0][1] * m_mat[0][2] + out_Q[1][1] * m_mat[1][2] + out_Q[2][1] * m_mat[2][2];
        R[2][2] = out_Q[0][2] * m_mat[0][2] + out_Q[1][2] * m_mat[1][2] + out_Q[2][2] * m_mat[2][2];

        // the scaling component
        out_D[0] = R[0][0];
        out_D[1] = R[1][1];
        out_D[2] = R[2][2];

        // the shear component
        float inv_d0 = 1.0f / out_D[0];
        out_U[0]     = R[0][1] * inv_d0;
        out_U[1]     = R[0][2] * inv_d0;
        out_U[2]     = R[1][2] / out_D[1];
    }

    void Matrix3x3::toAngleAxis(Vector3& axis, Radian& radian) const
    {
        // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
        // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
        // I is the identity and
        //
        //       +-        -+
        //   P = |  0 -z +y |
        //       | +z  0 -x |
        //       | -y +x  0 |
        //       +-        -+
        //
        // If A > 0, R represents a counterclockwise rotation about the axis in
        // the sense of looking from the tip of the axis vector towards the
        // origin.  Some algebra will show that
        //
        //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
        //
        // In the event that A = pi, R-R^t = 0 which prevents us from extracting
        // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
        // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
        // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
        // it does not matter which sign you choose on the square roots.

        float trace = m_mat[0][0] + m_mat[1][1] + m_mat[2][2];
        float cos_v = 0.5f * (trace - 1.0f);
        radian      = Math::acos(cos_v); // in [0,PI]

        if (radian > Radian(0.0))
        {
            if (radian < Radian(Math_PI))
            {
                axis.x = m_mat[2][1] - m_mat[1][2];
                axis.y = m_mat[0][2] - m_mat[2][0];
                axis.z = m_mat[1][0] - m_mat[0][1];
                axis.normalise();
            }
            else
            {
                // angle is PI
                float half_inv;
                if (m_mat[0][0] >= m_mat[1][1])
                {
                    // r00 >= r11
                    if (m_mat[0][0] >= m_mat[2][2])
                    {
                        // r00 is maximum diagonal term
                        axis.x   = 0.5f * Math::sqrt(m_mat[0][0] - m_mat[1][1] - m_mat[2][2] + 1.0f);
                        half_inv = 0.5f / axis.x;
                        axis.y   = half_inv * m_mat[0][1];
                        axis.z   = half_inv * m_mat[0][2];
                    }
                    else
                    {
                        // r22 is maximum diagonal term
                        axis.z   = 0.5f * Math::sqrt(m_mat[2][2] - m_mat[0][0] - m_mat[1][1] + 1.0f);
                        half_inv = 0.5f / axis.z;
                        axis.x   = half_inv * m_mat[0][2];
                        axis.y   = half_inv * m_mat[1][2];
                    }
                }
                else
                {
                    // r11 > r00
                    if (m_mat[1][1] >= m_mat[2][2])
                    {
                        // r11 is maximum diagonal term
                        axis.y   = 0.5f * Math::sqrt(m_mat[1][1] - m_mat[0][0] - m_mat[2][2] + 1.0f);
                        half_inv = 0.5f / axis.y;
                        axis.x   = half_inv * m_mat[0][1];
                        axis.z   = half_inv * m_mat[1][2];
                    }
                    else
                    {
                        // r22 is maximum diagonal term
                        axis.z   = 0.5f * Math::sqrt(m_mat[2][2] - m_mat[0][0] - m_mat[1][1] + 1.0f);
                        half_inv = 0.5f / axis.z;
                        axis.x   = half_inv * m_mat[0][2];
                        axis.y   = half_inv * m_mat[1][2];
                    }
                }
            }
        }
        else
        {
            // The angle is 0 and the matrix is the identity.  Any axis will
            // work, so just use the x-axis.
            axis.x = 1.0;
            axis.y = 0.0;
            axis.z = 0.0;
        }
    }
    //-----------------------------------------------------------------------
    void Matrix3x3::fromAngleAxis(const Vector3& axis, const Radian& radian)
    {
        float cos_v         = Math::cos(radian);
        float sin_v         = Math::sin(radian);
        float one_minus_cos = 1.0f - cos_v;
        float x2            = axis.x * axis.x;
        float y2            = axis.y * axis.y;
        float z2            = axis.z * axis.z;
        float xym           = axis.x * axis.y * one_minus_cos;
        float xzm           = axis.x * axis.z * one_minus_cos;
        float yzm           = axis.y * axis.z * one_minus_cos;
        float x_sin_v       = axis.x * sin_v;
        float y_sin_v       = axis.y * sin_v;
        float z_sinv        = axis.z * sin_v;

        m_mat[0][0] = x2 * one_minus_cos + cos_v;
        m_mat[0][1] = xym - z_sinv;
        m_mat[0][2] = xzm + y_sin_v;
        m_mat[1][0] = xym + z_sinv;
        m_mat[1][1] = y2 * one_minus_cos + cos_v;
        m_mat[1][2] = yzm - x_sin_v;
        m_mat[2][0] = xzm - y_sin_v;
        m_mat[2][1] = yzm + x_sin_v;
        m_mat[2][2] = z2 * one_minus_cos + cos_v;
    }

    const Matrix4x4 Matrix4x4::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    const Matrix4x4 Matrix4x4::ZEROAFFINE(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);

    const Matrix4x4 Matrix4x4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

    //-----------------------------------------------------------------------
    Matrix4x4 Matrix4x4::adjoint() const
    {
        return Matrix4x4(getMinor(1, 2, 3, 1, 2, 3),
                         -getMinor(0, 2, 3, 1, 2, 3),
                         getMinor(0, 1, 3, 1, 2, 3),
                         -getMinor(0, 1, 2, 1, 2, 3),

                         -getMinor(1, 2, 3, 0, 2, 3),
                         getMinor(0, 2, 3, 0, 2, 3),
                         -getMinor(0, 1, 3, 0, 2, 3),
                         getMinor(0, 1, 2, 0, 2, 3),

                         getMinor(1, 2, 3, 0, 1, 3),
                         -getMinor(0, 2, 3, 0, 1, 3),
                         getMinor(0, 1, 3, 0, 1, 3),
                         -getMinor(0, 1, 2, 0, 1, 3),

                         -getMinor(1, 2, 3, 0, 1, 2),
                         getMinor(0, 2, 3, 0, 1, 2),
                         -getMinor(0, 1, 3, 0, 1, 2),
                         getMinor(0, 1, 2, 0, 1, 2));
    }

    //-----------------------------------------------------------------------
    Matrix4x4 Matrix4x4::inverseAffine(void) const
    {
        assert(isAffine());

        float m10 = m_mat[1][0], m11 = m_mat[1][1], m12 = m_mat[1][2];
        float m20 = m_mat[2][0], m21 = m_mat[2][1], m22 = m_mat[2][2];

        float t00 = m22 * m11 - m21 * m12;
        float t10 = m20 * m12 - m22 * m10;
        float t20 = m21 * m10 - m20 * m11;

        float m00 = m_mat[0][0], m01 = m_mat[0][1], m02 = m_mat[0][2];

        float inv_det = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

        t00 *= inv_det;
        t10 *= inv_det;
        t20 *= inv_det;

        m00 *= inv_det;
        m01 *= inv_det;
        m02 *= inv_det;

        float r00 = t00;
        float r01 = m02 * m21 - m01 * m22;
        float r02 = m01 * m12 - m02 * m11;

        float r10 = t10;
        float r11 = m00 * m22 - m02 * m20;
        float r12 = m02 * m10 - m00 * m12;

        float r20 = t20;
        float r21 = m01 * m20 - m00 * m21;
        float r22 = m00 * m11 - m01 * m10;

        float m03 = m_mat[0][3], m13 = m_mat[1][3], m23 = m_mat[2][3];

        float r03 = -(r00 * m03 + r01 * m13 + r02 * m23);
        float r13 = -(r10 * m03 + r11 * m13 + r12 * m23);
        float r23 = -(r20 * m03 + r21 * m13 + r22 * m23);

        return Matrix4x4(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, 0, 0, 0, 1);
    }
    //-----------------------------------------------------------------------
    void Matrix4x4::makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
    {
        // Ordering:
        //    1. Scale
        //    2. Rotate
        //    3. Translate

        Matrix3x3 rot3x3;
        orientation.toRotationMatrix(rot3x3);

        // Set up final matrix with scale, rotation and translation
        m_mat[0][0] = scale.x * rot3x3[0][0];
        m_mat[0][1] = scale.y * rot3x3[0][1];
        m_mat[0][2] = scale.z * rot3x3[0][2];
        m_mat[0][3] = position.x;
        m_mat[1][0] = scale.x * rot3x3[1][0];
        m_mat[1][1] = scale.y * rot3x3[1][1];
        m_mat[1][2] = scale.z * rot3x3[1][2];
        m_mat[1][3] = position.y;
        m_mat[2][0] = scale.x * rot3x3[2][0];
        m_mat[2][1] = scale.y * rot3x3[2][1];
        m_mat[2][2] = scale.z * rot3x3[2][2];
        m_mat[2][3] = position.z;

        // No projection term
        m_mat[3][0] = 0;
        m_mat[3][1] = 0;
        m_mat[3][2] = 0;
        m_mat[3][3] = 1;
    }

    //-----------------------------------------------------------------------
    void Matrix4x4::makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
    {
        // Invert the parameters
        Vector3    inv_translate = -position;
        Vector3    inv_scale(1 / scale.x, 1 / scale.y, 1 / scale.z);
        Quaternion inv_rot = orientation.inverse();

        // Because we're inverting, order is translation, rotation, scale
        // So make translation relative to scale & rotation
        inv_translate = inv_rot * inv_translate; // rotate
        inv_translate *= inv_scale;              // scale

        // Next, make a 3x3 rotation matrix
        Matrix3x3 rot3x3;
        inv_rot.toRotationMatrix(rot3x3);

        // Set up final matrix with scale, rotation and translation
        m_mat[0][0] = inv_scale.x * rot3x3[0][0];
        m_mat[0][1] = inv_scale.x * rot3x3[0][1];
        m_mat[0][2] = inv_scale.x * rot3x3[0][2];
        m_mat[0][3] = inv_translate.x;
        m_mat[1][0] = inv_scale.y * rot3x3[1][0];
        m_mat[1][1] = inv_scale.y * rot3x3[1][1];
        m_mat[1][2] = inv_scale.y * rot3x3[1][2];
        m_mat[1][3] = inv_translate.y;
        m_mat[2][0] = inv_scale.z * rot3x3[2][0];
        m_mat[2][1] = inv_scale.z * rot3x3[2][1];
        m_mat[2][2] = inv_scale.z * rot3x3[2][2];
        m_mat[2][3] = inv_translate.z;

        // No projection term
        m_mat[3][0] = 0;
        m_mat[3][1] = 0;
        m_mat[3][2] = 0;
        m_mat[3][3] = 1;
    }
    //-----------------------------------------------------------------------
    void Matrix4x4::decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const
    {
        // ASSERT(isAffine());

        Matrix3x3 m3x3;
        extract3x3Matrix(m3x3);

        Matrix3x3 mat_q;
        Vector3   vec_u;
        m3x3.calculateQDUDecomposition(mat_q, scale, vec_u);

        orientation = Quaternion(mat_q);
        position    = Vector3(m_mat[0][3], m_mat[1][3], m_mat[2][3]);
    }

    //-----------------------------------------------------------------------
    void Matrix4x4::decompositionWithoutScale(class Vector3& position, class Quaternion& rotation) const
    {
        // ASSERT(isAffine());

        Matrix3x3 m3x3;
        extract3x3Matrix(m3x3);

        Matrix3x3 mat_q;
        Vector3   vec_u;
        Vector3   scale;
        m3x3.calculateQDUDecomposition(mat_q, scale, vec_u);

        rotation = Quaternion(mat_q);
        position = Vector3(m_mat[0][3], m_mat[1][3], m_mat[2][3]);
    }

    Vector4 operator*(const Vector4& v, const Matrix4x4& mat)
    {
        return Vector4(v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0] + v.w * mat[3][0],
                       v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1] + v.w * mat[3][1],
                       v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2] + v.w * mat[3][2],
                       v.x * mat[0][3] + v.y * mat[1][3] + v.z * mat[2][3] + v.w * mat[3][3]);
    }
}  // namespace Ethereal