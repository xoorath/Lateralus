export module Lateralus.Core.Quaternion;

import Lateralus.Core.Matrix;
import Lateralus.Core.Vector;

namespace Lateralus::Core
{
export struct Quaternion
{
    ~Quaternion() = default;
    Quaternion() = default;
    Quaternion(Quaternion const &) = default;
    Quaternion &operator=(Quaternion const &) = default;
    static Quaternion FromAxisAngle(Vector3 const &axis, float angle) { return Quaternion(); }
    static Quaternion FromEuler(Vector3 const &angles) { return Quaternion(); }

    Quaternion operator+(Quaternion const &other) const { return Quaternion(); }

    Quaternion operator*(float scalar) const { return Quaternion(); }
    Quaternion operator-() const { return Quaternion(); }

    float Length() const { return 0.0f; }
    float LengthSquared() const { return 0.0f; }
    Quaternion Normalized() const { return Quaternion(); }
    Quaternion &Normalize() { return *this; }

    Matrix4x4 ToMatrix() const { return Matrix4x4(); }

    static Quaternion Invert(Quaternion const &quat) { return Quaternion(); }
    static float DotProduct(Quaternion const &left, Quaternion const &right) { return 0.0f; }
    static Quaternion Lerp(Quaternion const &start, Quaternion const &end, float t)
    {
        return Quaternion();
    }
    static Quaternion Slerp(Quaternion const &start, Quaternion const &end, float t)
    {
        return Quaternion();
    }

    bool RoughlyEqual(Quaternion const &other) { return false; }
    bool ExactlyEqual(Quaternion const &other) { return false; }

    float i = 0.0f;
    float j = 0.0f;
    float k = 0.0f;
    float r = 1.0f;
};
} // namespace Lateralus::Core