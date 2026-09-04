import na62rich_analysis as rich

filename = "output/data_na62rich_sim.root"

print(rich.get_branches(filename))

events = rich.load_events(
    filename,
    entry_stop=5,
)

print(events)