using Sharpmake;

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
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\..\..\[target.DevEnv]";

            conf.AddProject<HelloWorldProject>(target);
        }
    }
}