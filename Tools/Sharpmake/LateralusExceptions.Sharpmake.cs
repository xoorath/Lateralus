using Sharpmake;
using System;

namespace Lateralus
{
    internal class LateralusTargetNotSupportedException : Exception
    {
        internal LateralusTargetNotSupportedException(Target target)
            : base($@"Lateralus is not yet configured to build for target: {target.Name}")
        {
        }
    }
}