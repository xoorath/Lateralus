export module Lateralus.Core.StringUtils;

import <algorithm>;
import <string>;
import <vector>;

using namespace std;

namespace Lateralus::Core::StringUtils
{

template <typename CharType> bool CaseInsensitiveCompareChar(CharType a, CharType b)
{
    return tolower(a) == tolower(b);
}

template<typename StringType>
bool CaseInsensitiveCompareStrings(StringType const &a, StringType const &b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                      [](StringType::value_type a, StringType::value_type b) {
                          return CaseInsensitiveCompareChar(a, b);
                      });
}

export bool CaseInsensitiveCompare(string_view const& a, string_view const& b)
{
    return CaseInsensitiveCompareStrings(a, b);
}

export bool CaseInsensitiveCompare(const u8string_view &a, const u8string_view &b)
{
    return CaseInsensitiveCompareStrings(a, b);
}

export bool CaseInsensitiveCompare(char a, char b)
{
    return CaseInsensitiveCompareChar(a, b);
}

export bool CaseInsensitiveCompare(char8_t a, char8_t b)
{
    return CaseInsensitiveCompareChar(a, b);
}

/// <summary>
/// Splits a string view into views of the input separated by delims.
/// Examples where letters are comma deliminated:
///     "a,b,c" -> ["a", "b", "c"]
///     ",a,b,c" -> ["a", "b", "c"]
///     "a,b,c," -> ["a", "b", "c"]
///     "a,,b,,c" -> ["a", "b", "c"]
///     "" -> []
///     "," -> []
/// </summary>
/// <param name="input">The input string to split.</param>
/// <param name="delims">A string with characters to split on.</param>
/// <returns>A collection of string views for each non-delim sequence.</returns>
export template <typename string_view_type>
vector<string_view_type> SplitStringView(string_view_type const &input,
                                         string_view_type const &delims)
{
    auto IsDelimChar = [delims](char8_t c) {
        return delims.find_first_of(c) != string_view_type::npos;
    };

    vector<string_view_type> result;

    // fast forward to the first legitimate begining position
    // (needed when strings begin with delims)
    size_t i = 0;
    while (i < input.size() && IsDelimChar(input[i]))
    {
        ++i;
    }
    size_t lastStart = i;

    // Guarentee one allocation for the function.
    result.reserve(count_if(input.begin(), input.end(), IsDelimChar));

    for (; i < input.size(); ++i)
    {
        if (IsDelimChar(input[i]))
        {
            // add the new string up to this point
            result.emplace_back(input.substr(lastStart, i - lastStart));

            // skip all following delims
            while (i < input.size() && IsDelimChar(input[i]))
            {
                ++i;
            }

            // record a new starting position
            lastStart = i;
        }
    }

    // (needed when strings don't end in delims)
    if (lastStart < input.size())
    {
        result.emplace_back(&input.data()[lastStart], input.size() - lastStart);
    }

    return result;
}

/// <summary>
/// Helper for SplitStringView providing \n and \r as delims
/// Note: Sequential newlines do not produce multiple entries as all blank entries are ignored.
/// Example:
///     "a\nb" -> ["a", "b"]
///     "a\n\n\nb" -> ["a", "b"]
/// </summary>
/// <param name="input">The input string to split.</param>
/// <returns>A collection of string views for each line of input.</returns>
export vector<string_view> SplitStringViewNewlines(string_view const &input)
{
    return SplitStringView<string_view>(input, "\n\r");
}
export vector<u8string_view> SplitStringViewNewlines(u8string_view const &input)
{
    return SplitStringView<u8string_view>(input, u8"\n\r");
}

export template <typename string_view_type>
string_view_type TrimRight(string_view_type const &str,
                           string_view_type const &toTrim = " \t\n\r\f\v")
{
    size_t const lastIndex = str.find_last_not_of(toTrim);
    if (lastIndex != string_view_type::npos)
    {
        return str.substr(0, lastIndex + 1);
    }
    return str;
}

export template <typename string_view_type>
string_view_type TrimLeft(string_view_type const &str,
                          string_view_type const &toTrim = " \t\n\r\f\v")
{
    size_t const firstIndex = str.find_first_not_of(toTrim);
    if (firstIndex != string_view_type::npos)
    {
        return str.substr(firstIndex);
    }
    return str;
}

export template <typename string_view_type>
string_view_type Trim(string_view_type const &str, string_view_type const &toTrim = " \t\n\r\f\v")
{
    return TrimLeft(TrimRight(str, toTrim), toTrim);
}

} // namespace Lateralus::Core::StringUtils