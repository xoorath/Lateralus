export module Lateralus.Platform;

import <memory>;
import <optional>;

import Lateralus.Platform.iPlatform;
import Lateralus.Platform.iWindow;

#if PLATFORM_WIN64
import Lateralus.Platform.Windows.Platform;
import Lateralus.Platform.GLFW.Window;
#else
import Lateralus.Platform.Null.Platform;
import Lateralus.Platform.Null.Window;
#endif

using namespace std;

namespace Lateralus::Platform
{
    export
    shared_ptr<iPlatform> CreatePlatform()
    {
#if PLATFORM_WIN64 || PLATFORM_WIN32
        return make_shared<Windows::Platform>();
#else
        return make_shared<Null::Platform>();
#endif
    }

    export
    shared_ptr<iWindow> CreateWindow()
    {
#if USE_GLFW_WINDOW
        return make_shared<GLFW::Window>();
#else
        return make_shared<Null::Window>();
#endif
    }
}