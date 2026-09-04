from setuptools import setup, find_packages

setup(
    name="na62rich-analysis",
    version="0.1.0",
    description="Python analysis tools for NA62RICHSim output files",

    packages=find_packages(where="tools"),
    package_dir={"": "tools"},

    python_requires=">=3.10",

    install_requires=[
        "uproot",
        "awkward",
        "numpy",
        "matplotlib",
    ],
)