using Sharpmake;
using System.Diagnostics;
using System.Reflection;
using System.IO;
using System;

namespace Lateralus
{
    public abstract class LateralusProjectBase : Project
    {
        public abstract string ProjectName { get; }

        public LateralusProjectBase()
        {
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2022,
                Optimization.Debug | Optimization.Release | Optimization.Retail
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

            Func<bool, int> btoi = (bool b) => b ? 1 : 0;

            conf.Options.AddRange(new object[] {
                Options.Vc.General.WarningLevel.Level2,
                Options.Vc.General.TreatWarningsAsErrors.Enable,
                Options.Vc.General.CharacterSet.Unicode,

                Options.Vc.Compiler.CppLanguageStandard.CPP20,
                Options.Vc.Compiler.Exceptions.Enable,
                Options.Vc.Compiler.RTTI.Disable
            });

            {
                bool isDesktop = 
                    target.Platform.HasFlag(Platform.win64) 
                    || target.Platform.HasFlag(Platform.win32) 
                    || target.Platform.HasFlag(Platform.mac)
                    || target.Platform.HasFlag(Platform.linux);

                conf.Defines.Add(new[] {
                    $@"CONF_DEBUG={btoi(target.Optimization.HasFlag(Optimization.Debug))}",
                    $@"CONF_RELEASE={btoi(target.Optimization.HasFlag(Optimization.Release))}",
                    $@"CONF_RETAIL={btoi(target.Optimization.HasFlag(Optimization.Retail))}",

                    $@"PLATFORM_DESKTOP={btoi(isDesktop)}",
                    $@"PLATFORM_WIN64={btoi(target.Platform.HasFlag(Platform.win64))}"
                });

            }

            conf.Defines.Add("SPDLOG_ACTIVE_LEVEL=0");
            ThirdParty.ReferenceExternal(conf, target, new[] { 
                ThirdParty.ExternalProject.spdlog,
                ThirdParty.ExternalProject.fmt
            });
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
