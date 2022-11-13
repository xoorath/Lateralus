param(
    [switch] $Verbose,
    [switch] $Debug,
    [switch] $Force
)

$ScriptDir = Split-Path $MyInvocation.MyCommand.Path;
Push-Location $ScriptDir;

$ThirdPartyDirectory = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($ScriptDir, "..", "..", "ThirdParty"));
$SharpmakeDir = [System.IO.Path]::Combine($ThirdPartyDirectory, "Sharpmake");
$SharpmakeBootstrap = [System.IO.Path]::Combine($SharpmakeDir, "bootstrap.bat");
$SharpmakeCompile = [System.IO.Path]::Combine($SharpmakeDir, "CompileSharpmake.bat");
$SharpmakeSolution = [System.IO.Path]::Combine($SharpmakeDir, "Sharpmake.sln");
$SharpmakeConfiguration = "release";
if($Debug)
{
    $SharpmakeConfiguration = "debug";
}
$SharpmakeApplication = [System.IO.Path]::Combine($SharpmakeDir, "tmp", "bin", $SharpmakeConfiguration, "net6.0", "SharpMake.Application.exe");

# inputs to sharpmake
$SharpmakeInputDirectories = @(
    [System.IO.Path]::Combine($ScriptDir, "..", "Sharpmake"),
    [System.IO.Path]::Combine($ScriptDir, "..", "..", "Applications"),
    [System.IO.Path]::Combine($ScriptDir, "..", "..", "Engine")
);
$SharpmakeInputFiles = [System.Collections.ArrayList]@();

# Add every *.Sharpmake.cs from $SharpmakeInputDirectories
$SharpmakeFilePaths = ($SharpmakeInputDirectories | ForEach-Object -Process {((Get-ChildItem -Path $_ -Filter *.Sharpmake.cs -Recurse -File) | ForEach-Object -Process { $_.FullName })} | Resolve-Path -Relative);
if($SharpmakeFilePaths.GetType() -eq [string])
{
    $SharpmakeInputFiles.Add($SharpmakeFilePaths);
}
else
{
    $SharpmakeInputFiles.AddRange($SharpmakeFilePaths);
}


# Add every *.Sharpmake.cs file from the ThirdParty directory except for sharpmake itself.
$thirdPartySharpmakeFiles = (Get-ChildItem -Path $ThirdPartyDirectory -Filter *.Sharpmake.cs -Recurse -File | ForEach-Object -Process { if(-not (($_.FullName).StartsWith($SharpmakeDir, [StringComparison]::OrdinalIgnoreCase))) { $_.FullName } });
# If there is only result: $thirdPartySharpmakeFiles will be a string and not an array.
if($thirdPartySharpmakeFiles.GetType() -eq [string])
{
    $SharpmakeInputFiles.Add($thirdPartySharpmakeFiles);
}
else
{
    $SharpmakeInputFiles.AddRange($thirdPartySharpmakeFiles);
}


if($Force -or -not [System.IO.File]::Exists($SharpmakeSolution))
{
    &$SharpmakeBootstrap
}
else
{
    Write-Host "Skipping sharpmake bootstrap.";
}

if($Force -or -not [System.IO.File]::Exists($SharpmakeApplication))
{
    &$SharpmakeCompile @("""$SharpmakeSolution""", """$SharpmakeConfiguration""", """Any CPU""")
}
else
{
    Write-Host "Skipping sharpmake compile.";
}

$SharpmakeParams = [System.Collections.ArrayList]@(
    [string]::Format("/sources({0})", [string]::Join(",", ($SharpmakeInputFiles | ForEach-Object -Process { return [string]::Format("@'{0}'", $_.Replace("\", "/")  ); })))
);

if($Debug)
{
    $SharpmakeParams.Add("/generateDebugSolution");  
}

if($Verbose)
{
    $SharpmakeParams.Add("/verbose");
}

&$SharpmakeApplication $SharpmakeParams

Pop-Location #$ScriptDir

