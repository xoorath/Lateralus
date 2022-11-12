using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class PlatformProject : EngineProject
    {
        public override string ProjectName => "Platform";

        public PlatformProject()
            : base()
        {
            SourceRootPath = @"[project.SharpmakeCsPath]";
        }
    }
}
