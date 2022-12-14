export module Lateralus.Platform.Window.Null;

import Lateralus.Platform.Error;
import Lateralus.Platform.Window;
import <optional>;

using namespace std;

namespace Lateralus::Platform::Null
{
export class Window : public iWindow
{
    optional<Error> Create(WindowCreateContext const &ctx) override
    {
        return Error("Null::Window::Create");
    }

    bool ShouldClose() const override { return true; };

    void PollEvents() override {}

    void Clear() override {}

    void NewFrame() override {}

    void Render() override {}

    void SwapBuffers() override {}
};
} // namespace Lateralus::Platform::Null
