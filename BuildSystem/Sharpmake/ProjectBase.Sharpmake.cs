using Sharpmake;
using System.Diagnostics;
using System.Reflection;
using System.IO;
using System;

using Optimization = Sharpmake.Optimization;

namespace Lateralus
{
    public abstract class LateralusProjectBase : Project
    {
        public abstract string ProjectName { get; }

        public LateralusProjectBase(Optimization? exclude = null)
        {
            Optimization optimizationTargets = Optimization.Debug | Optimization.Release | Optimization.Retail;
            if(exclude.HasValue)
            {
                optimizationTargets &= ~exclude.Value;
            }
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2022,
                optimizationTargets
            ));

            // Add module extensions 
            SourceFilesExtensions.Add(".ixx");
            SourceFilesCompileExtensions.Add(".ixx");

            // Add sharpmake extension to the project (for quick editing: not used in compilation).
            SourceFilesExtensions.Add(".cs");

            // Add asset file types:
            SourceFilesExtensions.AddRange(new[] { 
                // todo: add other asset types we might want to see in visual studio.
                ".fs", 
                ".vs",
                ".png"
            });
        }

        [Configure()]
        public virtual void ConfigureAll(Configuration conf, Target target)
        {
            conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
            conf.ProjectPath = $@"{GetLateralusRootDirectory()}\generated\[project.Name]_[target.DevEnv]_[target.Platform]";

            if (target.DevEnv.HasAnyFlag(DevEnv.VisualStudio))
            {
                ConfigureVisualStudio(conf, target);
            }

            {
                Func<bool, int> btoi = (bool b) => b ? 1 : 0;
                bool isX86 = target.Platform.HasFlag(Platform.win32);
                bool isAMD64 = 
                    target.Platform.HasFlag(Platform.win64) 
                    || target.Platform.HasFlag(Platform.mac)
                    || target.Platform.HasFlag(Platform.linux);

                bool isDesktop = isX86 || isAMD64;

                conf.Defines.Add(new[] {
                    // Config Specific
                    $@"CONF_DEBUG={btoi(target.Optimization.HasFlag(Optimization.Debug))}",
                    $@"CONF_RELEASE={btoi(target.Optimization.HasFlag(Optimization.Release))}",
                    $@"CONF_RETAIL={btoi(target.Optimization.HasFlag(Optimization.Retail))}",

                    // Config Optimization
                    $@"CONF_IS_OPTIMIZED={btoi(target.Optimization.HasAnyFlag(Optimization.Release|Optimization.Retail))}",
                    
                    //Platform specific
                    $@"PLATFORM_WIN32={btoi(target.Platform.HasFlag(Platform.win32))}",
                    $@"PLATFORM_WIN64={btoi(target.Platform.HasFlag(Platform.win64))}",
                    $@"PLATFORM_LINUX={btoi(target.Platform.HasFlag(Platform.linux))}",
                    $@"PLATFORM_MAC={btoi(target.Platform.HasFlag(Platform.mac))}",          

                    // Platform category
                    $@"PLATFORM_IS_DESKTOP={btoi(isDesktop)}",
                    $@"PLATFORM_IS_WINDOWS={btoi(target.Platform.HasAnyFlag(Platform.win32|Platform.win64))}",
                    
                    // Platform architecture
                    $@"PLATFORM_IS_X86={btoi(isX86)}",
                    $@"PLATFORM_IS_AMD64={btoi(isAMD64)}",
                });
            }
        }

        private void ConfigureVisualStudio(Configuration conf, Target target)
        {
            conf.Defines.Add(new[] {
                    "_CRT_SECURE_NO_WARNINGS=1",
                    "NOMINMAX=1",
                    "WIN32_LEAN_AND_MEAN=1"
                });

            conf.Options.AddRange(new object[] {
                Options.Vc.General.WarningLevel.Level2,
                Options.Vc.General.TreatWarningsAsErrors.Enable,
                Options.Vc.General.CharacterSet.Unicode,

                Options.Vc.Compiler.CppLanguageStandard.CPP20,
                Options.Vc.Compiler.Exceptions.Enable,
                Options.Vc.Compiler.RTTI.Disable,

                new Options.Vc.Linker.IgnoreSpecificLibraryNames("LIBCMT"),
                new Options.Vc.Linker.DisableSpecificWarnings(
                    "4006" // https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-warning-lnk4006?view=msvc-170
                ),
                new Options.Vc.Librarian.DisableSpecificWarnings(
                    "4006" // https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-warning-lnk4006?view=msvc-170
                )
            });

            if (target.Platform.HasFlag(Platform.win64))
            {
                // This define is intended to allow us to include files that depend on winnt
                // without including all of windows.h (in some cases). Example: debugapi.h
                conf.Defines.Add($@"_AMD64_");
            }
        }

        private static string GetLateralusRootDirectory()
        {
            const int depth = 1;
            StackTrace stackTrace = new StackTrace(true);
            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(LateralusProjectBase))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);

                    return Path.GetFullPath(Path.Combine(new FileInfo(stackFrame.GetFileName()).DirectoryName, "..", ".."));
                }
            }
            throw new LateralusError("error in Lateralus.LateralusProjectBase.GetLateralusRootDirectory()");
        }
    }
}
