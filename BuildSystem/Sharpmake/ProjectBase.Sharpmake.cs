using Sharpmake;
using System.Diagnostics;
using System.Reflection;
using System.IO;
using System;

using Optimization = Lateralus.Optimization;
using Compiler = Lateralus.Compiler;
using System.Collections.Generic;
using System.Linq;
using System.Globalization;

namespace Lateralus
{
    public static class ConfigurePriorities
    {
        public const int All = -75;
        public const int Platform = -50;
        public const int Optimization = -25;
        /*     SHARPMAKE DEFAULT IS 0     */
        public const int Blobbing = 25;
        public const int BuildSystem = 50;
        public const int Compiler = 75;
    }

    public abstract class LateralusProjectBase : Project
    {
        public abstract string ProjectName { get; }

        public LateralusProjectBase()
            : base(typeof(GameTarget))
        {
            AddTargets(new GameTarget());

            // Add module extensions 
            SourceFilesExtensions.Add(".ixx");
            SourceFilesCompileExtensions.Add(".ixx");

            // Add sharpmake extension to the project (for quick editing: not used in compilation).
            SourceFilesExtensions.Add(".cs");

            // Add asset file types:
            SourceFilesExtensions.AddRange(new[] { 
                // todo: add other asset types we might want to see in visual studio.
                ".fs", 
                ".vs",
                ".png"
            });
        }

        [ConfigurePriority(ConfigurePriorities.All)]
        [Configure()]
        public virtual void ConfigureAll(Configuration conf, GameTarget target)
        {
            conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Compiler]_[target.Platform]";
            conf.ProjectPath = $@"{GetLateralusRootDirectory()}\generated\[project.Name]_[target.DevEnv]_[target.Compiler]_[target.Platform]";

            {
                Func<bool, int> btoi = (bool b) => b ? 1 : 0;
                var platform = target.GetPlatform();
                var optimization = target.Optimization;
                bool isX86 = platform.HasFlag(Platform.win32);
                bool isAMD64 =
                    platform.HasFlag(Platform.win64) 
                    || platform.HasFlag(Platform.mac)
                    || platform.HasFlag(Platform.linux);

                bool isDesktop = isX86 || isAMD64;

                conf.Defines.Add(new[] {
                    // Config Specific
                    $@"CONF_DEBUG={btoi(optimization.HasFlag(Optimization.Debug))}",
                    $@"CONF_RELEASE={btoi(optimization.HasFlag(Optimization.Release))}",
                    $@"CONF_RETAIL={btoi(optimization.HasFlag(Optimization.Retail))}",

                    // Config Optimization
                    $@"CONF_IS_OPTIMIZED={btoi(optimization.HasAnyFlag(Optimization.Release|Optimization.Retail))}",
                    
                    //Platform specific
                    $@"PLATFORM_WIN32={btoi(platform.HasFlag(Platform.win32))}",
                    $@"PLATFORM_WIN64={btoi(platform.HasFlag(Platform.win64))}",
                    $@"PLATFORM_LINUX={btoi(platform.HasFlag(Platform.linux))}",
                    $@"PLATFORM_MAC={btoi(platform.HasFlag(Platform.mac))}",          

                    // Platform category
                    $@"PLATFORM_IS_DESKTOP={btoi(isDesktop)}",
                    $@"PLATFORM_IS_WINDOWS={btoi(platform.HasAnyFlag(Platform.win32|Platform.win64))}",
                    
                    // Platform architecture
                    $@"PLATFORM_IS_X86={btoi(isX86)}",
                    $@"PLATFORM_IS_AMD64={btoi(isAMD64)}",
                });
            }
        }

        [ConfigurePriority(ConfigurePriorities.Platform)]
        [Configure(Platform.win64)]
        public virtual void ConfigureWin64(Configuration conf, GameTarget target)
        {
            conf.Defines.Add("_HAS_EXCEPTIONS=0");
        }

        [ConfigurePriority(ConfigurePriorities.Optimization)]
        [Configure(Optimization.Debug)]
        public virtual void ConfigureDebug(Configuration conf, GameTarget target)
        {
            conf.DefaultOption = Options.DefaultTarget.Debug;
        }

