import matplotlib.pyplot as plt
import na62rich_analysis as rich

events = rich.load_events(
    "tools/examples/example.root",
    branches=[
        "sensor_pos_x_mm",
        "sensor_pos_y_mm",
    ],
)

detector = rich.load_events(
    "tools/examples/example.root",
    tree_name="DetectorConfig",
    branches=[
        "pmt_radius_mm",
        "pmt_position_r_mm",
        "pmt_disk_radius_mm",
    ],
)

fig, ax = rich.plot_sensor_heatmap(
    events,
    pmt_params=detector,
    disk="both",
)

plt.show()