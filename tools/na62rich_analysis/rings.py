import numpy as np
from scipy.optimize import least_squares


def fit_circle(x, y, sigma_position=None) -> dict:
    """
    Fit a circle to a set of points using a geometrical least-squares fit.

    Parameters
    ----------
    x
        x coordinates of the sensor hits.

    y
        y coordinates of the sensor hits.

    sigma_position
        Position uncertainty of each sensor hit.
        If None, an unweighted fit is performed.

    Returns
    -------
    dict
        Dictionary containing the reconstructed circle parameters
        and their uncertainties.
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


def select_sensor_hits(
    event,
    *,
    x_field="sensor_pos_x_mm",
    y_field="sensor_pos_y_mm",
    disk="both",
    hit_mode="sensors",
):
    """
    Extract sensor-hit coordinates from an event.

    Parameters
    ----------
    event
        Event record returned by load_events.

    x_field
        Name of the field containing the x positions of the hit sensors.

    y_field
        Name of the field containing the y positions of the hit sensors.

    disk
        Which disk to use:
        - "both": use all sensor hits;
        - "right": use only hits with x > 0;
        - "left": use only hits with x < 0;
        - "auto": choose the disk with more selected hits.

    hit_mode
        How repeated hits on the same sensor are treated:
        - "photons": repeated sensor positions are kept;
        - "sensors": each sensor position is used only once.

    Returns
    -------
    tuple
        (x, y, disk_used)
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


def _unique_sensors(x, y):
    """
    Remove duplicated sensor positions.
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

def reconstruct_ring(
    event,
    *,
    x_field="sensor_pos_x_mm",
    y_field="sensor_pos_y_mm",
    disk="auto",
    hit_mode="sensors",
    sigma_position=None,
):
    """
    Reconstruct a Cherenkov ring from the sensor hits of an event.
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
        raise ValueError(
            "At least three sensor hits on the same disk "
            "are required to reconstruct a ring."
        )

    ring = fit_circle(
        selected_x,
        selected_y,
        sigma_position=sigma_position,
    )

    ring["disk"] = disk_used

    return ring