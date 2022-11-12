using Sharpmake;

/// <summary>
/// Interface to enforce supported build environments to be explicit about certain options
/// </summary>
public interface ILateralusOptions
{
    object WarningLevel { get; }
    object TreatWarningsAsErrors { get; }
    object CharacterSet { get; }

    object CppLanguageStandard {get; }
    object Exceptions {get; }
    object RTTI { get; }
}