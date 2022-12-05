# Project Types

## Core
Engine core is included by every other engine library and every application. It is the top most dependancy of everything and is intended to be fast to compile and slim.

## Platform
The platform layer contains windowing and platform interaction code with abstractions to prevent leaking details to other libraries.

The code for window creation, system level input handling and imgui bindings live here.

## Utilities

Utility projects live in [../Tools/Utilities](../Tools/Utilities) as stand alone applications that don't implicitly get all the same dependancies as engine and application projects.

The idea is that these are individual programs that have very few responsibilities and dependancies and are not held to the same standard of robustness and architecture as engine projects. One example is [FontToSource](../Tools/Utilities/FontToSource) which takes a .TTF font as input and outputs a .cpp or .ixx file with that data.

# Modules and namespaces

There is a pattern to how modules and namespaces are used in the engine. Take the following example:

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

In the above example the module scope and namespaces match. The pattern used is: `<engine name>::<project name>::<file name>`. For the above example we can deduce that this file is `Input.ixx` in the `Platform` project in the engine folder.

An exception is made for files that exist as variants of another type. For example there is a `GLFW.Window` and a `NULL.Window`. It's completely valid to group these together as window types (ie: [`Window.GLFW`, `Window.NULL`]). It is just as valid however to group the other way around such as [`GLFW.Window`, `GLFW.Input`] which may help keep related implementation details grouped closer when files are alphabetically sorted.

# Defines

The following defines are available with some value in every configuration.

| Define           | Value |
| ---------------- |------ |
| CONF_DEBUG       | 1 in the **debug** configuration, otherwise 0. |
| CONF_RELEASE     | Like *CONF_DEBUG* for the **release** configuration. |
| CONF_RETAIL      | Like *CONF_DEBUG* for the **retail** configuration. |
| PLATFORM_DESKTOP | 1 in desktop configurations (ie: win64), otherwise 0. |
| PLATFORM_WIN64   | 1 in win64, otherwise 0. |
| USE_GLFW_WINDOW  | Same value as PLATFORM_DESKTOP. |
| IMGUI_SUPPORT    | 1 in all configurations except retail. 0 in retail. |
| SPDLOG_ACTIVE_LEVEL | Always set to 1. |

The following are context sensitive defines that are not always defined.

| Define           | Value |
| ---------------- |------ |
| IMGUI_USE_WCHAR32 | 1 when IMGUI_SUPPORT is 1. |