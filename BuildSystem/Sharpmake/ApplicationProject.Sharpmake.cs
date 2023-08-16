using Sharpmake;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

using Optimization = Lateralus.Optimization;
using Compiler = Lateralus.Compiler;

namespace Lateralus
{
    public abstract class ApplicationProject : LateralusProjectBase
    {
        private IReadOnlyCollection<string> m_ProjectSource => new[] {
            @"[project.SharpmakeCsPath]\Src"
        };

        public ApplicationProject()
            : base()
        {
            Name = ProjectName;

            // [project.SharpMakeCsPath] can't be used because it would identify this file's path.
            SourceRootPath = Util.PathMakeStandard(GetCurrentCallingFileInfo().DirectoryName);
        }

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);

            // Project configuration
            conf.IncludePaths.AddRange(m_ProjectSource);
            conf.Output = Configuration.OutputType.Exe;
            conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions);
            // Dependancy configuration
            conf.AddPublicDependency<CoreProject>(target);
            conf.AddPublicDependency<PlatformProject>(target);

            // Decorative configuration
            conf.SolutionFolder = "Applications";

            conf.EventPostBuild.AddRange(new[] {
                $@"if not exist ""$(OutDir)Assets"" mklink /j ""$(OutDir)Assets"" ""{Util.PathMakeStandard(GetCurrentCallingFileInfo().DirectoryName)}/Assets"""
            });

            conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings()
            {
                LocalDebuggerWorkingDirectory = "$(OutDir)"
            };

            {
                Func<bool, int> btoi = (bool b) => b ? 1 : 0;
                bool hasImgui = !target.Optimization.HasFlag(Optimization.Retail);
                var platform = target.GetPlatform();

                bool isDesktop =
                    platform.HasFlag(Platform.win64)
                    || platform.HasFlag(Platform.win32)
                    || platform.HasFlag(Platform.mac)
                    || platform.HasFlag(Platform.linux);

                conf.Defines.Add(new[] {
                    $@"ENABLE_GLFW={btoi(isDesktop)}"
                });

                if (hasImgui)
                {
                    base.AddImguiDependency(conf, target);
                }
                else
                {
                    conf.Defines.Add($@"ENABLE_IMGUI=0");
                }
            }

            conf.Defines.Add("SPDLOG_ACTIVE_LEVEL=0");
            Conan.AddExternalDependencies(conf, target, this, new ConanDependencies()
            {
                Requires = new[]
                {
                    "spdlog/[>=1.4.1]"
                }
            });
        }

        private FileInfo GetCurrentCallingFileInfo()
        {
            const int depth = 2;
            StackTrace stackTrace = new StackTrace(true);
            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(ApplicationProject))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);
                    string filename = stackFrame.GetFileName();
                    if (string.IsNullOrEmpty(filename))
                    {
                        throw new LateralusError($@"Is {Name} missing ConfigureAll? error in Lateralus.ApplicationProject.GetCurrentCallingFileInfo()");
                    }
                    return new FileInfo(filename);
                }
            }
            throw new LateralusError("error in Lateralus.ApplicationProject.GetCurrentCallingFileInfo()");
        }
    }
}
