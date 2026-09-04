import matplotlib.pyplot as plt
import na62rich_analysis as rich

events = rich.load_events(
    "output/data_na62rich_sim.root",
    branches=[
        "sensor_pos_x_mm",
        "sensor_pos_y_mm",
        "hit_x_mm",
        "hit_y_mm",
    ],
)

detector = rich.load_events(
    "output/data_na62rich_sim.root",
    tree_name="DetectorConfig",
    branches=[
        "pmt_radius_mm",
        "pmt_position_r_mm",
        "pmt_disk_radius_mm",
    ],
)

event = events[0]

fig, ax, ring = rich.plot_event(
    event,
    pmt_params=detector,
)

print(ring)
plt.show()