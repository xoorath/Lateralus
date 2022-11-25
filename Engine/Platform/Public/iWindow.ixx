module;

#include <Core.h>

export module Lateralus.Platform.iWindow;

import Lateralus.Core;
import Lateralus.Platform.Error;
import Lateralus.Platform.iPlatform;
import <memory>;
import <optional>;
import <string_view>;

using namespace std;

namespace Lateralus::Platform
{
    struct WindowCreateContext;

    export
    class iWindow
    {
    public:
        virtual
        ~iWindow()
        {
        }

        virtual
        optional<Error> Create(WindowCreateContext const& ctx) = 0;

        virtual
        bool ShouldClose() const = 0;

        virtual
        void PollEvents() = 0;

        virtual
        void SwapBuffers() = 0;

        // For now we just provide the glfw window to bind with imgui
        // The future design should have a Lateralus::Platform layer imgui binding that depends on GLFW.Window directly.
        virtual
        void* HACK_GetGlfwWindow() { return nullptr; }

        virtual
        string GetGlslVersion() = 0;
    };

    export
    struct WindowCreateContext
    {
        WindowCreateContext() = delete;
        WindowCreateContext(WindowCreateContext const&) = delete;
        WindowCreateContext(WindowCreateContext&&) = delete;

        WindowCreateContext(shared_ptr<iPlatform> platform)
            : m_Platform(platform)
        {
        }

        // (required) Current platform object. See: Lateralus::Platform::CreatePlatform
        ENCAPSULATE_RO_V(shared_ptr<iPlatform>, Platform);

        // Window/buffer dimensions
        ENCAPSULATE_O(uint32, Width, 1920);
        ENCAPSULATE_O(uint32, Height, 1080);

        // Window title
        ENCAPSULATE_O(string, Title, "Lateralus");
    };
}