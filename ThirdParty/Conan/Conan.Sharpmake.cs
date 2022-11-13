using Sharpmake;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.Json;



namespace Lateralus
{
    class Conan
    {
        public static void Install(Configuration conf, Target target)
        {
            string lateralusRootDir = Path.GetFullPath(Path.Combine(
                Util.PathMakeStandard(Util.GetCurrentSharpmakeFileInfo().DirectoryName),
                "..", ".."
            ));

            string generatedPath = Path.GetFullPath(Path.Combine(
                lateralusRootDir,
                "generated"
            ));

            string conanGeneratedDir = Path.GetFullPath(Path.Combine(
                generatedPath,
                "conan",
                $@"{target.Platform.ToString()}_{target.DevEnv.ToString()}_{target.Optimization.ToString()}"
            ));

            string conanfileDir = Path.GetFullPath(Path.Combine(
                lateralusRootDir,
                "ThirdParty",
                "conan"
            ));

            const string conanBuildInfoFileName = "conanbuildinfo.json";
            string conanBuildInfoFilePath = Path.GetFullPath(Path.Combine(conanGeneratedDir, conanBuildInfoFileName));


            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = "conan";
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;

            startInfo.WorkingDirectory = conanGeneratedDir;
            startInfo.Arguments = $@"install {Path.GetRelativePath(conanGeneratedDir, conanfileDir)} --build=missing";

            foreach (SettingProvider settingProvider in new SettingProvider[] {
                GetConanSettingArch,
                GetConanSettingBuildType,
                GetConanSettingCompiler,
                GetConanSettingRuntime,
                // testing to see if this setting override is forcing compilation failure of our dependancies.
                //GetConanSettingCompilerVersion,
                GetConanSettingOS
            })
            {
                string value = settingProvider(conf, target);
                if (!string.IsNullOrWhiteSpace(value))
                {
                    startInfo.Arguments += $@" -s ""{value}""";
                }
            }

            if (!Directory.Exists(startInfo.WorkingDirectory))
            {
                Directory.CreateDirectory(startInfo.WorkingDirectory);
            }

            // Start the process with the info we specified.
            // Call WaitForExit and then the using statement will close.
            using (Process exeProcess = Process.Start(startInfo))
            {
                exeProcess.WaitForExit();
                if (exeProcess.ExitCode != 0)
                {
                    throw new LateralusError("Conan reported errors while running install");
                }
            }

            if(!File.Exists(conanBuildInfoFilePath))
            {
                throw new LateralusError($@"Conan didn't produce a {conanBuildInfoFileName}");
            }

            

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

        private static string GetConanSettingCompilerVersion(Configuration conf, Target target)
        {
            const string var = "compiler.version";
            switch(target.DevEnv)
            {
                case DevEnv.vs2019: return $@"{var}=15";
                case DevEnv.vs2022: return $@"{var}=16";
            }
            throw new LateralusError($@"Sharpmake devenv '{target.DevEnv.ToString()}' not yet mapped to conan {var} for target {target.Name}.");
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