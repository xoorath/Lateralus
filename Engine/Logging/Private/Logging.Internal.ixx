export module Lateralus.Logging.Internal;

import <iostream>;
import <string_view>;
import Lateralus.Core;

using namespace std;
using namespace std::string_view_literals;

namespace Lateralus::Logging
{
    constexpr
    string_view k_DirectorySeparator = "/\\"sv;

    constexpr
    string_view GetSourcePath() 
    {
        return string_view(__FILE__);
    }

    constexpr
    string_view GetSourceDir()
    {
        return GetSourcePath().substr(0, GetSourcePath().find_last_of(k_DirectorySeparator));
    }

    constexpr
    string_view GetOneDirBack(string_view directory)
    {
        return directory.substr(0, directory.find_last_of(k_DirectorySeparator));
    }

    constexpr
    string_view GetLateralusDir()
    {
        return GetOneDirBack(GetOneDirBack(GetOneDirBack(GetSourceDir())));
    }

    constexpr
    string_view k_LateralusDir = GetLateralusDir();

    export
    constexpr
    string_view PathRelativeToLateralus(string_view path)
    {
        if(path.starts_with(k_LateralusDir))
        {
            constexpr usz s = k_LateralusDir.size();
            return path.substr(s != string_view::npos ? s + 1 : string_view::npos);
        }
        return path;
    }
}
