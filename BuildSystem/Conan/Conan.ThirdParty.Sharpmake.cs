using Sharpmake;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Xml.Linq;

namespace Lateralus
{
    public static partial class ThirdParty
    {
        public enum ExternalProject
        {
            glew,

            glfw,

            freetype,
            /**/libpng, // from freetype
            /**/brotli, // from freetype
            /**/bzip2, // from freetype
            /**/zlib, // from freetype

            spdlog,
            fmt,

            gtest,
        }

        public static void ReferenceExternal(Project.Configuration conf, Target target, ExternalProject externalDependancy)
        {
            ReferenceExternal(conf, target, new[] { externalDependancy });
        }

        public static void ReferenceExternal(Project.Configuration conf, Target target, ExternalProject[] externalDependancies)
        {
            var buildInfo = GetBuildInfo(conf, target);
            foreach(var dependancyRequest in externalDependancies)
            {
                var matchingBuildInfo = buildInfo.dependencies
                .Where(
                    d => new[] {d.name, d.names?.cmake_find_package, d.names?.cmake_find_package_multi}
                    .Where(n => !string.IsNullOrEmpty(n) && n.Equals(dependancyRequest.ToString(), System.StringComparison.InvariantCultureIgnoreCase)).Any()
                ).ToArray();

                if(matchingBuildInfo.Length != 1)
                {
                    throw new LateralusError($@"Failed to find exactly one match for {dependancyRequest.ToString()}. Found {matchingBuildInfo.Length} instead.");
                }
                ConanDependency dependancyInfo = matchingBuildInfo.First();

                conf.IncludePaths.AddRange(dependancyInfo.include_paths.Where(inc => !conf.IncludePaths.Contains(inc)));
                conf.LibraryPaths.AddRange(dependancyInfo.lib_paths.Where(inc => !conf.LibraryPaths.Contains(inc)));
                 
                if (conf.Output.Equals(Project.Configuration.OutputType.Exe))
                {
                    conf.LibraryFiles.AddRange(dependancyInfo.libs.Where(inc => !conf.LibraryFiles.Contains(inc)));
                }
                else
                {
                    conf.DependenciesLibraryFiles.AddRange(dependancyInfo.libs.Where(inc => !conf.DependenciesLibraryFiles.Contains(inc)));
                }

                conf.Defines.AddRange(dependancyInfo.defines.Where(inc => !conf.Defines.Contains(inc)));
            }
        }

        private static ConanBuildInfo GetBuildInfo(Configuration conf, Target target)
        {
            string lateralusRootDir = Path.GetFullPath(Path.Combine(
                Util.PathMakeStandard(Util.GetCurrentSharpmakeFileInfo().DirectoryName),
                "..", ".."
            ));

            string generatedPath = Path.GetFullPath(Path.Combine(
                lateralusRootDir,
                "generated"
            ));

            string conanGeneratedDir = Path.GetFullPath(Path.Combine(
                generatedPath,
                "conan",
                $@"{target.Platform.ToString()}_{target.DevEnv.ToString()}_{target.Optimization.ToString()}"
            ));

            const string conanBuildInfoFileName = "conanbuildinfo.json";
            string conanBuildInfoFilePath = Path.GetFullPath(Path.Combine(conanGeneratedDir, conanBuildInfoFileName));

            return ConanBuildInfo.FromBuildInfoJsonFileName(conanBuildInfoFilePath);
        }
    }
}