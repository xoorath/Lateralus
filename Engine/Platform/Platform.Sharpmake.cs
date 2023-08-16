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

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);

            if(target.GetPlatform().HasAnyFlag(Platform.win32 | Platform.win64 | Platform.linux | Platform.mac))
            {
                ConfigurePC(conf, target);
            }
            else
            {
                throw new LateralusError($@"Unsupported platform {target.GetPlatform()}");
            }
        }

        private void ConfigurePC(Configuration conf, GameTarget target)
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
