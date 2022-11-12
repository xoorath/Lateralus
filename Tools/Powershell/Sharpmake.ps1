param(
    [switch] $Verbose,
    [switch] $Debug,
    [switch] $Force
)

$ScriptDir = Split-Path $MyInvocation.MyCommand.Path
Push-Location $ScriptDir

$SharpmakeDir = [System.IO.Path]::Combine($ScriptDir, "..", "..", "ThirdParty", "Sharpmake");
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
$SharpmakeInputFiles = $SharpmakeInputDirectories | ForEach-Object -Process {((Get-ChildItem -Path $_ -Filter *.Sharpmake.cs -Recurse -File) | ForEach-Object -Process { $_.FullName })} | Resolve-Path -Relative; 

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

