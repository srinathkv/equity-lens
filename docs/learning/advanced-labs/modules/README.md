# C++20 Modules Lab (MSVC)

Modules require a multi-step build: compile the interface to an IFC before compiling the consumer. They are not supported by the single-translation-unit lab runner. Run these commands from the repository root in Developer PowerShell for Visual Studio:

```powershell
$root = 'docs\learning\advanced-labs'
$out = Join-Path $env:TEMP 'equitylens-cpp20-modules'
New-Item -ItemType Directory -Force -Path $out | Out-Null
cl /nologo /std:c++20 /EHsc /experimental:module /interface "/ifcOutput$out\" "/Fo$out\quote_values.obj" /c "$root\cpp20-modules-reference.cpp"
if ($LASTEXITCODE -ne 0) { throw 'Module interface compilation failed' }
cl /nologo /std:c++20 /EHsc /experimental:module "/referencequote_values=$out\quote_values.ifc" "/Fo$out\consumer.obj" /c "$root\cpp20-modules-consumer.cpp"
if ($LASTEXITCODE -ne 0) { throw 'Module consumer compilation failed' }
$link = Get-Command link.exe -ErrorAction Stop
& $link.Source /nologo "/out:$out\modules-lab.exe" "$out\consumer.obj" "$out\quote_values.obj"
if ($LASTEXITCODE -ne 0) { throw 'Module link failed' }
& "$out\modules-lab.exe"
exit $LASTEXITCODE
```

The starter intentionally returns a placeholder close, so the compiled consumer exits with failure. Complete the implementation and run both builds to get exit code zero. Change `ModuleQuote` by adding a validation function, export it, and update the consumer. This lab is compiler/build-specific; retain header-based APIs where a toolchain's module support is unavailable or incompatible with the project's build system.
