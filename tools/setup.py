from setuptools import setup, find_packages

setup(
    name="na62rich-analysis",
    version="0.1.1",
    description="Python analysis tools for NA62RICHSim output files",

    packages=find_packages(),

    python_requires=">=3.10",

    install_requires=[
        "uproot",
        "awkward",
        "numpy",
        "matplotlib",
        "scipy",
    ],
)