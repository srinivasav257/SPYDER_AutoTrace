from setuptools import setup, find_packages
import os

def read_requirements():
    """Reads dependencies from requirements.txt if available."""
    req_file = "requirements.txt"
    if os.path.exists(req_file):
        with open(req_file, "r") as req:
            return req.read().splitlines()
    return []

def read_long_description():
    """Reads README.md if available, otherwise returns a fallback description."""
    if os.path.exists("README.md"):
        with open("README.md", encoding="utf-8") as f:
            return f.read()
    return "Infotest - A reusable automation framework for infotainment diagnostics and system validation."

setup(
    name="infotest",
    version="1.0.0",
    author="Srinivasa Vasamshetty",
    author_email="srinivasa.v@aptiv.com",
    description="Reusable Automation Framework for Infotainment System Testing, Diagnostics, and Validation",
    long_description=read_long_description(),
    long_description_content_type="text/markdown",
    packages=find_packages(),
    include_package_data=True,
    install_requires=read_requirements(),
    python_requires=">=3.8",
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: Microsoft :: Windows",
        "Topic :: Software Development :: Testing",
        "License :: OSI Approved :: MIT License",
    ],
)
