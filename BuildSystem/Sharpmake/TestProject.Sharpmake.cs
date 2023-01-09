using Sharpmake;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Lateralus
{
    public abstract class TestProject<ProjectTypeToTest> : LateralusProjectBase
        where ProjectTypeToTest : LateralusProjectBase
    {
        private IReadOnlyCollection<string> m_ProjectSource =>
            new[] { @"[project.SharpmakeCsPath]\Tests" };

        public TestProject() : base()
        {
            Name = ProjectName;

            // [project.SharpMakeCsPath] can't be used because it would identify this file's path.
            SourceRootPath =
                Path.Combine(Util.PathMakeStandard(GetCurrentCallingFileInfo().DirectoryName), "Tests");
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            // Project configuration
            conf.IncludePaths.AddRange(m_ProjectSource);
            conf.Output = Configuration.OutputType.Exe;

            // Precompile headers
            conf.PrecompHeader = $@"[project.SharpmakeCsPath]\Tests\pch.h";
            conf.PrecompSource = $@"[project.SharpmakeCsPath]\Tests\pch.cpp";
            conf.Options.Add(Options.Vc.SourceFile.PrecompiledHeader.UsePrecompiledHeader);
            conf.Defines.Add($@"PCH_FILE=""[project.SharpmakeCsPath]\Tests\pch.h""");

            string toTestBaseName = typeof(ProjectTypeToTest).BaseType.ToString();
            if (toTestBaseName == typeof(EngineProject).ToString())
            {
                conf.SolutionFolder = "Engine";
            }
            else if (toTestBaseName == typeof(ApplicationProject).ToString())
            {
                conf.SolutionFolder = "Applications";
            }
            else if (toTestBaseName == typeof(UtilityProject).ToString())
            {
                conf.SolutionFolder = "Applications/Utilities";
            }
            else
            {
                conf.SolutionFolder = "Tests";
            }

            conf.AddPublicDependency<ProjectTypeToTest>(target);

            ThirdParty.ReferenceExternal(conf, target, new[] { ThirdParty.ExternalProject.gtest });
        }

        private FileInfo GetCurrentCallingFileInfo()
        {
            const int depth = 2;
            StackTrace stackTrace = new StackTrace(true);
            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                MethodBase method = stackFrame.GetMethod();
                var to = typeof(TestProject<ProjectTypeToTest>);
                if (method.DeclaringType.Name == to.Name)
                {
                    stackFrame = stackTrace.GetFrame(i + depth);
                    string filename = stackFrame.GetFileName();
                    if (string.IsNullOrEmpty(filename))
                    {
                        throw new LateralusError(
                            $@"Is {Name} missing ConfigureAll? error in Lateralus.UtilityProject.GetCurrentCallingFileInfo()");
                    }
                    return new FileInfo(filename);
                }
            }
            throw new LateralusError(
                "error in Lateralus.ApplicationProject.GetCurrentCallingFileInfo()");
        }
    }
}
