# Minimal C kernel for Jupyter

## Use with Docker (recommended)

 * `docker pull brendanrius/jupyter-c-kernel`
 * `docker run -d -p 8888:8888 brendanrius/jupyter-c-kernel`
 * Go to [http://localhost:8888](http://localhost:8888) (or your VM address if you are using Docker Machine)

## Manual installation

### Requirements

 * gcc
 * jupyter
 * python
 * pip

## Installation

 * `git clone git@github.com:brendan-rius/jupyter-c-kernel.git`
 * `pip install jupyter-c-kernel`
 * `cd jupyter-c-kernel`
 * `jupyter-kernelspec install c_spec/`

## Usage

 * Open the example notebook: `jupyter-notebook example-notebook.ipynb`
 * Enjoy!

## Example of notebook

![Example of notebook](example-notebook.png?raw=true "Example of notebook")
