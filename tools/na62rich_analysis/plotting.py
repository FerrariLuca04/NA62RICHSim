from collections.abc import Iterable
from typing import Any

import matplotlib.pyplot as plt
from matplotlib.axes import Axes
from matplotlib.collections import PatchCollection
from matplotlib.figure import Figure
from matplotlib.patches import RegularPolygon, Wedge

import numpy as np

from .rings import (
    reconstruct_ring,
    select_sensor_hits,
)


def plot_sensor_heatmap(
    events: Iterable[Any],
    *,
    x_field: str = "sensor_pos_x_mm",
    y_field: str = "sensor_pos_y_mm",
    disk: str = "both",
    hit_mode: str = "photons",
    pmt_params: Any | None = None,
    pmt_dim_field: str = "pmt_radius_mm",
    disk_pos_field: str = "pmt_position_r_mm",
    disk_radius_field: str = "pmt_disk_radius_mm",
    bins: int = 100,
    ax: Axes | None = None,
) -> tuple[Figure, Axes]:
    """
    Plot the sensor occupancy accumulated over all simulated events.

    If the PMT detector configuration is provided, the physical hexagonal
    PMT grid is reconstructed and each sensor is colored according to its
    occupancy. Otherwise, a standard two-dimensional histogram is used.

    ### Parameters

    `events`
        Event collection returned by `load_events`.

    `x_field`
        Name of the field containing the x positions of the hit sensors.

    `y_field`
        Name of the field containing the y positions of the hit sensors.

    `disk`
        Sensor disk selection.

        Accepted values are:

        - `"both"`: use both PMT disks;
        - `"right"`: use only sensors with x > 0;
        - `"left"`: use only sensors with x < 0;
        - `"auto"`: for each event, use the disk containing the largest
          number of selected hits.

    `hit_mode`
        Defines how multiple photons hitting the same sensor are treated.

        Accepted values are:

        - `"photons"`: repeated sensor positions are kept, so every detected
          photon contributes one count;
        - `"sensors"`: each sensor contributes at most one count per event.

    `pmt_params`
        Configuration table for detectors returned by
        `load_events(tree_name="DetectorConfig")`.

        If `None`, a standard two-dimensional histogram is used.

    `pmt_dim_field`
        Name of the field containing the PMT apothem.

    `disk_pos_field`
        Name of the field containing the radial position of the PMT disks.

    `disk_radius_field`
        Name of the field containing the radius of the PMT disks.

    `bins`
        Number of bins used by the fallback two-dimensional histogram.

        This parameter is ignored when `pmt_params` is provided.

    `ax`
        Optional Matplotlib axes on which the plot is drawn.

        If `None`, a new figure and axes are created.

    ### Returns

    `tuple`
        Tuple `(fig, ax)` containing the Matplotlib figure and axes.

    ### Raises

    `ValueError`
        If no sensor hits are available, if an unsupported disk or hit mode
        is requested, or if the PMT detector configuration is invalid.
    """

    if ax is None:
        fig, ax = plt.subplots()
    else:
        fig = ax.figure

    all_x = []
    all_y = []

    # Collect selected hits from all events.
    for event in events:

        sensor_x, sensor_y, _ = select_sensor_hits(
            event,
            x_field=x_field,
            y_field=y_field,
            disk=disk,
            hit_mode=hit_mode,
        )

        if len(sensor_x) == 0:
            continue

        all_x.append(
            np.asarray(sensor_x, dtype=float)
        )

        all_y.append(
            np.asarray(sensor_y, dtype=float)
        )

    if len(all_x) == 0:
        raise ValueError(
            "No sensor hits are available to build the heatmap."
        )

    all_x = np.concatenate(all_x)
    all_y = np.concatenate(all_y)

    # ---------------------------------------------------------
    # Hexagonal PMT geometry
    # ---------------------------------------------------------

    if pmt_params is not None:

        sensor_positions, circum_radius = _build_sensor_grid(
            pmt_params,
            pmt_dim_field=pmt_dim_field,
            disk_pos_field=disk_pos_field,
            disk_radius_field=disk_radius_field,
            disk=disk,
        )

        occupancy = np.zeros(
            len(sensor_positions),
            dtype=float,
        )

        # Associate every hit to the nearest sensor.
        for x_hit, y_hit in zip(all_x, all_y):

            distances = np.sqrt(
                (sensor_positions[:, 0] - x_hit)**2
                + (sensor_positions[:, 1] - y_hit)**2
            )

            sensor_index = np.argmin(
                distances
            )

            occupancy[sensor_index] += 1

        patches = []

        for x_sensor, y_sensor in sensor_positions:

            hexagon = RegularPolygon(
                (x_sensor, y_sensor),
                numVertices=6,
                radius=circum_radius,
                orientation=np.pi / 6.0,
            )

            patches.append(hexagon)

        collection = PatchCollection(
            patches,
            edgecolor="none",
            linewidth=0.0,
        )

        collection.set_array(
            occupancy
        )

        ax.add_collection(
            collection
        )

        fig.colorbar(
            collection,
            ax=ax,
            label="Sensor counts",
        )

        ax.autoscale_view()

    # ---------------------------------------------------------
    # Fallback histogram
    # ---------------------------------------------------------

    else:

        heatmap = ax.hist2d(
            all_x,
            all_y,
            bins=bins,
        )

        fig.colorbar(
            heatmap[3],
            ax=ax,
            label="Sensor counts",
        )

    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")
    ax.set_aspect("equal")

    ax.set_title(
        "PMT occupancy\n"
        f"disk={disk}, hit_mode={hit_mode}"
    )

    return fig, ax


