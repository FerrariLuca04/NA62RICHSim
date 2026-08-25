# Configuration reference

This document lists the keywords supported by the NA62RICHSim configuration files.

## Detector configuration

The following keywords can be used in detector configuration files such as:

```text
config/detector_default.conf
```

| Keyword | Type / format | Unit | Description | Constraints |
|---|---|---|---|---|
| `world_length` | `double` | length | Length of the world volume | Must be > 0 |
| `world_height` | `double` | length | Height of the world volume | Must be > 0 |
| `gas_length` | `double` | length | Length of the gas vessel along the z axis | Must be > 0 |
| `gas_radius` | `double` | length | Radius of the gas vessel | Must be > 0 |
| `mirror_curvature_radius` | `double` | length | Radius of curvature of the spherical mirror | Must be > 0 |
| `mirror_outer_radius` | `double` | length | Outer radius of the mirror | Must be > 0 and ≤ `gas_radius` |
| `mirror_inner_radius` | `double` | length | Inner radius of the mirror | Must be ≥ 0 and ≤ `mirror_outer_radius` |
| `mirror_thickness` | `double` | length | Mirror thickness | Must be > 0 and < `mirror_curvature_radius` |
| `mirror_position_z` | `double` | length | Mirror position along the z axis | Must satisfy `abs(mirror_position_z) ≤ gas_length / 2` |
| `PMT_radius` | `double` | length | Radius of each photomultiplier tube | Must be > 0 and ≤ `disk_radius` |
| `disk_radius` | `double` | length | Radius of the PMT support disk | Must be > 0 |
| `disk_thick` | `double` | length | Thickness of the PMT support disk | Must be > 0 |
| `disk_position_z` | `double` | length | Position of the PMT support disk along the z axis | Must satisfy `abs(disk_position_z) ≤ gas_length / 2` |
| `disk_position_r` | `double` | length | Radial position of the PMT support disk | Must be ≥ 0; the disk must remain inside the gas volume and must not cross the detector axis |
| `world_material` | `string` | — | Geant4 material used for the world volume | — |
| `gas_material` | `string` | — | Geant4 material used for the radiator gas | — |
| `mirror_material` | `string` | — | Geant4 material used for the mirror | — |
| `PMT_material` | `string` | — | Geant4 material used for the photomultiplier tubes | — |
| `gas_photon_energies` | `list<double>` | energy | Photon-energy grid used for the gas optical properties | Values must be > 0 and strictly increasing |
| `gas_refractive_index` | `list<double>` | — | Refractive-index values corresponding to `gas_photon_energies` | Each value must be ≥ 1; size must match `gas_photon_energies` |
| `mirror_photon_energies` | `list<double>` | energy | Photon-energy grid used for mirror optical properties | Values must be > 0 and strictly increasing |
| `mirror_reflectivity` | `list<double>` | — | Mirror reflectivity corresponding to `mirror_photon_energies` | Each value must be in [0, 1]; size must match `mirror_photon_energies` |
| `PMT_photon_energies` | `list<double>` | energy | Photon-energy grid used for the PMT optical response | Values must be > 0 and strictly increasing |
| `PMT_efficiency` | `list<double>` | — | Photon-detection efficiency corresponding to `PMT_photon_energies` | Each value must be in [0, 1]; size must match `PMT_photon_energies` |

### Additional geometrical constraints

The PMT support disk must remain entirely inside the gas volume:

`disk_position_r + disk_radius <= gas_radius`

and it must not cross the detector axis:

`disk_position_r - disk_radius >= 0`

### Optical-property vectors

Each optical-property vector must contain the same number of elements as its corresponding photon-energy vector.

Photon-energy values must be positive and strictly increasing.

Reflectivity and PMT efficiency values represent probabilities and must therefore lie in the interval `[0, 1]`.

## Primary generator configuration

The following keywords can be used in primary generator configuration files such as:

```text
config/generator_default.conf
```

| Keyword | Type / format | Unit | Description | Constraints |
|---|---|---|---|---|
| `entrance_radius_min` | `double` | length | Minimum radial coordinate of the primary-particle entrance point | Must be ≥ 0 and < `entrance_radius_max` |
| `entrance_radius_max` | `double` | length | Maximum radial coordinate of the primary-particle entrance point | Must be > 0 and > `entrance_radius_min` |
| `entrance_phi_min` | `double` | angle | Minimum azimuthal angle of the primary-particle entrance point | Must be ≥ 0, < `entrance_phi_max`, and < 360° |
| `entrance_phi_max` | `double` | angle | Maximum azimuthal angle of the primary-particle entrance point | Must be > 0, > `entrance_phi_min`, and ≤ 360° |
| `decay_region_start` | `double` | length | Longitudinal position defining the start of the virtual decay region | Must be > 0 and > `decay_region_length` |
| `decay_region_length` | `double` | length | Length of the virtual decay region | Must be > 0 and < `decay_region_start` |
| `decay_region_sigma_x` | `double` | length | Standard deviation of the virtual decay-vertex distribution along the x axis | Must be ≥ 0 |
| `decay_region_sigma_y` | `double` | length | Standard deviation of the virtual decay-vertex distribution along the y axis | Must be ≥ 0 |
| `particle_momentum_min` | `double` | energy | Minimum primary-particle momentum | Must be ≥ 0 and < `particle_momentum_max` |
| `particle_momentum_max` | `double` | energy | Maximum primary-particle momentum | Must be > 0 and > `particle_momentum_min` |
| `particle_type` | `string` | — | Geant4 particle name used for primary-particle generation | Must identify a valid particle |

> [!WARNING]
>
> All configuration parameters are required and cannot be left empty.
>
> Vector-valued parameters must contain at least one element.
>
> Length, angle, and momentum parameters must include a Geant4-compatible unit, such as `mm`, `cm`, `m`, `deg`, or `GeV`.
>
> Configuration keywords are case-sensitive.
