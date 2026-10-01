param(
	[ValidateSet('Test', 'Run', 'All')]
	[string]$Action = 'All'
)

$ErrorActionPreference = 'Stop'
$compiler = Get-Command cl.exe -ErrorAction Stop
$projectRoot = $PSScriptRoot
$include = Join-Path $projectRoot 'include'
$sourceFiles = @(
	(Join-Path $projectRoot 'src\Quote.cpp'),
	(Join-Path $projectRoot 'src\Parser.cpp'),
	(Join-Path $projectRoot 'src\Analysis.cpp')
)
$outputDirectory = Join-Path $env:TEMP 'equitylens-quote-analyzer-project'
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null

function Build-Target {
	param(
		[string]$Name,
		[string]$EntryPoint
	)

	$outputPath = Join-Path $outputDirectory "$Name.exe"
	$arguments = @('/nologo', '/std:c++17', '/EHsc', '/W4', "/I$include", "/Fe:$outputPath") + $sourceFiles + $EntryPoint
	& $compiler.Source @arguments | Out-Host
	if ($LASTEXITCODE -ne 0) {
		exit $LASTEXITCODE
	}
	return $outputPath
}

if ($Action -in @('Test', 'All')) {
	$testExecutable = Build-Target -Name 'QuoteAnalyzerTests' -EntryPoint (Join-Path $projectRoot 'tests\QuoteAnalyzerTests.cpp')
	& $testExecutable
	if ($LASTEXITCODE -ne 0) {
		exit $LASTEXITCODE
	}
}

if ($Action -in @('Run', 'All')) {
	$appExecutable = Build-Target -Name 'QuoteAnalyzer' -EntryPoint (Join-Path $projectRoot 'src\main.cpp')
	& $appExecutable
	exit $LASTEXITCODE
}

exit 0
