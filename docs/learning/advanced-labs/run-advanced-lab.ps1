param(
	[Parameter(Mandatory = $true)]
	[ValidateSet('cpp11-templates', 'cpp11-templates-starter', 'cpp11-polymorphism', 'cpp11-polymorphism-starter', 'cpp11-containers', 'cpp11-containers-starter', 'cpp11-publication', 'cpp11-publication-starter', 'cpp11-exception-safety', 'cpp11-exception-safety-starter', 'cpp17-filesystem', 'cpp17-filesystem-starter', 'cpp20-barrier', 'cpp20-barrier-starter', 'cpp20-coroutine', 'cpp20-coroutine-starter', 'cpp20-coroutine-errors', 'cpp20-coroutine-errors-starter', 'cpp23-mdspan', 'cpp23-mdspan-starter', 'cpp26-ranges', 'cpp26-ranges-starter')]
	[string]$Lab,
	[Parameter(Mandatory = $true)]
	[ValidateSet('c++14', 'c++17', 'c++20', 'c++latest')]
	[string]$Standard
)

$ErrorActionPreference = 'Stop'
$compiler = Get-Command cl.exe -ErrorAction Stop
$sourceName = "$Lab.cpp"
$sourcePath = Join-Path $PSScriptRoot $sourceName
if (-not (Test-Path -LiteralPath $sourcePath)) {
	$sourcePath = Join-Path $PSScriptRoot "$Lab-starter.cpp"
}
if (-not (Test-Path -LiteralPath $sourcePath)) {
	throw "No reference or starter source found for lab '$Lab'."
}
$outputDirectory = Join-Path $env:TEMP 'equitylens-cpp-advanced-labs'
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
$outputPath = Join-Path $outputDirectory "$Lab.exe"
if (Test-Path -LiteralPath $outputPath) {
	Remove-Item -LiteralPath $outputPath -Force
}
$includePath = Join-Path $PSScriptRoot '..\exercises'
& $compiler.Source /nologo "/std:$Standard" /EHsc /W4 "/I$includePath" "/Fe:$outputPath" $sourcePath | Out-Host
if ($LASTEXITCODE -ne 0) {
	exit $LASTEXITCODE
}
& $outputPath
exit $LASTEXITCODE