        [ConfigurePriority(ConfigurePriorities.Optimization)]
        [Configure(Optimization.Release)]
        public virtual void ConfigureRelease(Configuration conf, GameTarget target)
        {
            conf.DefaultOption = Options.DefaultTarget.Release;
        }

        [Configure(Blob.FastBuildUnitys)]
        [ConfigurePriority(ConfigurePriorities.Blobbing)]
        public virtual void FastBuildUnitys(Configuration conf, GameTarget target)
        {
            conf.FastBuildBlobbed = true;
            conf.FastBuildUnityPath = @"temp\unity\[project.Name]";
            conf.IncludeBlobbedSourceFiles = false;
            conf.IsBlobbed = false;
        }

        [Configure(Blob.NoBlob)]
        [ConfigurePriority(ConfigurePriorities.Blobbing)]
        public virtual void BlobNoBlob(Configuration conf, GameTarget target)
        {
            conf.FastBuildBlobbed = false;
            conf.IsBlobbed = false;

            if (conf.IsFastBuild)
                conf.ProjectName += "_NoBlob";
        }

        [ConfigurePriority(ConfigurePriorities.BuildSystem)]
        [Configure(BuildSystem.MSBuild)]
        public virtual void ConfigureMSBuild(Configuration conf, GameTarget target)
        {
            // starting with vs2019 16.10, need this to fix warning: argument unused during compilation: '/MP'
            conf.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Disable);
        }

        [ConfigurePriority(ConfigurePriorities.BuildSystem)]
        [Configure(BuildSystem.FastBuild)]
        public virtual void ConfigureFastBuild(Configuration conf, GameTarget target)
        {
            conf.Defines.Add("USES_FASTBUILD");

            // Force writing to pdb from different cl.exe process to go through the pdb server
            if (target.Compiler == Lateralus.Compiler.MSVC)
                conf.AdditionalCompilerOptions.Add("/FS");
        }

        [ConfigurePriority(ConfigurePriorities.Compiler)]
        [Configure(Lateralus.Compiler.MSVC)]
        public virtual void ConfigureMSVC(Configuration conf, GameTarget target)
        {
            conf.Defines.Add(new[] {
                "_CRT_SECURE_NO_WARNINGS=1",
                "NOMINMAX=1",
                "WIN32_LEAN_AND_MEAN=1"
            });

            conf.Options.AddRange(new object[] {
                Options.Vc.General.WarningLevel.Level2,
                Options.Vc.General.TreatWarningsAsErrors.Enable,
                Options.Vc.General.CharacterSet.Unicode,

                Options.Vc.Compiler.CppLanguageStandard.CPP20,
                Options.Vc.Compiler.Exceptions.Enable,
                Options.Vc.Compiler.RTTI.Disable,

                new Options.Vc.Linker.IgnoreSpecificLibraryNames("LIBCMT"),
                new Options.Vc.Linker.DisableSpecificWarnings(
                    "4006" // https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-warning-lnk4006?view=msvc-170
                ),
                new Options.Vc.Librarian.DisableSpecificWarnings(
                    "4006" // https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-warning-lnk4006?view=msvc-170
                )
            });

            if (target.GetPlatform().HasFlag(Platform.win64))
            {
                // This define is intended to allow us to include files that depend on winnt
                // without including all of windows.h (in some cases). Example: debugapi.h
                conf.Defines.Add($@"_AMD64_");
            }
        }

        [ConfigurePriority(ConfigurePriorities.Compiler)]
        [Configure(Lateralus.Compiler.ClangCl)]
        public virtual void ConfigureClangCl(Configuration conf, GameTarget target)
        {
            conf.Options.AddRange(new object[] { 
                Options.Vc.General.PlatformToolset.ClangCL,

                //Options.Vc.General.WarningLevel.Level2,
                Options.Vc.General.TreatWarningsAsErrors.Disable,

                Options.Vc.Compiler.CppLanguageStandard.CPP20,
                //Options.Vc.Compiler.Exceptions.Enable,
                //Options.Vc.Compiler.RTTI.Disable,
            });

            conf.AdditionalCompilerOptions.Add(
                //"-Wno-#pragma-messages",
                //"-Wno-c++98-compat",
                "-Wno-microsoft-include",
                "-Wnonportable-include-path"
            );
        }

        // Sourced from: https://stackoverflow.com/a/29578292
        /// <summary>
        /// Gets the exact case used on the file system for an existing file or directory.
        /// </summary>
        /// <param name="path">A relative or absolute path.</param>
        /// <param name="exactPath">The full path using the correct case if the path exists.  Otherwise, null.</param>
        /// <returns>True if the exact path was found.  False otherwise.</returns>
        /// <remarks>
        /// This supports drive-lettered paths and UNC paths, but a UNC root
        /// will be returned in title case (e.g., \\Server\Share).
        /// </remarks>
        public static bool TryGetExactPath(string path, out string exactPath)
        {
            bool result = false;
            exactPath = null;

            // DirectoryInfo accepts either a file path or a directory path, and most of its properties work for either.
            // However, its Exists property only works for a directory path.
            DirectoryInfo directory = new DirectoryInfo(path);
            if (File.Exists(path) || directory.Exists)
            {
                List<string> parts = new List<string>();

                DirectoryInfo parentDirectory = directory.Parent;
                while (parentDirectory != null)
                {
                    FileSystemInfo entry = parentDirectory.EnumerateFileSystemInfos(directory.Name).First();
                    parts.Add(entry.Name);

                    directory = parentDirectory;
                    parentDirectory = directory.Parent;
                }

                // Handle the root part (i.e., drive letter or UNC \\server\share).
                string root = directory.FullName;
                if (root.Contains(':'))
                {
                    root = root.ToUpper();
                }
                else
                {
                    string[] rootParts = root.Split('\\');
                    
                    root = string.Join("\\", rootParts.Select(part => CultureInfo.CurrentCulture.TextInfo.ToTitleCase(part)));
                }

                parts.Add(root);
                parts.Reverse();
                exactPath = Path.Combine(parts.ToArray());
                result = true;
            }

            return result;
        }

        protected void AddImguiDependency(Configuration conf, GameTarget target)
        {
            // We jump through a few hoops to get the exact case sensitive absolute path so clang
            // will trust that this path is portable.
            string relativeUserConfigPath = $@"{GetCurrentCallingFileInfo().DirectoryName}\..\..\Engine\Platform\Private\imconfig.h";
            string userConfigPath;
            if(!TryGetExactPath(relativeUserConfigPath, out userConfigPath))
            {
                throw new LateralusError($@"Could not find imconfig.h at {relativeUserConfigPath}");
            }
            
            conf.Defines.AddRange(new[] {
                $@"ENABLE_IMGUI=1",
                $@"IMGUI_USER_CONFIG=""{userConfigPath}""" 
            });

            Conan.AddExternalDependencies(conf, target, this, new ConanDependencies()
            {
                Requires = new[]
                {
                    "freetype/2.12.1"
                },
                Options = new[]
                {
                    "freetype:with_png=True"
                }
            });

            conf.AddPublicDependency<ImguiProject>(target, DependencySetting.Default);
        }

        private FileInfo GetCurrentCallingFileInfo()
        {
            StackTrace stackTrace = new StackTrace(true);
            const int depth = 2;

            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(LateralusProjectBase))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);
                    string fileName = stackFrame.GetFileName();
                    if (string.IsNullOrEmpty(fileName))
                    {
                        throw new LateralusError($@"Is {Name} missing ConfigureAll? error in Lateralus.LateralusProjectBase.GetCurrentCallingFileInfo()");
                    }
                    return new FileInfo(fileName);
                }
            }
            throw new LateralusError("error in Lateralus.LateralusProjectBase.GetCurrentCallingFileInfo()");
        }

        private static string GetLateralusRootDirectory()
        {
            const int depth = 1;
            StackTrace stackTrace = new StackTrace(true);
            for (int i = 0; i < stackTrace.FrameCount - depth; ++i)
            {
                StackFrame stackFrame = stackTrace.GetFrame(i);
                
                MethodBase method = stackFrame.GetMethod();
                if (method.DeclaringType == typeof(LateralusProjectBase))
                {
                    stackFrame = stackTrace.GetFrame(i + depth);

                    return Path.GetFullPath(Path.Combine(new FileInfo(stackFrame.GetFileName()).DirectoryName, "..", ".."));
                }
            }
            throw new LateralusError("error in Lateralus.LateralusProjectBase.GetLateralusRootDirectory()");
        }
    }
}
