module;
#if IMGUI_SUPPORT
#include <imgui.h>
#endif
export module Lateralus.Platform.ImGuiWidget.Core;
#if IMGUI_SUPPORT
import Lateralus.Core.CPUID;
import Lateralus.Core.SIMDSupport;


namespace Lateralus::Platform::ImGuiWidget
{

export void Core()
{
    static Core::CPUID const cpuid;
    ::ImGui::Begin("Lateralus Core");
    ::ImGui::LabelText("CPU", "%s", cpuid.ProcessorBrand.data());
    ::ImGui::LabelText("CPU SSE", "%s", Core::SSEGetName(cpuid.GetSSEVersion()));
    ::ImGui::LabelText("Code SSE", "%s", ::Lateralus::Core::SSEVersionName);
    ::ImGui::End();
}

} // namespace Lateralus::Platform::ImGuiWidget
#endif