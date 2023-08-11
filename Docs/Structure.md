# Project Types

## Core
Engine core is included by every other engine library and every application. It is the top most dependency of everything and is intended to be fast to compile and slim.

## Platform
The platform layer contains windowing and platform interaction code with abstractions to prevent leaking details to other libraries.

The code for window creation, system level input handling and imgui bindings live here.

## Utilities

Utility projects live in [../Tools/Utilities](../Tools/Utilities) as stand alone applications that don't implicitly get all the same dependencies as engine and application projects.

The idea is that these are individual programs that have very few responsibilities and dependencies and are not held to the same standard of robustness and architecture as engine projects. One example is [FontToSource](../Tools/Utilities/FontToSource) which takes a .TTF font as input and outputs a .cpp or .ixx file with that data.

## Third Party

Third party libraries and tools are used inconsistently depending on the need and how they need to integrate.

For example: SharpMake is pulled in as a git submodule into /ThirdParty/SharpMake and built & invoked using powershell scripts in Tools/. Many runtime third party libraries utilize conan whereas ImGui is pulled in as a submodule and we generate it a buildable project in the Lateralus solution using sharpmake. 

Each new usecase should try to conform to patterns that exist when possible (submodlues or conan).

# Modules and namespaces

There is a pattern to how modules and namespaces are used in the engine. Take the following example:

`Lateralus.Platform.Input.ixx`
```
export module Lateralus.Platform.Input;

namespace Lateralus::Platform::Input
{
    // ...
    export
    enum class KeyAction
    {
        Release,
        Press,
        Repeat
    };
    // ...
}
```

In the above example the file name, module scope and namespaces match. The pattern used is: `<engine/app name>::<project name>::<module name>`. 

The namespace may be excluded if a singular module-named export is used. For example:

`Lateralus.Core.CPUID.ixx`
```
export module Lateralus.Core.CPUID;

namespace Lateralus::Core
{
    export struct CPUID
    {
        //...
    };
}
```

# Defines

The following defines are available with some value in every configuration.

| Define              | Value                                                 |
| ------------------- |------------------------------------------------------ |
| CONF_DEBUG          | 1 in the **debug** configuration, otherwise 0.        |
| CONF_RELEASE        | Like *CONF_DEBUG* for the **release** configuration.  |
| CONF_RETAIL         | Like *CONF_DEBUG* for the **retail** configuration.   |
| PLATFORM_WIN32      | 1 on win32, otherwise 0. (target unsupported)         |
| PLATFORM_WIN64      | 1 on win64, otherwise 0.                              |
| PLATFORM_MAC        | 1 on mac, otherwise 0. (target unsupported)           |
| PLATFORM_LINUX      | 1 on linux, otherwise 0. (target unsupported)         |
| CONF_IS_OPTIMIZED   | 1 in release and retail, otherwise 0.                 |
| PLATFORM_IS_WINDOWS | 1 on win32 and win64, otherwise 0.                    |
| PLATFORM_IS_DESKTOP | 1 on windows, mac and linux, otherwise 0.             |
| ENABLE_GLFW         | Same value as PLATFORM_IS_DESKTOP.                    |
| ENABLE_IMGUI        | 1 in all configurations except retail. 0 in retail.   |
| SPDLOG_ACTIVE_LEVEL | Always set to 0.                                      |

The following are context sensitive defines that are not always defined.

| Define           | Value |
| ---------------- |------ |
| IMGUI_USE_WCHAR32 | 1 when ENABLE_IMGUI is 1. |