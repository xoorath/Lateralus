using Sharpmake;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Lateralus
{
    public abstract class UtilityProject : LateralusProjectBase
    {
        private IReadOnlyCollection<string> m_ProjectSource => new[] {
            @"[project.SharpmakeCsPath]\Src"
        };

        public UtilityProject()
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

            // Decorative configuration
            conf.SolutionFolder = "Applications/Utilities";

            conf.EventPostBuild.AddRange(new[] {
                $@"if not exist ""$(OutDir)Assets"" mklink /j ""$(OutDir)Assets"" ""{Util.PathMakeStandard(GetCurrentCallingFileInfo().DirectoryName)}/Assets"""
            });

            conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings()
            {
                LocalDebuggerWorkingDirectory = "$(OutDir)"
            };
        }

        private FileInfo GetCurrentCallingFileInfo()
        {
            const int depth = 2;
            StackTrace stackTrace = new StackTrace(true);
            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(UtilityProject))
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
