module;

#include <cmath>

export module Lateralus.Core.Matrix;

import Lateralus.Core.Vector;
import Lateralus.Core.Math;

namespace Lateralus::Core
{
export struct Matrix4x4
{
    Vector4 r0, r1, r2, r3;

    Matrix4x4()
        : r0(1.0f, 0.0f, 0.0f, 0.0f), r1(0.0f, 1.0f, 0.0f, 0.0f), r2(0.0f, 0.0f, 1.0f, 0.0f),
          r3(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    Matrix4x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12,
              float m13, float m20, float m21, float m22, float m23, float m30, float m31,
              float m32, float m33)
        : r0(m00, m01, m02, m03), r1(m10, m11, m12, m13), r2(m20, m21, m22, m23),
          r3(m30, m31, m32, m33)
    {
    }

    Matrix4x4 operator*(const Matrix4x4 &other) const
    {
        Matrix4x4 result;

        result.r0.x = r0.x * other.r0.x + r0.y * other.r1.x + r0.z * other.r2.x + r0.w * other.r3.x;
        result.r0.y = r0.x * other.r0.y + r0.y * other.r1.y + r0.z * other.r2.y + r0.w * other.r3.y;
        result.r0.z = r0.x * other.r0.z + r0.y * other.r1.z + r0.z * other.r2.z + r0.w * other.r3.z;
        result.r0.w = r0.x * other.r0.w + r0.y * other.r1.w + r0.z * other.r2.w + r0.w * other.r3.w;

        result.r1.x = r1.x * other.r0.x + r1.y * other.r1.x + r1.z * other.r2.x + r1.w * other.r3.x;
        result.r1.y = r1.x * other.r0.y + r1.y * other.r1.y + r1.z * other.r2.y + r1.w * other.r3.y;
        result.r1.z = r1.x * other.r0.z + r1.y * other.r1.z + r1.z * other.r2.z + r1.w * other.r3.z;
        result.r1.w = r1.x * other.r0.w + r1.y * other.r1.w + r1.z * other.r2.w + r1.w * other.r3.w;

        result.r2.x = r2.x * other.r0.x + r2.y * other.r1.x + r2.z * other.r2.x + r2.w * other.r3.x;
        result.r2.y = r2.x * other.r0.y + r2.y * other.r1.y + r2.z * other.r2.y + r2.w * other.r3.y;
        result.r2.z = r2.x * other.r0.z + r2.y * other.r1.z + r2.z * other.r2.z + r2.w * other.r3.z;
        result.r2.w = r2.x * other.r0.w + r2.y * other.r1.w + r2.z * other.r2.w + r2.w * other.r3.w;

        result.r3.x = r3.x * other.r0.x + r3.y * other.r1.x + r3.z * other.r2.x + r3.w * other.r3.x;
        result.r3.y = r3.x * other.r0.y + r3.y * other.r1.y + r3.z * other.r2.y + r3.w * other.r3.y;
        result.r3.z = r3.x * other.r0.z + r3.y * other.r1.z + r3.z * other.r2.z + r3.w * other.r3.z;
        result.r3.w = r3.x * other.r0.w + r3.y * other.r1.w + r3.z * other.r2.w + r3.w * other.r3.w;

        return result;
    }

    Vector4 operator*(const Vector4 &vector) const
    {
        Vector4 result;

        result.x = r0.Dot(vector);
        result.y = r1.Dot(vector);
        result.z = r2.Dot(vector);
        result.w = r3.Dot(vector);

        return result;
    }

    bool RoughlyEquals(const Matrix4x4 &other) const
    {
        return r0.RoughlyEquals(other.r0) && r1.RoughlyEquals(other.r1) &&
               r2.RoughlyEquals(other.r2) && r3.RoughlyEquals(other.r3);
    }

    bool ExactlyEquals(const Matrix4x4 &other) const
    {
        return r0.ExactlyEquals(other.r0) && r1.ExactlyEquals(other.r1) &&
               r2.ExactlyEquals(other.r2) && r3.ExactlyEquals(other.r3);
    }

    Matrix4x4 Transpose() const
    {
        Matrix4x4 result;

        result.r0.x = r0.x;
        result.r0.y = r1.x;
        result.r0.z = r2.x;
        result.r0.w = r3.x;

        result.r1.x = r0.y;
        result.r1.y = r1.y;
        result.r1.z = r2.y;
        result.r1.w = r3.y;

        result.r2.x = r0.z;
        result.r2.y = r1.z;
        result.r2.z = r2.z;
        result.r2.w = r3.z;

        result.r3.x = r0.w;
        result.r3.y = r1.w;
        result.r3.z = r2.w;
        result.r3.w = r3.w;

        return result;
    }

