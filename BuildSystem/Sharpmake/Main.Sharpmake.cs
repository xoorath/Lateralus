using Lateralus;
using Sharpmake;

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Arguments arguments)
    {
        arguments.Generate<LateralusSolution>();
    }
}
