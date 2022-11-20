export module Lateralus.Platform;

import <optional>;

import Lateralus.Platform.Windows;
import Lateralus.Platform.Error;

using namespace std;

namespace Lateralus::Platform
{
    export optional<Error> Init()
    {
        return Windows::Init();
    }
}