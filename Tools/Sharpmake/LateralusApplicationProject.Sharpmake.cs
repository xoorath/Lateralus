using Sharpmake;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

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

            // [project.SharpMakeCsPath] can't be used because it would identify the this file's path.
            SourceRootPath = Util.PathMakeStandard(GetCurrentCallingFileInfo().DirectoryName);
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            // Project configuration
            conf.IncludePaths.AddRange(m_ProjectSource);
            conf.Output = Configuration.OutputType.Exe;

            // Dependancy configuration
            conf.AddPrivateDependency<CoreProject>(target);
            conf.AddPrivateDependency<PlatformProject>(target);

            // Decorative configuration
            conf.SolutionFolder = "Applications";
        }

        private static FileInfo GetCurrentCallingFileInfo()
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
                    return new FileInfo(stackFrame.GetFileName());
                }
            }
            throw new LateralusError("error in Lateralus.ConanProject.GetCurrentCallingFileInfo()");
        }

    }
}