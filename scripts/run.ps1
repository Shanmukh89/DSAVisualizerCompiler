Param(
    [switch]$Regenerate
)

$ErrorActionPreference = 'Stop'

# Paths: script lives in scripts/, project root is parent
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$root = Split-Path -Parent $scriptDir
Set-Location $root

# Generate parser/lexer if requested or missing
if ($Regenerate -or -not (Test-Path "$root\parser.tab.c") -or -not (Test-Path "$root\lex.yy.c")) {
    Write-Host "[gen] Running win_bison and win_flex" -ForegroundColor Cyan
    & win_bison -d src\parser.y
    & win_flex -o lex.yy.c src\lexer.l
}

# Build with strict flags (sources in src/)
Write-Host "[build] Compiling with warnings as errors" -ForegroundColor Cyan
$gccArgs = @(
    '-std=c11','-Wall','-Wextra','-Werror','-Isrc',
    '-o','dsa_compiler',
    'parser.tab.c','lex.yy.c','src\ast.c','src\ir.c','src\codegen.c','src\main.c',
    '-lfl'
)
& gcc @gccArgs

# Test discovery
$inputsDir = Join-Path $root 'tests/inputs'
$expectedDir = Join-Path $root 'tests/expected'
$actualDir = Join-Path $root 'tests/actual'
if (-not (Test-Path $actualDir)) { New-Item -ItemType Directory -Path $actualDir | Out-Null }

$failures = @()
$inputs = Get-ChildItem $inputsDir -Filter *.algo
foreach ($inFile in $inputs) {
    $name = [System.IO.Path]::GetFileNameWithoutExtension($inFile.Name)
    Write-Host "[test] $name" -ForegroundColor Yellow

    # run compiler
    .\dsa_compiler.exe $inFile.FullName | Out-Null

    $outPath = Join-Path $root 'output.json'
    if (-not (Test-Path $outPath)) {
        Write-Host "  ! output.json not found" -ForegroundColor Red
        $failures += $name
        continue
    }

    $actualPath = Join-Path $actualDir ("$name.json")
    Copy-Item $outPath $actualPath -Force

    $expectedPath = Join-Path $expectedDir ("$name.json")
    if (-not (Test-Path $expectedPath)) {
        Write-Host "  ! expected not found: $expectedPath" -ForegroundColor Red
        $failures += $name
        continue
    }

    $diff = Compare-Object -ReferenceObject (Get-Content $expectedPath) -DifferenceObject (Get-Content $actualPath)
    if ($diff) {
        Write-Host "  ✖ Diff found" -ForegroundColor Red
        $diff | Format-Table | Out-String | Write-Host
        $failures += $name
    } else {
        Write-Host "  ✔ Passed" -ForegroundColor Green
    }
}

if ($failures.Count -gt 0) {
    Write-Host "`nFailed: $($failures -join ', ')" -ForegroundColor Red
    exit 1
} else {
    Write-Host "`nAll tests passed" -ForegroundColor Green
}
