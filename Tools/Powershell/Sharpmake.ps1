param(
    # Run Nuke.ps1 before anything else (deeply cleans the project)
    [switch] $Nuke,
    # Open the solution(s) once generated
    [switch] $Open,
    # Verbose output from sharpmake during generation
    [switch] $Verbose,
    # Use(/build) sharpmake debug configuration and generate a debug sharpmake solution
    [switch] $Debug,
    # Force re-building sharpmake
    [switch] $Force,
    # disable sharpmake multithreading (may help in debugging)
    [switch] $DisableMultithread
)

# /Tools/Powershell
$ScriptDir = Split-Path $MyInvocation.MyCommand.Path;
Push-Location $ScriptDir;

function GetSharpmakeFilesFromDirectory
{
    Param(
        [parameter(Mandatory=$true)]
        [System.Collections.ArrayList]
        $Directories, 
        
        [System.Collections.ArrayList]
        $ExcludeDirectories
    )

    $results = New-Object System.Collections.ArrayList;
    foreach($dir in $Directories)
    {
        $files = New-Object System.Collections.ArrayList;
        $gciResult = (Get-ChildItem -Path $dir -Filter *.Sharpmake.cs -Recurse -File);
        if($null -eq $gciResult)
        {
            continue;
        }
        if($gciResult.GetType() -eq [System.IO.FileInfo])
        {
            [void]$files.Add($gciResult);
        }
        else
        {
            [void]$files.AddRange($gciResult);
        }
        foreach($file in $files)
        {
            $isExcluded = $false;
            foreach($excluded in $ExcludeDirectories)
            {
                if($file.FullName.StartsWith($excluded))
                {
                    $isExcluded = $true;
                    break;
                }
            }
            if(-not $isExcluded) {
                [void]$results.Add([string]($file.FullName | Resolve-Path -Relative) );
            }
        }
    }

    return ,$results
}

function GetSourcesParam
{
    [OutputType([string])]
    Param(
        [parameter(Mandatory=$true)]
        [System.Collections.ArrayList]
        $SharpmakeFiles 
    )

    $SharpmakeFiles = ($SharpmakeFiles | ForEach-Object -Process { return $_.Replace("\", "/"); });
    $SharpmakeFiles = ($SharpmakeFiles | ForEach-Object -Process { return [string]::Format("'{0}'", $_); });
    
    return [string]::Format("/sources({0})", ($SharpmakeFiles -join ','));
}

if($Nuke)
{
    Write-Host "Nuke param passed. Deeply cleaning with Nuke.ps1."
    &"./Nuke.ps1"
}

# /
$ProjectRootDir = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($ScriptDir, "..", ".."));
# /generated
$GeneratedDir = [System.IO.Path]::Combine($ProjectRootDir, "generated");
# /generated/lateralus_vs2022_win64.sln
$LateralusSolution = [System.IO.Path]::Combine($GeneratedDir, "lateralus_vs2022_win64.sln");
# /generated
$DebugSlnDirectory = $GeneratedDir;
# /generated/lateralus_vs2022_win64.sln
$DebugSln = [System.IO.Path]::Combine($DebugSlnDirectory, "sharpmake_debugsolution.vs2022.sln");
# /BuildSystem
$BuildSystemDir = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($ProjectRootDir, "BuildSystem"));
# /ThirdParty
$ThirdPartyDir = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($ProjectRootDir, "ThirdParty"));
# /ThirdParty/Sharpmake
$SharpmakeDir = [System.IO.Path]::Combine($ThirdPartyDir, "Sharpmake");
# /ThirdParty/Sharpmake/bootstrap.bat
$SharpmakeBootstrap = [System.IO.Path]::Combine($SharpmakeDir, "bootstrap.bat");
# /ThirdParty/Sharpmake/CompileSharpmake.bat
$SharpmakeCompile = [System.IO.Path]::Combine($SharpmakeDir, "CompileSharpmake.bat");
# /ThirdParty/Sharpmake/Sharpmake.sln
$SharpmakeSolution = [System.IO.Path]::Combine($SharpmakeDir, "Sharpmake.sln");

$SharpmakeConfiguration = If($Debug) {"Debug"} else {"release"};
# /ThirdParty/Sharpmake/tmp/bin/{config}/net6.0/SharpMake.Application.exe
$SharpmakeApplication = [System.IO.Path]::Combine($SharpmakeDir, "tmp", "bin", $SharpmakeConfiguration, "net6.0", "SharpMake.Application.exe");

# input sharpmake file directories
$SharpmakeInputDirectories = @(
    $BuildSystemDir,
    [System.IO.Path]::Combine($ProjectRootDir, "Applications"),
    [System.IO.Path]::Combine($ProjectRootDir, "Engine")
);
# Directories to ignore when searching for sharpmake files.
$SharpmakeExcludeDirectories = New-Object System.Collections.ArrayList;

# Add every *.Sharpmake.cs from $SharpmakeInputDirectories
$SharpmakeInputFiles = (GetSharpmakeFilesFromDirectory -Directories $SharpmakeInputDirectories -ExcludeDirectories $SharpmakeExcludeDirectories);

if($Force -or -not [System.IO.File]::Exists($SharpmakeSolution))
{
    &$SharpmakeBootstrap
}

if($Force -or -not [System.IO.File]::Exists($SharpmakeApplication))
{
    &$SharpmakeCompile @("""$SharpmakeSolution""", """$SharpmakeConfiguration""", """Any CPU""")
}

$SharpmakeParams = [System.Collections.ArrayList]@(
    (GetSourcesParam -SharpmakeFiles $SharpmakeInputFiles)
);

if($Debug)
{
    [void]$SharpmakeParams.Add("/generateDebugSolution");
    New-Item -ItemType Directory -Force -Path $DebugSlnDirectory
    $DebugSlnDirectory = $DebugSlnDirectory | Resolve-Path -Relative;
    $DebugSlnDirectory = $DebugSlnDirectory.Replace("\", "/");
    [void]$SharpmakeParams.Add("/debugSolutionPath('$DebugSlnDirectory')");
}

if($Verbose)
{
    [void]$SharpmakeParams.Add("/verbose");
}

if($DisableMultithread)
{
    [void]$SharpmakeParams.Add("/Multithreaded(false)");
}

&$SharpmakeApplication $SharpmakeParams

if($Open)
{
    &Start-Process $LateralusSolution
    if($Debug)
    {
        &Start-Process $DebugSln
    }
}

Pop-Location #$ScriptDir

