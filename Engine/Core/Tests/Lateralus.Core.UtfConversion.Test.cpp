#include PCH_FILE

import Lateralus.Core;
import Lateralus.Core.UtfConversion;

#include <gtest/gtest.h>
#include <string>

using namespace Lateralus::Core::UtfConversion;
using namespace Lateralus::Core;

template <typename T> inline byte const *byte_cast(T const *t)
{
    return reinterpret_cast<byte const *>(t);
}
template <typename T> inline byte *byte_cast(T *t)
{
    return reinterpret_cast<byte *>(t);
}

namespace Lateralus::Core::Tests
{

TEST(Core_UtfConversionTest, Utf32ToAscii)
{
    std::u32string utf32 = U"Hello, world!";
    std::vector<char> ascii(CountReEncodedSize<Encoding::UTF32, Encoding::ASCII>(
        byte_cast(utf32.data()), utf32.size() * sizeof(char32_t)));
    ReEncode<Encoding::UTF32, Encoding::ASCII>(byte_cast(utf32.data()), byte_cast(ascii.data()),
                                               utf32.size() * sizeof(char32_t));
    std::string asciiStr(ascii.begin(), ascii.end());
    EXPECT_EQ(asciiStr, "Hello, world!");
}

TEST(Core_UtfConversionTest, AsciiToUtf32)
{
    std::string ascii = "Hello, world!";
    std::vector<char32_t> utf32(CountReEncodedSize<Encoding::ASCII, Encoding::UTF32>(
                                    byte_cast(ascii.data()), ascii.size()) /
                                sizeof(char32_t));
    ReEncode<Encoding::ASCII, Encoding::UTF32>(byte_cast(ascii.data()), byte_cast(utf32.data()),
                                               ascii.size());
    std::u32string utf32Str(utf32.begin(), utf32.end());
    EXPECT_EQ(utf32Str.compare(U"Hello, world!"), 0);
}

TEST(Core_UtfConversionTest, Utf32ToUtf16)
{
    std::u32string utf32 = U"Hello, world!";
    std::vector<char16_t> utf16(CountReEncodedSize<Encoding::UTF32, Encoding::UTF16>(
                                    byte_cast(utf32.data()), utf32.size() * sizeof(char32_t)) /
                                sizeof(char16_t));
    ReEncode<Encoding::UTF32, Encoding::UTF16>(byte_cast(utf32.data()), byte_cast(utf16.data()),
                                               utf32.size() * sizeof(char32_t));
    std::u16string utf16Str(utf16.begin(), utf16.end());
    EXPECT_EQ(utf16Str.compare(u"Hello, world!"), 0);
}

TEST(Core_UtfConversionTest, Utf32ToUtf8)
{
    std::u32string utf32 = U"Hello, world!";
    std::vector<uint8_t> utf8(CountReEncodedSize<Encoding::UTF32, Encoding::UTF8>(
        byte_cast(utf32.data()), utf32.size() * sizeof(char32_t)));
    ReEncode<Encoding::UTF32, Encoding::UTF8>(byte_cast(utf32.data()), byte_cast(utf8.data()),
                                              utf32.size() * sizeof(char32_t));
    std::u8string utf8Str(utf8.begin(), utf8.end());
    EXPECT_EQ(utf8Str.compare(u8"Hello, world!"), 0);
}

TEST(Core_UtfConversionTest, Utf16ToUtf32)
{
    std::u16string utf16 = u"Hello, world!";
    std::vector<char32_t> utf32(CountReEncodedSize<Encoding::UTF16, Encoding::UTF32>(
                                    byte_cast(utf16.data()), utf16.size() * sizeof(char16_t)) /
                                sizeof(char32_t));
    ReEncode<Encoding::UTF16, Encoding::UTF32>(byte_cast(utf16.data()), byte_cast(utf32.data()),
                                               utf16.size() * sizeof(char16_t));
    std::u32string utf32Str(utf32.begin(), utf32.end());
    EXPECT_EQ(utf32Str.compare(U"Hello, world!"), 0);
}

TEST(Core_UtfConversionTest, Utf16ToUtf8)
{
    std::u16string utf16 = u"Hello, world!";
    std::vector<uint8_t> utf8(CountReEncodedSize<Encoding::UTF16, Encoding::UTF8>(
        byte_cast(utf16.data()), utf16.size() * sizeof(char16_t)));
    ReEncode<Encoding::UTF16, Encoding::UTF8>(byte_cast(utf16.data()), byte_cast(utf8.data()),
                                              utf16.size() * sizeof(char16_t));
    std::u8string utf8Str(utf8.begin(), utf8.end());
    EXPECT_EQ(utf8Str.compare(u8"Hello, world!"), 0);
}

TEST(Core_UtfConversionTest, Utf8ToUtf32)
{
    std::u8string utf8 = u8"Hello, world!";
    std::vector<char32_t> utf32(
        CountReEncodedSize<Encoding::UTF8, Encoding::UTF32>(byte_cast(utf8.data()), utf8.size()) /
        sizeof(char32_t));
    ReEncode<Encoding::UTF8, Encoding::UTF32>(byte_cast(utf8.data()), byte_cast(utf32.data()),
                                              utf8.size());
    std::u32string utf32Str(utf32.begin(), utf32.end());
    EXPECT_EQ(utf32Str.compare(U"Hello, world!"), 0);
}

TEST(Core_UtfConversionTest, Utf8ToUtf16)
{
    std::u8string utf8 = u8"Hello, world!";
    std::vector<char16_t> utf16(
        CountReEncodedSize<Encoding::UTF8, Encoding::UTF16>(byte_cast(utf8.data()), utf8.size()) /
        sizeof(char16_t));
    ReEncode<Encoding::UTF8, Encoding::UTF16>(byte_cast(utf8.data()), byte_cast(utf16.data()),
                                              utf8.size());
    std::u16string utf16Str(utf16.begin(), utf16.end());
    EXPECT_EQ(utf16Str.compare(u"Hello, world!"), 0);
}
}
    //
