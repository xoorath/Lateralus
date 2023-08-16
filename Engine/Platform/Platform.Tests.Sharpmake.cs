using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class PlatformTestProject : TestProject<PlatformProject>
    {
        public override string ProjectName => "Platform.Tests";

        public PlatformTestProject()
            : base()
        {
        }

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}
