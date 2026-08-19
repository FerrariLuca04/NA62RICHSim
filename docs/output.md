# Output format

NA62RICHSim stores simulation results in ROOT files.

By default, the output file is created at:

```text
output/data_na62rich_sim.root
```

The file contains a `TTree` named `PhotonHits`.

## PhotonHits TTree

The `PhotonHits` TTree contains one entry for each simulated Geant4 event.

Each entry stores:

* the event identifier;
* the properties of the generated primary particle;
* the position of the primary vertex;
* the identifiers and positions of the hit photosensors;
* the position and energy of each detected optical photon.

### Branch structure

| Branch                 | Type                  |  Unit | Description                                                  |
| :--------------------- | :-------------------- | :---: | :----------------------------------------------------------- |
| `eventID`              | `Int_t`               |   —   | Identifier of the simulated event                            |
| `primary_pdg_code`     | `Int_t`               |   —   | PDG code of the primary particle                             |
| `primary_momentum_GeV` | `Double_t`            | GeV/c | Magnitude of the primary-particle momentum                   |
| `primary_dir_x`        | `Double_t`            |   —   | x component of the primary-particle direction                |
| `primary_dir_y`        | `Double_t`            |   —   | y component of the primary-particle direction                |
| `primary_dir_z`        | `Double_t`            |   —   | z component of the primary-particle direction                |
| `primary_vertex_x_mm`  | `Double_t`            |   mm  | x coordinate of the primary-particle generation vertex       |
| `primary_vertex_y_mm`  | `Double_t`            |   mm  | y coordinate of the primary-particle generation vertex       |
| `primary_vertex_z_m`   | `Double_t`            |   m   | z coordinate of the primary-particle generation vertex       |
| `sensorIDs`            | `std::vector<int>`    |   —   | IDs of the photosensors associated with the detected photons |
| `sensor_pos_x_mm`      | `std::vector<double>` |   mm  | x coordinates of the hit photosensors                        |
| `sensor_pos_y_mm`      | `std::vector<double>` |   mm  | y coordinates of the hit photosensors                        |
| `hit_x_mm`             | `std::vector<double>` |   mm  | x coordinates of the photon hits                             |
| `hit_y_mm`             | `std::vector<double>` |   mm  | y coordinates of the photon hits                             |
| `hit_energy_eV`        | `std::vector<double>` |   eV  | Energies of the detected optical photons                     |

## Event organization

Scalar branches describe quantities that are defined once for each simulated event:

```text
eventID
primary_pdg_code
primary_momentum_GeV

primary_dir_x
primary_dir_y
primary_dir_z

primary_vertex_x_mm
primary_vertex_y_mm
primary_vertex_z_m
```

Hit-related quantities are stored in vectors because an event can produce an arbitrary number of detected optical photons:

```text
sensorIDs[]
sensor_pos_x_mm[]
sensor_pos_y_mm[]

hit_x_mm[]
hit_y_mm[]
hit_energy_eV[]
```

All hit-related vectors have the same size. Elements with the same index refer to the same detected optical photon.

For example:

```text
sensorIDs[0]
sensor_pos_x_mm[0]
sensor_pos_y_mm[0]
hit_x_mm[0]
hit_y_mm[0]
hit_energy_eV[0]
```

all describe the same photon detection.

Therefore, for an event containing `N` detected photons:

```text
sensorIDs.size()
    = sensor_pos_x_mm.size()
    = sensor_pos_y_mm.size()
    = hit_x_mm.size()
    = hit_y_mm.size()
    = hit_energy_eV.size()
    = N
```

An event with no detected optical photons is still represented by one TTree entry, with empty hit vectors.

## Example

A simplified event may contain:

```text
eventID              = 42
primary_pdg_code     = 211
primary_momentum_GeV = 35.0

primary_dir          = (0.012, -0.003, 0.999)

sensorIDs            = [14, 28, 31]
sensor_pos_x_mm      = [...]
sensor_pos_y_mm      = [...]

hit_x_mm             = [...]
hit_y_mm             = [...]
hit_energy_eV        = [3.12, 2.87, 3.35]
```

In this example, event `42` contains three detected optical photons.
