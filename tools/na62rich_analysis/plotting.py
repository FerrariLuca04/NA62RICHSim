import matplotlib.pyplot as plt
import numpy as np

from .rings import (
    reconstruct_ring,
    select_sensor_hits,
)


def plot_sensor_heatmap(
    events,
    *,
    x_field="sensor_pos_x_mm",
    y_field="sensor_pos_y_mm",
    disk="both",
    hit_mode="sensors",
    bins=100,
    ax=None,
):
    """
    Plot a 2D heatmap of the sensor hits from all events.

    Parameters
    ----------
    events
        Event collection returned by load_events.

    x_field
        Name of the field containing the x positions of the hit sensors.

    y_field
        Name of the field containing the y positions of the hit sensors.

    disk
        Which disk to use:
        - "both"
        - "right"
        - "left"
        - "auto"

    hit_mode
        - "photons": repeated sensor positions are kept;
        - "sensors": each sensor position is used only once per event.

    bins
        Number of bins for the 2D histogram.

    ax
        Optional matplotlib axes.

    Returns
    -------
    tuple
        (fig, ax)
    """

    if ax is None:
        fig, ax = plt.subplots()
    else:
        fig = ax.figure

    all_x = []
    all_y = []

    for event in events:
        x, y, _ = select_sensor_hits(
            event,
            x_field=x_field,
            y_field=y_field,
            disk=disk,
            hit_mode=hit_mode,
        )

        if len(x) == 0:
            continue

        all_x.append(x)
        all_y.append(y)

    if len(all_x) == 0:
        raise ValueError(
            "No sensor hits are available to build the heatmap."
        )

    all_x = np.concatenate(all_x)
    all_y = np.concatenate(all_y)

    heatmap = ax.hist2d(
        all_x,
        all_y,
        bins=bins,
    )

    fig.colorbar(
        heatmap[3],
        ax=ax,
        label="Counts",
    )

    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")
    ax.set_title(
        f"Sensor hit heatmap (disk={disk}, hit_mode={hit_mode})"
    )
    ax.set_aspect("equal")

    return fig, ax


def plot_event(
    event,
    *,
    sensor_x_field="sensor_pos_x_mm",
    sensor_y_field="sensor_pos_y_mm",
    true_x_field="hit_x_mm",
    true_y_field="hit_y_mm",
    disk="auto",
    hit_mode="sensors",
    sigma_position=None,
    bins=30,
    show_sensor_scatter=True,
    ax=None,
):
    """
    Plot a single event.

    The plot contains:
    - a heatmap of the selected sensor hits;
    - a scatter plot of the true detected-photon positions;
    - the fitted circle.

    Parameters
    ----------
    event
        Single event record returned by load_events.

    sensor_x_field
        Name of the field containing the x positions of the hit sensors.

    sensor_y_field
        Name of the field containing the y positions of the hit sensors.

    true_x_field
        Name of the field containing the true x positions of the detected photons.
        Set to None to disable this overlay.

    true_y_field
        Name of the field containing the true y positions of the detected photons.
        Set to None to disable this overlay.

    disk
        Disk used for the reconstruction:
        - "right"
        - "left"
        - "auto"

    hit_mode
        - "photons"
        - "sensors"

    sigma_position
        Position uncertainty passed to reconstruct_ring.

    bins
        Number of bins for the sensor heatmap.

    show_sensor_scatter
        If True, overlay the selected sensor positions as a scatter plot.

    ax
        Optional matplotlib axes.

    Returns
    -------
    tuple
        (fig, ax, ring)
    """

    if ax is None:
        fig, ax = plt.subplots()
    else:
        fig = ax.figure

    ring = reconstruct_ring(
        event,
        x_field=sensor_x_field,
        y_field=sensor_y_field,
        disk=disk,
        hit_mode=hit_mode,
        sigma_position=sigma_position,
    )

    disk_used = ring["disk"]

    sensor_x, sensor_y, _ = select_sensor_hits(
        event,
        x_field=sensor_x_field,
        y_field=sensor_y_field,
        disk=disk_used,
        hit_mode=hit_mode,
    )

    heatmap = ax.hist2d(
        sensor_x,
        sensor_y,
        bins=bins,
    )

    fig.colorbar(
        heatmap[3],
        ax=ax,
        label="Sensor counts",
    )

    if show_sensor_scatter:
        ax.scatter(
            sensor_x,
            sensor_y,
            marker="s",
            label="Hit sensors",
        )

    if true_x_field is not None and true_y_field is not None:
        try:
            true_x = np.asarray(event[true_x_field], dtype=float)
            true_y = np.asarray(event[true_y_field], dtype=float)

            valid = np.isfinite(true_x) & np.isfinite(true_y)

            true_x = true_x[valid]
            true_y = true_y[valid]

            if disk_used == "right":
                mask = true_x > 0
                true_x = true_x[mask]
                true_y = true_y[mask]

            elif disk_used == "left":
                mask = true_x < 0
                true_x = true_x[mask]
                true_y = true_y[mask]

            ax.scatter(
                true_x,
                true_y,
                marker=".",
                label="True photon positions",
            )

        except (KeyError, ValueError, IndexError):
            pass

    theta = np.linspace(0.0, 2.0 * np.pi, 500)

    circle_x = (
        ring["center_x"]
        + ring["radius"] * np.cos(theta)
    )

    circle_y = (
        ring["center_y"]
        + ring["radius"] * np.sin(theta)
    )

    ax.plot(
        circle_x,
        circle_y,
        label="Fitted circle",
    )

    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")
    ax.set_aspect("equal")

    ax.set_title(
        "Event display\n"
        f"disk={disk_used}, "
        f"R={ring['radius']:.2f} ± {ring['sigma_radius']:.2f} mm, "
        f"N={ring['n_hits']}"
    )

    ax.legend()

    return fig, ax, ring