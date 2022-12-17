#include PCH_FILE

import Lateralus.Core;
import Lateralus.Core.Math;

using namespace Lateralus;

namespace Lateralus::Core::Tests
{
TEST(Core_Math, FloatAlmostEqual)
{
    EXPECT_TRUE(CloseEnough(0.00000001f, 0.00000009f));
    EXPECT_FALSE(CloseEnough(0.00000001f, 0.0000009f));
}

TEST(Core_Math, MathFuncSantiyCheck)
{
    EXPECT_EQ(1.0f, Abs(-1.0f));

    EXPECT_TRUE(CloseEnough(0.0f, Cos(HalfPi)));
    EXPECT_TRUE(CloseEnough(1.0f, Sin(HalfPi)));
    EXPECT_TRUE(CloseEnough(0.0f, ACos(1.0f)));
    EXPECT_TRUE(CloseEnough(HalfPi, ASin(1.0f)));

    EXPECT_EQ(-1.0f, Clamp(-10.0f, -1.0f, 1.0f));
    EXPECT_EQ(1.0f, Clamp(10.0f, -1.0f, 1.0f));
    EXPECT_EQ(5.0f, Lerp(0.0f, 10.0f, 0.5f));
    EXPECT_EQ(10.0f, Lerp(0.0f, 5.0f, 2.0f));

    EXPECT_EQ(1.0f, Min(1.0f, 2.0f));
    EXPECT_EQ(1.0f, Min(1.0f, 2.0f, 3.0f));
}
} // namespace Lateralus::Core::Tests
