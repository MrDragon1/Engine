#include "Math.h"

#include "Base/Math/Matrix.h"

namespace Ethereal
{
    namespace Math{
        float Radians(float degrees) { return degrees * PI / 180.f; }
        Vector3 Radians(const Vector3& degrees) { return degrees * PI / 180.f; }
        float Degrees(float radians) { return radians * 180.f / PI; }
        Vector3 Degrees(const Vector3& radians) { return radians * 180.f / PI; }

        float Sin(float v) { return sinf(v); }
        Vector2 Sin(const Vector2& v) { return Vector2(sinf(v.x), sinf(v.y)); }
        Vector3 Sin(const Vector3& v) { return Vector3(sinf(v.x), sinf(v.y), sinf(v.z)); }
        Vector4 Sin(const Vector4& v) { return Vector4(sinf(v.x), sinf(v.y), sinf(v.z), sinf(v.w)); }
        float Cos(float v) { return cosf(v); }
        Vector2 Cos(const Vector2& v) { return Vector2(cosf(v.x), cosf(v.y)); }
        Vector3 Cos(const Vector3& v) { return Vector3(cosf(v.x), cosf(v.y), cosf(v.z)); }
        Vector4 Cos(const Vector4& v) { return Vector4(cosf(v.x), cosf(v.y), cosf(v.z), cosf(v.w)); }
        float Tan(float v) { return tanf(v); }
        Vector2 Tan(const Vector2& v) { return Vector2(tanf(v.x), tanf(v.y)); }
        Vector3 Tan(const Vector3& v) { return Vector3(tanf(v.x), tanf(v.y), tanf(v.z)); }
        Vector4 Tan(const Vector4& v) { return Vector4(tanf(v.x), tanf(v.y), tanf(v.z), tanf(v.w)); }

        float Min(float a, float b) { return a < b ? a : b; }
        float Max(float a, float b) { return a > b ? a : b; }

        bool EpsilonEqual(float a, float b, float epsilon /* = EPSILONF*/ ) { return fabsf(a - b) < epsilon; }
        bool EpsilonEqual(const Vector2& a, const Vector2& b, float epsilon /* = EPSILONF*/ ) { return EpsilonEqual(a.x, b.x, epsilon) && EpsilonEqual(a.y, b.y, epsilon); }

        float Clamp(float v, float min, float max) { return v < min ? min : v > max ? max : v; }
        /*********************************************************************
        ******************************* Vector *******************************
        **********************************************************************/
        float* Ptr(Vector2& v) { return &v.x; }
        const float* Ptr(const Vector2& v) { return &v.x; }
        float* Ptr(Vector3& v) { return &v.x; }
        const float* Ptr(const Vector3& v) { return &v.x; }
        float* Ptr(Vector4& v) { return &v.x; }
        const float* Ptr(const Vector4& v) { return &v.x; }

        float Dot(const Vector2& a,const Vector2& b){
            Vector2 tmp(a * b);
            return tmp.x + tmp.y;
        }
        float Dot(const Vector3& a,const Vector3& b){
            Vector3 tmp(a * b);
            return tmp.x + tmp.y + tmp.z;
        }
        float Dot(const Vector4& a,const Vector4& b){
            Vector4 tmp(a * b);
            return tmp.x + tmp.y + tmp.z + tmp.w;
        }

        Vector2 Cross(const Vector2& a,const Vector2& b){
            return {a.x * b.y - a.y * b.x};
        }
        Vector3 Cross(const Vector3& a,const Vector3& b){
            return {a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x};
        }
        Vector4 Cross(const Vector4& a,const Vector4& b){
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x,
                0
            };
        }

        float Length(const Vector2& v){
            return sqrtf(Dot(v,v));
        }
        float Length(const Vector3& v){
            return sqrtf(Dot(v,v));
        }
        float Length(const Vector4& v){
            return sqrtf(Dot(v,v));
        }



