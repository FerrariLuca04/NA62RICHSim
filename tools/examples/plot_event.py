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

event = events[0]

fig, ax, ring = rich.plot_event(
    event,
    sensor_x_field="sensor_pos_x_mm",
    sensor_y_field="sensor_pos_y_mm",
    true_x_field="hit_x_mm",
    true_y_field="hit_y_mm",
    disk="auto",
    hit_mode="sensors",
    sigma_position=5.2,
    bins=25,
)

print(ring)
plt.show()