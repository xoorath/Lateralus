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

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            
            ThirdParty.ReferenceExternal(conf, target, new[] {
                ThirdParty.ExternalProject.glfw,
                ThirdParty.ExternalProject.glew
            });

            conf.LibraryFiles.Add("opengl32");
        }
    }
}