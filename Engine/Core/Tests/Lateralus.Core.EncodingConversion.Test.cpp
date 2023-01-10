#include PCH_FILE

import Lateralus.Core;
import Lateralus.Core.EncodingConversion;

#include <gtest/gtest.h>
#include <string>

using namespace Lateralus::Core::EncodingConversion;
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
TEST(Core_EncodingConversionTest, AsciiToUtf8)
{
    std::string ascii = "Hello, world!";
    std::u8string utf8(
        CountReEncodedSize<Encoding::ASCII, Encoding::UTF8>(byte_cast(ascii.data()), ascii.size()) /
            sizeof(char8_t),
        u8'\0');
    ReEncode<Encoding::ASCII, Encoding::UTF8>(byte_cast(ascii.data()), byte_cast(utf8.data()),
                                              ascii.size());
    EXPECT_EQ(utf8.compare(u8"Hello, world!"), 0);
}

TEST(Core_EncodingConversionTest, AsciiToUtf16)
{
    std::string ascii = "Hello, world!";
    std::u16string utf16(CountReEncodedSize<Encoding::ASCII, Encoding::UTF16>(
                             byte_cast(ascii.data()), ascii.size()) /
                             sizeof(char16_t),
                         u'\0');
    ReEncode<Encoding::ASCII, Encoding::UTF16>(byte_cast(ascii.data()), byte_cast(utf16.data()),
                                               ascii.size());
    EXPECT_EQ(utf16.compare(u"Hello, world!"), 0);
}

TEST(Core_EncodingConversionTest, AsciiToUtf32)
{
    std::string ascii = "Hello, world!";
    std::u32string utf32(CountReEncodedSize<Encoding::ASCII, Encoding::UTF32>(
                             byte_cast(ascii.data()), ascii.size()) /
                             sizeof(char32_t),
                         U'\0');
    ReEncode<Encoding::ASCII, Encoding::UTF32>(byte_cast(ascii.data()), byte_cast(utf32.data()),
                                               ascii.size());
    EXPECT_EQ(utf32.compare(U"Hello, world!"), 0);
}

