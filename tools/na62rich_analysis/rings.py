from typing import Any
from numpy.typing import ArrayLike

import numpy as np
from scipy.optimize import least_squares
import warnings


def fit_circle(
    x: ArrayLike,
    y: ArrayLike,
    sigma_position: float | None = None,
) -> dict[str, float | int]:
    """
    Fit a circle to a set of points using a geometrical least-squares fit.

    ### Parameters

    `x`
        One-dimensional array containing the x coordinates of the hits.

    `y`
        One-dimensional array containing the y coordinates of the hits.

    `sigma_position`
        Position uncertainty associated with each hit.

        If `None`, an unweighted fit is performed.

    ### Returns

    `dict`
        Dictionary containing the reconstructed circle parameters:

        - `center_x`: reconstructed x coordinate of the circle center;
        - `center_y`: reconstructed y coordinate of the circle center;
        - `radius`: reconstructed circle radius;
        - `sigma_center_x`: uncertainty on the x coordinate of the center;
        - `sigma_center_y`: uncertainty on the y coordinate of the center;
        - `sigma_radius`: uncertainty on the reconstructed radius;
        - `n_hits`: number of points used in the fit;
        - `rms_residual`: RMS of the radial residuals.

    ### Raises

    `ValueError`
        If `x` and `y` are not one-dimensional arrays, have different
        lengths, or contain fewer than three valid points.
    """

    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)

    if x.ndim != 1 or y.ndim != 1:
        raise ValueError(
            "x and y must be one-dimensional arrays."
        )

    if x.size != y.size:
        raise ValueError(
            "x and y must contain the same number of points."
        )

    valid = np.isfinite(x) & np.isfinite(y)

    x = x[valid]
    y = y[valid]

    if x.size < 3:
        raise ValueError(
            "At least three valid points are required "
            "to reconstruct a circle."
        )

    # Initial estimate
    center_x_0 = np.mean(x)
    center_y_0 = np.mean(y)

    radius_0 = np.mean(
        np.sqrt(
            (x - center_x_0)**2
            + (y - center_y_0)**2
        )
    )

    initial_parameters = [
        center_x_0,
        center_y_0,
        radius_0,
    ]

    def residuals(parameters):

        center_x, center_y, radius = parameters

        distances = np.sqrt(
            (x - center_x)**2
            + (y - center_y)**2
        )

        residual = distances - radius

        if sigma_position is not None:
            residual = residual / sigma_position

        return residual

    result = least_squares(
        residuals,
        initial_parameters,
    )

    center_x, center_y, radius = result.x

    # Parameter covariance matrix
    n_points = x.size
    n_parameters = 3

    dof = n_points - n_parameters

    if dof > 0:

        jacobian = result.jac

        covariance = np.linalg.inv(
            jacobian.T @ jacobian
        )

        # If sigma_position is unknown, estimate the residual variance
        if sigma_position is None:
            residual_variance = (
                2 * result.cost / dof
            )

            covariance *= residual_variance

        uncertainties = np.sqrt(
            np.diag(covariance)
        )

    else:
        uncertainties = np.full(3, np.nan)

    sigma_center_x = uncertainties[0]
    sigma_center_y = uncertainties[1]
    sigma_radius = uncertainties[2]

    distances = np.sqrt(
        (x - center_x)**2
        + (y - center_y)**2
    )

    rms_residual = np.sqrt(
        np.mean(
            (distances - radius)**2
        )
    )

    return {
        "center_x": float(center_x),
        "center_y": float(center_y),
        "radius": float(radius),

        "sigma_center_x": float(sigma_center_x),
        "sigma_center_y": float(sigma_center_y),
        "sigma_radius": float(sigma_radius),

        "n_hits": int(x.size),
        "rms_residual": float(rms_residual),
    }

def _unique_sensors(
    x: ArrayLike,
    y: ArrayLike,
) -> tuple[np.ndarray, np.ndarray]:
    """
    Remove duplicated sensor positions.

    ### Parameters

    `x`
        x coordinates of the sensor positions.

    `y`
        y coordinates of the sensor positions.

    ### Returns

    `tuple`
        Two NumPy arrays containing the x and y coordinates of the unique
        sensor positions.
    """

    points = np.column_stack((x, y))

    unique_points = np.unique(
        points,
        axis=0,
    )

    return (
        unique_points[:, 0],
        unique_points[:, 1],
    )

