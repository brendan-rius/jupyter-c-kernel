# Minimal C kernel for Jupyter

## Requirements

* gcc
* jupyter
* python
* pip

## Installation

### Manually

 * `git clone git@github.com:brendan-rius/jupyter-c-kernel.git`
 * `pip install jupyter-c-kernel`
 * `cd jupyter-c-kernel`
 * `jupyter-kernelspec install c_kernel`

### Via Docker

 * `docker build .`
 * `docker run -d -p 8888:8888 CONSTRUCTED_IMAGE`
 * Go to [http://localhost:8888](http://localhost:8888) (or your VM if you are using Docker Machine)

## Usage

 * Open the example notebook: `jupyter-notebook example-notebook.ipynb`
 * Enjoy!

## Example of notebook

![Example of notebook](example-notebook.png?raw=true "Example of notebook")