    Matrix4x4 Inverse() const
    {
        Matrix4x4 result;

        float det = Determinant();

        if (det == 0.0f)
        {
            // error?
            return *this;
        }

        float invDet = 1.0f / det;

    // Calculate the elements of the inverse matrix
        result.r0.x = (r1.y * r2.z * r3.w + r2.y * r3.z * r1.w + r3.y * r1.z * r2.w -
                       r1.y * r3.z * r2.w - r2.y * r1.z * r3.w - r3.y * r2.z * r1.w) *
                      invDet;
        result.r0.y = -(r0.y * r2.z * r3.w + r2.y * r3.z * r0.w + r3.y * r0.z * r2.w -
                        r0.y * r3.z * r2.w - r2.y * r0.z * r3.w - r3.y * r2.z * r0.w) *
                      invDet;
        result.r0.z = (r0.y * r1.z * r3.w + r1.y * r3.z * r0.w + r3.y * r0.z * r1.w -
                       r0.y * r3.z * r1.w - r1.y * r0.z * r3.w - r3.y * r1.z * r0.w) *
                      invDet;
        result.r0.w = -(r0.y * r1.z * r2.w + r1.y * r2.z * r0.w + r2.y * r0.z * r1.w -
                        r0.y * r2.z * r1.w - r1.y * r0.z * r2.w - r2.y * r1.z * r0.w) *
                      invDet;

        result.r1.x = -(r1.x * r2.z * r3.w + r2.x * r3.z * r1.w + r3.x * r1.z * r2.w -
                        r1.x * r3.z * r2.w - r2.x * r1.z * r3.w - r3.x * r2.z * r1.w) *
                      invDet;
        result.r1.y = (r0.x * r2.z * r3.w + r2.x * r3.z * r0.w + r3.x * r0.z * r2.w -
                       r0.x * r3.z * r2.w - r2.x * r0.z * r3.w - r3.x * r2.z * r0.w) *
                      invDet;
        result.r1.z = -(r0.x * r1.z * r3.w + r1.x * r3.z * r0.w + r3.x * r0.z * r1.w -
                        r0.x * r3.z * r1.w - r1.x * r0.z * r3.w - r3.x * r1.z * r0.w) *
                      invDet;
        result.r1.w = (r0.x * r1.z * r2.w + r1.x * r2.z * r0.w + r2.x * r0.z * r1.w -
                       r0.x * r2.z * r1.w - r1.x * r0.z * r2.w - r2.x * r1.z * r0.w) *
                      invDet;

        result.r2.x = (r1.x * r2.y * r3.w + r2.x * r3.y * r1.w + r3.x * r1.y * r2.w -
                       r1.x * r3.y * r2.w - r2.x * r1.y * r3.w - r3.x * r2.y * r1.w) *
                      invDet;
        result.r2.y = -(r0.x * r2.y * r3.w + r2.x * r3.y * r0.w + r3.x * r0.y * r2.w -
                        r0.x * r3.y * r2.w - r2.x * r0.y * r3.w - r3.x * r2.y * r0.w) *
                      invDet;
        result.r2.z = (r0.x * r1.y * r3.w + r1.x * r3.y * r0.w + r3.x * r0.y * r1.w -
                       r0.x * r3.y * r1.w - r1.x * r0.y * r3.w - r3.x * r1.y * r0.w) *
                      invDet;
        result.r2.w = -(r0.x * r1.y * r2.w + r1.x * r2.y * r0.w + r2.x * r0.y * r1.w -
                        r0.x * r2.y * r1.w - r1.x * r0.y * r2.w - r2.x * r1.y * r0.w) *
                      invDet;

        result.r3.x = -(r1.x * r2.y * r3.z + r2.x * r3.y * r1.z + r3.x * r1.y * r2.z -
                        r1.x * r3.y * r2.z - r2.x * r1.y * r3.z - r3.x * r2.y * r1.z) *
                      invDet;
        result.r3.y = (r0.x * r2.y * r3.z + r2.x * r3.y * r0.z + r3.x * r0.y * r2.z -
                       r0.x * r3.y * r2.z - r2.x * r0.y * r3.z - r3.x * r2.y * r0.z) *
                      invDet;
        result.r3.z = -(r0.x * r1.y * r3.z + r1.x * r3.y * r0.z + r3.x * r0.y * r1.z -
                        r0.x * r3.y * r1.z - r1.x * r0.y * r3.z - r3.x * r1.y * r0.z) *
                      invDet;
        result.r3.w = (r0.x * r1.y * r2.z + r1.x * r2.y * r0.z + r2.x * r0.y * r1.z -
                       r0.x * r2.y * r1.z - r1.x * r0.y * r2.z - r2.x * r1.y * r0.z) *
                      invDet;

        return result;
    }


    static Matrix4x4 Identity()
    {
        return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }


    static Matrix4x4 CreateTranslation(const Vector3 &translation)
    {
        return Matrix4x4(1.0f, 0.0f, 0.0f, translation.x, 0.0f, 1.0f, 0.0f, translation.y, 0.0f,
                         0.0f, 1.0f, translation.z, 0.0f, 0.0f, 0.0f, 1.0f);
    }