def select_sensor_hits(
    event: Any,
    *,
    x_field: str = "sensor_pos_x_mm",
    y_field: str = "sensor_pos_y_mm",
    disk: str = "both",
    hit_mode: str = "photons",
) -> tuple[np.ndarray, np.ndarray, str]:
    """
    Extract sensor-hit coordinates from a simulated event.

    ### Parameters

    `event`
        Event record returned by `load_events`.

    `x_field`
        Name of the field containing the x coordinates of the hit sensors.

    `y_field`
        Name of the field containing the y coordinates of the hit sensors.

    `disk`
        Sensor disk selection.

        Accepted values are:

        - `"both"`: use all sensor hits;
        - `"right"`: use only hits with x > 0;
        - `"left"`: use only hits with x < 0;
        - `"auto"`: use the disk containing the largest number of selected
          hits.

    `hit_mode`
        Defines how multiple photons hitting the same sensor are treated.

        Accepted values are:

        - `"photons"`: repeated sensor positions are kept;
        - `"sensors"`: each sensor position is used only once.

    ### Returns

    `tuple`
        Tuple `(x, y, disk_used)` containing:

        - `x`: selected sensor x coordinates;
        - `y`: selected sensor y coordinates;
        - `disk_used`: disk selected by the function.

    ### Raises

    `KeyError`
        If `x_field` or `y_field` is not present in the event.

    `ValueError`
        If `disk` or `hit_mode` contains an unsupported value.
    """

    try:
        x = np.asarray(event[x_field], dtype=float)
        y = np.asarray(event[y_field], dtype=float)

    except (KeyError, ValueError, IndexError):
        raise KeyError(
            f"Fields '{x_field}' and/or '{y_field}' not found in the event."
        ) from None

    if hit_mode not in ("photons", "sensors"):
        raise ValueError(
            "hit_mode must be 'photons' or 'sensors'."
        )

    if disk not in ("both", "right", "left", "auto"):
        raise ValueError(
            "disk must be 'both', 'right', 'left', or 'auto'."
        )

    valid = np.isfinite(x) & np.isfinite(y)

    x = x[valid]
    y = y[valid]

    if hit_mode == "sensors":
        x, y = _unique_sensors(x, y)

    if disk == "both":
        return x, y, "both"

    right_mask = x > 0
    left_mask = x < 0

    right_x = x[right_mask]
    right_y = y[right_mask]

    left_x = x[left_mask]
    left_y = y[left_mask]

    if disk == "right":
        return right_x, right_y, "right"

    if disk == "left":
        return left_x, left_y, "left"

    # disk == "auto"
    if len(right_x) >= len(left_x):
        return right_x, right_y, "right"

    return left_x, left_y, "left"

def reconstruct_ring(
    event: Any,
    *,
    x_field: str = "sensor_pos_x_mm",
    y_field: str = "sensor_pos_y_mm",
    disk: str = "auto",
    hit_mode: str = "photons",
    sigma_position: float | None = None,
) -> dict[str, float | int | str] | None:
    """
    Reconstruct a Cherenkov ring from the sensor hits of a simulated event.

    The function extracts the positions of the hit sensors, optionally removes
    repeated sensor positions, selects one of the two sensor disks, and
    performs a circular fit using `fit_circle`.

    ### Parameters

    `event`
        Event record returned by `load_events`.

    `x_field`
        Name of the field containing the x coordinates of the hit sensors,
        expressed in millimeters.

    `y_field`
        Name of the field containing the y coordinates of the hit sensors,
        expressed in millimeters.

    `disk`
        Sensor disk used for the reconstruction.

        Accepted values are:

        - `"right"`: use only sensors with x > 0;
        - `"left"`: use only sensors with x < 0;
        - `"auto"`: automatically select the disk containing the largest
          number of observations used in the fit.

        The value `"both"` is not supported for ring reconstruction.

    `hit_mode`
        Defines how multiple photons hitting the same sensor are treated.

        Accepted values are:

        - `"photons"`: repeated sensor positions are kept. A sensor hit by
          multiple photons therefore contributes multiple times to the fit;
        - `"sensors"`: each sensor position is used only once, independently
          of the number of photons detected by that sensor.

    `sigma_position`
        Position uncertainty associated with each sensor hit, expressed in
        millimeters.

        If provided, the value is passed to `fit_circle` and is used to
        weight the geometrical residuals and estimate the uncertainties on
        the reconstructed circle parameters.

        If `None`, an unweighted fit is performed.

    ### Returns

    `dict` or `None`
        Dictionary containing the reconstructed ring parameters:

        - `center_x`: reconstructed x coordinate of the ring center;
        - `center_y`: reconstructed y coordinate of the ring center;
        - `radius`: reconstructed ring radius;
        - `sigma_center_x`: uncertainty on the x coordinate of the center;
        - `sigma_center_y`: uncertainty on the y coordinate of the center;
        - `sigma_radius`: uncertainty on the reconstructed radius;
        - `n_hits`: number of observations used in the fit;
        - `rms_residual`: RMS of the radial residuals;
        - `disk`: disk actually used for the reconstruction.

        `None` is returned when fewer than three sensor hits are available on
        the selected disk.

    ### Raises

    `KeyError`
        If `x_field` or `y_field` is not present in the event.

    `ValueError`
        If `disk` or `hit_mode` contains an unsupported value.

    ### Warns

    `RuntimeWarning`
        Raised when fewer than three sensor hits are available on the selected
        disk and the ring reconstruction is skipped.

    ### Notes

    When `hit_mode="photons"`, multiple photons detected by the same sensor
    effectively increase the statistical weight of that sensor position.

    When `hit_mode="sensors"`, each sensor contributes only once to the fit,
    so the reconstruction depends only on the spatial distribution of the
    hit sensors.
    """


    if disk == "both":
        raise ValueError(
            "reconstruct_ring does not support disk='both'. "
            "Use 'left', 'right', or 'auto'."
        )

    selected_x, selected_y, disk_used = select_sensor_hits(
        event,
        x_field=x_field,
        y_field=y_field,
        disk=disk,
        hit_mode=hit_mode,
    )

    if len(selected_x) < 3:
        warnings.warn(
            "Ring reconstruction skipped: fewer than three "
            "sensor hits are available on the selected disk.",
            RuntimeWarning,
            stacklevel=2,
        )

        return None

    ring = fit_circle(
        selected_x,
        selected_y,
        sigma_position=sigma_position,
    )

    ring["disk"] = disk_used

    return ring