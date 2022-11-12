using Sharpmake;

namespace Lateralus
{
    public class LateralusOptionsVs2022 : ILateralusOptions
    {
        object ILateralusOptions.WarningLevel => Options.Vc.General.WarningLevel.Level3;
        object ILateralusOptions.TreatWarningsAsErrors => Options.Vc.General.TreatWarningsAsErrors.Enable;
        object ILateralusOptions.CharacterSet => Options.Vc.General.CharacterSet.Unicode;

        object ILateralusOptions.CppLanguageStandard => Options.Vc.Compiler.CppLanguageStandard.CPP20;
        object ILateralusOptions.Exceptions => Options.Vc.Compiler.Exceptions.Enable;
        object ILateralusOptions.RTTI => Options.Vc.Compiler.RTTI.Disable;
    }
}