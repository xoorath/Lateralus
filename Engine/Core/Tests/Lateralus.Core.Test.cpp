#include PCH_FILE

import Lateralus.Core;

namespace Lateralus::Core::Tests
{
enum class FlagType8 : uint8
{
    A = 1 << 2,
    B = 1 << 4,
    C = 1 << 6
};
ENUM_FLAG_OPERATORS(FlagType8);

enum class FlagType16 : uint16
{
    A = 1 << 3,
    B = 1 << 6,
    C = 1 << 9
};
ENUM_FLAG_OPERATORS(FlagType16);

enum class FlagType32 : uint32
{
    A = 1 << 10,
    B = 1 << 20,
    C = 1 << 30
};
ENUM_FLAG_OPERATORS(FlagType32);

TEST(Core, FlagTypesCanBeCombined)
{
    EXPECT_EQ(FlagType8::A | FlagType8::B, static_cast<FlagType8>((1 << 2) | (1 << 4)));
    EXPECT_EQ(FlagType8::A | FlagType8::B | FlagType8::C,
              static_cast<FlagType8>((1 << 2) | (1 << 4) | (1 << 6)));

    EXPECT_EQ(FlagType16::A | FlagType16::B, static_cast<FlagType16>((1 << 3) | (1 << 6)));
    EXPECT_EQ(FlagType16::A | FlagType16::B | FlagType16::C,
              static_cast<FlagType16>((1 << 3) | (1 << 6) | (1 << 9)));

    EXPECT_EQ(FlagType32::A | FlagType32::B, static_cast<FlagType32>((1 << 10) | (1 << 20)));
    EXPECT_EQ(FlagType32::A | FlagType32::B | FlagType32::C,
              static_cast<FlagType32>((1 << 10) | (1 << 20) | (1 << 30)));
}
} // namespace Lateralus::Core::Tests
