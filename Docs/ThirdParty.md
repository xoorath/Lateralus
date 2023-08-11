This page outlines our third party dependencies.
# Sharpmake
Categories: Dev dependency, non-shipping
License: [Apache 2.0](https://github.com/ubisoft/Sharpmake/blob/main/LICENSE.md) [tldrlegal for Apache 2.0](https://www.tldrlegal.com/license/apache-license-2-0-apache-2-0)
Homepage: [github.com/ubisoft/sharpmake](https://github.com/ubisoft/Sharpmake)
Integration: Submodule in the ThirdParty directory
Modifications: None

# Dear ImGui
Categories: Dev dependency, non-shipping
License: [MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt) [tldrlegal for MIT](https://www.tldrlegal.com/license/mit-license)
Homepage: [github.com/ocornut/imgui](https://github.com/ocornut/imgui)
Integration: Submodule in the ThirdParty directory
Modifications: None

# Conan 
Categories: Dev dependency, non-shipping
License: [MIT](https://github.com/conan-io/conan/blob/release/2.0/LICENSE.md) [tldrlegal for MIT](https://www.tldrlegal.com/license/mit-license)
Homepage: [conan.io](https://conan.io) [github.com/conan-io/conan](https://github.com/conan-io/conan)
Integration: External (developers are required to install)
Modifications: None

# GLEW
Categories: Platform, Shipping
License: [Zlib-libpng](https://www.glfw.org/license.html) [tldrlegal for Zlib-libpng](https://www.tldrlegal.com/license/zlib-libpng-license-zlib)
Homepage: [glfw.org](https://www.glfw.org/) [conan.io/center/recepies/glfw](https://conan.io/center/recipes/glfw?version=3.3.8) 
Integration: Included in conanfile.txt and referenced by `BuildSystem/Conan/Conan.ThirdParty.Sharpmake.cs`