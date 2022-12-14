using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class CoreProject : EngineProject
    {
        public override string ProjectName => "Core";

        public CoreProject()
            : base()
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}
