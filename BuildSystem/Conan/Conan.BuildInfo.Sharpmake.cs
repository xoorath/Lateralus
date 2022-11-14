using System.Collections.Generic;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Lateralus
{
    public class ConanBuildInfo
    {
        public ConanDepsEnvInfo deps_env_info { get; set; }
        public List<ConanDependency> dependencies { get; set; }
        public ConanSettings settings { get; set; }

        // Below settings are specific to the libraries we depend on.
        // This information can be restored if we need it
        // If we do: we should restore them with an abstraction like a dictionary or interface + type registry.

        //public DepsUserInfo deps_user_info { get; set; }
        //public Options options { get; set; }

        public static ConanBuildInfo FromBuildInfoJsonFileName(string filename)
        {
            string conanFileText = System.IO.File.ReadAllText(filename);
            ConanBuildInfo root = JsonSerializer.Deserialize<ConanBuildInfo>(conanFileText);
            return root;
        }

    }

    public class ConanDepsEnvInfo
    {
        public List<string> PATH { get; set; }
    }

    public class ConanDependency
    {
        public string name { get; set; }
        public string version { get; set; }
        public string description { get; set; }
        public string rootpath { get; set; }
        public string sysroot { get; set; }
        public List<string> include_paths { get; set; }
        public List<string> lib_paths { get; set; }
        public List<string> bin_paths { get; set; }
        public List<string> build_paths { get; set; }
        public List<string> res_paths { get; set; }
        public List<string> libs { get; set; }
        public List<string> system_libs { get; set; }
        public List<string> defines { get; set; }
        public List<object> cflags { get; set; }
        public List<object> cxxflags { get; set; }
        public List<object> sharedlinkflags { get; set; }
        public List<object> exelinkflags { get; set; }
        public List<object> frameworks { get; set; }
        public List<object> framework_paths { get; set; }
        internal ConanNames names { get; set; }
        internal ConanFilenames filenames { get; set; }
        internal ConanBuildModules build_modules { get; set; }
        internal ConanBuildModulesPaths build_modules_paths { get; set; }
        public List<object> cppflags { get; set; }

        public override string ToString() => name;
    }

    public class ConanSettings
    {
        public string arch { get; set; }
        public string arch_build { get; set; }
        public string build_type { get; set; }
        public string compiler { get; set; }

        [JsonPropertyName("compiler.runtime")]
        public string compilerruntime { get; set; }

        [JsonPropertyName("compiler.version")]
        public string compilerversion { get; set; }
        public string os { get; set; }
        public string os_build { get; set; }
    }

    // Root myDeserializedClass = JsonConvert.DeserializeObject<Root>(myJsonResponse);
    internal class ConanBuildModules
    {
        public List<string> cmake_find_package { get; set; }
        public List<string> cmake_find_package_multi { get; set; }
    }

    internal class ConanBuildModulesPaths
    {
        public List<string> cmake_find_package { get; set; }
        public List<string> cmake_find_package_multi { get; set; }
    }


    internal class ConanFilenames
    {
        public string cmake_find_package { get; set; }
        public string cmake_find_package_multi { get; set; }
    }

    internal class ConanNames
    {
        public string cmake_find_package { get; set; }
        public string cmake_find_package_multi { get; set; }
        public string pkg_config { get; set; }
    }
}