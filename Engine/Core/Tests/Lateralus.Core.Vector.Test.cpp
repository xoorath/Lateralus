#include <gtest/gtest.h>

import Lateralus.Core;
import Lateralus.Core.Math;
import Lateralus.Core.Vector;

using namespace Lateralus;

namespace Lateralus::Core::Tests
{
TEST(Core_Vector, Vector2)
{
    {
        Vector2 defaultConstruct;
        EXPECT_EQ(defaultConstruct.x, 0.0f);
        EXPECT_EQ(defaultConstruct.y, 0.0f);
    }
    EXPECT_TRUE(CloseEnough(0.0f, Vector2().LengthSq()));
    EXPECT_TRUE(CloseEnough(0.0f, Vector2().Length()));
    EXPECT_TRUE(CloseEnough(4.0f, Vector2(2.0f, 0.0f).LengthSq()));
    EXPECT_TRUE(CloseEnough(2.0f, Vector2(2.0f, 0.0f).Length()));
    {
        Vector2 v(2.0f, 0.0f);
        v.Normalize();
        EXPECT_TRUE(CloseEnough(1.0f, v.Length()));
    }
    {
        Vector2 v(2.0f, 0.0f);
        EXPECT_TRUE(CloseEnough(1.0f, v.Normalized().Length()));
        EXPECT_TRUE(CloseEnough(2.0f, v.Length()));
    }

    {
        Vector2 a(1.0f, 1.0f), b(2.0f, 2.0f);
        Vector2 c = a + b;
        EXPECT_TRUE(CloseEnough(3.0f, c.x));
        EXPECT_TRUE(CloseEnough(3.0f, c.y));
    }
    {
        Vector2 a(1.0f, 1.0f), b(2.0f, 2.0f);
        Vector2 c = a - b;
        EXPECT_TRUE(CloseEnough(-1.0f, c.x));
        EXPECT_TRUE(CloseEnough(-1.0f, c.y));
    }
    {
        Vector2 a(2.0f, 2.0f), b(2.0f, 2.0f);
        Vector2 c = a * b;
        EXPECT_TRUE(CloseEnough(4.0f, c.x));
        EXPECT_TRUE(CloseEnough(4.0f, c.y));
    }
    {
        Vector2 a(1.0f, 1.0f), b(2.0f, 2.0f);
        Vector2 c = a / b;
        EXPECT_TRUE(CloseEnough(0.5f, c.x));
        EXPECT_TRUE(CloseEnough(0.5f, c.y));
    }

    {
        Vector2 a(1.0f, 1.0f), b(2.0f, 2.0f);
        a += b;
        EXPECT_TRUE(CloseEnough(3.0f, a.x));
        EXPECT_TRUE(CloseEnough(3.0f, a.y));
    }
    {
        Vector2 a(1.0f, 1.0f), b(2.0f, 2.0f);
        a -= b;
        EXPECT_TRUE(CloseEnough(-1.0f, a.x));
        EXPECT_TRUE(CloseEnough(-1.0f, a.y));
    }
    {
        Vector2 a(2.0f, 2.0f), b(2.0f, 2.0f);
        a *= b;
        EXPECT_TRUE(CloseEnough(4.0f, a.x));
        EXPECT_TRUE(CloseEnough(4.0f, a.y));
    }
    {
        Vector2 a(1.0f, 1.0f), b(2.0f, 2.0f);
        a /= b;
        EXPECT_TRUE(CloseEnough(0.5f, a.x));
        EXPECT_TRUE(CloseEnough(0.5f, a.y));
    }

    {
        Vector2 a(1.0f, 1.0f), b(1.0f, 1.0f);
        EXPECT_TRUE(a.ExactlyEquals(b));
        EXPECT_TRUE(a.RoughlyEquals(b));
    }
    {
        Vector2 a(0.00000001f, 0.00000001f), b(0.00000009f, 0.00000009f);
        EXPECT_FALSE(a.ExactlyEquals(b));
        EXPECT_TRUE(a.RoughlyEquals(b));
    }
}

TEST(Core_Vector, Vector3)
{
    {
        Vector3 defaultConstruct;
        EXPECT_EQ(defaultConstruct.x, 0.0f);
        EXPECT_EQ(defaultConstruct.y, 0.0f);
        EXPECT_EQ(defaultConstruct.z, 0.0f);
    }
    EXPECT_TRUE(CloseEnough(0.0f, Vector3().LengthSq()));
    EXPECT_TRUE(CloseEnough(0.0f, Vector3().Length()));
    EXPECT_TRUE(CloseEnough(4.0f, Vector3(2.0f, 0.0f, 0.0f).LengthSq()));
    EXPECT_TRUE(CloseEnough(2.0f, Vector3(2.0f, 0.0f, 0.0f).Length()));
    {
        Vector3 v(2.0f, 0.0f, 0.0f);
        v.Normalize();
        EXPECT_TRUE(CloseEnough(1.0f, v.Length()));
    }
    {
        Vector3 v(2.0f, 0.0f, 0.0f);
        EXPECT_TRUE(CloseEnough(1.0f, v.Normalized().Length()));
        EXPECT_TRUE(CloseEnough(2.0f, v.Length()));
    }

    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f);
        Vector3 c = a + b;
        EXPECT_TRUE(CloseEnough(3.0f, c.x));
        EXPECT_TRUE(CloseEnough(3.0f, c.y));
        EXPECT_TRUE(CloseEnough(3.0f, c.z));
    }
    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f);
        Vector3 c = a - b;
        EXPECT_TRUE(CloseEnough(-1.0f, c.x));
        EXPECT_TRUE(CloseEnough(-1.0f, c.y));
        EXPECT_TRUE(CloseEnough(-1.0f, c.z));
    }
    {
        Vector3 a(2.0f, 2.0f, 2.0f), b(2.0f, 2.0f, 2.0f);
        Vector3 c = a * b;
        EXPECT_TRUE(CloseEnough(4.0f, c.x));
        EXPECT_TRUE(CloseEnough(4.0f, c.y));
        EXPECT_TRUE(CloseEnough(4.0f, c.z));
    }
    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f);
        Vector3 c = a / b;
        EXPECT_TRUE(CloseEnough(0.5f, c.x));
        EXPECT_TRUE(CloseEnough(0.5f, c.y));
        EXPECT_TRUE(CloseEnough(0.5f, c.z));
    }

    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f);
        a += b;
        EXPECT_TRUE(CloseEnough(3.0f, a.x));
        EXPECT_TRUE(CloseEnough(3.0f, a.y));
        EXPECT_TRUE(CloseEnough(3.0f, a.z));
    }
    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f);
        a -= b;
        EXPECT_TRUE(CloseEnough(-1.0f, a.x));
        EXPECT_TRUE(CloseEnough(-1.0f, a.y));
        EXPECT_TRUE(CloseEnough(-1.0f, a.z));
    }
    {
        Vector3 a(2.0f, 2.0f, 2.0f), b(2.0f, 2.0f, 2.0f);
        a *= b;
        EXPECT_TRUE(CloseEnough(4.0f, a.x));
        EXPECT_TRUE(CloseEnough(4.0f, a.y));
        EXPECT_TRUE(CloseEnough(4.0f, a.z));
    }
    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f);
        a /= b;
        EXPECT_TRUE(CloseEnough(0.5f, a.x));
        EXPECT_TRUE(CloseEnough(0.5f, a.y));
        EXPECT_TRUE(CloseEnough(0.5f, a.z));
    }

    {
        Vector3 a(1.0f, 1.0f, 1.0f), b(1.0f, 1.0f, 1.0f);
        EXPECT_TRUE(a.ExactlyEquals(b));
        EXPECT_TRUE(a.RoughlyEquals(b));
    }
    {
        Vector3 a(0.00000001f, 0.00000001f, 0.00000001f), b(0.00000009f, 0.00000009f, 0.00000009f);
        EXPECT_FALSE(a.ExactlyEquals(b));
        EXPECT_TRUE(a.RoughlyEquals(b));
    }
    {
        Vector3 a(1.0f, 0.0f, 0.0f), b(0.0f, 1.0f, 0.0f);
        EXPECT_TRUE(CloseEnough(0.0f, a.Dot(b)));
    }

    {
        Vector3 a(1.0f, 0.0f, 0.0f), b(1.0f, 0.0f, 0.0f);
        EXPECT_TRUE(CloseEnough(1.0f, a.Dot(b)));
    }

    {
        Vector3 a(1.0f, 0.0f, 0.0f), b(-1.0f, 0.0f, 0.0f);
        EXPECT_TRUE(CloseEnough(-1.0f, a.Dot(b)));
    }

    {
        Vector3 a(1.0f, 0.0f, 0.0f), b(0.0f, 1.0f, 0.0f);
        EXPECT_TRUE(a.Cross(b).RoughlyEquals(Vector3(0.0f, 0.0f, 1.0f)));
    }
}

