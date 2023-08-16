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

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add($@"[project.SharpmakeCsPath]\{ProjectName}\");

            conf.Defines.Add($@"IMGUI_USER_CONFIG=""[project.SharpmakeCsPath]\..\Engine\Platform\Private\imconfig.h""");

            conf.AddPublicDependency<LunaSVGProject>(target, DependencySetting.Default);
        }
    }
}
