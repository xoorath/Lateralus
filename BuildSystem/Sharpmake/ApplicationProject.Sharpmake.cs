using Sharpmake;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.Intrinsics.X86;

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

        public override void ConfigureAll(Configuration conf, Target target)
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

                bool isDesktop =
                    target.Platform.HasFlag(Platform.win64)
                    || target.Platform.HasFlag(Platform.win32)
                    || target.Platform.HasFlag(Platform.mac)
                    || target.Platform.HasFlag(Platform.linux);

                conf.Defines.Add(new[] {
                    $@"USE_GLFW_WINDOW={btoi(isDesktop)}",
                    $@"IMGUI_SUPPORT={btoi(hasImgui)}"
                });

                if (hasImgui)
                {
                    conf.Defines.Add($@"IMGUI_USER_CONFIG=""{GetCurrentCallingFileInfo().DirectoryName}\..\..\Engine\Platform\Private\imconfig.h""");
                    ThirdParty.ReferenceExternal(conf, target, new[] {
                        ThirdParty.ExternalProject.freetype,
                        ThirdParty.ExternalProject.libpng,
                        ThirdParty.ExternalProject.brotli,
                        ThirdParty.ExternalProject.bzip2,
                        ThirdParty.ExternalProject.zlib
                    });

                    conf.AddPublicDependency<ImguiProject>(target, DependencySetting.Default);
                }
            }

            conf.Defines.Add("SPDLOG_ACTIVE_LEVEL=0");
            ThirdParty.ReferenceExternal(conf, target, new[] {
                ThirdParty.ExternalProject.spdlog,
                ThirdParty.ExternalProject.fmt
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
                    if(string.IsNullOrEmpty(filename))
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
