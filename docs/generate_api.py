import inspect
import na62rich_analysis as rich


OUTPUT_FILE = "docs/api.md"


def write_function_doc(file, function):
    """
    Write the Markdown documentation for a function.
    """

    name = function.__name__
    signature = inspect.signature(function)
    docstring = inspect.getdoc(function)

    file.write(f"## {name}\n\n")
    file.write(f" `{name}{signature}`\n\n")

    if docstring:
        file.write(docstring)
        file.write("\n\n")


def main():

    functions = [
        rich.get_branches,
        rich.load_events,
        rich.fit_circle,
        rich.reconstruct_ring,
        rich.select_sensor_hits,
        rich.plot_event,
        rich.plot_sensor_heatmap,
    ]

    with open(
        OUTPUT_FILE,
        "w",
        encoding="utf-8",
    ) as file:

        file.write("# Python API reference\n\n")

        file.write(
            "This file is generated automatically from "
            "the Python docstrings.\n\n"
        )

        for function in functions:
            write_function_doc(
                file,
                function,
            )


if __name__ == "__main__":
    main()