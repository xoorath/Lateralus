using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class CoreTestProject : TestProject<CoreProject>
    {
        public override string ProjectName => "Core.Tests";

        public CoreTestProject()
            : base()
        {
        }

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}
