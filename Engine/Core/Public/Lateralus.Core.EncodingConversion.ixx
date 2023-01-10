export module Lateralus.Core.EncodingConversion;

import <bit>;
import <concepts>;
import <string>;
import Lateralus.Core;

using namespace std;

namespace Lateralus::Core::EncodingConversion
{
export
template <typename Type>
concept StringTypes = std::is_same<Type, string>::value || std::is_same<Type, wstring>::value ||
                    std::is_same<Type, u8string>::value || std::is_same<Type, u16string>::value ||
                    std::is_same<Type, u32string>::value;

export template <typename Type>
concept CharTypes = std::is_same<Type, char>::value || std::is_same<Type, wchar_t>::value ||
                      std::is_same<Type, char8_t>::value || std::is_same<Type, char16_t>::value ||
                      std::is_same<Type, char32_t>::value;


export enum class Encoding {
    ASCII,
    UTF8,
    // Always uses native endian. Does not support BOM.
    UTF16,
    // Always uses native endian. Does not support BOM.
    UTF32
};

/// <summary>
/// Get the size in bytes required to store the re-encoded string.
/// Does not include the size for a null terminator.
/// </summary>
/// <typeparam name="sourceEncoding">the source encoding</typeparam>
/// <typeparam name="destEncoding">the destination encoding</typeparam>
/// <param name="sourceBytes">the input string to be converted (relevant for variable width source
/// encodings)</param> <param name="sourceSize">source string size in bytes</param> <returns>Number
/// of bytes required to encode sourceBytes with fromEncoding to toEncoding not including a null
/// terminator.</returns>
export template <Encoding sourceEncoding, Encoding destEncoding>
usz CountReEncodedSize(byte const *sourceBytes, usz sourceSize) = delete;

export template <Encoding sourceEncoding, Encoding destEncoding>
void ReEncode(byte const *sourceBytes, byte *destBytes, usz sourceSize) = delete;


export template<typename T>
inline void EnsureLittleEndian(T& value)
{
    if constexpr(std::endian::native == std::endian::big)
    {
        union 
        {
            T swapped;
            std::array<byte, sizeof(T)> asArray;
        } u { value };

        for(size_t i = 0; i < sizeof(T)/2; ++i)
        {
            byte swap = asArray[i];
            u.asArray[i] = u.asArray[sizeof(T)-i-1];
            u.asArray[sizeof(T)-i-1] = u.asArray[i];
        }
        value = u.swapped;
    }
}

//////////////////////////////////////////////////////////////////////////
// Ascii conversions
export template <>
void ReEncode<Encoding::ASCII, Encoding::UTF8>(byte const *sourceBytes, byte *destBytes,
                                               usz sourceSize)
{
    auto sourceAsAscii = reinterpret_cast<char const *>(sourceBytes);
    auto destAs8 = reinterpret_cast<char8_t *>(destBytes);
    for (usz i = 0; i < sourceSize; ++i)
    {
        destAs8[i] = static_cast<char8_t>(sourceAsAscii[i] & 0b01111111);
    }
}
export template <>
usz CountReEncodedSize<Encoding::ASCII, Encoding::UTF8>(byte const *sourceBytes, usz sourceSize)
{
    (void)sourceBytes;
    return sourceSize;
}

export template <>
void ReEncode<Encoding::ASCII, Encoding::UTF16>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize)
{
    auto sourceAsAscii = reinterpret_cast<char const *>(sourceBytes);
    auto destAs16 = reinterpret_cast<char16_t *>(destBytes);
    for (usz i = 0; i < sourceSize; ++i)
    {
        destAs16[i] = sourceAsAscii[i] <= static_cast<byte>(0b01111111)
                          ? static_cast<char16_t>(sourceAsAscii[i])
                          : u'?';
    }
}
export template <>
usz CountReEncodedSize<Encoding::ASCII, Encoding::UTF16>(byte const *sourceBytes, usz sourceSize)
{
    (void)sourceBytes;
    return sourceSize * 2;
}

