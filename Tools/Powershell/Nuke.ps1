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

# Delete debug solution and it's output
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "Tools",  "Sharpmake", "obj")) -Force -Recurse -ErrorAction SilentlyContinue
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "Tools",  "Sharpmake", "output")) -Force -Recurse -ErrorAction SilentlyContinue
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "Tools",  "Sharpmake", "properties")) -Force -Recurse -ErrorAction SilentlyContinue
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "Tools",  "Sharpmake", "sharpmake_debugsolution.vs2022.sln")) -Force -ErrorAction SilentlyContinue
Remove-Item -Path ([System.IO.Path]::Combine($ProjectRoot, "Tools",  "Sharpmake", "sharpmake_debug.vs2022.csproj")) -Force -ErrorAction SilentlyContinue

# Delete all imgui.ini files
foreach($dir in (Get-ChildItem $ProjectRoot -Recurse -File | Where-Object {$_.name -like "imgui.ini"} | ForEach-Object { $_.fullname }))
{
    Remove-Item -Path $dir -Force -ErrorAction SilentlyContinue
}

Pop-Location #$ScriptDir