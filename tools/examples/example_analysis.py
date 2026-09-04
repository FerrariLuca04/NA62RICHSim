import na62rich_analysis as rich

import matplotlib.pyplot as plt
import numpy as np


SIMULATION_NAME = "tools/examples/example.root"


# ---------------------------------------------------------
# Load simulation data
# ---------------------------------------------------------

events = rich.load_events(
    SIMULATION_NAME
)


# ---------------------------------------------------------
# Load detector configuration
# ---------------------------------------------------------

detector = rich.load_events(
    SIMULATION_NAME,
    tree_name="DetectorConfig",
)


# ---------------------------------------------------------
# Load generator configuration
# ---------------------------------------------------------

generator = rich.load_events(
    SIMULATION_NAME,
    tree_name="PrimaryGeneratorConfig",
)


# ---------------------------------------------------------
# Estimate sensor position resolution
# ---------------------------------------------------------

d = detector["pmt_radius_mm"][0]

sigma = d / np.sqrt(12.0)


# ---------------------------------------------------------
# Ring reconstruction
# ---------------------------------------------------------

fitted_radius = []
fitted_radius_error = []
primary_momentum = []

for event in events:

    ring = rich.reconstruct_ring(
        event,
        sigma_position=sigma,
    )

    if ring is None:
        continue

    fitted_radius.append(
        ring["radius"]
    )

    fitted_radius_error.append(
        ring["sigma_radius"]
    )

    primary_momentum.append(
        event["primary_momentum_GeV"]
    )


# Convert to NumPy arrays
fitted_radius = np.asarray(
    fitted_radius
)

fitted_radius_error = np.asarray(
    fitted_radius_error
)

primary_momentum = np.asarray(
    primary_momentum
)


# ---------------------------------------------------------
# Plot
# ---------------------------------------------------------


### Total plot ###

fig, ax = plt.subplots()

ax.errorbar(
    primary_momentum,
    fitted_radius,
    yerr=fitted_radius_error,
    fmt=".",
)

ax.set_xlabel(
    "Primary momentum [GeV/c]"
)

ax.set_ylabel(
    "Reconstructed ring radius [mm]"
)

ax.set_title(
    "Reconstructed Cherenkov ring radius"
)


### radius distributions in selected momentum ranges ###

momentum_ranges = [
    (20.0, 25.0),   # [GeV/c]
    (40.0, 45.0),   # [GeV/c]
]

for p_min, p_max in momentum_ranges:

    momentum_mask = (
        (primary_momentum >= p_min)
        & (primary_momentum < p_max)
    )

    selected_radii = fitted_radius[
        momentum_mask
    ]

    fig, ax = plt.subplots()

    ax.hist(
        selected_radii,
        bins=100,
    )

    ax.set_xlabel(
        "Reconstructed ring radius [mm]"
    )

    ax.set_ylabel(
        "Events"
    )

    ax.set_title(
        f"Ring radius distribution\n"
        f"{p_min:.1f} ≤ p < {p_max:.1f} GeV/c"
    )


### casual event plot ###

u = np.random.uniform()

i_event = int(u * len(events))

rich.plot_event(
    events[i_event],
    sigma_position = sigma,
    pmt_params = detector,
)

plt.show()