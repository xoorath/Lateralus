using Sharpmake;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Lateralus
{
    public abstract class EngineProject : LateralusProjectBase
    {
        public EngineProject()
            : base()
        {
            Name = $@"{ProjectName}";

            // [project.SharpMakeCsPath] can't be used because it would identify the this file's path.
            SourceRootPath = Util.PathMakeStandard(GetCurrentCallingFileInfo().DirectoryName);

            // AllProjectBMIsArePublic: "All Modules Are Public" from the VC++ Directories project page
            // This allows us to specifically define the API of each engine library using the public/private folders & exports.
            CustomProperties.Add("AllProjectBMIsArePublic", "false");

            // PublicModuleDirectories: "Public C++ Module Directories" from the VC++ Directories project page
            // This setting exports only modules from the indicated directory
            CustomProperties.Add("PublicModuleDirectories", @"$(ProjectDir)..\Public");
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            // Project configuration
            conf.IncludePaths.AddRange(new[] {
                @"[project.SharpmakeCsPath]\Public",
                @"[project.SharpmakeCsPath]\Private"
            });
            conf.Output = Configuration.OutputType.Lib;

            // Dependancy configuration
            if (!(this is CoreProject))
            {
                conf.AddPrivateDependency<CoreProject>(target);
            }

            // Decorative configuration
            conf.SolutionFolder = "Engine";
        }

        private static FileInfo GetCurrentCallingFileInfo()
        {
            const int depth = 2;
            StackTrace stackTrace = new StackTrace(true);
            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(EngineProject))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);
                    return new FileInfo(stackFrame.GetFileName());
                }
            }
            throw new LateralusError("error in Lateralus.ConanProject.GetCurrentCallingFileInfo()");
        }
    }
}