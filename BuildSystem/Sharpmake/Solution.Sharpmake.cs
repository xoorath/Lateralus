using Sharpmake;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Lateralus
{
    [Generate]
    public class LateralusSolution : Solution
    {
        public LateralusSolution()
        {
            Name = "Lateralus";

            AddTargets(new Target(
                    Platform.win64,
                    DevEnv.vs2022,
                    Optimization.Debug | Optimization.Release
            ));
        }

        [Configure()]
        public void ConfigureAll(Configuration conf, Target target)
        {
            conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\..\..\generated";

            Conan.Install(conf, target);

            conf.AddProject<HelloWorldProject>(target);
            conf.AddProject<CoreProject>(target);
            conf.AddProject<PlatformProject>(target);
            conf.AddProject<LoggingProject>(target);
        }
    }
}