//
// import Lateralus.Core;
// import Lateralus.Core.UtfConversion;
//
// import <array>;
// import <string>;
//
// using namespace std;
// using namespace std::string_view_literals;
// using namespace std::string_literals;
//
// using namespace Lateralus::Core::UtfConversion;
//
// namespace Lateralus::Core::Tests
//{
//
//// gtest can't print the new string types so we wrap expect_true and provide our own output
///instead.
// #define EXPECT_EQ_STR(leftStr, rightStr) EXPECT_TRUE(leftStr == rightStr) << #leftStr << " == "
// << #rightStr
//
// TEST(Core, UtfConversion_CodepointTo)
//{
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x00000061), L"a"s) << " (test ascii - wide)";
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x0000007a), L"z"s) << " (test ascii - wide)";
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x00000041), L"A"s) << " (test ascii - wide)";
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x0000005a), L"Z"s) << " (test ascii - wide)";
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x0000007e), L"~"s) << " (test ascii - wide)";
//
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x0000263a), L"\u263A"s) << " (test emoji - wide)";
//     EXPECT_EQ_STR(CodepointConvert<wstring>(0x0001f968), L"\U0001F968"s) << " (test emoji -
//     wide)";
//
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x00000061), u8"a"s) << " (test ascii - u8)";
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x0000007a), u8"z"s) << " (test ascii - u8)";
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x00000041), u8"A"s) << " (test ascii - u8)";
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x0000005a), u8"Z"s) << " (test ascii - u8)";
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x0000007e), u8"~"s) << " (test ascii - u8)";
//
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x0000263a), u8"\u263A"s) << " (test emoji - u8)";
//     EXPECT_EQ_STR(CodepointConvert<u8string>(0x0001f968), u8"\U0001F968"s) << " (test emoji -
//     u8)";
//
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x00000061), u"a"s) << " (test ascii - u16)";
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x0000007a), u"z"s) << " (test ascii - u16)";
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x00000041), u"A"s) << " (test ascii - u16)";
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x0000005a), u"Z"s) << " (test ascii - u16)";
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x0000007e), u"~"s) << " (test ascii - u16)";
//
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x0000263a), u"\u263A"s) << " (test emoji - u16)";
//     EXPECT_EQ_STR(CodepointConvert<u16string>(0x0001f968), u"\U0001F968"s) << " (test emoji -
//     u16)";
// }
//
// TEST(Core, UtfConversion_string_to)
//{
//     EXPECT_EQ_STR(StringConvert<u8string>("abc"s), u8"abc"s);
//     EXPECT_EQ_STR(StringConvert<u16string>("abc"s), u"abc"s);
//     EXPECT_EQ_STR(StringConvert<u32string>("abc"s), U"abc"s);
//     EXPECT_EQ_STR(StringConvert<wstring>("abc"s), L"abc"s);
// }
//
// TEST(Core, UtfConversion_wstring_to)
//{
//     EXPECT_EQ_STR(StringConvert<string>(L"abc\u982d\u8166"s), "abc??"s);
//     EXPECT_EQ_STR(StringConvert<u8string>(L"abc\u982d\u8166"s), u8"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u16string>(L"abc\u982d\u8166"s), u"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u32string>(L"abc\u982d\u8166"s), U"abc\u982d\u8166"s);
// }
//
// TEST(Core, UtfConversion_u8string_to)
//{
//     EXPECT_EQ_STR(StringConvert<string>(u8"abc\u982d\u8166"), "abc??"s);
//     EXPECT_EQ_STR(StringConvert<wstring>(u8"abc\u982d\u8166"), L"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u16string>(u8"abc\u982d\u8166"), u"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u32string>(u8"abc\u982d\u8166"), U"abc\u982d\u8166"s);
// }
//
// TEST(Core, UtfConversion_u16string_to)
//{
//     EXPECT_EQ_STR(StringConvert<string>(u"abc\u982d\u8166"s), "abc??"s);
//     EXPECT_EQ_STR(StringConvert<wstring>(u"abc\u982d\u8166"s), L"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u8string>(u"abc\u982d\u8166"s), u8"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u32string>(u"abc\u982d\u8166"s), U"abc\u982d\u8166"s);
// }
//
// TEST(Core, UtfConversion_u32string_to)
//{
//     EXPECT_EQ_STR(StringConvert<string>(U"abc\u982d\u8166"s), "abc??"s);
//     EXPECT_EQ_STR(StringConvert<wstring>(U"abc\u982d\u8166"s), L"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u8string>(U"abc\u982d\u8166"s), u8"abc\u982d\u8166"s);
//     EXPECT_EQ_STR(StringConvert<u16string>(U"abc\u982d\u8166"s), u"abc\u982d\u8166"s);
// }
//
// } // namespace Lateralus::Core::Tests
