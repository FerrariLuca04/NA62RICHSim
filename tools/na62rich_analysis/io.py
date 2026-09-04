from __future__ import annotations

from os import PathLike
from pathlib import Path
from collections.abc import Sequence

import awkward as ak
import uproot


DEFAULT_TREE_NAME = "PhotonHits"


def load_events(
    filename: str | PathLike[str],
    *,
    tree_name: str = DEFAULT_TREE_NAME,
    branches: Sequence[str] | None = None,
    entry_start: int | None = None,
    entry_stop: int | None = None,
) -> ak.Array:
    """
    Load events from a NA62RICHSim ROOT output file.

    Parameters
    ----------
    filename
        Path to the ROOT file.

    tree_name
        Name of the TTree containing the simulated events.

    branches
        Branches to load. If None, all branches are loaded.

    entry_start
        Index of the first event to load.

    entry_stop
        Index after the last event to load.

    Returns
    -------
    awkward.Array
        Events stored as an Awkward Array.
    """

    path = Path(filename)

    if not path.is_file():
        raise FileNotFoundError(
            f"ROOT file not found: {path}"
        )

    with uproot.open(path) as root_file:

        if tree_name not in root_file:
            raise KeyError(
                f"TTree '{tree_name}' not found in '{path}'. "
                f"Available objects: {list(root_file.keys())}"
            )

        tree = root_file[tree_name]

        if branches is not None:
            available_branches = set(tree.keys())

            missing_branches = [
                branch
                for branch in branches
                if branch not in available_branches
            ]

            if missing_branches:
                raise KeyError(
                    "Branches not found: "
                    + ", ".join(missing_branches)
                )

        return tree.arrays(
            expressions=branches,
            entry_start=entry_start,
            entry_stop=entry_stop,
            library="ak",
        )


def get_branches(
    filename: str | PathLike[str],
    *,
    tree_name: str = DEFAULT_TREE_NAME,
) -> list[str]:
    """
    Return the names of the branches stored in a TTree.
    """

    path = Path(filename)

    if not path.is_file():
        raise FileNotFoundError(
            f"ROOT file not found: {path}"
        )

    with uproot.open(path) as root_file:

        if tree_name not in root_file:
            raise KeyError(
                f"TTree '{tree_name}' not found in '{path}'."
            )

        tree = root_file[tree_name]

        return list(tree.keys())