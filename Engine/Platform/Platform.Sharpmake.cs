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
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.AddPrivateDependency<LoggingProject>(target);

            if(target.Platform.HasAnyFlag(Platform.win32 | Platform.win64 | Platform.linux | Platform.mac))
            {
                ConfigurePC(conf, target);
            }
            else
            {
                throw new LateralusError($@"Unsupported platform {target.Platform}");
            }

            // to limit to one graph.
            conf.DumpDependencyGraph = target.Optimization.HasFlag(Optimization.Debug);
        }

        private void ConfigurePC(Configuration conf, Target target)
        {
            ThirdParty.ReferenceExternal(conf, target, new[] {
                    ThirdParty.ExternalProject.glfw,
                    ThirdParty.ExternalProject.glew
                });
            conf.LibraryFiles.Add("opengl32");
        }
    }
}
