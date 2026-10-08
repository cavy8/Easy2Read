#Requires -Version 5.1
<#
.SYNOPSIS
Builds Easy2Read and creates a mod-manager-ready release archive.
.EXAMPLE
.\build-release.ps1
.EXAMPLE
.\build-release.ps1 -Suffix test -IncludeSymbols
#>
[CmdletBinding()]
param(
    [string]$BuildDirectory = 'build/release-package',
    [string]$OutputDirectory = 'dist',
    [ValidatePattern('^[A-Za-z0-9._-]*$')]
    [string]$Suffix = '',
    [switch]$IncludeSymbols
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Resolve-RepoPath([string]$Path) {
    if ([IO.Path]::IsPathRooted($Path)) {
        return [IO.Path]::GetFullPath($Path)
    }
    return [IO.Path]::GetFullPath((Join-Path $PSScriptRoot $Path))
}

function Invoke-CMake([string[]]$Arguments) {
    & cmake @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "CMake failed (exit $LASTEXITCODE). No new release archives were created."
    }
}

function Write-Archive([string]$Source, [string]$Name) {
    $destination = Join-Path $outputPath "$Name.zip"
    $temporary = Join-Path $stagePath "$Name.zip"
    [IO.Compression.ZipFile]::CreateFromDirectory(
        $Source, $temporary, [IO.Compression.CompressionLevel]::Optimal, $false)
    Move-Item -LiteralPath $temporary -Destination $destination -Force
    Write-Host "Created $destination"
    $stream = [IO.File]::OpenRead($destination)
    $sha256 = [Security.Cryptography.SHA256]::Create()
    try {
        $hash = [BitConverter]::ToString($sha256.ComputeHash($stream)).Replace('-', '')
        Write-Host "  SHA256: $hash"
    }
    finally {
        $sha256.Dispose()
        $stream.Dispose()
    }
}

Get-Command cmake -ErrorAction Stop | Out-Null
if (-not $env:VCPKG_ROOT -or
    -not (Test-Path -LiteralPath (Join-Path $env:VCPKG_ROOT 'scripts/buildsystems/vcpkg.cmake'))) {
    throw 'Set VCPKG_ROOT to your vcpkg installation before running this script.'
}

$buildPath = Resolve-RepoPath $BuildDirectory
$outputPath = Resolve-RepoPath $OutputDirectory
# Keep generated files out of the tree that is copied into the archive.
$assetPath = Resolve-RepoPath 'Data'
$assetPrefix = $assetPath + [IO.Path]::DirectorySeparatorChar
foreach ($generatedPath in @($buildPath, $outputPath)) {
    if ($generatedPath.Equals($assetPath, [StringComparison]::OrdinalIgnoreCase) -or
        $generatedPath.StartsWith($assetPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Build and output directories must be outside Data.'
    }
}
$versionMatch = [regex]::Match(
    (Get-Content -LiteralPath (Join-Path $PSScriptRoot 'CMakeLists.txt') -Raw),
    '(?m)^set\(PROJECT_VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)\)')
if (-not $versionMatch.Success) {
    throw 'Could not read PROJECT_VERSION from CMakeLists.txt.'
}
$archiveName = "Easy2Read-$($versionMatch.Groups[1].Value)"
if ($Suffix) { $archiveName += "-$Suffix" }

# Configure afresh so cached post-build commands cannot install into the game.
$previousModsFolder = $env:SKYRIM_MODS_FOLDER
$stagePath = $null
Push-Location -LiteralPath $PSScriptRoot
try {
    Remove-Item Env:SKYRIM_MODS_FOLDER -ErrorAction SilentlyContinue
    Invoke-CMake @('--preset', 'default', '-B', $buildPath)
    Invoke-CMake @('--build', $buildPath, '--config', 'Release')

    $dllPath = Join-Path $buildPath 'Release/Easy2Read.dll'
    $pdbPath = Join-Path $buildPath 'Release/Easy2Read.pdb'
    if (-not (Test-Path -LiteralPath $dllPath -PathType Leaf)) {
        throw "Release DLL missing: $dllPath"
    }
    if ($IncludeSymbols -and -not (Test-Path -LiteralPath $pdbPath -PathType Leaf)) {
        throw "Release PDB missing: $pdbPath"
    }

    New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
    $stagePath = Join-Path $outputPath ('.stage-' + [guid]::NewGuid().ToString('N'))
    $mainPath = Join-Path $stagePath 'main'
    New-Item -ItemType Directory -Path $mainPath -Force | Out-Null
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'Data/SKSE') -Destination $mainPath -Recurse
    # Only the freshly built DLL belongs in the package, never checked-in binaries.
    foreach ($binary in Get-ChildItem -LiteralPath $mainPath -Recurse -File |
        Where-Object { $_.Extension -in '.dll', '.pdb' }) {
        Remove-Item -LiteralPath $binary.FullName -Force
    }
    Copy-Item -LiteralPath $dllPath -Destination (Join-Path $mainPath 'SKSE/Plugins/Easy2Read.dll')
    foreach ($document in @('README.md', 'CHANGELOG.md', 'LICENSE')) {
        Copy-Item -LiteralPath (Join-Path $PSScriptRoot $document) -Destination $mainPath
    }

    Add-Type -AssemblyName System.IO.Compression.FileSystem
    Write-Archive $mainPath $archiveName
    if ($IncludeSymbols) {
        $symbolsPath = Join-Path $stagePath 'symbols'
        New-Item -ItemType Directory -Path $symbolsPath | Out-Null
        Copy-Item -LiteralPath $pdbPath -Destination $symbolsPath
        Write-Archive $symbolsPath "$archiveName-Symbols"
    }
}
finally {
    $env:SKYRIM_MODS_FOLDER = $previousModsFolder
    Pop-Location
    if ($stagePath -and (Test-Path -LiteralPath $stagePath)) {
        # Delete only this invocation's staging directory inside the output folder.
        $resolvedStage = [IO.Path]::GetFullPath($stagePath)
        $outputPrefix = $outputPath.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
        if (-not $resolvedStage.StartsWith($outputPrefix, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Refusing to remove staging directory outside $outputPath"
        }
        Remove-Item -LiteralPath $resolvedStage -Recurse -Force
    }
}

