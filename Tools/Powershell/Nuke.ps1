$ScriptDir = Split-Path $MyInvocation.MyCommand.Path;
Push-Location $ScriptDir;

function Prompt($title, $question)
{
    $choices  = @(
        (New-Object -Typename "System.Management.Automation.Host.ChoiceDescription" -ArgumentList @('&Yes', 'Delete all generated folders in the project.')),
        (New-Object -Typename "System.Management.Automation.Host.ChoiceDescription" -ArgumentList @('&No', 'Do nothing. Relax. Have a brew.'))
    )
    return $Host.UI.PromptForChoice($title, $question, $choices, 1) -eq 1
}

if (Prompt("Nuke?", "This script will delete all generated files in the project. Continue?")) {
    Pop-Location #$ScriptDir
    exit 0;    
}

$ProjectRoot = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($ScriptDir, "..", ".."));

$OldErrorActionPreference = $ErrorActionPreference
$ErrorActionPreference = "SilentlyContinue"

# Delete all generated folders
foreach($dir in (Get-ChildItem $ProjectRoot -Recurse -Directory | Where-Object {$_.name -like "generated"} | ForEach-Object { $_.fullname }))
{
    Remove-Item -Path $dir -Force -Recurse -ErrorAction SilentlyContinue
}

# Delete the tmp directory that gets created when building sharpmake
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "Tools", "Powershell", "tmp")) -Force -Recurse -ErrorAction SilentlyContinue

# Delete the sharpmake build directories and solution
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "ThirdParty", "Sharpmake", "tmp")) -Force -Recurse -ErrorAction SilentlyContinue
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "ThirdParty",  "Sharpmake", "Sharpmake.sln")) -Force -ErrorAction SilentlyContinue

# Delete all imgui.ini files
foreach($dir in (Get-ChildItem $ProjectRoot -Recurse -File | Where-Object {$_.name -like "imgui.ini"} | ForEach-Object { $_.fullname }))
{
    Remove-Item -Path $dir -Force -ErrorAction SilentlyContinue
}

$ErrorActionPreference = $OldErrorActionPreference

Pop-Location #$ScriptDir