TEST(Core_EncodingConversionTest, Utf8ToAscii)
{
    {
        std::u8string utf8 = u8"Hello, world!";
        std::string ascii(CountReEncodedSize<Encoding::UTF8, Encoding::ASCII>(
                              byte_cast(utf8.data()), utf8.size()) /
                              sizeof(char),
                          u'\0');
        ReEncode<Encoding::UTF8, Encoding::ASCII>(byte_cast(utf8.data()), byte_cast(ascii.data()),
                                                  utf8.size());
        EXPECT_EQ(ascii, "Hello, world!");
    }
    EXPECT_EQ("Hello, world!", string_cast<std::string>(u8"Hello, world!"));
    EXPECT_EQ("(?)", string_cast<std::string>(u8"(\u07ff)")); // \u07ff == ߿
    EXPECT_EQ("(?)",
              string_cast<std::string>(u8"(\u982d)")); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf8ToUtf16)
{
    {
        std::u8string utf8 = u8"Hello, world!";
        std::u16string utf16(CountReEncodedSize<Encoding::UTF8, Encoding::UTF16>(
                                 byte_cast(utf8.data()), utf8.size()) /
                                 sizeof(char16_t),
                             u'\0');
        ReEncode<Encoding::UTF8, Encoding::UTF16>(byte_cast(utf8.data()), byte_cast(utf16.data()),
                                                  utf8.size());
        EXPECT_EQ(utf16.compare(u"Hello, world!"), 0);
    }

    EXPECT_EQ(std::u16string(u"(\u982d)").compare(u"(\u982d)"),
              0); // \u982d == 頭

    EXPECT_EQ(string_cast<std::u16string>(u8"(\u982d)").compare(u"(\u982d)"),
              0); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf8ToUtf32)
{
    {
        std::u8string utf8 = u8"Hello, world!";
        std::u32string utf32(CountReEncodedSize<Encoding::UTF8, Encoding::UTF32>(
                                 byte_cast(utf8.data()), utf8.size()) /
                                 sizeof(char32_t),
                             U'\0');
        ReEncode<Encoding::UTF8, Encoding::UTF32>(byte_cast(utf8.data()), byte_cast(utf32.data()),
                                                  utf8.size());
        EXPECT_EQ(utf32.compare(U"Hello, world!"), 0);
    }
    EXPECT_EQ(string_cast<std::u32string>(u8"(\u07ff)").compare(U"(\u07ff)"),
              0); // \u07ff == ߿
    EXPECT_EQ(string_cast<std::u32string>(u8"(\u982d)").compare(U"(\u982d)"),
              0); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf16ToAscii)
{
    {
        std::u16string utf16 = u"Hello, world!";
        std::string ascii(CountReEncodedSize<Encoding::UTF16, Encoding::ASCII>(
                              byte_cast(utf16.data()), utf16.size() * sizeof(char16_t)),
                          '\0');
        ReEncode<Encoding::UTF16, Encoding::ASCII>(byte_cast(utf16.data()), byte_cast(ascii.data()),
                                                   utf16.size() * sizeof(char16_t));
        EXPECT_EQ(ascii, "Hello, world!");
    }
    EXPECT_EQ("(?)", string_cast<std::string>(u"(\u07ff)")); // \u07ff == ߿
    EXPECT_EQ("(?)",
              string_cast<std::string>(u"(\u982d)")); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf16ToUtf8)
{
    {
        std::u16string utf16 = u"Hello, world!";
        std::u8string utf8(CountReEncodedSize<Encoding::UTF16, Encoding::UTF8>(
                               byte_cast(utf16.data()), utf16.size() * sizeof(char16_t)),
                           u8'\0');
        ReEncode<Encoding::UTF16, Encoding::UTF8>(byte_cast(utf16.data()), byte_cast(utf8.data()),
                                                  utf16.size() * sizeof(char16_t));
        EXPECT_EQ(utf8.compare(u8"Hello, world!"), 0);
    }
    EXPECT_EQ(string_cast<std::u8string>(u"(\u07ff)").compare(u8"(\u07ff)"),
              0); // \u07ff == ߿
    EXPECT_EQ(string_cast<std::u8string>(u"(\u982d)").compare(u8"(\u982d)"),
              0); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf16ToUtf32)
{
    {
        std::u16string utf16 = u"Hello, world!";
        std::u32string utf32(CountReEncodedSize<Encoding::UTF16, Encoding::UTF32>(
                                 byte_cast(utf16.data()), utf16.size() * sizeof(char16_t)) /
                                 sizeof(char32_t),
                             U'\0');
        ReEncode<Encoding::UTF16, Encoding::UTF32>(byte_cast(utf16.data()), byte_cast(utf32.data()),
                                                   utf16.size() * sizeof(char16_t));
        EXPECT_EQ(utf32.compare(U"Hello, world!"), 0);
    }
    EXPECT_EQ(string_cast<std::u32string>(u"(\u07ff)").compare(U"(\u07ff)"),
              0); // \u07ff == ߿
    EXPECT_EQ(string_cast<std::u32string>(u"(\u982d)").compare(U"(\u982d)"),
              0); // \u982d == 頭
}
TEST(Core_EncodingConversionTest, Utf32ToAscii)
{
    {
        std::u32string utf32 = U"Hello, world!";
        std::string ascii(CountReEncodedSize<Encoding::UTF32, Encoding::ASCII>(
                              byte_cast(utf32.data()), utf32.size() * sizeof(char32_t)),
                          '\0');
        ReEncode<Encoding::UTF32, Encoding::ASCII>(byte_cast(utf32.data()), byte_cast(ascii.data()),
                                                   utf32.size() * sizeof(char32_t));
        EXPECT_EQ(ascii, "Hello, world!");
    }
    EXPECT_EQ("(?)", string_cast<std::string>(U"(\u07ff)")); // \u07ff == ߿
    EXPECT_EQ("(?)",
              string_cast<std::string>(U"(\u982d)")); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf32ToUtf8)
{
    {
        std::u32string utf32 = U"Hello, world!";
        std::u8string utf8(CountReEncodedSize<Encoding::UTF32, Encoding::UTF8>(
                               byte_cast(utf32.data()), utf32.size() * sizeof(char32_t)),
                           u8'\0');
        ReEncode<Encoding::UTF32, Encoding::UTF8>(byte_cast(utf32.data()), byte_cast(utf8.data()),
                                                  utf32.size() * sizeof(char32_t));
        EXPECT_EQ(utf8.compare(u8"Hello, world!"), 0);
    }
    EXPECT_EQ(string_cast<std::u8string>(U"(\u07ff)").compare(u8"(\u07ff)"),
              0); // \u07ff == ߿
    EXPECT_EQ(string_cast<std::u8string>(U"(\u982d)").compare(u8"(\u982d)"),
              0); // \u982d == 頭
}

TEST(Core_EncodingConversionTest, Utf32ToUtf16)
{
    {
        std::u32string utf32 = U"Hello, world!";
        std::u16string utf16(CountReEncodedSize<Encoding::UTF32, Encoding::UTF16>(
                                 byte_cast(utf32.data()), utf32.size() * sizeof(char32_t)) /
                                 sizeof(char16_t),
                             u'\0');
        ReEncode<Encoding::UTF32, Encoding::UTF16>(byte_cast(utf32.data()), byte_cast(utf16.data()),
                                                   utf32.size() * sizeof(char32_t));
        EXPECT_EQ(utf16.compare(u"Hello, world!"), 0);
    }
    EXPECT_EQ(string_cast<std::u16string>(U"(\u07ff)").compare(u"(\u07ff)"),
              0); // \u07ff == ߿
    EXPECT_EQ(string_cast<std::u16string>(U"(\u982d)").compare(u"(\u982d)"),
              0); // \u982d == 頭
}
} // namespace Lateralus::Core::Tests