        Vector2 Normalize(const Vector2& v) {
            return v / Length(v);
        }
        Vector3 Normalize(const Vector3& v) {
            return v / Length(v);
        }
        Vector4 Normalize(const Vector4& v) {
            return v / Length(v);
        }

        float Mix(const float& a, const float& b, float t) {
            return a * (1 - t) + b * t;
        }
        Vector2 Mix(const Vector2& a,const Vector2& b,float t){
            return a * (1 - t) + b * t;
        }
        Vector3 Mix(const Vector3& a,const Vector3& b,float t){
            return a * (1 - t) + b * t;
        }
        Vector4 Mix(const Vector4& a,const Vector4& b,float t){
            return a * (1 - t) + b * t;
        }

        Vector2 Scale(const Vector2& v,float length) {
            return Normalize(v) * length;
        }
        Vector3 Scale(const Vector3& v,float length) {
            return Normalize(v) * length;
        }
        Vector4 Scale(const Vector4& v,float length) {
            return Normalize(v) * length;
        }

        Vector2 Combine(const Vector2& a,const Vector2& b,float fa,float fb) {
            return a * fa + b * fb;
        }
        Vector3 Combine(const Vector3& a,const Vector3& b,float fa,float fb) {
            return a * fa + b * fb;
        }
        Vector4 Combine(const Vector4& a,const Vector4& b,float fa,float fb) {
            return a * fa + b * fb;
        }

        /*********************************************************************
        ******************************* Matrix *******************************
        **********************************************************************/
        float* Ptr(Matrix3& m){
            return &m.value0[0];
        }
        const float* Ptr(const Matrix3& m){
            return &m.value0[0];
        }
        float* Ptr(Matrix4& m){
            return &m.value0[0];
        }
        const float* Ptr(const Matrix4& m){
            return &m.value0[0];
        }


        Matrix4 Inverse(const Matrix4& m) {
            float Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            float Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
            float Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

            float Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            float Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
            float Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

            float Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            float Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
            float Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

            float Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            float Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
            float Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

            float Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            float Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
            float Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

            float Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
            float Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
            float Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

            Vector4 Fac0(Coef00, Coef00, Coef02, Coef03);
            Vector4 Fac1(Coef04, Coef04, Coef06, Coef07);
            Vector4 Fac2(Coef08, Coef08, Coef10, Coef11);
            Vector4 Fac3(Coef12, Coef12, Coef14, Coef15);
            Vector4 Fac4(Coef16, Coef16, Coef18, Coef19);
            Vector4 Fac5(Coef20, Coef20, Coef22, Coef23);

            Vector4 Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
            Vector4 Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
            Vector4 Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
            Vector4 Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

            Vector4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
            Vector4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
            Vector4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
            Vector4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

            Vector4 SignA(+1, -1, +1, -1);
            Vector4 SignB(-1, +1, -1, +1);
            Matrix4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

            Vector4 Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

            Vector4 Dot0(m[0] * Row0);
            float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

            float OneOverDeterminant = static_cast<float>(1) / Dot1;

            return Inverse * OneOverDeterminant;
        }

