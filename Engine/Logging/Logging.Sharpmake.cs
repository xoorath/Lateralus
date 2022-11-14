using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class LoggingProject : EngineProject
    {
        public override string ProjectName => "Logging";

        public LoggingProject()
            : base()
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}
