export module Lateralus.Core.UtfConversion;

import <array>;
import <string>;
import Lateralus.Core;

using namespace std;

namespace Lateralus
{

// Written by referencing https://gist.github.com/Miouyouyou/864130e8734afe3f806512b14022226f
// which in turn was using Using http://www.unicode.org/versions/Unicode9.0.0/ch03.pdf page 56 as a
// basis.
export constexpr array<char8_t, 4> UTF32_to_UTF8(char32_t codepoint, usz &lengthOut)
{
    array<char8_t, 4> result;

    // Comments below are in the format:
    // codepoint: 00000000'00000000'00000000'0xxxxxxx
    // result:    0xxxxxxx'????????'????????'????????
    // The codepoint format is each bit high to low with a ' visually separating each byte.
    // The result format is each element of the array result from 0 to 3 separated by '.
    // (Elemets are also bits high to low.)

    if (codepoint <= 0b01111111)
    {
        lengthOut = 1;
        // codepoint: 00000000'00000000'00000000'0Xxxxxxx
        // result:    0Xxxxxxx'00000000'00000000'00000000
        result[0] = codepoint & 0b01111111; // 0Xxxxxxx;
        result[1] = 0;
        result[2] = 0;
        result[3] = 0;
    }
    else if (codepoint <= 0b00000000'00000000'00000111'11111111)
    {
        lengthOut = 2;
        // codepoint: 00000000'00000000'00000yyy'yyxxxxxx
        // result:    11yyyyyy'10xxxxxx'00000000'00000000
        result[0] = 0b11000000 | (codepoint >> 6);         // 10yyyyyy;
        result[1] = 0b10000000 | (codepoint & 0b00111111); // 10xxxxxx;
        result[2] = 0;
        result[3] = 0;
    }
    else if (codepoint <= 0b00000000'00000000'11111111'11111111)
    {
        lengthOut = 3;
        // codepoint: 00000000'00000000'zzzzyyyy'yyxxxxxx
        // result:    1110zzzz'10yyyyyy'10xxxxxx'00000000
        result[0] = 0b11100000 | (codepoint >> 12);               // 1110zzzz;
        result[1] = 0b10000000 | ((codepoint >> 6) & 0b00111111); // 10yyyyyy;
        result[2] = 0b10000000 | (codepoint & 0b00111111);        // 10xxxxxx;
        result[3] = 0;
    }
    else if (codepoint <= 0b00000000'00011111'11111111'11111111)
    {
        lengthOut = 4;
        // codepoint: 00000000'000UUUuu'zzzzyyyy'yyxxxxxx
        // result:    11110UUU'10uuzzzz'10yyyyyy'10xxxxxx
        result[0] = 0b11110000 | ((codepoint >> 18) & 0b00000111); // 11110UUU;
        result[1] = 0b10000000                                     //   10000000
                    | (((codepoint >> 16) & 0b00000011) << 4)      // | 00uu0000
                    | ((codepoint >> 12) & 0b00001111);            // | 0000zzzz;
        result[2] = 0b10000000 | ((codepoint >> 6) & 0b00111111);  // 10yyyyyy;
        result[3] = 0b10000000 | (codepoint & 0b00111111);         // 10xxxxxx;
    }
    else
    {
        lengthOut = 0;
    }
    return result;
}

export constexpr array<char8_t, 5> UTF32_to_UTF8(char32_t codepoint)
{
    usz unusedSize;
    array<char8_t, 4> uncapped = UTF32_to_UTF8(codepoint, unusedSize);
    array<char8_t, 5> nullTermed;
    std::copy(uncapped.begin(), uncapped.end(), nullTermed.begin());
    nullTermed[4] = u8'\0';
    return nullTermed;
}

export u8string UTF32_to_u8string(char32_t codepoint)
{
    usz size;
    array<char8_t, 4> uncapped = UTF32_to_UTF8(codepoint, size);
    return size != 0 ? u8string(uncapped.begin(), uncapped.begin() + size) : u8string();
}
} // namespace Lateralus