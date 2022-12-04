using Sharpmake;

namespace Lateralus
{
    [Generate]
    public class FontToSourceProject : UtilityProject
    {
        public override string ProjectName => "FontToSource";

        public FontToSourceProject()
            : base()
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}