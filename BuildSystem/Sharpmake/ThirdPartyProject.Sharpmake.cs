using Sharpmake;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;

namespace Lateralus
{
    public abstract class ThirdPartyProject : LateralusProjectBase
    {
        public ThirdPartyProject(string thirdPartyDirectory)
            : base()
        {
            Name = ProjectName;

            int sourceLineNumber;
            string projectFileDirectory = Util.PathMakeStandard(GetCurrentCallingFileInfo(out sourceLineNumber).DirectoryName);

            // [project.SharpMakeCsPath] can't be used because it would identify the this file's path.
            SourceRootPath = Path.Combine(projectFileDirectory, thirdPartyDirectory);
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.Output = Configuration.OutputType.Lib;

            // Decorative configuration
            conf.SolutionFolder = "ThirdParty";

            // Hack|Bug:
            // vcxproj.cs has a line like this determining if dependencies should be linked:
            /*
                bool addDependencies = context.Project.AllowInconsistentDependencies
                    ? context.ProjectConfigurations.Any(c => ConfigurationNeedReferences(c))
                    : ConfigurationNeedReferences(firstConf);
             */
            // ConfigurationNeedReferences will return false whenever we're building a lib but
            // the following flag is not set. I'm not sure why this flag needs to be set or
            // what the proper codepath/option set is to enable lib's referencing 
            conf.ExportAdditionalLibrariesEvenForStaticLib = true;
            Debug.Assert(conf.Output == Configuration.OutputType.Lib, "above hack only relevant for libs.");
        }

        private FileInfo GetCurrentCallingFileInfo(out int lineNumber)
        {
            lineNumber = -1;
            StackTrace stackTrace = new StackTrace(true);
            const int depth = 2;

            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(ThirdPartyProject))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);
                    lineNumber = stackFrame.GetFileLineNumber();
                    string fileName = stackFrame.GetFileName();
                    if (string.IsNullOrEmpty(fileName))
                    {
                        throw new LateralusError($@"Is {Name} missing ConfigureAll? error in Lateralus.EngineProject.GetCurrentCallingFileInfo()");
                    }
                    return new FileInfo(fileName);
                }
            }
            throw new LateralusError("error in Lateralus.EngineProject.GetCurrentCallingFileInfo()");
        }
    }
}