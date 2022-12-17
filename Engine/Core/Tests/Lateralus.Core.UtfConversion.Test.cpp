#include PCH_FILE

import Lateralus.Core;
import Lateralus.Core.UtfConversion;

import <array>;
import <string>;

using namespace std;
using namespace std::string_view_literals;
using namespace std::string_literals;

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
}

TEST(Core, UtfConversion_TruncateUtf8)
{
    // 4 byte unicode str
    EXPECT_EQ(u8string_to_string(u8"ab.c"), "ab.c"s);
    // 5 byte unicode str
    EXPECT_EQ(u8string_to_string(u8"ab\u00c0c"), "ab?c"s);
    // 6 byte unicode str
    EXPECT_EQ(u8string_to_string(u8"ab\u263Ac"), "ab?c"s);
    // 7 byte unicode str
    EXPECT_EQ(u8string_to_string(u8"ab\U0001F968c"), "ab?c"s);
    // back to back emoji (11 byte unicode str)
    EXPECT_EQ(u8string_to_string(u8"ab\U0001F968\U0001F968c"), "ab??c"s);
}

} // namespace Lateralus::Core::Tests
