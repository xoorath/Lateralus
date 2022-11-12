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
            SourceRootPath = @"[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}