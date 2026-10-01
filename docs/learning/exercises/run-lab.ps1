param(
	[Parameter(Mandatory = $true)]
	[ValidateSet('c++14', 'c++17', 'c++20', 'c++latest')]
	# c++latest supports experimental C++23/C++26 on MSVC; feature-test macros select fallbacks
	[string]$Standard,
	[Parameter(Mandatory = $true)]
	[string]$Source
)

$ErrorActionPreference = 'Stop'
$compiler = Get-Command cl.exe -ErrorAction Stop
$sourcePath = (Resolve-Path -LiteralPath $Source).Path
$outputDirectory = Join-Path $env:TEMP 'equitylens-cpp-learning-labs'
$outputPath = Join-Path $outputDirectory ([IO.Path]::GetFileNameWithoutExtension($sourcePath) + '.exe')

New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
& $compiler.Source /nologo "/std:$Standard" /EHsc /W4 "/Fe:$outputPath" $sourcePath
if ($LASTEXITCODE -ne 0) {
	exit $LASTEXITCODE
}

& $outputPath
exit $LASTEXITCODE
