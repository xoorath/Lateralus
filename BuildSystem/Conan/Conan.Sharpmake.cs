using Sharpmake;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;



namespace Lateralus
{
    static class Conan
    {
        // Shared state needs to be thread safe.
        private static object s_Lock = new object();
        private static Dictionary<string, ConanBuildInfo> s_BuildInfoCache = new Dictionary<string, ConanBuildInfo>();

        public static void AddExternalDependencies(Project.Configuration conf, Target target, Project project, ConanDependencies dependencies)
        {
            string conanGeneratedDir, conanfilePath, conanBuildInfoFilePath;
            GetPaths(conf, target, project, 
                out conanGeneratedDir, 
                out conanfilePath,
                out conanBuildInfoFilePath);

            if (!Directory.Exists(conanGeneratedDir))
            {
                Directory.CreateDirectory(conanGeneratedDir);
            }

            ConanBuildInfo existingBuildInfo = null;

            string configText = dependencies.SerializeAsConanfile();
            lock (s_Lock)
            {
                s_BuildInfoCache.TryGetValue(configText, out existingBuildInfo);
            }
            if (existingBuildInfo == null)
            {
                File.WriteAllText(conanfilePath, configText);
            }

            if (existingBuildInfo == null)
            {
                // This is so we can restore the CWD after running.
                string cwd = Directory.GetCurrentDirectory();

                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.CreateNoWindow = false;
                startInfo.UseShellExecute = false;
                // Todo: ensure conan is installed and print a friendly message if it isn't.
                startInfo.FileName = "conan";
                startInfo.WindowStyle = ProcessWindowStyle.Hidden;

                startInfo.WorkingDirectory = conanGeneratedDir;
                startInfo.Arguments = $@"install ./ --build=missing{GetSettingsArguments(conf, target)}";

                // Start the process with the info we specified.
                // Call WaitForExit and then the using statement will close.
                try
                {
                    using (Process exeProcess = Process.Start(startInfo))
                    {
                        exeProcess.WaitForExit();
                        if (exeProcess.ExitCode != 0)
                        {
                            throw new LateralusError("Conan reported errors while running install");
                        }
                    }
                    if (!File.Exists(conanBuildInfoFilePath))
                    {
                        throw new LateralusError($@"Conan didn't produce a conan build info file at {conanBuildInfoFilePath}");
                    }
                }
                finally
                {
                    string changedcwd = Directory.GetCurrentDirectory();
                    Directory.SetCurrentDirectory(cwd);
                }
            }

            var buildInfo = existingBuildInfo == null ? ConanBuildInfo.FromBuildInfoJsonFileName(conanBuildInfoFilePath) : existingBuildInfo;

            if (existingBuildInfo == null)
            {
                lock (s_Lock)
                {
                    s_BuildInfoCache[configText] = buildInfo;
                }
            }

            foreach (ConanDependency dependencyInfo in buildInfo.dependencies)
            {
                conf.IncludePaths.AddRange(dependencyInfo.include_paths);
                conf.LibraryPaths.AddRange(dependencyInfo.lib_paths);

                if (conf.Output.Equals(Project.Configuration.OutputType.Lib))
                {
                    conf.DependenciesBuiltTargetsLibraryFiles.AddRange(dependencyInfo.libs);
                }
                else
                {
                    conf.LibraryFiles.AddRange(dependencyInfo.libs);
                }
                conf.Defines.AddRange(dependencyInfo.defines);
                conf.ExportDefines.AddRange(dependencyInfo.defines);
            }
        }

        private static void GetPaths(Project.Configuration conf, Target target, Project project,
            out string conanGeneratedDir,
            out string conanfilePath,
            out string conanBuildInfoFilePath)
        {
            string lateralusRootDir = Path.GetFullPath(Path.Combine(
                Util.PathMakeStandard(Util.GetCurrentSharpmakeFileInfo().DirectoryName),
                "..", ".."
            ));

            string generatedPath = Path.GetFullPath(Path.Combine(
                lateralusRootDir,
                "generated"
            ));

            conanGeneratedDir = Path.GetFullPath(Path.Combine(
                generatedPath,
                "conan",
                target.GetTargetString(),
                project.Name
            ));

            // The conan process will look for this file name in the working directory.
            // That implies this hard coded value needs to map to what conan expects.
            const string conanfileFileName = "conanfile.txt";
            conanfilePath = Path.Combine(conanGeneratedDir, conanfileFileName);

            // The conan process will write to this file name in the working directory.
            // That implies this hard coded value needs to map to what conan expects.
            const string conanBuildInfoFileName = "conanbuildinfo.json";
            conanBuildInfoFilePath = Path.GetFullPath(Path.Combine(conanGeneratedDir, conanBuildInfoFileName));
        }

        private static string GetSettingsArguments(Project.Configuration conf, Target target)
        {
            StringBuilder sb = new StringBuilder();
            foreach (SettingProvider settingProvider in new SettingProvider[] {
                GetConanSettingArch,
                GetConanSettingBuildType,
                GetConanSettingCompiler,
                GetConanSettingRuntime,
                GetConanSettingOS
            })
            {
                string value = settingProvider(conf, target);
                if (!string.IsNullOrWhiteSpace(value))
                {
                    sb.Append($@" -s """);
                    sb.Append(value);
                    sb.Append('"');
                }
            }
            return sb.ToString();
        }

        private delegate string SettingProvider(Configuration conf, Target target);
        private static string GetConanSettingArch(Configuration conf, Target target)
        {
            const string var = "arch";
            switch (target.Platform)
            {
                case Platform.win32: return $@"{var}=x86";
                case Platform.win64: return $@"{var}=x86_64";
                // See possible values at: https://docs.conan.io/en/latest/extending/custom_settings.html
                default: throw new LateralusError($@"Sharpmake platform not yet mapped to conan {var} for target {target.Name}.");
            };
        }

        private static string GetConanSettingBuildType(Configuration conf, Target target)
        {
            const string var = "compiler";
            if (target.Optimization.HasAnyFlag(Optimization.Debug))
            {
                return $@"{var}=Debug";
            }
            return $@"{var}=Release";
        }

        private static string GetConanSettingCompiler(Configuration conf, Target target)
        {
            const string var = "compiler";
            if (target.DevEnv.HasAnyFlag(DevEnv.VisualStudio))
            {
                return $@"{var}=Visual Studio";
            }
            throw new LateralusError($@"Sharpmake compiler not yet mapped to conan {var} for target {target.Name}.");
        }

        private static string GetConanSettingRuntime(Configuration conf, Target target)
        {
            const string var = "compiler.runtime";
            if (conf.Options.Contains(Options.Vc.Compiler.RuntimeLibrary.MultiThreaded))
            {
                return $@"{var}=MT";
            }
            else if (conf.Options.Contains(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebug))
            {
                return $@"{var}=MTd";
            }
            else if (conf.Options.Contains(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL))
            {
                return $@"{var}=MD";
            }
            else if (conf.Options.Contains(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL))
            {
                return $@"{var}=MDd";
            }
            else if(target.Optimization.HasFlag(Optimization.Debug))
            {
                return $@"{var}=MTd";
            }
            else
            {
                return $@"{var}=MT";
            }
        }

        private static string GetConanSettingOS(Configuration conf, Target target)
        {
            const string var = "os";
            switch (target.Platform)
            {
                case Platform.win32: return $@"{var}=Windows";
                case Platform.win64: return $@"{var}=Windows";
            }
            throw new LateralusError($@"Sharpmake platform '{target.Platform.ToString()}' not yet mapped to conan {var} for target {target.Name}.");
        }
    }
}