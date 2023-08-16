using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class LunaSVGProject : ThirdPartyProject
    {
        public override string ProjectName => "lunasvg";

        public LunaSVGProject()
            : base("lunasvg")
        {
            SourceFilesExclude.AddRange(new[] { "svg2png.cpp", "luna.png" });
            SourceFilesExcludeRegex.AddRange(new[] { $@"\\3rdparty\\stb\\", $@"\\.github\\" });
        }

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.AddRange(new[] { 
                $@"[project.SharpmakeCsPath]\{ProjectName}\include",
                $@"[project.SharpmakeCsPath]\{ProjectName}\3rdparty\plutovg"
            });
        }
    }
}
