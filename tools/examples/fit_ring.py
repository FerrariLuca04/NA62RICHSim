import na62rich_analysis as rich

events = rich.load_events(
    "output/data_na62rich_sim.root",
    entry_stop=5,
)

circle = rich.reconstruct_ring(
    events[0]
)

print(circle)