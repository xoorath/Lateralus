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
}