using Sharpmake;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Linq;

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
                Optimization.Debug | Optimization.Release
            ));

            // Set SourceRootPath from derrived projects so the [project.SharpMakeCsPath] is accurate
            //SourceRootPath = @"[project.SharpmakeCsPath]";

            // Add module extensions 
            SourceFilesExtensions.Add(".ixx");
            SourceFilesCompileExtensions.Add(".ixx");

            // Add sharpmake extension to the project (for quick editing: not used in compilation).
            SourceFilesExtensions.Add(".cs");
        }


        [Configure()]
        public virtual void ConfigureAll(Configuration conf, Target target)
        {
            conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
            conf.ProjectPath = @"[project.SharpmakeCsPath]\generated";

            switch(target.Platform)
            {
                default:
                    throw new LateralusTargetNotSupportedException(target);
                case Platform.win64:
                    conf.Defines.Add(new[] {
                        "LATERALUS_DESKTOP=1",
                        "LATERALUS_WIN64=1",
                        "LATERALUS_ADDRESS_64=1",
                    });
                    break;
            }

            ILateralusOptions optionProvider = null;
            if (target.DevEnv.HasAnyFlag(DevEnv.VisualStudio))
            {
                optionProvider = new LateralusOptionsVs2022();
            }
            else
            {
                throw new LateralusTargetNotSupportedException(target);
            }

            conf.Options.AddRange(new object[] {
                optionProvider.WarningLevel,
                optionProvider.TreatWarningsAsErrors,
                optionProvider.CharacterSet,

                optionProvider.CppLanguageStandard,
                optionProvider.Exceptions,
                optionProvider.RTTI
            });
        }
    }

    public abstract class EngineProject : LateralusProjectBase
    {
        public EngineProject()
            : base()
        {
            Name = $@"Engine.{ProjectName}";

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
            conf.SolutionFolder = "Lateralus";
        }
    }

    public abstract class ApplicationProject : LateralusProjectBase
    {
        private IReadOnlyCollection<string> m_ProjectSource => new[] {
            @"[project.SharpmakeCsPath]\Src"
        };

        public ApplicationProject()
            : base()
        {
            Name = ProjectName;
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
    }
}
