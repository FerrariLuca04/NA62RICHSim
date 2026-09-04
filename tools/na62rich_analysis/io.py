from __future__ import annotations

from collections.abc import Sequence
from os import PathLike
from pathlib import Path

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