    static Matrix4x4 CreateRotation(const Vector3 &axis, float angle)
    {
        float radAngle = angle * Deg2Rad;
        float cosAngle = std::cos(radAngle);
        float sinAngle = std::sin(radAngle);
        float oneMinusCos = 1.0f - cosAngle;

        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        Matrix4x4 result(cosAngle + x * x * oneMinusCos, x * y * oneMinusCos - z * sinAngle,
                         x * z * oneMinusCos + y * sinAngle, 0.0f,
                         y * x * oneMinusCos + z * sinAngle, cosAngle + y * y * oneMinusCos,
                         y * z * oneMinusCos - x * sinAngle, 0.0f,
                         z * x * oneMinusCos - y * sinAngle, z * y * oneMinusCos + x * sinAngle,
                         cosAngle + z * z * oneMinusCos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        return result;
    }

    static Matrix4x4 CreateRotation(const Vector3 &rotation)
    {
        float radX = rotation.x * Deg2Rad;
        float radY = rotation.y * Deg2Rad;
        float radZ = rotation.z * Deg2Rad;

        float cosX = std::cos(radX);
        float sinX = std::sin(radX);
        float cosY = std::cos(radY);
        float sinY = std::sin(radY);
        float cosZ = std::cos(radZ);
        float sinZ = std::sin(radZ);

        Matrix4x4 result(cosY * cosZ, -cosX * sinZ + sinX * sinY * cosZ,
                         sinX * sinZ + cosX * sinY * cosZ, 0.0f, cosY * sinZ,
                         cosX * cosZ + sinX * sinY * sinZ, -sinX * cosZ + cosX * sinY * sinZ, 0.0f,
                         -sinY, sinX * cosY, cosX * cosY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        return result;
    }


    static Matrix4x4 CreateScale(const Vector3 &scale)
    {
        return Matrix4x4(scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z,
                         0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }



    static Matrix4x4 CreateOrthographicProjection(float left, float right, float bottom,
                                                      float top, float near, float far)
    {
        float width = right - left;
        float height = top - bottom;
        float depth = far - near;

        Matrix4x4 result(
            2.0f / width, 0.0f, 0.0f, -((right + left) / width),
            0.0f, 2.0f / height, 0.0f, -((top + bottom) / height),
            0.0f, 0.0f, 2.0f / depth, -((far + near) / depth),
            0.0f, 0.0f, 0.0f, 1.0f);

        return result;
    }


    static Matrix4x4 CreatePerspectiveProjection(float fov, float aspectRatio, float near,
                                                     float far)
    {
        float tanHalfFov = std::tan(0.5f * fov * Deg2Rad);
        float zRange = far - near;

        Matrix4x4 result(1.0f / (aspectRatio * tanHalfFov), 0.0f, 0.0f, 0.0f, 0.0f,
                         1.0f / tanHalfFov, 0.0f, 0.0f, 0.0f, 0.0f, -(far + near) / zRange,
                         -2.0f * far * near / zRange, 0.0f, 0.0f, -1.0f, 0.0f);

        return result;
    }


static Matrix4x4 CreateLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
{
    Vector3 forward = (target - eye).Normalized();
    Vector3 right = forward.Cross(up).Normalized(); // Calculate right vector using cross product
    Vector3 upVector = right.Cross(forward); // Calculate up vector using cross product

    Matrix4x4 result(
        right.x, right.y, right.z, -right.Dot(eye),
        upVector.x, upVector.y, upVector.z, -upVector.Dot(eye),
        forward.x, forward.y, forward.z, -forward.Dot(eye), // Reverse the direction of forward vector
        0.0f, 0.0f, 0.0f, 1.0f);

    return result;
}



    float Determinant() const
    {
        // Calculate the determinant using the expansion by minors formula
        float det =
            r0.x * (r1.y * (r2.z * r3.w - r3.z * r2.w) - r2.y * (r1.z * r3.w - r3.z * r1.w) +
                    r3.y * (r1.z * r2.w - r2.z * r1.w)) -
            r0.y * (r1.x * (r2.z * r3.w - r3.z * r2.w) - r2.x * (r1.z * r3.w - r3.z * r1.w) +
                    r3.x * (r1.z * r2.w - r2.z * r1.w)) +
            r0.z * (r1.x * (r2.y * r3.w - r3.y * r2.w) - r2.x * (r1.y * r3.w - r3.y * r1.w) +
                    r3.x * (r1.y * r2.w - r2.y * r1.w)) -
            r0.w * (r1.x * (r2.y * r3.z - r3.y * r2.z) - r2.x * (r1.y * r3.z - r3.y * r1.z) +
                    r3.x * (r1.y * r2.z - r2.y * r1.z));

        return det;
    }
};
} // namespace Lateralus::Core