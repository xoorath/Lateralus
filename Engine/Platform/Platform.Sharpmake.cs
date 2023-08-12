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

            if(target.Platform.HasAnyFlag(Platform.win32 | Platform.win64 | Platform.linux | Platform.mac))
            {
                ConfigurePC(conf, target);
            }
            else
            {
                throw new LateralusError($@"Unsupported platform {target.Platform}");
            }

        }

        private void ConfigurePC(Configuration conf, Target target)
        {
            Conan.AddExternalDependencies(conf, target, this, new ConanDependencies() { 
                Requires = new[]
                {
                    "glfw/3.3.8",
                    "glew/2.2.0"
                },
                Options = new[]
                {
                    "glew:shared=False"
                }
            });
            conf.DependenciesOtherLibraryFiles.Add("opengl32");
        }
    }
}
