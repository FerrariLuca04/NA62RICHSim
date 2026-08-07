Educational Geant4 simulation inspired by the NA62 RICH detector. This project is not official NA62 software.

# NA62RICHSim

## Build

From the project root directory (`NA62RICHSim`, if you cloned the GitHub repository), run the following commands:

```bash
cmake -S . -B build
cmake --build build -j
```

### Rebuilding the project

After modifying the source code, you usually only need to run:

```bash
cmake --build build -j
```

CMake will automatically recompile only the files that have changed.

If you want to perform a clean build, remove the entire `build` directory and configure the project again:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j
```
