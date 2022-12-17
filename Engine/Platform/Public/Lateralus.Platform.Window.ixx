module;

#include <Core.h>

export module Lateralus.Platform.Window;

import Lateralus.Core;
import Lateralus.Platform.Error;
import Lateralus.Platform.Platform;
import <memory>;
import <optional>;
import <string_view>;

using namespace std;
using namespace Lateralus::Core;

namespace Lateralus::Platform
{
struct WindowCreateContext;

export class iWindow
{
public:
    virtual ~iWindow() {}

    virtual optional<Error> Create(WindowCreateContext const &ctx) = 0;

    // Returns true if the window has been requested to close (ie: hitting a close button on PC)
    virtual bool ShouldClose() const = 0;

    // Poll events periodically (once per frame is typical)
    virtual void PollEvents() = 0;

    // Call once to begin a new frame.
    virtual void NewFrame() = 0;

    // Clear once per frame before rendering anything
    virtual void Clear() = 0;

    // Render once per frame before swapping buffers.
    virtual void Render() = 0;

    // Swap buffers to end the render frame.
    virtual void SwapBuffers() = 0;
};

export struct WindowCreateContext
{
    WindowCreateContext() = delete;
    WindowCreateContext(WindowCreateContext const &) = delete;
    WindowCreateContext(WindowCreateContext &&) = delete;

    WindowCreateContext(shared_ptr<iPlatform> platform) : m_Platform(platform) {}

    // (required) Current platform object. See: Lateralus::Platform::CreatePlatform
    ENCAPSULATE_RO_V(shared_ptr<iPlatform>, Platform);

    // Window/buffer dimensions
    ENCAPSULATE_O(uint32, Width, 1920);
    ENCAPSULATE_O(uint32, Height, 1080);

    // Window title
    ENCAPSULATE_O(string, Title, "Lateralus");
};
} // namespace Lateralus::Platform