        Matrix3 Inverse(const Matrix3& m) {

            float OneOverDeterminant = static_cast<float>(1) / (
                                                                   + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
                                                                   - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
                                                                   + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

            Matrix3 Inverse;
            Inverse[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDeterminant;
            Inverse[1][0] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDeterminant;
            Inverse[2][0] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDeterminant;
            Inverse[0][1] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDeterminant;
            Inverse[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDeterminant;
            Inverse[2][1] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDeterminant;
            Inverse[0][2] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDeterminant;
            Inverse[1][2] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDeterminant;
            Inverse[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDeterminant;

            return Inverse;
        }

        Matrix3 Transpose(const Matrix3& m) {
            return Matrix3(m[0][0], m[1][0], m[2][0],
                           m[0][1], m[1][1], m[2][1],
                           m[0][2], m[1][2], m[2][2]);
        }
        Matrix4 Transpose(const Matrix4& m) {
            return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
                           m[0][1], m[1][1], m[2][1], m[3][1],
                           m[0][2], m[1][2], m[2][2], m[3][2],
                           m[0][3], m[1][3], m[2][3], m[3][3]);
        }

        float Determinant(const Matrix3& m) {
            return m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
                   - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
                   + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
        }
        float Determinant(const Matrix4& m) {
            float SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            float SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            float SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            float SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            float SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            float SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            Vector4 DetCof(
                + (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02),
                - (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04),
                + (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05),
                - (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05));

            return
                m[0][0] * DetCof[0] + m[0][1] * DetCof[1] +
                m[0][2] * DetCof[2] + m[0][3] * DetCof[3];
        }

        Matrix4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
            Matrix4 Result(static_cast<float>(1));
            Result[0][0] = static_cast<float>(2) / (right - left);
            Result[1][1] = static_cast<float>(2) / (top - bottom);
            Result[2][2] = - static_cast<float>(2) / (zFar - zNear);
            Result[3][0] = - (right + left) / (right - left);
            Result[3][1] = - (top + bottom) / (top - bottom);
            Result[3][2] = - (zFar + zNear) / (zFar - zNear);
            return Result;
        }

        Matrix4 Perspective(float fovy, float aspect, float zNear, float zFar) {
            assert(abs(aspect - EPSILONF) > static_cast<float>(0));

            float const tanHalfFovy = tan(fovy / static_cast<float>(2));

            Matrix4 Result(static_cast<float>(0));
            Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
            Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
            Result[2][2] = - (zFar + zNear) / (zFar - zNear);
            Result[2][3] = - static_cast<float>(1);
            Result[3][2] = - (static_cast<float>(2) * zFar * zNear) / (zFar - zNear);
            return Result;
        }

        Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
            Vector3 const f(Normalize(center - eye));
            Vector3 const s(Normalize(Cross(f, up)));
            Vector3 const u(Cross(s, f));

            Matrix4 Result(1);
            Result[0][0] = s.x;
            Result[1][0] = s.y;
            Result[2][0] = s.z;
            Result[0][1] = u.x;
            Result[1][1] = u.y;
            Result[2][1] = u.z;
            Result[0][2] =-f.x;
            Result[1][2] =-f.y;
            Result[2][2] =-f.z;
            Result[3][0] =-Dot(s, eye);
            Result[3][1] =-Dot(u, eye);
            Result[3][2] = Dot(f, eye);
            return Result;
        }

        Matrix4 Translate(const Matrix4& m, const Vector3& v) {
            Matrix4 Result(m);
            Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
            return Result;
        }

        Matrix4 Rotate(const Matrix4& m, float angle, const Vector3& v) {
            float const c = cos(angle);
            float const s = sin(angle);

            Vector3 axis(Normalize(v));
            Vector3 temp((static_cast<float>(1) - c) * axis);

            Matrix4 Rotate;
            Rotate[0][0] = c + temp[0] * axis[0];
            Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
            Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

            Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
            Rotate[1][1] = c + temp[1] * axis[1];
            Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

            Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
            Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
            Rotate[2][2] = c + temp[2] * axis[2];

            Matrix4 Result;
            Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
            Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
            Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
            Result[3] = m[3];
            return Result;
        }

        Matrix4 Rotate(const Matrix4& m, const Quaternion& q) {
            return m * Matrix4(q);
        }

        Vector3 Rotate(const Quaternion& q, const Vector3& v) { return q*v; }

        Matrix4 Scale(const Matrix4& m, const Vector3& v) {
            Matrix4 Result(m);
            Result[0][0] *= v.x;
            Result[1][1] *= v.y;
            Result[2][2] *= v.z;
            return Result;
        }
        
        bool DecomposeTransformMatrix(const Matrix4& m, Vector3& translation, Quaternion& rotation, Vector3& scale, Vector3& skew, Vector4& perspective) {
            Matrix4 LocalMatrix(m);

            // Normalize the matrix.
            if(Math::EpsilonEqual(LocalMatrix[3][3], static_cast<float>(0)))
                return false;

            for(int i = 0; i < 4; ++i)
                for(int j = 0; j < 4; ++j)
                    LocalMatrix[i][j] /= LocalMatrix[3][3];

            // perspectiveMatrix is used to solve for perspective, but it also provides
            // an easy way to test for singularity of the upper 3x3 component.
            Matrix4 PerspectiveMatrix(LocalMatrix);

            for(int i = 0; i < 3; i++)
                PerspectiveMatrix[i][3] = static_cast<float>(0);
            PerspectiveMatrix[3][3] = static_cast<float>(1);

            /// TODO: Fixme!
            if(Math::EpsilonEqual(Math::Determinant(PerspectiveMatrix), static_cast<float>(0)))
                return false;

            // First, isolate perspective.  This is the messiest.
            if(
                Math::EpsilonEqual(LocalMatrix[0][3], static_cast<float>(0)) ||
                Math::EpsilonEqual(LocalMatrix[1][3], static_cast<float>(0)) ||
                Math::EpsilonEqual(LocalMatrix[2][3], static_cast<float>(0)))
            {
                // rightHandSide is the right hand side of the equation.
                Vector4 RightHandSide;
                RightHandSide[0] = LocalMatrix[0][3];
                RightHandSide[1] = LocalMatrix[1][3];
                RightHandSide[2] = LocalMatrix[2][3];
                RightHandSide[3] = LocalMatrix[3][3];

                // Solve the equation by inverting PerspectiveMatrix and multiplying
                // rightHandSide by the inverse.  (This is the easiest way, not
                // necessarily the best.)
                Matrix4 InversePerspectiveMatrix = Math::Inverse(PerspectiveMatrix);//   inverse(PerspectiveMatrix, inversePerspectiveMatrix);
                Matrix4 TransposedInversePerspectiveMatrix = Math::Transpose(InversePerspectiveMatrix);//   transposeMatrix4(inversePerspectiveMatrix, transposedInversePerspectiveMatrix);

                perspective = TransposedInversePerspectiveMatrix * RightHandSide;
                //  v4MulPointByMatrix(rightHandSide, transposedInversePerspectiveMatrix, perspectivePoint);

                // Clear the perspective partition
                LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
                LocalMatrix[3][3] = static_cast<float>(1);
            }
            else
            {
                // No perspective.
                perspective = Vector4(0, 0, 0, 1);
            }

            // Next take care of translation (easy).
            translation = Vector3(LocalMatrix[3]);
            LocalMatrix[3] = Vector4(0, 0, 0, LocalMatrix[3].w);

            Vector3 Row[3], Pdum3;

            // Now get scale and shear.
            for(int i = 0; i < 3; ++i)
                for(int j = 0; j < 3; ++j)
                    Row[i][j] = LocalMatrix[i][j];

            // Compute X scale factor and normalize first row.
            scale.x = Math::Length(Row[0]);// v3Length(Row[0]);

            Row[0] = Math::Scale(Row[0], static_cast<float>(1));

            // Compute XY shear factor and make 2nd row orthogonal to 1st.
            skew.z = Math::Dot(Row[0], Row[1]);
            Row[1] = Math::Combine(Row[1], Row[0], static_cast<float>(1), -skew.z);

            // Now, compute Y scale and normalize 2nd row.
            scale.y = Math::Length(Row[1]);
            Row[1] = Math::Scale(Row[1], static_cast<float>(1));
            skew.z /= scale.y;

            // Compute XZ and YZ shears, orthogonalize 3rd row.
            skew.y = Math::Dot(Row[0], Row[2]);
            Row[2] = Math::Combine(Row[2], Row[0], static_cast<float>(1), -skew.y);
            skew.x = Math::Dot(Row[1], Row[2]);
            Row[2] = Math::Combine(Row[2], Row[1], static_cast<float>(1), -skew.x);

            // Next, get Z scale and normalize 3rd row.
            scale.z = Math::Length(Row[2]);
            Row[2] = Math::Scale(Row[2], static_cast<float>(1));
            skew.y /= scale.z;
            skew.x /= scale.z;
            // At this point, the matrix (in rows[]) is orthonormal.
            // Check for a coordinate system flip.  If the determinant
            // is -1, then negate the matrix and the scaling factors.
            Pdum3 = Math::Cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
            if(Math::Dot(Row[0], Pdum3) < 0)
            {
                for(int i = 0; i < 3; i++)
                {
                    scale[i] *= static_cast<float>(-1);
                    Row[i] *= static_cast<float>(-1);
                }
            }

            // Now, get the rotations out, as described in the gem.

            // FIXME - Add the ability to return either quaternions (which are
            // easier to recompose with) or Euler angles (rx, ry, rz), which
            // are easier for authors to deal with. The latter will only be useful
            // when we fix https://bugs.webkit.org/show_bug.cgi?id=23799, so I
            // will leave the Euler angle code here for now.

            // ret.rotateY = asin(-Row[0][2]);
            // if (cos(ret.rotateY) != 0) {
            //     ret.rotateX = atan2(Row[1][2], Row[2][2]);
            //     ret.rotateZ = atan2(Row[0][1], Row[0][0]);
            // } else {
            //     ret.rotateX = atan2(-Row[2][0], Row[1][1]);
            //     ret.rotateZ = 0;
            // }

            int i, j, k = 0;
            float root, trace = Row[0].x + Row[1].y + Row[2].z;
            if(trace > static_cast<float>(0))
            {
                root = sqrt(trace + static_cast<float>(1.0));
                rotation.w = static_cast<float>(0.5) * root;
                root = static_cast<float>(0.5) / root;
                rotation.x = root * (Row[1].z - Row[2].y);
                rotation.y = root * (Row[2].x - Row[0].z);
                rotation.z = root * (Row[0].y - Row[1].x);
            } // End if > 0
            else
            {
                // TODO: check this
                static int Next[3] = {1, 2, 0};
                i = 0;
                if(Row[1].y > Row[0].x) i = 1;
                if(Row[2].z > Row[i][i]) i = 2;
                j = Next[i];
                k = Next[j];

                root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<float>(1.0));

                rotation[i] = static_cast<float>(0.5) * root;
                root = static_cast<float>(0.5) / root;
                rotation[j] = root * (Row[i][j] + Row[j][i]);
                rotation[k] = root * (Row[i][k] + Row[k][i]);
                rotation.w = root * (Row[j][k] - Row[k][j]);
            } // End if <= 0

            return true;
        }
            
        
        float Dot(const Quaternion& a, const Quaternion& b) {
            return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
        }

        Quaternion Mix(const Quaternion& a, const Quaternion& b, float t) {
            float const cosTheta = Dot(a, b);
            
            // Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
            if(cosTheta > static_cast<float>(1) - EPSILONF)
            {
                // Linear interpolation
                return Quaternion(
                    Mix(a.w, b.w, t),
                    Mix(a.x, b.x, t),
                    Mix(a.y, b.y, t),
                    Mix(a.z, b.z, t));
            }
            else
            {
                // Essential Mathematics, page 467
                float angle = acos(cosTheta);
                return (sin((static_cast<float>(1) - t) * angle) * a + sin(t * angle) * b) / sin(angle);
            }
        }
        
        float Length(const Quaternion& q) {
            return sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
        }
        Quaternion Normalize(const Quaternion& q) {
            float len = Length(q);
            return Quaternion(q.w / len, q.x / len, q.y / len, q.z / len);
        }
        Quaternion Conjugate(const Quaternion& q) {
            return Quaternion(q.w, -q.x, -q.y, -q.z);
        }
        Quaternion Inverse(const Quaternion& q) {
            return Conjugate(q) / (q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
        }


    }

}  // namespace Ethereal
