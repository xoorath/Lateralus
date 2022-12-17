export module Lateralus.Platform;

import <memory>;
import <optional>;

import Lateralus.Platform.Platform;
#if PLATFORM_WIN64
import Lateralus.Platform.Platform.Windows;
#else
import Lateralus.Platform.Platform.Null;
#endif

import Lateralus.Platform.Window;
#if USE_GLFW_WINDOW
import Lateralus.Platform.Window.GLFW;
#else
import Lateralus.Platform.Window.Null;
#endif

using namespace std;

namespace Lateralus::Platform
{
export shared_ptr<iPlatform> CreatePlatform()
{
#if PLATFORM_WIN64 || PLATFORM_WIN32
    return make_shared<Windows::Platform>();
#else
    return make_shared<Null::Platform>();
#endif
}

export shared_ptr<iWindow> CreateWindow()
{
#if USE_GLFW_WINDOW
    return make_shared<GLFW::Window>();
#else
    return make_shared<Null::Window>();
#endif
}
} // namespace Lateralus::Platform