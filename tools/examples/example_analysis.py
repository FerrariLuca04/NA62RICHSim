import na62rich_analysis as rich

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

from scipy.optimize import curve_fit


SIMULATION_NAME = "tools/examples/example.root"

OUTPUT_DIR = Path("plots")
OUTPUT_DIR.mkdir(
    parents=True,
    exist_ok=True,
)

def double_gaussian(
    x,
    amplitude_1,
    mean_1,
    sigma_1,
    amplitude_2,
    mean_2,
    sigma_2,
):
    return (
        amplitude_1
        * np.exp(
            -0.5 * ((x - mean_1) / sigma_1) ** 2
        )
        + amplitude_2
        * np.exp(
            -0.5 * ((x - mean_2) / sigma_2) ** 2
        )
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
    SIMULATION_NAME,
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
event_indices = []

for i_event,event in enumerate(events):

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

    event_indices.append(
        i_event
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

event_indices = np.asarray(
    event_indices
)

# ---------------------------------------------------------
# Filter events
# ---------------------------------------------------------

mask_filtered = fitted_radius < 300

fitted_radius = fitted_radius[mask_filtered]
fitted_radius_error = fitted_radius_error[mask_filtered]
primary_momentum = primary_momentum[mask_filtered]
event_indices = event_indices[mask_filtered]

# ---------------------------------------------------------
# Reconstructed radius vs momentum
# ---------------------------------------------------------

fig, ax = plt.subplots(
    figsize=(8 * CM_TO_INCH, 6 * CM_TO_INCH),
    constrained_layout=True,
)

hist = ax.hist2d(
    primary_momentum,
    fitted_radius,
    bins=(50, 50),
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

fig.colorbar(
    hist[3],
    ax=ax,
    label="Events",
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

    # -----------------------------------------------------
    # Histogram
    # -----------------------------------------------------

    counts, bin_edges = np.histogram(
        selected_radii,
        bins=100,
    )

    bin_centers = 0.5 * (
        bin_edges[:-1] + bin_edges[1:]
    )

    # -----------------------------------------------------
    # Initial parameters for double Gaussian
    # -----------------------------------------------------

    # These values must be approximately adapted
    # to the position of your two peaks.

    initial_guess = [
        np.max(counts),
        np.percentile(selected_radii, 35),
        2.0,

        np.max(counts),
        np.percentile(selected_radii, 65),
        2.0,
    ]

    parameters, covariance = curve_fit(
        double_gaussian,
        bin_centers,
        counts,
        p0=initial_guess,
        maxfev=10000,
    )

    (
        amplitude_1,
        mean_1,
        sigma_1,
        amplitude_2,
        mean_2,
        sigma_2,
    ) = parameters

    sigma_1 = abs(sigma_1)
    sigma_2 = abs(sigma_2)

    # -----------------------------------------------------
    # Peak separation
    # -----------------------------------------------------

    separation = (
        abs(mean_1 - mean_2)
        / np.sqrt(
            0.5 * (
                sigma_1**2
                + sigma_2**2
            )
        )
    )

    print(
        f"\n{p_min:.0f} <= p < {p_max:.0f} GeV/c"
    )

    print(
        f"Peak 1: "
        f"R = {mean_1:.2f} mm, "
        f"sigma = {sigma_1:.2f} mm"
    )

    print(
        f"Peak 2: "
        f"R = {mean_2:.2f} mm, "
        f"sigma = {sigma_2:.2f} mm"
    )

    print(
        f"Separation = {separation:.2f} sigma"
    )

    # -----------------------------------------------------
    # Plot
    # -----------------------------------------------------

    n_sigma = 4.0

    x_min = min(
        mean_1 - n_sigma * sigma_1,
        mean_2 - n_sigma * sigma_2,
    )

    x_max = max(
        mean_1 + n_sigma * sigma_1,
        mean_2 + n_sigma * sigma_2,
    )

    fig, ax = plt.subplots(
        figsize=(6 * CM_TO_INCH, 6 * CM_TO_INCH),
        constrained_layout=True,
    )

    ax.hist(
        selected_radii,
        bins=bin_edges,
        histtype="step",
        label="Simulation",
    )

    x_fit = np.linspace(
        x_min,
        x_max,
        500,
    )

    ax.plot(
        x_fit,
        double_gaussian(
            x_fit,
            *parameters,
        ),
        label="Double Gaussian fit",
    )

    ax.set_xlim(
        x_min,
        x_max,
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

    ax.legend()

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