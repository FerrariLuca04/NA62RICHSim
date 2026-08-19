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

### Rebuilding the project

After modifying the source code, you usually only need to run:

```bash
cmake --build build -j
```

CMake will automatically recompile only the files affected by the changes.

To perform a clean build, remove the entire `build/` directory, configure the project again, and rebuild it:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j
```

## Run

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
./build/bin/na62_rich_sim macro.mac
```

or:

```bash
./build/bin/na62_rich_sim --macro macro.mac
```

> Macro files (`.mac`) must be located in the `macros/` directory.


## Output file

The default output directory is defined as `output/` in `include/na62rich/io/Paths.hh`.

During program execution, the `output/tmp/` directory is created for temporary files and removed when the program terminates.

The default output file is:

```text
output/data_na62rich_sim.root
```

A different output file name can be specified using the `--output` argument:

```bash
./build/bin/na62_rich_sim --output name_file.root
```

Alternatively, a `.root` file name can be passed directly as a command-line argument:

```bash
./build/bin/na62_rich_sim name_file.root
```

The file contains a `TTree` named `PhotonHits`, with one entry for each simulated event. Each entry stores the primary-particle properties and the optical-photon hits detected during the event.

For the complete output data structure, see [Output format](docs/output.md).

## Detector configuration

The default detector configuration is stored in:

```text
config/detector_default.conf
```

This file contains the geometrical and material parameters used to construct the detector.

You can modify the configuration file and run the program again without recompiling it. This allows you to change the detector geometry and materials independently of the source code.

You can also create a custom `.conf` file and select it using the `--config-detector` argument:

```bash
./build/bin/na62_rich_sim --config-detector name_file.conf
```

Alternatively, a `.conf` file can be passed directly as a command-line argument:

```bash
./build/bin/na62_rich_sim name_file.conf
```

> Config files (`.conf`) must be located in the `config/` directory.