TEST(Core_Vector, Vector4)
{
    {
        Vector4 defaultConstruct;
        EXPECT_EQ(defaultConstruct.x, 0.0f);
        EXPECT_EQ(defaultConstruct.y, 0.0f);
        EXPECT_EQ(defaultConstruct.z, 0.0f);
        EXPECT_EQ(defaultConstruct.w, 0.0f);
    }
    EXPECT_TRUE(CloseEnough(0.0f, Vector4().LengthSq()));
    EXPECT_TRUE(CloseEnough(0.0f, Vector4().Length()));
    EXPECT_TRUE(CloseEnough(4.0f, Vector4(2.0f, 0.0f, 0.0f, 0.0f).LengthSq()));
    EXPECT_TRUE(CloseEnough(2.0f, Vector4(2.0f, 0.0f, 0.0f, 0.0f).Length()));
    {
        Vector4 v(2.0f, 0.0f, 0.0f, 0.0f);
        v.Normalize();
        EXPECT_TRUE(CloseEnough(1.0f, v.Length()));
    }
    {
        Vector4 v(2.0f, 0.0f, 0.0f, 0.0f);
        EXPECT_TRUE(CloseEnough(1.0f, v.Normalized().Length()));
        EXPECT_TRUE(CloseEnough(2.0f, v.Length()));
    }

    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        Vector4 c = a + b;
        EXPECT_TRUE(CloseEnough(3.0f, c.x));
        EXPECT_TRUE(CloseEnough(3.0f, c.y));
        EXPECT_TRUE(CloseEnough(3.0f, c.z));
        EXPECT_TRUE(CloseEnough(3.0f, c.w));
    }
    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        Vector4 c = a - b;
        EXPECT_TRUE(CloseEnough(-1.0f, c.x));
        EXPECT_TRUE(CloseEnough(-1.0f, c.y));
        EXPECT_TRUE(CloseEnough(-1.0f, c.z));
        EXPECT_TRUE(CloseEnough(-1.0f, c.w));
    }
    {
        Vector4 a(2.0f, 2.0f, 2.0f, 2.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        Vector4 c = a * b;
        EXPECT_TRUE(CloseEnough(4.0f, c.x));
        EXPECT_TRUE(CloseEnough(4.0f, c.y));
        EXPECT_TRUE(CloseEnough(4.0f, c.z));
        EXPECT_TRUE(CloseEnough(4.0f, c.w));
    }
    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        Vector4 c = a / b;
        EXPECT_TRUE(CloseEnough(0.5f, c.x));
        EXPECT_TRUE(CloseEnough(0.5f, c.y));
        EXPECT_TRUE(CloseEnough(0.5f, c.z));
        EXPECT_TRUE(CloseEnough(0.5f, c.w));
    }

    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        a += b;
        EXPECT_TRUE(CloseEnough(3.0f, a.x));
        EXPECT_TRUE(CloseEnough(3.0f, a.y));
        EXPECT_TRUE(CloseEnough(3.0f, a.z));
        EXPECT_TRUE(CloseEnough(3.0f, a.w));
    }
    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        a -= b;
        EXPECT_TRUE(CloseEnough(-1.0f, a.x));
        EXPECT_TRUE(CloseEnough(-1.0f, a.y));
        EXPECT_TRUE(CloseEnough(-1.0f, a.z));
        EXPECT_TRUE(CloseEnough(-1.0f, a.w));
    }
    {
        Vector4 a(2.0f, 2.0f, 2.0f, 2.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        a *= b;
        EXPECT_TRUE(CloseEnough(4.0f, a.x));
        EXPECT_TRUE(CloseEnough(4.0f, a.y));
        EXPECT_TRUE(CloseEnough(4.0f, a.z));
        EXPECT_TRUE(CloseEnough(4.0f, a.w));
    }
    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(2.0f, 2.0f, 2.0f, 2.0f);
        a /= b;
        EXPECT_TRUE(CloseEnough(0.5f, a.x));
        EXPECT_TRUE(CloseEnough(0.5f, a.y));
        EXPECT_TRUE(CloseEnough(0.5f, a.z));
        EXPECT_TRUE(CloseEnough(0.5f, a.w));
    }

    {
        Vector4 a(1.0f, 1.0f, 1.0f, 1.0f), b(1.0f, 1.0f, 1.0f, 1.0f);
        EXPECT_TRUE(a.ExactlyEquals(b));
        EXPECT_TRUE(a.RoughlyEquals(b));
    }
    {
        Vector4 a(0.00000001f, 0.00000001f, 0.00000001f, 0.00000001f), b(0.00000009f, 0.00000009f, 0.00000009f, 0.00000009f);
        EXPECT_FALSE(a.ExactlyEquals(b));
        EXPECT_TRUE(a.RoughlyEquals(b));
    }
}

} // namespace Lateralus::Core::Tests
