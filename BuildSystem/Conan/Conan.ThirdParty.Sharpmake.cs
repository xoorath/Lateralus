using Sharpmake;
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
            glfw,
            freetype,
            /**/libpng, // from freetype
            /**/brotli, // from freetype
            /**/bzip2, // from freetype
            /**/zlib, // from freetype
            glew,

            spdlog,
            fmt,
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

                AddRangeUnique(conf.IncludePaths, dependancyInfo.include_paths);
                AddRangeUnique(conf.LibraryPaths, dependancyInfo.lib_paths);
                AddRangeUnique(conf.LibraryFiles, dependancyInfo.libs);
                //AddRangeUnique(conf.LibraryFiles, dependancyInfo.system_libs);

                AddRangeUnique(conf.Defines, dependancyInfo.defines);
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

        private static void AddRangeUnique(ICollection<string> strings, IReadOnlyCollection<string> input)
        {
            if (input != null)
            {
                foreach (var i in input)
                {
                    if (!string.IsNullOrEmpty(i) && !strings.Contains(i))
                    {
                        strings.Add(i);
                    }
                }
            }
        }

        private static void AddRangeUnique(UniqueList<string> strings, IReadOnlyCollection<string> input)
        {
            if(input != null && input.Any())
            {
                strings.AddRange(input.Where(s => !string.IsNullOrEmpty(s)));
            }
        }
    }
}