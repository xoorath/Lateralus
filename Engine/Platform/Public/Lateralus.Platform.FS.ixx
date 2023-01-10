module;

#if PLATFORM_WIN64
// A lot of this code is just to cope with a macro redefinition bug:
// https://developercommunity.visualstudio.com/t/warning-C4005:-Outptr:-macro-redefinit/1546919 Each
// define or undefine below marked with [hack] should be removed when the above is fixed.
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0 // [#hack]
#define __SPECSTRINGS_STRICT_LEVEL 0                                         // [#hack]
#undef APIENTRY
#include <windows.h>
#undef __nullnullterminated // [#hack]
#endif

export module Lateralus.Platform.FS;

import <filesystem>;
import <format>;
import <fstream>;
import <istream>;
import <optional>;

import Lateralus.Core;
import Lateralus.Core.StringUtils;
import Lateralus.Core.EncodingConversion;
import Lateralus.Platform.Error;

namespace fs = std::filesystem;
using namespace std;
using namespace std::string_view_literals;
using namespace Lateralus::Core;
using namespace Lateralus::Core::StringUtils;
using namespace Lateralus::Core::EncodingConversion;

namespace Lateralus::Platform::FS
{

/// <summary>
/// Special locations that can be refered to when building a path
/// see: GetLocationPath
/// see: ResolveSpecialPath
/// </summary>
export enum class Location {
    // The install location of the application
    App,
    // The current working directoty
    CWD,
    // A temporary location suitable for writing files
    Temp
};

optional<Error> GetLocationPath(Location location, fs::path &pathOut)
{
    switch (location)
    {
    default:
    case Location::CWD: pathOut = fs::weakly_canonical(fs::path(".")); return Success;
    case Location::Temp: pathOut = fs::temp_directory_path(); return Success;
#if PLATFORM_WIN64
    case Location::App: {
        thread_local optional<fs::path> appPath;
        if (!appPath.has_value())
        {
            wchar_t buffer[MAX_PATH] = {0};
            // result of 0 is an error
            // result of nSize is an insufficient buffer size which we consider an error
            // any other result is the size of the path including the null terminator
            DWORD result = GetModuleFileName(nullptr, buffer, MAX_PATH);
            if (result == 0)
            {
                DWORD const err = GetLastError();
                return Error(
                    format("Unknown error getting application path. Windows error code: {}", err));
            }
            else if (result == MAX_PATH)
            {
                DWORD const err = GetLastError();
                if (err == ERROR_INSUFFICIENT_BUFFER)
                {
                    return Error("Application path is too long (longer than MAX_PATH)");
                }
                return Error(
                    format("Unknown error getting application path. Windows error code: {}", err));
            }
            pathOut = fs::path(buffer);
        }
        pathOut = appPath.value();
        return Success;
    }
#elif PLATFORM_LINUX
    case Location::App: {
        pathOut = fs::weakly_canonical("/proc/self/exe");
        return Success;
    }
#endif
    }
}

constexpr char8_t k_SpecialPathTag_App[] = u8"{app}";
constexpr char8_t k_SpecialPathTag_CWD[] = u8"{cwd}";
constexpr char8_t k_SpecialPathTag_Temp[] = u8"{temp}";

optional<Error> ResolveSpecialPath(u8string_view specialPath, fs::path &pathOut)
{
    auto split = SplitStringView(specialPath, u8"/\\"sv);
    // a container to hold temp strings for the lifetime of the func
    // this allows us to simply replace the special string views inside of split
    vector<u8string> specialPathParts;
    specialPathParts.reserve(split.size());

    constexpr struct
    {
        char8_t const *tag;
        Location loc;
    } k_ValidStartingLocations[] = {
        {k_SpecialPathTag_App, Location::App},
        {k_SpecialPathTag_CWD, Location::CWD},
        {k_SpecialPathTag_Temp, Location::Temp},
    };

    for (usz i = 0; i < split.size(); ++i)
    {
        auto &part = split[i];
        if (i == 0)
        {
            bool isStartingTag = false;
            for (auto const &validStartingLocation : k_ValidStartingLocations)
            {
                if (CaseInsensitiveCompare(validStartingLocation.tag, part))
                {
                    isStartingTag = true;
                    fs::path locPath;
                    if(auto err = GetLocationPath(validStartingLocation.loc, locPath); err.has_value())
                    {
                        return err;
                    }
                    specialPathParts.push_back(locPath.u8string());
                    break;
                }
            }

            if (!isStartingTag)
            {
                specialPathParts.push_back(u8string(part));
            }
        }
        else
        {
            for (auto const &validStartingLocation : k_ValidStartingLocations)
            {
                if (CaseInsensitiveCompare(validStartingLocation.tag, part))
                {
                    return Error(
                        format("Special path tag {} is only a valid as the first part of a path.",
                                string_cast<string>(k_SpecialPathTag_App)));
                }
            }
            specialPathParts.push_back(u8string(part));
        }

        // pathOut += fs::path(u8string_to_string(part));
    }

    // specialPathParts
    //
    //
    usz finalLength = 0;
    for (auto const &part : specialPathParts)
    {
        // +1 for the delim
        finalLength = part.size() + 1;
    }
    u8string combinedStr;
    combinedStr.reserve(finalLength);
    for (usz i = 0; i < specialPathParts.size(); ++i)
    {
        combinedStr += specialPathParts[i];
        if (i != specialPathParts.size() - 1)
        {
            combinedStr += u8"/";
        }
    }

    pathOut = fs::path(combinedStr);

    //   return fs::path(specialPath);
    return Success;
}
//
//optional<Error> OpenFileRead(Location relativeTo, fs::path const &path, OpenedFile *outOpenedFile)
//{
//    if (outOpenedFile == nullptr)
//    {
//        return Error("outOpenedFile is required");
//    }
//
//    outOpenedFile->AbsolutePath = fs::absolute(GetLocationPath(relativeTo) / path);
//
//    outOpenedFile->Stream = ifstream(outOpenedFile->AbsolutePath, ios_base::in | ios_base::binary);
//
//    outOpenedFile->Stream.exceptions(ifstream::failbit | ifstream::badbit);
//
//    try
//    {
//        // fsize = file.tellg();
//        outOpenedFile->Stream.seekg(0, std::ios::end);
//        outOpenedFile->Length = outOpenedFile->Stream.tellg();
//        outOpenedFile->Stream.seekg(0, std::ios::beg);
//    }
//    catch (ifstream::failure e)
//    {
//        return Error(format("Error opening file ({}): \"{}\"", e.code().message(), e.what()));
//    }
//
//    return Success;
//}

} // namespace Lateralus::Platform::FS