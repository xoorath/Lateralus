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
            Name = ProjectName;

            int sourceLineNumber;
            string projectFileDirectory = Util.PathMakeStandard(GetCurrentCallingFileInfo(out sourceLineNumber).DirectoryName);

            // [project.SharpMakeCsPath] can't be used because it would identify the this file's path.
            SourceRootPath = projectFileDirectory;

            // AllProjectBMIsArePublic: "All Modules Are Public" from the VC++ Directories project page
            // This allows us to specifically define the API of each engine library using the public/private folders & exports.
            CustomProperties.Add("AllProjectBMIsArePublic", "false");

            // PublicModuleDirectories: "Public C++ Module Directories" from the VC++ Directories project page
            // This setting exports only modules from the indicated directory
            string publicModuleDirectory = Util.PathMakeStandard(Path.GetFullPath(Path.Combine(projectFileDirectory, "..", "Public")));
            CustomProperties.Add("PublicModuleDirectories", publicModuleDirectory);
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            int sourceLineNumber;
            FileInfo callingFileInfo = GetCurrentCallingFileInfo(out sourceLineNumber);
            string projectFileDirectory = Util.PathMakeStandard(callingFileInfo.DirectoryName);

            // Project configuration
            conf.IncludePaths.AddRange(new[] {
                Path.Combine(projectFileDirectory, "Public"),
                Path.Combine(projectFileDirectory, "Private")
            });
            conf.Output = Configuration.OutputType.Lib;

            // Dependancy configuration
            if (!(this is CoreProject))
            {
                conf.AddPublicDependency<CoreProject>(target, DependencySetting.Default, callingFileInfo.FullName, sourceLineNumber);
            }

            // Decorative configuration
            conf.SolutionFolder = "Engine";

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
                if (method.DeclaringType == typeof(EngineProject))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);
                    lineNumber = stackFrame.GetFileLineNumber();
                    string fileName = stackFrame.GetFileName();
                    if(string.IsNullOrEmpty(fileName))
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