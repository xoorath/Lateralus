#if ENABLE_IMGUI
export module Lateralus.Platform.ImGui.Impl;

import Lateralus.Platform.Error;
import <optional>;

using namespace std;

namespace Lateralus::Platform::ImGui
{
export class iImpl
{
public:
    virtual ~iImpl() = default;

    virtual optional<Error> Init() = 0;
    virtual void Shutdown() = 0;
    virtual void NewFrame() {}
    virtual void Render() {}
};
} // namespace Lateralus::Platform::ImGui
#endif