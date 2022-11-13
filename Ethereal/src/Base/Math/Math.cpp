#include "Math.h"

#include "Base/Math/Matrix.h"

namespace Ethereal
{
    Math::AngleUnit Math::k_AngleUnit;

    Math::Math() { k_AngleUnit = AngleUnit::AU_DEGREE; }

    bool Math::realEqual(float a, float b, float tolerance /* = std::numeric_limits<float>::epsilon() */) { return std::fabs(b - a) <= tolerance; }

    float Math::degreesToRadians(float degrees) { return degrees * Math_fDeg2Rad; }
    Vector3 Math::degreesToRadians(const Vector3& degrees) { return degrees * Math_fDeg2Rad; }

    float Math::radiansToDegrees(float radians) { return radians * Math_fRad2Deg; }
    Vector3 Math::radiansToDegrees(const Vector3& radians) { return radians * Math_fRad2Deg; }

    float Math::angleUnitsToRadians(float angleunits) {
        if (k_AngleUnit == AngleUnit::AU_DEGREE) return angleunits * Math_fDeg2Rad;

        return angleunits;
    }

    float Math::radiansToAngleUnits(float radians) {
        if (k_AngleUnit == AngleUnit::AU_DEGREE) return radians * Math_fRad2Deg;

        return radians;
    }

    float Math::angleUnitsToDegrees(float angleunits) {
        if (k_AngleUnit == AngleUnit::AU_RADIAN) return angleunits * Math_fRad2Deg;

        return angleunits;
    }

    float Math::degreesToAngleUnits(float degrees) {
        if (k_AngleUnit == AngleUnit::AU_RADIAN) return degrees * Math_fDeg2Rad;

        return degrees;
    }

    Radian Math::acos(float value) {
        if (-1.0 < value) {
            if (value < 1.0) return Radian(::acos(value));

            return Radian(0.0);
        }

        return Radian(Math_PI);
    }
    //-----------------------------------------------------------------------
    Radian Math::asin(float value) {
        if (-1.0 < value) {
            if (value < 1.0) return Radian(::asin(value));

            return Radian(Math_HALF_PI);
        }

        return Radian(-Math_HALF_PI);
    }

