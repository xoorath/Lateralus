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
}
