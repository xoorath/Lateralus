using Sharpmake;
using System;
using System.Diagnostics;

namespace Lateralus
{
    internal class LateralusError : Error
    {
        public LateralusError(string msg)
            : base(msg)
        { 
            if(Debugger.IsAttached)
            {
                Debugger.Break();
            }
        }
    }

    internal class LateralusTargetNotSupportedException : LateralusError
    {
        internal LateralusTargetNotSupportedException(Target target)
            : base($@"Lateralus is not yet configured to build for target: {target.Name}")
        {
        }
    }

}