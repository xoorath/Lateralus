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
        : base(typeof(GameTarget))
    {
        Name = "Lateralus";
        AddTargets(new GameTarget());
    }

    [Configure()]
    public void ConfigureAll(Configuration conf, GameTarget target)
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
