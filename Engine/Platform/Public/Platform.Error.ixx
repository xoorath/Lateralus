module;

#include <Core.h>

export module Lateralus.Platform.Error;

import <optional>;
import <string>;
import <string_view>;

using namespace std;

namespace Lateralus::Platform
{
    export 
    struct Error
    {
        constexpr
        Error() = default;

        constexpr
        Error(string_view message)
            : m_ErrorMessage(message)
        {
        }

        ENCAPSULATE(string, ErrorMessage);
    };

    export const optional<Error> Success;
}