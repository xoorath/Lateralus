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

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}
