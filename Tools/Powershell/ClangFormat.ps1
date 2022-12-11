param(
    # Location of the clang format tool
    [string] $ClangFormat
)

# /Tools/Powershell
$ScriptDir = Split-Path $MyInvocation.MyCommand.Path
Push-Location $ScriptDir


if([string]::IsNullOrEmpty($ClangFormat) -or -not ([IO.File]::Exists($ClangFormat) -and -not (Get-Command $ClangFormat -ErrorAction SilentlyContinue)))
{
    if (Get-Command "clang-format.exe" -ErrorAction SilentlyContinue) 
    { 
        $ClangFormat = "clang-format.exe"
    }
    else
    {
        $defaultClangFormatLocation = "$env:APPDATA\ClangPowerTools\LLVM\LLVM15.0.4\bin\clang-format.exe";
        $ClangFormat = $defaultClangFormatLocation;
    }
}

if([string]::IsNullOrEmpty($ClangFormat) -and -not ([IO.File]::Exists($ClangFormat)) -and -not (Get-Command $ClangFormat -ErrorAction SilentlyContinue))
{
    Write-Host "Clang format not found.";
    Pop-Location
    exit -1
}

&$ClangFormat "--version"

# /
$ProjectRootDir = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($ScriptDir, "..", ".."));
# /Tools
$ToolsDir = [System.IO.Path]::Combine($ProjectRootDir, "Tools");
# /Applications
$ApplicationsDir = [System.IO.Path]::Combine($ProjectRootDir, "Applications");
# /Tools/Utilities/
$UtilitiesDir = [System.IO.Path]::Combine($ToolsDir, "Utilities");
# /Engine
$EngineDir = [System.IO.Path]::Combine($ProjectRootDir, "Engine");

function GetSourceFilesFromDirectory
{
    Param(
        [parameter(Mandatory=$true)]
        [System.Collections.ArrayList]
        $Directories
    )
    $results = New-Object System.Collections.ArrayList;
    foreach($dir in $Directories)
    {
        $filter = [System.IO.Path]::Combine($dir, "*");
        [void]$results.AddRange((Get-ChildItem $filter -include ('*.cpp', '*.ixx', '*.h') -recurse));
    }
    return $results;
}

$Sources = GetSourceFilesFromDirectory -Directories @($ApplicationsDir, $UtilitiesDir, $EngineDir);

# Write-Host $Sources

&$ClangFormat -style=file -i $Sources

Pop-Location