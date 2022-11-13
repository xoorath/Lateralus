using Sharpmake;

namespace Lateralus
{
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
}