template <>
void ReEncode<Encoding::ASCII, Encoding::UTF32>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize)
{
    union {
        char32_t val;
        byte bytes[4];
    } u;

    auto sourceAsAscii = reinterpret_cast<char const *>(sourceBytes);
    auto destAs32 = reinterpret_cast<char32_t *>(destBytes);
    for (usz i = 0; i < sourceSize; ++i)
    {
        u.val = static_cast<char32_t>(sourceAsAscii[i]);
        destAs32[i] = (u.bytes[3] << 24) | (u.bytes[2] << 16) | (u.bytes[1] << 8) | u.bytes[0];
    }
}
export template <>
usz CountReEncodedSize<Encoding::ASCII, Encoding::UTF32>(byte const *sourceBytes, usz sourceSize)
{
    (void)sourceBytes;
    return sourceSize * 4;
}

//////////////////////////////////////////////////////////////////////////
// UTF8 Conversions
export template <>
void ReEncode<Encoding::UTF8, Encoding::ASCII>(byte const *sourceBytes, byte *destBytes,
                                               usz sourceSize)
{
    auto sourceAs8 = reinterpret_cast<char8_t const *>(sourceBytes);
    auto destAsAscii = reinterpret_cast<char *>(destBytes);
    usz i = 0;
    while (i < sourceSize)
    {
        char8_t const c = sourceAs8[i];
        if (c < 0x80)
        {
            *destAsAscii++ = static_cast<char>(c);
            i += 1;
        }
        else if (c < 0xE0)
        {
            *destAsAscii++ = '?';
            i += 2;
        }
        else if (c < 0xF0)
        {
            *destAsAscii++ = '?';
            i += 3;
        }
        else
        {
            *destAsAscii++ = '?';
            i += 4;
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF8, Encoding::ASCII>(byte const *sourceBytes, usz sourceSize)
{
    auto sourceAs8 = reinterpret_cast<char8_t const *>(sourceBytes);
    usz encodedSize = 0;
    for (usz i = 0; i < sourceSize; ++i)
    {
        char8_t c = sourceAs8[i];
        if (c < 0x80)
        {
            // ASCII character, 1 byte in UTF-8, 1 byte in ASCII (direct conversion)
            encodedSize += 1;
        }
        else if (c < 0xE0)
        {
            // 2-byte character, 2 bytes in UTF-8, 1 byte in ASCII ('?' character)
            encodedSize += 1;
            i++;
        }
        else if (c < 0xF0)
        {
            // 3-byte character, 3 bytes in UTF-8, 1 byte in ASCII ('?' character)
            encodedSize += 1;
            i += 2;
        }
        else
        {
            // 4-byte character, 4 bytes in UTF-8, 1 byte in ASCII ('?' character)
            encodedSize += 1;
            i += 3;
        }
    }
    return encodedSize;
}

export template <>
void ReEncode<Encoding::UTF8, Encoding::UTF16>(byte const *sourceBytes, byte *destBytes,
                                               usz sourceSize)
{
    auto sourceAs8 = reinterpret_cast<char8_t const *>(sourceBytes);
    auto destAs16 = reinterpret_cast<char16_t *>(destBytes);

    usz i = 0;
    while (i < sourceSize)
    {
        char8_t c = sourceAs8[i];
        if ((c & 0b10000000) == 0)
        {
            // ASCII character
            *destAs16++ = c;
            ++i;
        }
        else if ((c & 0b11100000) == 0b11000000)
        {
            // 2-byte character
            *destAs16++ = ((c & 0b00011111) << 6) | (sourceAs8[i + 1] & 0b00111111);
            i += 2;
        }
        else if ((c & 0b11110000) == 0b11100000)
        {
            // 3-byte character
            *destAs16++ = ((c & 0b00001111) << 12) | ((sourceAs8[i + 1] & 0b00111111) << 6) |
                          (sourceAs8[i + 2] & 0b00111111);
            i += 3;
        }
        else if ((c & 0b11111000) == 0b11110000)
        {
            // 4-byte character
            *destAs16++ = ((c & 0b00000111) << 18) | ((sourceAs8[i + 1] & 0b00111111) << 12) |
                          ((sourceAs8[i + 2] & 0b00111111) << 6) | (sourceAs8[i + 3] & 0b00111111);
            i += 4;
        }
        else
        {
            // Invalid UTF-8 character
            *destAs16++ = 0xFFFD;
            ++i;
        }
    }
}

export template <>
usz CountReEncodedSize<Encoding::UTF8, Encoding::UTF16>(byte const *sourceBytes, usz sourceSize)
{
    usz encodedSize = 0;
    auto sourceAs8 = reinterpret_cast<char8_t const *>(sourceBytes);
    for (usz i = 0; i < sourceSize; i++)
    {
        auto const &c = sourceAs8[i];
        if (c < 0x80)
        {
            // ASCII character, 1 byte in UTF-8, 2 bytes in UTF-16
            encodedSize += 2;
        }
        else if (c < 0xE0)
        {
            // 2-byte character, 2 bytes in UTF-8, 2 bytes in UTF-16
            encodedSize += 2;
            i++;
        }
        else if (c < 0xF0)
        {
            // 3-byte character, 3 bytes in UTF-8, 4 bytes in UTF-16
            encodedSize += 4;
            i += 2;
        }
        else
        {
            // 4-byte character, 4 bytes in UTF-8, 4 bytes in UTF-16
            encodedSize += 4;
            i += 3;
        }
    }

    return encodedSize;
}

export template <>
void ReEncode<Encoding::UTF8, Encoding::UTF32>(byte const *sourceBytes, byte *destBytes,
                                               usz sourceSize)
{
    auto sourceAs8 = reinterpret_cast<char8_t const *>(sourceBytes);
    auto destAs32 = reinterpret_cast<char32_t *>(destBytes);

    usz i = 0;
    while (i < sourceSize)
    {
        char8_t const &c = sourceAs8[i];
        if (!(c & 0b10000000))
        {
            // c is a single-byte character
            *destAs32++ = c;
            i++;
        }
        else if (!(c & 0b00100000))
        {
            // c is the start of a 2-byte character
            char32_t codepoint = c & 0b00011111;
            if (i + 1 < sourceSize && (sourceAs8[i + 1] & 0b11000000) == 0b10000000)
            {
                // the next byte is part of the 2-byte character
                codepoint = (codepoint << 6) | (sourceAs8[i + 1] & 0b00111111);
                *destAs32++ = codepoint;
                i += 2;
            }
            else
            {
                // the next byte is not part of the 2-byte character, treat it as an error
                *destAs32++ = U'?';
                i++;
            }
        }
        else if (!(c & 0b00010000))
        {
            // c is the start of a 3-byte character
            char32_t codepoint = c & 0b00001111;
            if (i + 2 < sourceSize && (sourceAs8[i + 1] & 0b11000000) == 0b10000000 &&
                (sourceAs8[i + 2] & 0b11000000) == 0b10000000)
            {
                // the next two bytes are part of the 3-byte character
                codepoint = (codepoint << 12) | ((sourceAs8[i + 1] & 0b00111111) << 6) |
                            (sourceAs8[i + 2] & 0b00111111);
                *destAs32++ = codepoint;
                i += 3;
            }
            else
            {
                // the next byte is not part of the 3-byte character, treat it as an error
                *destAs32++ = U'?';
                i++;
            }
        }
        else if (!(c & 0b00001000))
        {
            char32_t codepoint = c & 0b00000111;
            if (i + 3 < sourceSize && (sourceAs8[i + 1] & 0b11000000) == 0b10000000 &&
                (sourceAs8[i + 2] & 0b11000000) == 0b10000000 &&
                (sourceAs8[i + 3] & 0b11000000) == 0b10000000)
            {
                // c is the start of a 4-byte character
                codepoint = (codepoint << 18) | ((sourceAs8[i + 1] & 0b00111111) << 12) |
                            ((sourceAs8[i + 2] & 0b00111111) << 6) |
                            (sourceAs8[i + 3] & 0b00111111);
                *destAs32++ = codepoint;
                i += 4;
            }
            else
            {
                // the next byte is not part of the 3-byte character, treat it as an error
                *destAs32++ = U'?';
                i++;
            }
        }
        else
        {
            // this is an error or unsupported case
            *destAs32++ = U'?';
            i++;
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF8, Encoding::UTF32>(byte const *sourceBytes, usz sourceSize)
{
    auto sourceAs8 = reinterpret_cast<char8_t const *>(sourceBytes);
    usz encodedLength = 0;

    for (usz i = 0; i < sourceSize; ++i)
    {
        char8_t const &c = sourceAs8[i];
        // check if the first byte is a single-byte character (ASCII)
        if (!(c & 0b10000000))
        {
            encodedLength++;
        }
        // check if the first byte is the start of a 2-byte character
        else if (!(c & 0b00100000))
        {
            // check if the second byte is valid for a 2-byte character
            if (i + 1 < sourceSize && (sourceAs8[i + 1] & 0b11000000) == 0b10000000)
            {
                encodedLength++;
                i++;
            }
            // if the second byte is not valid, this is an error
            else
            {
                encodedLength++;
            }
        }
        // check if the first byte is the start of a 3-byte character
        else if (!(c & 0b00010000))
        {
            // check if the remaining bytes are valid for a 3-byte character
            if (i + 2 < sourceSize && (sourceAs8[i + 1] & 0b11000000) == 0b10000000 &&
                (sourceAs8[i + 2] & 0b11000000) == 0b10000000)
            {
                encodedLength++;
                i += 2;
            }
            // if the remaining bytes are not valid, this is an error
            else
            {
                encodedLength++;
            }
        }
        // check if the first byte is the start of a 4-byte character
        else if (!(c & 0b00001000))
        {
            // check if the remaining bytes are valid for a 4-byte character
            if (i + 3 < sourceSize && (sourceAs8[i + 1] & 0b11000000) == 0b10000000 &&
                (sourceAs8[i + 2] & 0b11000000) == 0b10000000 &&
                (sourceAs8[i + 3] & 0b11000000) == 0b10000000)
            {
                encodedLength++;
                i += 3;
            }
            // if the remaining bytes are not valid, this is an error
            else
            {
                encodedLength++;
            }
        }
        // this is an error or unsupported case
        else
        {
            encodedLength++;
        }
    }

    return encodedLength * sizeof(char32_t);
}

//////////////////////////////////////////////////////////////////////////
// utf16
export template <>
void ReEncode<Encoding::UTF16, Encoding::ASCII>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize)
{
    usz const sourceLength = sourceSize / 2;
    auto sourceAs16 = reinterpret_cast<char16_t const *>(sourceBytes);
    auto destAs8 = reinterpret_cast<char8_t *>(destBytes);

    for (usz i = 0; i < sourceLength; ++i)
    {
        char16_t codepoint = sourceAs16[i];
        EnsureLittleEndian(codepoint);
        if (codepoint <= 0x7F)
        {
            *(destAs8++) = codepoint;
        }
        else
        {
            *(destAs8++) = '?';
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF16, Encoding::ASCII>(byte const *sourceBytes, usz sourceSize)
{
    (void)sourceBytes;
    return sourceSize / 2;
}

export template <>
void ReEncode<Encoding::UTF16, Encoding::UTF8>(byte const *sourceBytes, byte *destBytes,
                                               usz sourceSize)
{
    usz const sourceLength = sourceSize / 2;
    auto sourceAs16 = reinterpret_cast<char16_t const *>(sourceBytes);
    auto destAs8 = reinterpret_cast<char8_t *>(destBytes);

    for (usz i = 0; i < sourceLength; ++i)
    {
        char16_t codepoint = sourceAs16[i];
        EnsureLittleEndian(codepoint);
        // check if the character is a BMP (Basic Multilingual Plane) character
        if (codepoint <= 0xFFFF)
        {
            // codepoint: 00000000'00000000'0000xxxx'xxxxxxxx
            // result:    0xxxxxxx'00000000'00000000'00000000
            *(destAs8++) = codepoint & 0b01111111;
        }
        // check if the character is a high surrogate (part of a supplementary character)
        else if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
        {
            // check if the next character is a low surrogate
            if (i + 1 < sourceLength && sourceAs16[i + 1] >= 0xDC00 && sourceAs16[i + 1] <= 0xDFFF)
            {
                // combine the high and low surrogates to form the codepoint
                char32_t codepoint32 =
                    ((codepoint - 0xD800) << 10) + (sourceAs16[i + 1] - 0xDC00) + 0x10000;
                // codepoint: 00000000'000yyyyy'yyyyxxxx'xxxxxxxx
                // result: 11110yyy'10yxxxxx'00000000'00000000
                *(destAs8++) = 0b11110000 | ((codepoint32 >> 18) & 0b00000111); // 11110yyy;
                // codepoint: 00000000'00000000'zzzzyyyy'yyxxxxxx
                // result: 10zzzzzz'10yyyyyy'10xxxxxx'00000000
                *(destAs8++) = 0b10000000 | ((codepoint32 >> 12) & 0b00111111); // 10zzzzzz;
                *(destAs8++) = 0b10000000 | ((codepoint32 >> 6) & 0b00111111);  // 10yyyyyy;
                *(destAs8++) = 0b10000000 | (codepoint32 & 0b00111111);         // 10xxxxxx;
                // skip the next character (the low surrogate)
                i++;
            }
            else
            {
                // this is an error or unsupported case
                *(destAs8++) = u8'?';
            }
        }
        else
        {
            // this is an error or unsupported case
            *(destAs8++) = u8'?';
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF16, Encoding::UTF8>(byte const *sourceBytes, usz sourceSize)
{
    usz const sourceLength = sourceSize / 2;
    auto sourceAs16 = reinterpret_cast<char16_t const *>(sourceBytes);
    usz encodedLength = 0;
    for (usz i = 0; i < sourceLength; ++i)
    {
        char16_t const &codepoint = sourceAs16[i];
        // check if the character is a BMP (Basic Multilingual Plane) character
        if (codepoint <= 0xFFFF)
        {
            encodedLength++;
        }
        // check if the character is a high surrogate (part of a supplementary character)
        else if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
        {
            // check if the next character is a low surrogate
            if (i + 1 < sourceLength && sourceAs16[i + 1] >= 0xDC00 && sourceAs16[i + 1] <= 0xDFFF)
            {
                // a supplementary character consists of 4 UTF-8 bytes
                encodedLength += 4;
                // skip the next character (the low surrogate)
                i++;
            }
            else
            {
                // this is an error or unsupported case
                encodedLength++;
            }
        }
        else
        {
            // this is an error or unsupported case
            encodedLength++;
        }
    }
    return encodedLength;
}

export template <>
void ReEncode<Encoding::UTF16, Encoding::UTF16>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize) = delete;
export template <>
usz CountReEncodedSize<Encoding::UTF16, Encoding::UTF16>(byte const *sourceBytes,
                                                         usz sourceSize) = delete;

export template <>
void ReEncode<Encoding::UTF16, Encoding::UTF32>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize)
{
    // calculate the number of UTF-16 characters in the source string
    usz const sourceLength = sourceSize / 2;
    // reinterpret the source and destination byte arrays as arrays of UTF-16 and UTF-32 characters
    // respectively
    auto sourceAs16 = reinterpret_cast<char16_t const *>(sourceBytes);
    auto destAs32 = reinterpret_cast<char32_t *>(destBytes);

    for (usz i = 0; i < sourceLength; ++i)
    {
        char16_t const &codepoint = sourceAs16[i];
        // check if the character is a BMP (Basic Multilingual Plane) character
        if (codepoint <= 0xFFFF)
        {
            *(destAs32++) = codepoint;
        }
        // check if the character is a high surrogate (part of a supplementary character)
        else if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
        {
            // check if the next character is a low surrogate
            if (i + 1 < sourceLength && sourceAs16[i + 1] >= 0xDC00 && sourceAs16[i + 1] <= 0xDFFF)
            {
                // combine the high and low surrogates to form the supplementary character
                char32_t supplementaryCharacter =
                    ((codepoint - 0xD800) << 10) + (sourceAs16[i + 1] - 0xDC00) + 0x10000;
                *(destAs32++) = supplementaryCharacter;
                // skip the next character (the low surrogate)
                i++;
            }
            else
            {
                // this is an error or unsupported case
                *(destAs32++) = U'?';
            }
        }
        else
        {
            // this is an error or unsupported case
            *(destAs32++) = U'?';
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF16, Encoding::UTF32>(byte const *sourceBytes, usz sourceSize)
{
    usz const sourceLength = sourceSize / 2;
    auto sourceAs16 = reinterpret_cast<char16_t const *>(sourceBytes);
    usz encodedLength = 0;
    for (usz i = 0; i < sourceLength; ++i)
    {
        char16_t const &codepoint = sourceAs16[i];
        // check if the character is a BMP (Basic Multilingual Plane) character
        if (codepoint <= 0xFFFF)
        {
            encodedLength++;
        }
        // check if the character is a high surrogate (part of a supplementary character)
        else if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
        {
            // check if the next character is a low surrogate
            if (i + 1 < sourceLength && sourceAs16[i + 1] >= 0xDC00 && sourceAs16[i + 1] <= 0xDFFF)
            {
                // skip the next character (the low surrogate)
                i++;
            }
            // this is an error or unsupported case
            encodedLength++;
        }
        else
        {
            // this is an error or unsupported case
            encodedLength++;
        }
    }
    return encodedLength * sizeof(char32_t);
}

//////////////////////////////////////////////////////////////////////////
// utf32
export template <>
void ReEncode<Encoding::UTF32, Encoding::ASCII>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize)
{
    auto sourceAs32 = reinterpret_cast<char32_t const *>(sourceBytes);
    auto destAsAscii = reinterpret_cast<char *>(destBytes);
    usz const sourceLength = sourceSize / sizeof(char32_t);
    for (usz i = 0; i < sourceLength; ++i)
    {
        destAsAscii[i] = sourceAs32[i] <= 0b01111111 ? static_cast<char>(sourceAs32[i]) : '?';
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF32, Encoding::ASCII>(byte const *sourceBytes, usz sourceSize)
{
    return sourceSize / 4;
}

export template <>
void ReEncode<Encoding::UTF32, Encoding::UTF8>(byte const *sourceBytes, byte *destBytes,
                                               usz sourceSize)
{
    usz const sourceLength = sourceSize / 4;
    auto sourceAs32 = reinterpret_cast<char32_t const *>(sourceBytes);
    auto destAs8 = reinterpret_cast<char8_t *>(destBytes);

    for (usz i = 0; i < sourceLength; ++i)
    {
        auto const &codepoint = sourceAs32[i];
        if (codepoint < 0b10000000)
        {
            // codepoint: 00000000'00000000'00000000'0Xxxxxxx
            // result:    0Xxxxxxx'00000000'00000000'00000000
            *(destAs8++) = codepoint & 0b01111111; // 0Xxxxxxx;
        }
        else if (codepoint < 0b00001000'00000000)
        {
            // codepoint: 00000000'00000000'00000yyy'yyxxxxxx
            // result:    110yyyyy'10xxxxxx'00000000'00000000
            *(destAs8++) = 0b11000000 | (codepoint >> 6);         // 110yyyyy;
            *(destAs8++) = 0b10000000 | (codepoint & 0b00111111); // 10xxxxxx;
        }
        else if (codepoint <= 0b11111111'11111111)
        {
            // codepoint: 00000000'00000000'zzzzyyyy'yyxxxxxx
            // result:    1110zzzz'10yyyyyy'10xxxxxx'00000000
            *(destAs8++) = 0b11100000 | (codepoint >> 12);               // 1110zzzz;
            *(destAs8++) = 0b10000000 | ((codepoint >> 6) & 0b00111111); // 10yyyyyy;
            *(destAs8++) = 0b10000000 | (codepoint & 0b00111111);        // 10xxxxxx;
        }
        else if (codepoint <= 0b00011111'11111111'11111111)
        {
            // codepoint: 00000000'000UUUuu'zzzzyyyy'yyxxxxxx
            // result:    11110UUU'10uuzzzz'10yyyyyy'10xxxxxx
            *(destAs8++) = 0b11110000 | ((codepoint >> 18) & 0b00000111); // 11110UUU;
            *(destAs8++) = 0b10000000                                     //   10000000
                           | (((codepoint >> 16) & 0b00000011) << 4)      // | 00uu0000
                           | ((codepoint >> 12) & 0b00001111);            // | 0000zzzz;
            *(destAs8++) = 0b10000000 | ((codepoint >> 6) & 0b00111111);  // 10yyyyyy;
            *(destAs8++) = 0b10000000 | (codepoint & 0b00111111);         // 10xxxxxx;
        }
        else
        {
            // this is an error or unsupported case
            *(destAs8++) = u8'?';
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF32, Encoding::UTF8>(byte const *sourceBytes, usz sourceSize)
{
    usz const sourceLength = sourceSize / sizeof(char32_t);
    usz encodedLength = 0;
    auto sourceAs32 = reinterpret_cast<char32_t const *>(sourceBytes);
    for (usz i = 0; i < sourceLength; ++i)
    {
        auto const &codepoint = sourceAs32[i];
        if (codepoint <= 0b01111111)
        {
            encodedLength++;
        }
        else if (codepoint <= 0b00000111'11111111)
        {
            encodedLength += 2;
        }
        else if (codepoint <= 0b11111111'11111111)
        {
            encodedLength += 3;
        }
        else if (codepoint <= 0b00011111'11111111'11111111)
        {
            encodedLength += 3;
        }
        else
        {
            // this is an error/unsupported case
            encodedLength++;
        }
    }
    return encodedLength;
}

export template <>
void ReEncode<Encoding::UTF32, Encoding::UTF16>(byte const *sourceBytes, byte *destBytes,
                                                usz sourceSize)
{
    usz const sourceLength = sourceSize / 4;
    auto sourceAs32 = reinterpret_cast<char32_t const *>(sourceBytes);
    auto destAs16 = reinterpret_cast<char16_t *>(destBytes);

    for (usz i = 0; i < sourceLength; ++i)
    {
        auto const &codepoint = sourceAs32[i];
        if (codepoint <= 0xFFFF)
        {
            // codepoint: 00000000'00000000'0000xxxx'xxxxxxxx
            // result:    xxxxxxxx'xxxxxxxx
            *(destAs16++) = codepoint;
        }
        else
        {
            // codepoint: 00000000'00000000'000yyyyy'yyyyxxxx'xxxxxxxx
            // result:    110110ww'wwzzzzzz'110111yy'yyyyxxxx
            // where wwzzzzzz = 000uuuuu (high 10 bits of codepoint)
            //       yyyyyxxx = xxxxxxxx (low 10 bits of codepoint)
            *(destAs16++) = 0xD800 | ((codepoint >> 10) & 0x3FF);
            *(destAs16++) = 0xDC00 | (codepoint & 0x3FF);
        }
    }
}
export template <>
usz CountReEncodedSize<Encoding::UTF32, Encoding::UTF16>(byte const *sourceBytes, usz sourceSize)
{
    usz const sourceLength = sourceSize / sizeof(char32_t);
    usz encodedLength = 0;
    auto sourceAs32 = reinterpret_cast<char32_t const *>(sourceBytes);
    for (usz i = 0; i < sourceLength; ++i)
    {
        auto const &codepoint = sourceAs32[i];
        if (codepoint <= 0xFFFF)
        {
            encodedLength++;
        }
        else
        {
            encodedLength += 2;
        }
    }
    return encodedLength * sizeof(char16_t);
}

//////////////////////////////////////////////////////////////////////////

// A helper to simplify casting code
template <Encoding k_SourceEncoding, Encoding k_DestEncoding, StringTypes StringType>
StringType ReEncodeToString(byte const *sourceBytes, usz const sourceSize)
{
    StringType destString(
        CountReEncodedSize<k_SourceEncoding, k_DestEncoding>(sourceBytes, sourceSize) / sizeof(StringType::value_type),
        StringType::value_type(0));
    byte *destBytes = reinterpret_cast<byte *>(destString.data());
    ReEncode<k_SourceEncoding, k_DestEncoding>(sourceBytes, destBytes, sourceSize);
    // utf-8 and utf-16 are multibyte character sets
    // Although the destString constructor should provide a correctly sized buffer the resulting string length
    // may be shorter whenever multibyte characters are present. This condition accounts for that.
    if constexpr(k_DestEncoding == Encoding::UTF8 || k_DestEncoding == Encoding::UTF16)
    {
        destString.resize(StringType::traits_type::length(destString.data()));
    }
    return destString;
}

// These are the conversion methods without any specialization.
// Specialization will only exist where it makes sense (example: utf8->utf8 will not be specialized)
export template <StringTypes StringType> StringType string_cast(string_view asciiView);
export template <StringTypes StringType> StringType string_cast(u8string_view utf8View);
export template <StringTypes StringType> StringType string_cast(u16string_view utf16View);
export template <StringTypes StringType> StringType string_cast(u32string_view utf32View);

// Cast to ascii string
export template <> string string_cast<string>(string_view asciiView) = delete;
export template <> string string_cast<string>(u8string_view utf8View)
{
    return ReEncodeToString<Encoding::UTF8, Encoding::ASCII, string>(
        reinterpret_cast<byte const *>(utf8View.data()), utf8View.size() * sizeof(char8_t));
}
export template <> string string_cast<string>(u16string_view utf16View)
{
    return ReEncodeToString<Encoding::UTF16, Encoding::ASCII, string>(
        reinterpret_cast<byte const *>(utf16View.data()), utf16View.size() * sizeof(char16_t));
}
export template <> string string_cast<string>(u32string_view utf32View)
{
    return ReEncodeToString<Encoding::UTF32, Encoding::ASCII, string>(
        reinterpret_cast<byte const *>(utf32View.data()), utf32View.size() * sizeof(char32_t));
}

// Cast to utf8 u8string
export template <> u8string string_cast<u8string>(string_view asciiView)
{
    return ReEncodeToString<Encoding::ASCII, Encoding::UTF8, u8string>(
        reinterpret_cast<byte const *>(asciiView.data()), asciiView.size() * sizeof(char));
}
export template <> u8string string_cast<u8string>(u8string_view utf8View) = delete;
export template <> u8string string_cast<u8string>(u16string_view utf16View)
{
    return ReEncodeToString<Encoding::UTF16, Encoding::UTF8, u8string>(
        reinterpret_cast<byte const *>(utf16View.data()), utf16View.size() * sizeof(char16_t));
}
export template <> u8string string_cast<u8string>(u32string_view utf32View)
{
    return ReEncodeToString<Encoding::UTF32, Encoding::UTF8, u8string>(
        reinterpret_cast<byte const *>(utf32View.data()), utf32View.size() * sizeof(char32_t));
}

// Cast to utf16 u16string
export template <> u16string string_cast<u16string>(string_view asciiView)
{
    return ReEncodeToString<Encoding::ASCII, Encoding::UTF16, u16string>(
        reinterpret_cast<byte const *>(asciiView.data()), asciiView.size() * sizeof(char));
}
export template <> u16string string_cast<u16string>(u8string_view utf8View)
{
    return ReEncodeToString<Encoding::UTF8, Encoding::UTF16, u16string>(
        reinterpret_cast<byte const *>(utf8View.data()), utf8View.size() * sizeof(char8_t));
}
export template <> u16string string_cast<u16string>(u16string_view utf16View) = delete;
export template <> u16string string_cast<u16string>(u32string_view utf32View)
{
    return ReEncodeToString<Encoding::UTF32, Encoding::UTF16, u16string>(
        reinterpret_cast<byte const *>(utf32View.data()), utf32View.size() * sizeof(char32_t));
}

// Cast to utf32 u32string
export template <> u32string string_cast<u32string>(string_view asciiView)
{
    return ReEncodeToString<Encoding::ASCII, Encoding::UTF32, u32string>(
        reinterpret_cast<byte const *>(asciiView.data()), asciiView.size() * sizeof(char));
}
export template <> u32string string_cast<u32string>(u8string_view utf8View)
{
    return ReEncodeToString<Encoding::UTF8, Encoding::UTF32, u32string>(
        reinterpret_cast<byte const *>(utf8View.data()), utf8View.size() * sizeof(char8_t));
}
export template <> u32string string_cast<u32string>(u16string_view utf16View)
{
    return ReEncodeToString<Encoding::UTF16, Encoding::UTF32, u32string>(
        reinterpret_cast<byte const *>(utf16View.data()), utf16View.size() * sizeof(char16_t));
}
export template <> u32string string_cast<u32string>(u32string_view utf32View) = delete;

} // namespace Lateralus::Core::EncodingConversion