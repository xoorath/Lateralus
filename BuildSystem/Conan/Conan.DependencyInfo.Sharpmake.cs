using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Lateralus
{
    public class ConanDependencies
    {
        public string SerializeAsConanfile()
        {
            StringBuilder stringBuilder = new StringBuilder();

            if (m_Requires != null && m_Requires.Any())
            {
                stringBuilder.AppendLine("[requires]");
                foreach (var item in m_Requires)
                {
                    stringBuilder.AppendLine(item);
                }
                stringBuilder.AppendLine();
            }
            if (m_Options != null && m_Options.Any())
            {
                stringBuilder.AppendLine("[options]");
                foreach (var item in m_Options)
                {
                    stringBuilder.AppendLine(item);
                }
                stringBuilder.AppendLine();
            }
            if (m_Imports != null && m_Imports.Any())
            {
                stringBuilder.AppendLine("[imports]");
                foreach (var item in m_Imports)
                {
                    stringBuilder.AppendLine(item);
                }
                stringBuilder.AppendLine();
            }

            // Our sharpmake integration of conan is built around the json generator.
            stringBuilder.AppendLine("[generators]");
            stringBuilder.AppendLine("json");
            return stringBuilder.ToString();
        }

        public IEnumerable<string> Requires
        { 
            get
            {
                return m_Requires.DefaultIfEmpty();
            }
            set
            {
                m_Requires = value.DefaultIfEmpty().ToArray();
            }
        }

        public IEnumerable<string> ToolRequires
        {
            get
            {
                return m_ToolRequires.DefaultIfEmpty();
            }
            set
            {
                m_ToolRequires = value.DefaultIfEmpty().ToArray();
            }
        }

        public IEnumerable<string> Options
        {
            get
            {
                return m_Options.DefaultIfEmpty();
            }
            set
            {
                m_Options = value.DefaultIfEmpty().ToArray();
            }
        }

        public IEnumerable<string> Imports
        {
            get
            {
                return m_Imports.DefaultIfEmpty();
            }
            set
            {
                m_Imports = value.DefaultIfEmpty().ToArray();
            }
        }

        private string[] m_Requires;
        private string[] m_ToolRequires;
        private string[] m_Options;
        private string[] m_Imports;
    }
}