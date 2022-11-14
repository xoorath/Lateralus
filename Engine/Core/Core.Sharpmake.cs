using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class CoreProject : EngineProject
    {
        public override string ProjectName => "Core";

        public CoreProject()
            : base()
        {
        }
    }
}
