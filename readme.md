# Prerequisites

- The [conan](conan.io/downloads.html) package manager.
    - **Tested with 1.54.0**
- [Visual Studio 2022](https://visualstudio.microsoft.com/vs/)
    - **Tested with 17.3.6**
    - .NET desktop development
    - Desktop development with C++
- Powershell version 5 or newer
    - **Tested with 5.1.19041.1682**
- Cmake
    - **Tested with 3.24.3**
    - **Used to build some dependancies when packages can't be found**

Make sure conan and cmake are available at path by running:

- `conan --version`
- `cmake --version`

Each command should cleanly print the version without error. If they don't: check your installation.

# Building

Run `./Tools/Powershell/Sharpmake.ps1` in powershell. This will create a solution under `./generated`
Open the solution in visual studio and build.

# Conan

Lateralus uses conan to import packages and uses sharpmake to parse the conan output to link those packages into projects.

# Sharpmake

[Sharpmake](https://github.com/ubisoft/Sharpmake) is used to generate the solution and project files programmatically. This allows us to use C# to define what gets built and how across any target on any platform.

`BuildSystem/Sharpmake/` contains all of the project-wide sharpmake code such as base classes for new projects and configurations that apply widely across Lateralus.
`ThirdParty/Sharpmake/` contains sharpmake itself which is pulled using [git submodules](.gitmodules).

To generate the solution run `./Tools/Powershell/Sharpmake.ps1`.

[Sharpmake.ps1](./Tools/Powershell/Sharpmake.ps1) has a few optional parameters:

| Parameter | Description                                                                                                                                        |
|-----------|----------------------------------------------------------------------------------------------------------------------------------------------------|
| -Verbose  | Produce more detailed output from sharpmake.                                                                                                       |
| -Debug    | Build the debug variant of sharpmake. Also create a solution for debugging sharpmake generation at `/generated/sharpmake_debugsolution.vs2022.sln` |
| -Force    | Re-build sharpmake itself even if it already exists. (useful when updating the sharpmake module)                                                   |

## New Projects 
Each subdirectory of `/Engine` contains a sharpmake file defining that project such as [Engine/Core/Core.Sharpmake.cs](Engine/Core/Core.Sharpmake.cs).
Similarly each subdirectory of `/Applications` contains a sharpmake file defining that project such as [Applications/HelloWorld/HelloWorld.Sharpmake.cs](Applications/HelloWorld/HelloWorld.Sharpmake.cs).

Engine projects should derive [Lateralus.EngineProject](./BuildSystem/Sharpmake/LateralusProject.Sharpmake.cs). It's source code should exist in a `Public/` and `Private/` directory next to the project sharpmake file. Any modules and headers in Public will be accessable to projects that depend on that library. Any code in the private directory will only be accessable to that engine library itself.

Similarly new application projects should derive [Lateralus.ApplicationProject](./BuildSystem/Sharpmake/LateralusProject.Sharpmake.cs) and it's source code should exist in a `Src/` directory next to the project sharpmake file.

### New files

When files are added or removed from projects you need to re-run sharpmake.