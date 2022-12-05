export module Lateralus.Core.ByteConversion;

import <string>;
import <sstream>;
import Lateralus.Core;

using namespace std;

namespace Lateralus
{
    export
    constexpr
    uint64 Kilobytes_to_Bytes(uint64 kilobytes)
    {
        return kilobytes * 1024ull;
    }

    export
    constexpr
    uint64 Megabytes_to_Bytes(uint64 megabytes)
    {
        return megabytes * (1024ull * 1024ull);
    }

    export
    constexpr
    uint64 Gigabytes_to_Bytes(uint64 gigabytes)
    {
        return gigabytes * (1024ull * 1024ull * 1024ull);
    }

    export
    constexpr
    uint64 Terabytes_to_Bytes(uint64 terabytes)
    {
        return terabytes * (1024ull * 1024ull * 1024ull * 1024ull);
    }

    export
    constexpr
    uint64 operator""_KiB(uint64 kilobytes)
    {
        return Kilobytes_to_Bytes(kilobytes);
    }

    export
    constexpr
    uint64 operator""_MiB(uint64 megabytes)
    {
        return Megabytes_to_Bytes(megabytes);
    }

    export
    constexpr
    uint64 operator""_GiB(uint64 gigabytes)
    {
        return Gigabytes_to_Bytes(gigabytes);
    }

    export
    constexpr
    uint64 operator""_TiB(uint64 terabytes)
    {
        return Terabytes_to_Bytes(terabytes);
    }

    export
    constexpr
    uint64 Bytes_to_Kilobytes(uint64 amount)
    {
        return amount / 1_KiB;
    }

    export
    constexpr
    uint64 Bytes_to_Megabytes(uint64 amount)
    {
        return amount / 1_MiB;
    }

    export
    constexpr
    uint64 Bytes_to_Gigabytes(uint64 amount)
    {
        return amount / 1_GiB;
    }

    export
    constexpr
    uint64 Bytes_to_Terabytes(uint64 amount)
    {
        return amount / 1_TiB;
    }

    export
    string Bytes_to_String(uint64 byteCount)
    {
        constexpr
        struct
        {
            char const*const Suffix;
            uint64 const Bytes;
        } 
        k_Suffixes[] =
        {
            {"TiB", 1_TiB},
            {"GiB", 1_GiB},
            {"MiB", 1_MiB},
            {"KiB", 1_KiB},
            {"B", 1ull}
        };

        for (auto const& suffixSize : k_Suffixes)
        {
            if (byteCount >= suffixSize.Bytes)
            {
                ostringstream stringStream;
                uint64 numOf = byteCount / suffixSize.Bytes;
                uint64 remainder = byteCount % suffixSize.Bytes;
                stringStream << numOf;
                if (remainder > 0)
                {
                    double const remainingFrac = static_cast<double>(remainder) / static_cast<double>(suffixSize.Bytes);
                    // *100 gives us the first two decimal points from remainingFrac.
                    // *1000 would give us three.
                    uint64 const decPointsToPrint = static_cast<uint64>(remainingFrac * 100.0);

                    stringStream << ".";
                    stringStream << decPointsToPrint;
                }

                stringStream << ' ' << suffixSize.Suffix;
                return stringStream.str();
            }
        }

        return "unknown";
    }

}