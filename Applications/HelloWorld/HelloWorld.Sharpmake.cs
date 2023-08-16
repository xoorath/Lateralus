using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class HelloWorldProject : ApplicationProject
    {
        public override string ProjectName => "Hello World";

        public HelloWorldProject()
            : base()
        {
        }

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}