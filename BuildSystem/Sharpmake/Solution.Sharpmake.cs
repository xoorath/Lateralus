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

        AddTargets(new Target(Platform.win64, DevEnv.vs2022,
                              Optimization.Debug | Optimization.Release | Optimization.Retail));
    }

    [Configure()]
    public void ConfigureAll(Configuration conf, Target target)
    {
        conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
        conf.SolutionPath = @"[solution.SharpmakeCsPath]\..\..\generated";

        conf.AddProject<HelloWorldProject>(target);

        conf.AddProject<FontToSourceProject>(target);
        conf.AddProject<CoreTestProject>(target);
        conf.AddProject<PlatformTestProject>(target);
    }
}
}
