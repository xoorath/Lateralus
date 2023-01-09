using Sharpmake;
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
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
            // #bug #hack Unfortunately in VS 2022 version 17.4 a bug was introduced where module resolution fails without this set to true.
            CustomProperties.Add("AllProjectBMIsArePublic", "true");

            // PublicModuleDirectories: "Public C++ Module Directories" from the VC++ Directories project page
            // This setting exports only modules from the indicated directory
            string publicModuleDirectory = Util.PathMakeStandard(Path.GetFullPath(Path.Combine(projectFileDirectory, "..", "Public")));
            CustomProperties.Add("PublicModuleDirectories", publicModuleDirectory);

            // Tests get their own project that inherits TestProject
            SourceFilesExcludeRegex.Add($@"\\Tests\\");
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            int sourceLineNumber;
            FileInfo callingFileInfo = GetCurrentCallingFileInfo(out sourceLineNumber);
            string projectFileDirectory = Util.PathMakeStandard(callingFileInfo.DirectoryName);

            // Project configuration
            string publicIncludeDir = Path.Combine(projectFileDirectory, "Public");
            conf.IncludePaths.AddRange(new[] {
                publicIncludeDir,
                Path.Combine(projectFileDirectory, "Private")
            });
            conf.Output = Configuration.OutputType.Lib;
            
            conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions);

            // Dependency configuration
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

            if (!(this is CoreProject))
            {
                Func<bool, int> btoi = (bool b) => b ? 1 : 0;
                bool hasImgui = !target.Optimization.HasFlag(Optimization.Retail);

                bool isDesktop =
                    target.Platform.HasFlag(Platform.win64)
                    || target.Platform.HasFlag(Platform.win32)
                    || target.Platform.HasFlag(Platform.mac)
                    || target.Platform.HasFlag(Platform.linux);

                conf.Defines.Add(new[] {
                    $@"ENABLE_GLFW={btoi(isDesktop)}",
                    $@"ENABLE_IMGUI={btoi(hasImgui)}"
                });

                if (hasImgui)
                {
                    int lineNumber;
                    conf.Defines.Add($@"IMGUI_USER_CONFIG=""{GetCurrentCallingFileInfo(out lineNumber).DirectoryName}\..\Platform\Private\imconfig.h""");
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