    Matrix4x4 Math::makeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4x4* reflect_matrix) {
        Matrix4x4 viewMatrix;

        // View matrix is:
        //
        //  [ Lx  Uy  Dz  Tx  ]
        //  [ Lx  Uy  Dz  Ty  ]
        //  [ Lx  Uy  Dz  Tz  ]
        //  [ 0   0   0   1   ]
        //
        // Where T = -(Transposed(Rot) * Pos)

        // This is most efficiently done using 3x3 Matrices
        Matrix3x3 rot;
        orientation.toRotationMatrix(rot);

        // Make the translation relative to new axes
        Matrix3x3 rotT = rot.transpose();
        Vector3 trans = -rotT * position;

        // Make final matrix
        viewMatrix = Matrix4x4::IDENTITY;
        viewMatrix.setMatrix3x3(rotT);  // fills upper 3x3
        viewMatrix[0][3] = trans.x;
        viewMatrix[1][3] = trans.y;
        viewMatrix[2][3] = trans.z;

        // Deal with reflections
        if (reflect_matrix) {
            viewMatrix = viewMatrix * (*reflect_matrix);
        }

        return viewMatrix;
    }

    Matrix4x4 Math::makeLookAtMatrix(const Vector3& eye_position, const Vector3& target_position, const Vector3& up_dir) {
        const Vector3& up = up_dir.normalisedCopy();

        Vector3 f = (target_position - eye_position).normalisedCopy();
        Vector3 s = f.crossProduct(up).normalisedCopy();
        Vector3 u = s.crossProduct(f);

        Matrix4x4 view_mat = Matrix4x4::IDENTITY;

        view_mat[0][0] = s.x;
        view_mat[0][1] = s.y;
        view_mat[0][2] = s.z;
        view_mat[0][3] = -s.dotProduct(eye_position);
        view_mat[1][0] = u.x;
        view_mat[1][1] = u.y;
        view_mat[1][2] = u.z;
        view_mat[1][3] = -u.dotProduct(eye_position);
        view_mat[2][0] = -f.x;
        view_mat[2][1] = -f.y;
        view_mat[2][2] = -f.z;
        view_mat[2][3] = f.dotProduct(eye_position);
        return view_mat;
    }

    Matrix4x4 Math::makePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar) {
        float tan_half_fovy = Math::tan(fovy / 2.f);

        Matrix4x4 ret = Matrix4x4::ZERO;
        ret[0][0] = 1.f / (aspect * tan_half_fovy);
        ret[1][1] = 1.f / tan_half_fovy;
        ret[2][2] = zfar / (znear - zfar);
        ret[3][2] = -1.f;
        ret[2][3] = -(zfar * znear) / (zfar - znear);

        return ret;
    }

    Matrix4x4 Math::makeOrthographicProjectionMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
        float inv_width = 1.0f / (right - left);
        float inv_height = 1.0f / (top - bottom);
        float inv_distance = 1.0f / (zfar - znear);

        float A = 2 * inv_width;
        float B = 2 * inv_height;
        float C = -(right + left) * inv_width;
        float D = -(top + bottom) * inv_height;
        float q = -2 * inv_distance;
        float qn = qn = -(zfar + znear) * inv_distance;

        // NB: This creates 'uniform' orthographic projection matrix,
        // which depth range [-1,1], right-handed rules
        //
        // [ A   0   0   C  ]
        // [ 0   B   0   D  ]
        // [ 0   0   q   qn ]
        // [ 0   0   0   1  ]
        //
        // A = 2 * / (right - left)
        // B = 2 * / (top - bottom)
        // C = - (right + left) / (right - left)
        // D = - (top + bottom) / (top - bottom)
        // q = - 2 / (far - near)
        // qn = - (far + near) / (far - near)

        Matrix4x4 proj_matrix = Matrix4x4::ZERO;
        proj_matrix[0][0] = A;
        proj_matrix[0][3] = C;
        proj_matrix[1][1] = B;
        proj_matrix[1][3] = D;
        proj_matrix[2][2] = q;
        proj_matrix[2][3] = qn;
        proj_matrix[3][3] = 1;

        return proj_matrix;
    }

    Matrix4x4 Math::getRotateMatrix(const Quaternion& quat) { return Matrix4x4{quat}; }
    Matrix4x4 Math::getRotateMatrix(Radian rad, const Vector3& axis) { return Matrix4x4{Quaternion{rad, axis}}; }
    Vector3 Math::getRotate(const Quaternion& quat, const Vector3& v) { return quat * v; }

    Matrix4x4 Math::getTranslateMatrix(const Vector3& trans) { return Matrix4x4::getTrans(trans); }

    Matrix4x4 Math::getScaleMatrix(const Vector3& scale) { return Matrix4x4::getScale(scale); }

    float* Math::getPtr(const Matrix4x4& mat4) {
        float data[16];
        mat4.transpose().toData(data);

        float* res = new float[16];
        memcpy(res, data, 16 * sizeof(float));
        return res;
    }

    float* Math::getPtr(const Matrix3x3& mat3) {
        float data[9];
        mat3.transpose().toData(data);

        float* res = new float[9];
        memcpy(res, data, 9 * sizeof(float));
        return res;
    }

    bool Math::decomposeTransformMatrix(const Matrix4x4& mat, Vector3& translation, Vector3& scale, Quaternion& rotation) {
        //        glm::vec3 sca, trans, skew;
        //        glm::vec4 perspective;
        //        glm::quat orientation;
        //        glm::decompose(mat.toGLM(), sca, orientation, trans, skew, perspective);
        //
        //        translation = Vector3(trans);
        //        scale = Vector3(sca);
        //        rotation = Quaternion(orientation);
        //        return true;

        using T = float;

        Matrix4x4 LocalMatrix(mat);

        // Normalize the matrix.
        if (Math::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0))) return false;

        // First, isolate perspective.  This is the messiest.
        if (Math::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0)) || Math::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0)) ||
            Math::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0))) {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        translation = Vector3(LocalMatrix[3]);
        LocalMatrix[3][0] = 0;
        LocalMatrix[3][1] = 0;
        LocalMatrix[3][2] = 0;

        Vector3 Row[3], Pdum3;

        // Now get scale and shear.
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        scale.x = Row[0].length();
        Row[0].normalise();
        scale.y = Row[1].length();
        Row[1].normalise();
        scale.z = Row[2].length();
        Row[2].normalise();

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
#if 0
        		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        		if (dot(Row[0], Pdum3) < 0)
        		{
        			for (length_t i = 0; i < 3; i++)
        			{
        				scale[i] *= static_cast<T>(-1);
        				Row[i] *= static_cast<T>(-1);
        			}
        		}
#endif
        Vector3 rot;
        rot.y = Math::asin(-Row[0][2]).valueRadians();
        if (cos(rot.y) != 0) {
            rot.x = Math::atan2(Row[1][2], Row[2][2]).valueRadians();
            rot.z = Math::atan2(Row[0][1], Row[0][0]).valueRadians();
        } else {
            rot.x = Math::atan2(-Row[2][0], Row[1][1]).valueRadians();
            rot.z = 0;
        }
        glm::mat4 a;
        rotation.fromEulerAngle(rot);
    }
}  // namespace Ethereal