def plot_event(
    event: Any,
    *,
    sensor_x_field: str = "sensor_pos_x_mm",
    sensor_y_field: str = "sensor_pos_y_mm",
    true_x_field: str | None = "hit_x_mm",
    true_y_field: str | None = "hit_y_mm",
    disk: str = "auto",
    hit_mode: str = "photons",
    sigma_position: float | None = None,
    pmt_params: Any | None = None,
    pmt_dim_field: str = "pmt_radius_mm",
    disk_pos_field: str = "pmt_position_r_mm",
    disk_radius_field: str = "pmt_disk_radius_mm",
    bins: int = 30,
    ax: Axes | None = None,
) -> tuple[Figure, Axes, dict[str, float | int | str]]:
    """
    Plot a single simulated event.

    The plot contains the sensor occupancy, the true positions of the
    detected photons, the reconstructed Cherenkov ring, the fitted center,
    and the uncertainty on the reconstructed radius.

    If the PMT detector configuration is provided, the physical hexagonal
    PMT grid is reconstructed and each sensor is colored according to its
    occupancy. Otherwise, a standard two-dimensional histogram is used.

    ### Parameters

    `event`
        Single event record returned by `load_events`.

    `sensor_x_field`
        Name of the field containing the x positions of the hit sensors.

    `sensor_y_field`
        Name of the field containing the y positions of the hit sensors.

    `true_x_field`
        Name of the field containing the true x positions of the detected
        photons.

        Set to `None` to disable this overlay.

    `true_y_field`
        Name of the field containing the true y positions of the detected
        photons.

        Set to `None` to disable this overlay.

    `disk`
        Disk used for the ring reconstruction.

        Accepted values are:

        - `"right"`: use the PMT disk with x > 0;
        - `"left"`: use the PMT disk with x < 0;
        - `"auto"`: automatically select the disk containing the largest
          number of observations used in the fit.

    `hit_mode`
        Defines how multiple photons hitting the same sensor are treated.

        Accepted values are:

        - `"photons"`: repeated sensor positions are kept, so the sensor
          occupancy corresponds to the number of detected photons;
        - `"sensors"`: each sensor position is used only once.

    `sigma_position`
        Position uncertainty associated with each sensor hit, expressed in
        millimeters.

        The value is passed to `reconstruct_ring`.

        If `None`, an unweighted fit is performed.

    `pmt_params`
        Configuration table for detectors returned by
        `load_events(tree_name="DetectorConfig")`.

        If `None`, the sensor occupancy is displayed using `hist2d`.

    `pmt_dim_field`
        Name of the field containing the PMT apothem.

    `disk_pos_field`
        Name of the field containing the radial position of the PMT disks.

    `disk_radius_field`
        Name of the field containing the radius of the PMT disks.

    `bins`
        Number of bins used by the fallback two-dimensional histogram.

        This parameter is ignored when `pmt_params` is provided.

    `ax`
        Optional Matplotlib axes on which the plot is drawn.

        If `None`, a new figure and axes are created.

    ### Returns

    `tuple`
        Tuple `(fig, ax, ring)` containing:

        - `fig`: Matplotlib figure;
        - `ax`: Matplotlib axes;
        - `ring`: dictionary containing the reconstructed ring parameters.

    ### Raises

    `ValueError`
        If the ring cannot be reconstructed or if the PMT detector
        configuration is invalid.
    """

    if ax is None:
        fig, ax = plt.subplots()
    else:
        fig = ax.figure

    # ---------------------------------------------------------
    # Ring reconstruction
    # ---------------------------------------------------------

    ring = reconstruct_ring(
        event,
        x_field=sensor_x_field,
        y_field=sensor_y_field,
        disk=disk,
        hit_mode=hit_mode,
        sigma_position=sigma_position,
    )

    if ring is None:
        raise ValueError(
            "The event does not contain enough sensor hits "
            "to reconstruct and plot a ring."
        )

    disk_used = ring["disk"]

    # Select exactly the same sensor hits used for the reconstruction.
    sensor_x, sensor_y, _ = select_sensor_hits(
        event,
        x_field=sensor_x_field,
        y_field=sensor_y_field,
        disk=disk_used,
        hit_mode=hit_mode,
    )

    sensor_x = np.asarray(
        sensor_x,
        dtype=float,
    )

    sensor_y = np.asarray(
        sensor_y,
        dtype=float,
    )

    # ---------------------------------------------------------
    # Sensor occupancy
    # ---------------------------------------------------------

    if pmt_params is not None:

        sensor_positions, circum_radius = _build_sensor_grid(
            pmt_params,
            pmt_dim_field=pmt_dim_field,
            disk_pos_field=disk_pos_field,
            disk_radius_field=disk_radius_field,
            disk=disk_used,
        )

        occupancy = np.zeros(
            len(sensor_positions),
            dtype=float,
        )

        for x_hit, y_hit in zip(sensor_x, sensor_y):

            distances = np.sqrt(
                (sensor_positions[:, 0] - x_hit)**2
                + (sensor_positions[:, 1] - y_hit)**2
            )

            sensor_index = np.argmin(
                distances
            )

            occupancy[sensor_index] += 1

        patches = []

        for x_sensor, y_sensor in sensor_positions:

            hexagon = RegularPolygon(
                (x_sensor, y_sensor),
                numVertices=6,
                radius=circum_radius,
                orientation=np.pi / 6.0,
            )

            patches.append(hexagon)

        collection = PatchCollection(
            patches,
            edgecolor="none",
            linewidth=0.0,
        )

        collection.set_array(
            occupancy
        )

        ax.add_collection(
            collection
        )

        fig.colorbar(
            collection,
            ax=ax,
            label="Sensor counts",
        )

        ax.autoscale_view()

    else:

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

    # ---------------------------------------------------------
    # True photon positions
    # ---------------------------------------------------------

    if true_x_field is not None and true_y_field is not None:

        try:
            true_x = np.asarray(
                event[true_x_field],
                dtype=float,
            )

            true_y = np.asarray(
                event[true_y_field],
                dtype=float,
            )

            valid = (
                np.isfinite(true_x)
                & np.isfinite(true_y)
            )

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

    # ---------------------------------------------------------
    # Fitted ring
    # ---------------------------------------------------------

    theta = np.linspace(
        0.0,
        2.0 * np.pi,
        500,
    )

    circle_x = (
        ring["center_x"]
        + ring["radius"] * np.cos(theta)
    )

    circle_y = (
        ring["center_y"]
        + ring["radius"] * np.sin(theta)
    )

    sigma_radius = ring["sigma_radius"]

    # Radius uncertainty band.
    if (
        np.isfinite(sigma_radius)
        and sigma_radius > 0
    ):

        halo = Wedge(
            center=(
                ring["center_x"],
                ring["center_y"],
            ),
            r=ring["radius"] + sigma_radius,
            theta1=0.0,
            theta2=360.0,
            width=2.0 * sigma_radius,
            alpha=0.2,
            label="1σ radius band",
        )

        ax.add_patch(
            halo
        )

    # Best-fit circle.
    ax.plot(
        circle_x,
        circle_y,
        color="red",
        label="Fitted circle",
    )

    # Fitted center and its uncertainties.
    ax.errorbar(
        ring["center_x"],
        ring["center_y"],
        xerr=ring["sigma_center_x"],
        yerr=ring["sigma_center_y"],
        fmt="o",
        label="Fitted center",
    )

    # ---------------------------------------------------------
    # Plot formatting
    # ---------------------------------------------------------

    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")
    ax.set_aspect("equal")

    ax.set_title(
        "Event display\n"
        f"disk={disk_used}, "
        f"R={ring['radius']:.2f} ± "
        f"{ring['sigma_radius']:.2f} mm, "
        f"N={ring['n_hits']}"
    )

    ax.legend()

    return fig, ax, ring


