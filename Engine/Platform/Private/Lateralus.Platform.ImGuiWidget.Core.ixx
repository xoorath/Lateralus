module;
#if ENABLE_IMGUI
#include <imgui.h>
#endif
export module Lateralus.Platform.ImGuiWidget.Core;
#if ENABLE_IMGUI

#if PLATFORM_IS_AMD64 || PLATFORM_IS_X86
import Lateralus.Core.CPUID;
#endif
import Lateralus.Core.SIMDSupport;

namespace Lateralus::Platform::ImGuiWidget
{

export void Core()
{
    ::ImGui::Begin("Lateralus Core");

#if PLATFORM_IS_AMD64 || PLATFORM_IS_X86
    static Core::CPUID const cpuid;
    ::ImGui::LabelText("CPU", "%s", cpuid.ProcessorBrand.data());
    ::ImGui::LabelText("Code SSE", "%s", ::Lateralus::Core::SSEVersionName);
    ::ImGui::LabelText("CPU SSE", "%s", Core::SSEGetName(cpuid.GetSSEVersion()));
#endif;

    ::ImGui::End();
}

} // namespace Lateralus::Platform::ImGuiWidget
#endif