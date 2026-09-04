# Python API reference

This file is generated automatically from the Python docstrings.

## get_branches

 `get_branches(filename: 'str | PathLike[str]', *, tree_name: 'str' = 'PhotonHits') -> 'list[str]'`

Return the names of the branches stored in a TTree.

### Parameters

`filename`
    Path to the ROOT file.

    The argument can be provided either as a string or as a path-like
    object.

`tree_name`
    Name of the TTree whose branch names are returned.

    The default value is `"PhotonHits"`.

### Returns

`list[str]`
    List containing the names of the branches stored in the selected
    TTree.

### Raises

`FileNotFoundError`
    If `filename` does not exist or is not a regular file.

`KeyError`
    If `tree_name` is not present in the ROOT file.

## load_events

 `load_events(filename: 'str | PathLike[str]', *, tree_name: 'str' = 'PhotonHits', branches: 'Sequence[str] | None' = None, entry_start: 'int | None' = None, entry_stop: 'int | None' = None) -> 'ak.Array'`

Load events from a NA62RICHSim ROOT output file.

The function opens the specified ROOT file, selects a TTree, and loads
the requested branches into an Awkward Array.

### Parameters

`filename`
    Path to the ROOT file.

    The argument can be provided either as a string or as a path-like
    object.

`tree_name`
    Name of the TTree to load.

    The default value is `"PhotonHits"`.

`branches`
    Sequence containing the names of the branches to load.

    If `None`, all available branches in the selected TTree are loaded.

`entry_start`
    Index of the first event to load.

    If `None`, loading starts from the first entry.

`entry_stop`
    Index after the last event to load.

    If `None`, entries are loaded up to the end of the TTree.

### Returns

`awkward.Array`
    Awkward Array containing the selected events and branches.

### Raises

`FileNotFoundError`
    If `filename` does not exist or is not a regular file.

`KeyError`
    If `tree_name` is not present in the ROOT file or if one or more
    requested branches are not present in the selected TTree.

## fit_circle

 `fit_circle(x: ArrayLike, y: ArrayLike, sigma_position: float | None = None) -> dict[str, float | int]`

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

## reconstruct_ring

 `reconstruct_ring(event: Any, *, x_field: str = 'sensor_pos_x_mm', y_field: str = 'sensor_pos_y_mm', disk: str = 'auto', hit_mode: str = 'photons', sigma_position: float | None = None) -> dict[str, float | int | str] | None`

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

## select_sensor_hits

 `select_sensor_hits(event: Any, *, x_field: str = 'sensor_pos_x_mm', y_field: str = 'sensor_pos_y_mm', disk: str = 'both', hit_mode: str = 'photons') -> tuple[numpy.ndarray, numpy.ndarray, str]`

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

## plot_event

 `plot_event(event: Any, *, sensor_x_field: str = 'sensor_pos_x_mm', sensor_y_field: str = 'sensor_pos_y_mm', true_x_field: str | None = 'hit_x_mm', true_y_field: str | None = 'hit_y_mm', disk: str = 'auto', hit_mode: str = 'photons', sigma_position: float | None = None, pmt_params: Any | None = None, pmt_dim_field: str = 'pmt_radius_mm', disk_pos_field: str = 'pmt_position_r_mm', disk_radius_field: str = 'pmt_disk_radius_mm', bins: int = 30, ax: matplotlib.axes._axes.Axes | None = None) -> tuple[matplotlib.figure.Figure, matplotlib.axes._axes.Axes, dict[str, float | int | str]]`

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

## plot_sensor_heatmap

 `plot_sensor_heatmap(events: collections.abc.Iterable[typing.Any], *, x_field: str = 'sensor_pos_x_mm', y_field: str = 'sensor_pos_y_mm', disk: str = 'both', hit_mode: str = 'photons', pmt_params: Any | None = None, pmt_dim_field: str = 'pmt_radius_mm', disk_pos_field: str = 'pmt_position_r_mm', disk_radius_field: str = 'pmt_disk_radius_mm', bins: int = 100, ax: matplotlib.axes._axes.Axes | None = None) -> tuple[matplotlib.figure.Figure, matplotlib.axes._axes.Axes]`

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

