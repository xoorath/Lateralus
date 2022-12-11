#if IMGUI_SUPPORT
export module Lateralus.Platform.Imgui.iImpl;

import Lateralus.Platform.Error;
import <optional>;

using namespace std;

namespace Lateralus::Platform::Imgui
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
} // namespace Lateralus::Platform::Imgui
#endif