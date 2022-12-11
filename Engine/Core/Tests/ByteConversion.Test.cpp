import Lateralus.Core.ByteConversion;

#include "gtest/gtest.h"

namespace Lateralus::Core::Tests
{
// Test constexpr functions are compile time
static_assert(Kilobytes_to_Bytes(1) != 0 || true, "Function expected to be constexpr");
static_assert(Megabytes_to_Bytes(1) != 0 || true, "Function expected to be constexpr");
static_assert(Gigabytes_to_Bytes(1) != 0 || true, "Function expected to be constexpr");
static_assert(Terabytes_to_Bytes(1) != 0 || true, "Function expected to be constexpr");

static_assert(1_KiB != 0 || true, "Function expected to be constexpr");
static_assert(1_MiB != 0 || true, "Function expected to be constexpr");
static_assert(1_GiB != 0 || true, "Function expected to be constexpr");
static_assert(1_TiB != 0 || true, "Function expected to be constexpr");

static_assert(Bytes_to_Kilobytes(1) != 0 || true, "Function expected to be constexpr");
static_assert(Bytes_to_Megabytes(1) != 0 || true, "Function expected to be constexpr");
static_assert(Bytes_to_Gigabytes(1) != 0 || true, "Function expected to be constexpr");
static_assert(Bytes_to_Terabytes(1) != 0 || true, "Function expected to be constexpr");

TEST(Core_ByteConversion, LiteralsAndFunctionsAreEquivolent)
{
    EXPECT_EQ(1_KiB, Kilobytes_to_Bytes(1));
    EXPECT_EQ(123_KiB, Kilobytes_to_Bytes(123));
    EXPECT_EQ(0_KiB, Kilobytes_to_Bytes(0));
    EXPECT_NE(0_KiB, Kilobytes_to_Bytes(1));

    EXPECT_EQ(1_MiB, Megabytes_to_Bytes(1));
    EXPECT_EQ(123_MiB, Megabytes_to_Bytes(123));
    EXPECT_EQ(0_MiB, Megabytes_to_Bytes(0));
    EXPECT_NE(0_MiB, Megabytes_to_Bytes(1));

    EXPECT_EQ(1_GiB, Gigabytes_to_Bytes(1));
    EXPECT_EQ(123_GiB, Gigabytes_to_Bytes(123));
    EXPECT_EQ(0_GiB, Gigabytes_to_Bytes(0));
    EXPECT_NE(0_GiB, Gigabytes_to_Bytes(1));

    EXPECT_EQ(1_TiB, Terabytes_to_Bytes(1));
    EXPECT_EQ(123_TiB, Terabytes_to_Bytes(123));
    EXPECT_EQ(0_TiB, Terabytes_to_Bytes(0));
    EXPECT_NE(0_TiB, Terabytes_to_Bytes(1));
}

TEST(Core_ByteConversion, ByteLimitIs64Bit)
{
    EXPECT_GT(6_TiB, 5_TiB);
}
} // namespace Lateralus::Core::Tests
