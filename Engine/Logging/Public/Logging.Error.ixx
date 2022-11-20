export module Lateralus.Logging.Error;

import <string_view>;
import <vector>;

using namespace std;

namespace Lateralus::Logging
{
    export
    struct Error
    {
        constexpr
        Error(string_view message)
            : ErrorMessage(message)
        {
        }

        string const ErrorMessage = nullptr;
    };
}