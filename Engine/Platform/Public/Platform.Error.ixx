export module Lateralus.Platform.Error;

import <string_view>;

using namespace std;

namespace Lateralus::Platform
{
    export 
    struct Error
    {
        constexpr
        Error(string_view message)
            : ErrorMessage(message)
        {
        }

        string_view const ErrorMessage = nullptr;
    };
}