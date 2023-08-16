using System;
using System.Collections.Generic;
using System.Diagnostics;
using Sharpmake;

namespace Lateralus
{
    [Fragment, Flags]
    public enum Optimization
    {
        Debug = 1 << 0,
        Release = 1 << 1,
        Retail = 1 << 2
    }

    [Fragment, Flags]
    public enum Compiler
    {
        MSVC = 1 << 0,
        ClangCl = 1 << 1
    }

    [DebuggerDisplay("\"{Platform}_{DevEnv}\" {Name}")]
    public class GameTarget : Sharpmake.ITarget
    {
        public Platform Platform;
        public Compiler Compiler;
        public DevEnv DevEnv;
        public Optimization Optimization;
        public Blob Blob;
        public BuildSystem BuildSystem;

        public GameTarget()
        {
            this.Platform = Platform.win64;
            this.Compiler = Compiler.MSVC | Compiler.ClangCl;
            this.DevEnv = DevEnv.vs2022;
            this.Optimization = Optimization.Debug | Optimization.Release | Optimization.Retail;
            this.Blob = Blob.NoBlob;
            this.BuildSystem = BuildSystem.MSBuild;
        }

        public override string Name
        {
            get
            {
                var nameParts = new List<string>
                {
                    this.Compiler.ToString(),
                    this.Optimization.ToString()
                };
                return string.Join(" ", nameParts);
            }
        }

        public string SolutionPlatformName
        {
            get
            {
                return this.BuildSystem.ToString();
            }
        }

        /// <summary>
        /// returns a string usable as a directory name, to use for instance for the intermediate path
        /// </summary>
        public string DirectoryName => $@"{this.Platform.ToString()}_{this.Compiler.ToString()}_{this.Optimization.ToString()}_{this.BuildSystem.ToString()}";

        public override Sharpmake.Optimization GetOptimization()
        {
            switch (this.Optimization)
            {
                case Optimization.Debug:
                    return Sharpmake.Optimization.Debug;
                case Optimization.Release:
                    return Sharpmake.Optimization.Release;
                case Optimization.Retail:
                    return Sharpmake.Optimization.Retail;
                default:
                    throw new NotSupportedException("Optimization value " + this.Optimization.ToString());
            }
        }

        public override Platform GetPlatform()
        {
            return this.Platform;
        }
    }
}