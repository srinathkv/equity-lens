# C++17 Lab: Filesystem, Streams, and Error Boundaries

## Worked example

`std::filesystem::path` composes platform paths; it does not create a file. Opening a stream is a separate fallible operation. Check stream state after opening and writing, close the output before opening input, and use `std::getline` to process records. A temporary artifact is removed even when the test finishes; production cleanup should also be protected by RAII if intervening operations can throw.

## Lab

The exercise writes three deterministic lines under the system temporary directory, checks output/input stream state, reads non-empty records, and removes the file. Implement `roundTripLines`:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp17-filesystem-starter -Standard c++17
```

Reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp17-filesystem -Standard c++17
```

Expected: `C++17 Filesystem and Streams: All 2 checks passed.` Add a test for an unwritable or nonexistent parent path and decide whether the API should return an error code, optional result, or throw.
