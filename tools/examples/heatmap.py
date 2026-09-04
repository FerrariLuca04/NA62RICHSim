import matplotlib.pyplot as plt
import na62rich_analysis as rich

events = rich.load_events(
    "output/data_na62rich_sim.root",
    branches=[
        "sensor_pos_x_mm",
        "sensor_pos_y_mm",
    ],
)

fig, ax = rich.plot_sensor_heatmap(
    events,
    disk="both",
    hit_mode="sensors",
    bins=100,
)

plt.show()