using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class ImguiProject : ThirdPartyProject
    {
        public override string ProjectName => "imgui";

        public ImguiProject()
            : base("imgui")
        {
            
            //AddTargets(new Target(
            //    Platform.win64,
            //    DevEnv.vs2022,
            //    Optimization.Debug | Optimization.Release
            //));

            SourceFilesExcludeRegex.AddRange(new[]
            {
                $@"\\.github\\",
                $@"\\backends\\",
                $@"\\docs\\",
                $@"\\examples\\",
                $@"\\misc\\",
                $@"\\imconfig.h\\"
            });
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add($@"[project.SharpmakeCsPath]\{ProjectName}\");

            conf.Defines.Add($@"IMGUI_USER_CONFIG=""[project.SharpmakeCsPath]\..\Engine\Platform\Private\imconfig.h""");
        }
    }
}
