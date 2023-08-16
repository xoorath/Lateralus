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

        public override void ConfigureAll(Configuration conf, GameTarget target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}