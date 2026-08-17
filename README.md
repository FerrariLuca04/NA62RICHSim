# NA62RICHSim

Monte Carlo simulation of the NA62 RICH detector based on Geant4.

The project aims to reproduce the main components and response of the NA62 Ring Imaging Cherenkov detector and to study its particle identification performance.

In addition, a secondary goal is to allow users to configure and test custom RICH detector setups inspired by the NA62 design.

> This project is developed for educational and simulation studies and is not an official NA62 software package.

## Structure

```text
NA62RICHSim/
├── app/               # Application entry points
├── config/            # Detector and simulation configuration files
├── docs/              # Logbook and changelog
├── include/na62rich/  # Public C++ headers
├── macros/            # Geant4 macro files
├── src/               # C++ implementation files
├── tests/             # Unit and integration tests
└── tools/             # Utility programs and scripts
```

## Requirements

* C++17-compatible compiler
* CMake
* Geant4 11.x
* ROOT

> The Geant4 installation must include support for optical physics and visualization.

## Compiling

### Clone the repository

Clone the Git repository using the following commands:

```bash
git clone https://github.com/FerrariLuca04/NA62RICHSim
cd NA62RICHSim
```

### Build

From the project root directory (`NA62RICHSim`, if you cloned the GitHub repository), run:

```bash
cmake -S . -B build
cmake --build build -j
```

### Run

The executable will be generated at:

```text
build/bin/na62_rich_sim
```

To start the simulation using the Geant4 interactive UI, run:

```bash
./build/bin/na62_rich_sim
```

Alternatively, the simulation can be executed in batch mode using a Geant4 macro:

```bash
./build/bin/na62_rich_sim path/to/macro.mac
```

> The `macros/` directory contains some preconfigured macros that can be used as examples or starting points.

### Rebuilding the project

After modifying the source code, you usually only need to run:

```bash
cmake --build build -j
```

CMake will automatically recompile only the files affected by the changes.

To perform a clean build, remove the entire `build` directory, configure the project again, and rebuild it:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j
```

## Output file

In `include/na62rich/io/OutputPaths.hh` is selected `output/` as default directory for output files.

During the execution of the program, will be created `output/tmp/` for temporary files, removed at the end of the program.

The default output file is `output/data_na62rich_sim.root` and contain a `TTree` named `PhotonHits`.

### TTree structure

