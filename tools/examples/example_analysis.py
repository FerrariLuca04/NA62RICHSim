import na62rich_analysis as rich

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


SIMULATION_NAME = "tools/examples/example.root"

OUTPUT_DIR = Path("plots")
OUTPUT_DIR.mkdir(
    parents=True,
    exist_ok=True,
)


# ---------------------------------------------------------
# Plot configuration
# ---------------------------------------------------------

CM_TO_INCH = 1.0 / 2.54


plt.rcParams.update({
    "font.size": 10,
    "axes.labelsize": 8,
    "axes.titlesize": 10,
    "xtick.labelsize": 8,
    "ytick.labelsize": 8,
    "legend.fontsize": 8,
})


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
# Reconstructed radius vs momentum
# ---------------------------------------------------------

fig, ax = plt.subplots(
    figsize=(6 * CM_TO_INCH, 6 * CM_TO_INCH),
    constrained_layout=True,
)

ax.errorbar(
    primary_momentum,
    fitted_radius,
    yerr=fitted_radius_error,
    fmt=".",
    markersize=3,
    linewidth=0.8,
)

ax.set_xlabel(
    "Primary momentum [GeV/c]"
)

ax.set_ylabel(
    "Ring radius [mm]"
)

ax.set_title(
    "Reconstructed Cherenkov ring"
)

fig.savefig(
    "plots/ring_radius_vs_momentum.pdf",
    bbox_inches="tight",
)

plt.close(fig)


# ---------------------------------------------------------
# Radius distributions in selected momentum ranges
# ---------------------------------------------------------

momentum_ranges = [
    (20.0, 25.0),
    (40.0, 45.0),
]

for p_min, p_max in momentum_ranges:

    momentum_mask = (
        (primary_momentum >= p_min)
        & (primary_momentum < p_max)
    )

    selected_radii = fitted_radius[
        momentum_mask
    ]

    fig, ax = plt.subplots(
        figsize=(6 * CM_TO_INCH, 6 * CM_TO_INCH),
        constrained_layout=True,
    )

    ax.hist(
        selected_radii,
        bins=100,
    )

    ax.set_xlabel(
        "Ring radius [mm]"
    )

    ax.set_ylabel(
        "Events"
    )

    ax.set_title(
        f"{p_min:.0f} ≤ p < {p_max:.0f} GeV/c"
    )

    fig.savefig(
        f"plots/ring_radius_{p_min:.0f}_{p_max:.0f}_GeV.pdf",
        bbox_inches="tight",
    )

    plt.close(fig)


# ---------------------------------------------------------
# Random event
# ---------------------------------------------------------

i_event = np.random.randint(
    len(events)
)

fig, ax = plt.subplots(
    figsize= (8 * CM_TO_INCH, 8 * CM_TO_INCH),
    constrained_layout=True,
)

rich.plot_event(
    events[i_event],
    sigma_position=sigma,
    pmt_params=detector,
    ax=ax,
)

fig.savefig(
    "plots/example_event.pdf",
    bbox_inches="tight",
)

plt.close(fig)

# ---------------------------------------------------------
# Heatmap
# ---------------------------------------------------------

fig, ax = plt.subplots(
    figsize= (16 * CM_TO_INCH, 8 * CM_TO_INCH),
    constrained_layout=True,
)

rich.plot_sensor_heatmap(
    events,
    pmt_params=detector,
    ax=ax,
)

fig.savefig(
    "plots/example_heatmap.pdf",
    bbox_inches="tight",
)

plt.close(fig)