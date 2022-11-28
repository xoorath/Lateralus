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
                ThirdParty.ExternalProject.imgui,
                ThirdParty.ExternalProject.glfw,
                ThirdParty.ExternalProject.glew
            });

            conf.Defines.Add("IMGUI_IMPL_OPENGL_LOADER_GLEW");
            conf.LibraryFiles.Add("opengl32");

            // to limit to one graph.
            conf.DumpDependencyGraph = target.Optimization.HasFlag(Optimization.Debug);
        }
    }
}