def _is_hexagon_inside_circle(
    x: float,
    y: float,
    hex_radius: float,
    circle_radius: float,
) -> bool:
    """
    Check whether a regular hexagon is fully contained inside a circle.

    ### Parameters

    `x`
        x coordinate of the hexagon center.

    `y`
        y coordinate of the hexagon center.

    `hex_radius`
        Hexagon radius used by the detector containment test.

    `circle_radius`
        Radius of the containing circle.

    ### Returns

    `bool`
        `True` if all six tested vertices of the hexagon are inside the
        circle, `False` otherwise.
    """

    for i in range(6):

        phi = i * np.pi / 3.0

        vertex_x = (
            x
            + hex_radius * np.cos(phi)
        )

        vertex_y = (
            y
            + hex_radius * np.sin(phi)
        )

        if (
            vertex_x**2
            + vertex_y**2
            > circle_radius**2
        ):
            return False

    return True


def _build_sensor_grid(
    pmt_params: Any,
    *,
    pmt_dim_field: str = "pmt_radius_mm",
    disk_pos_field: str = "pmt_position_r_mm",
    disk_radius_field: str = "pmt_disk_radius_mm",
    disk: str = "both",
) -> tuple[np.ndarray, float]:
    """
    Reconstruct the hexagonal PMT grid from the detector configuration.

    ### Parameters

    `pmt_params`
        Configuration table for detectors returned by
        `load_events(tree_name="DetectorConfig")`.

    `pmt_dim_field`
        Name of the field containing the PMT apothem.

    `disk_pos_field`
        Name of the field containing the radial position of the PMT disks.

    `disk_radius_field`
        Name of the field containing the radius of the PMT disks.

    `disk`
        Disk geometry to return.

        Accepted values are:

        - `"both"`: return both PMT disks;
        - `"right"`: return only the disk with x > 0;
        - `"left"`: return only the disk with x < 0;
        - `"auto"`: return both disks. The actual disk selection must be
          performed separately for each event.

    ### Returns

    `tuple`
        Tuple `(sensor_positions, circum_radius)` containing:

        - `sensor_positions`: NumPy array with shape `(N, 2)` containing
          the PMT center coordinates;
        - `circum_radius`: circumradius of the hexagonal PMTs.

    ### Raises

    `ValueError`
        If `disk` contains an unsupported value or if the detector
        configuration is invalid.
    """

    if disk not in ("both", "right", "left", "auto"):
        raise ValueError(
            "disk must be 'both', 'right', 'left', or 'auto'."
        )

    try:
        pmt_radius = float(
            pmt_params[pmt_dim_field][0]
        )

        disk_pos = float(
            pmt_params[disk_pos_field][0]
        )

        disk_radius = float(
            pmt_params[disk_radius_field][0]
        )

    except (KeyError, IndexError, TypeError, ValueError) as exc:
        raise ValueError(
            "Invalid PMT detector configuration."
        ) from exc

    circum_radius = (
        pmt_radius
        / np.cos(np.pi / 6.0)
    )

    dx = 1.5 * circum_radius
    dy = np.sqrt(3.0) * circum_radius

    n = (
        int(
            np.ceil(
                disk_radius / pmt_radius
            )
        )
        + 2
    )

    sensor_positions = []

    for i in range(2):

        disk_center_x = (
            (i * 2.0 - 1.0)
            * disk_pos
        )

        for q in range(-n, n + 1):

            for r in range(-n, n + 1):

                x_local = dx * q

                y_local = dy * (
                    r + 0.5 * q
                )

                if not _is_hexagon_inside_circle(
                    x_local,
                    y_local,
                    pmt_radius,
                    disk_radius,
                ):
                    continue

                sensor_positions.append(
                    (
                        disk_center_x + x_local,
                        y_local,
                    )
                )

    sensor_positions = np.asarray(
        sensor_positions,
        dtype=float,
    ).reshape(-1, 2)

    if disk == "right":

        sensor_positions = sensor_positions[
            sensor_positions[:, 0] > 0
        ]

    elif disk == "left":

        sensor_positions = sensor_positions[
            sensor_positions[:, 0] < 0
        ]

    return sensor_positions, circum_radius