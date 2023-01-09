#include PCH_FILE

import Lateralus.Core;
import Lateralus.Core.StringUtils;

import <array>;
import <string>;

using namespace std;
using namespace std::string_view_literals;
using namespace std::string_literals;

using namespace Lateralus::Core::StringUtils;

namespace Lateralus::Core::Tests
{
TEST(Core, StringUtils_CaseInsensitiveCompare)
{
    EXPECT_TRUE(CaseInsensitiveCompare("Abc", "abc"));
    EXPECT_TRUE(CaseInsensitiveCompare("abC", "abc"));
    EXPECT_TRUE(CaseInsensitiveCompare("ABC123!", "abc123!"));
    EXPECT_FALSE(CaseInsensitiveCompare("ABK 123!", "abc 123!"));

    EXPECT_TRUE(CaseInsensitiveCompare(u8"Abc", u8"abc"));
    EXPECT_TRUE(CaseInsensitiveCompare(u8"abC", u8"abc"));

    EXPECT_TRUE(CaseInsensitiveCompare(u8"ab\U0001F968\U0001F968c", u8"AB\U0001F968\U0001F968C"));
    EXPECT_FALSE(CaseInsensitiveCompare(u8"ab\U0001F968\U0001F968c", u8"AB\U0001F968\U0001F968K"));
}

} // namespace Lateralus::Core::Tests
