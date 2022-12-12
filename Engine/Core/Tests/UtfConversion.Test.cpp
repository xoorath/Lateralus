#include PCH_FILE

import Lateralus.Core;
import Lateralus.Core.UtfConversion;
import <array>;

using namespace std;
using namespace std::string_view_literals;

namespace Lateralus::Core::Tests
{
TEST(Core, UtfConversion_Ascii)
{
    EXPECT_TRUE(u8"a" == UTF32_to_u8string(0x00000061));
    EXPECT_TRUE(u8"z" == UTF32_to_u8string(0x0000007a));
    EXPECT_TRUE(u8"A" == UTF32_to_u8string(0x00000041));
    EXPECT_TRUE(u8"Z" == UTF32_to_u8string(0x0000005a));
    EXPECT_TRUE(u8"~" == UTF32_to_u8string(0x0000007e));
}

TEST(Core, UtfConversion_Emoji)
{
    EXPECT_TRUE(u8"\u263A" == UTF32_to_u8string(0x0000263a));
    EXPECT_TRUE(u8"\U0001F968" == UTF32_to_u8string(0x0001f968));
    EXPECT_TRUE(u8"\U0001F469" == UTF32_to_u8string(0x0001F469));
}

} // namespace Lateralus::Core::Tests
