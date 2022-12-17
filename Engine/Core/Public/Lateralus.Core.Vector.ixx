module;

#include <math.h>

export module Lateralus.Core.Vector;

import Lateralus.Core.Math;

namespace Lateralus::Core
{
    export struct Vector2
    {
        ~Vector2() = default;
        constexpr Vector2() = default;
        constexpr Vector2(float x, float y) : x(x), y(y) {}
        constexpr Vector2(Vector2 const& o) = default;
        Vector2& operator=(Vector2 const& o) = default;

        Vector2 operator + (Vector2 const& o) const { return Vector2(x + o.x, y + o.y); }
        Vector2 operator - (Vector2 const& o) const { return Vector2(x - o.x, y - o.y); }
        Vector2 operator * (Vector2 const& o) const { return Vector2(x * o.x, y * o.y); }
        Vector2 operator / (Vector2 const& o) const { return Vector2(x / o.x, y / o.y); }

        Vector2& operator += (Vector2 const& o) { x += o.x; y += o.y; return *this; }
        Vector2& operator -= (Vector2 const& o) { x -= o.x; y -= o.y; return *this; }
        Vector2& operator *= (Vector2 const& o) { x *= o.x; y *= o.y; return *this; }
        Vector2& operator /= (Vector2 const& o) { x /= o.x; y /= o.y; return *this; }

        bool ExactlyEquals(Vector2 const& o) const { return (x==o.x)&&(y==o.y); }
        bool RoughlyEquals(Vector2 const& o) const { return CloseEnough(x, o.x) && CloseEnough(y, o.y); }

        float LengthSq() const { return (x * x) + (y * y); }
        float Length() const { return sqrt((x * x) + (y * y)); }
        void Normalize() {
            float l = Length();
            if(l != 0.0f)
            {
                x /= l;
                y /= l;
            }
        }
        Vector2 Normalized() const {
            float l = Length();
            if(l != 0.0f)
            {
                return Vector2(x/l, y/l);
            }
            return Vector2();
        }

        float x = 0.0f;
        float y = 0.0f;
    };

    export struct Vector3
    {
        ~Vector3() = default;
        constexpr Vector3() = default;
        constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        constexpr Vector3(Vector3 const& o) = default;
        Vector3& operator=(Vector3 const& o) = default;

        Vector3 operator + (Vector3 const& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
        Vector3 operator - (Vector3 const& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
        Vector3 operator * (Vector3 const& o) const { return Vector3(x * o.x, y * o.y, z * o.z); }
        Vector3 operator / (Vector3 const& o) const { return Vector3(x / o.x, y / o.y, z / o.z); }

        Vector3& operator += (Vector3 const& o) { x += o.x; y += o.y; z += o.z; return *this; }
        Vector3& operator -= (Vector3 const& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
        Vector3& operator *= (Vector3 const& o) { x *= o.x; y *= o.y; z *= o.z; return *this; }
        Vector3& operator /= (Vector3 const& o) { x /= o.x; y /= o.y; z /= o.z; return *this; }

        bool ExactlyEquals(Vector3 const& o) const { return (x==o.x)&&(y==o.y)&&(z==o.z); }
        bool RoughlyEquals(Vector3 const& o) const
        {
            return CloseEnough(x, o.x) && CloseEnough(y, o.y) && CloseEnough(z, o.z);
        }

        float LengthSq() const { return (x * x) + (y * y) + (z * z); }
        float Length() const { return sqrt((x * x) + (y * y) + (z * z)); }
        void Normalize() {
            float l = Length();
            if(l != 0.0f)
            {
                x /= l;
                y /= l;
                z /= l;
            }
        }
        Vector3 Normalized() const {
            float l = Length();
            if(l != 0.0f)
            {
                return Vector3(x/l, y/l, z/l);
            }
            return Vector3();
        }
        float Sum() const {
            return x + y + z;
        }
        float Dot(Vector3 const& o) const {
            return ((*this) * o).Sum();
        }
        Vector3 Cross(Vector3 const &o) const {
            return Vector3(y * o.z - z * y, z * o.x - x * z, x * o.y - y * x);
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    export struct Vector4
    {
        ~Vector4() = default;
        constexpr Vector4() = default;
        constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        constexpr Vector4(Vector4 const& o) = default;
        Vector4& operator=(Vector4 const& o) = default;

        Vector4 operator + (Vector4 const& o) const { return Vector4(x + o.x, y + o.y, z + o.z, w + o.w); }
        Vector4 operator - (Vector4 const& o) const { return Vector4(x - o.x, y - o.y, z - o.z, w - o.w); }
        Vector4 operator * (Vector4 const& o) const { return Vector4(x * o.x, y * o.y, z * o.z, w * o.w); }
        Vector4 operator / (Vector4 const& o) const { return Vector4(x / o.x, y / o.y, z / o.z, w / o.w); }

        Vector4& operator += (Vector4 const& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
        Vector4& operator -= (Vector4 const& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
        Vector4& operator *= (Vector4 const& o) { x *= o.x; y *= o.y; z *= o.z; w *= o.w; return *this; }
        Vector4& operator /= (Vector4 const& o) { x /= o.x; y /= o.y; z /= o.z; w /= o.w; return *this; }

        bool ExactlyEquals(Vector4 const &o) const
        {
            return (x == o.x) && (y == o.y) && (z == o.z) && (w == o.w);
        }
        bool RoughlyEquals(Vector4 const &o) const
        {
            return CloseEnough(x, o.x) && CloseEnough(y, o.y) && CloseEnough(z, o.z) &&
                   CloseEnough(w, o.w);
        }

        float LengthSq() const { return (x * x) + (y * y) + (z * z) + (w * w); }
        float Length() const { return sqrt((x * x) + (y * y) + (z * z) + (w * w)); }
        void Normalize() {
            float l = Length();
            if(l != 0.0f)
            {
                x /= l;
                y /= l;
                z /= l;
                w /= l;
            }
        }
        Vector4 Normalized() const {
            float l = Length();
            if(l != 0.0f)
            {
                return Vector4(x/l, y/l, z/l, w/l);
            }
            return Vector4();
        }
        float Sum() const { return x + y + z + w; }
        float Dot(Vector4 const &o) const { return ((*this) * o).Sum(); }


        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };


}