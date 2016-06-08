# Minimal C kernel for Jupyter

## Use with Docker (recommended)

 * `docker pull brendanrius/jupyter-c-kernel`
 * `docker run -d -p 8888:8888 brendanrius/jupyter-c-kernel`
 * Go to [http://localhost:8888](http://localhost:8888) (or your VM address if you are using Docker Machine)

## Manual installation

 * Make sure you have the following requirements installed:
  * gcc
  * jupyter
  * python 3
  * pip

### Step-by-step:
 * `pip install jupyter-c-kernel`
 * `git clone https://github.com/brendan-rius/jupyter-c-kernel.git`
 * `cd jupyter-c-kernel`
 * `jupyter-kernelspec install c_spec/`
 * `jupyter-notebook`. Enjoy!

### Easy installation for Unix user:

 * `wget  -O - https://raw.githubusercontent.com/brendan-rius/jupyter-c-kernel/master/install.sh | sh`

## Example of notebook

![Example of notebook](example-notebook.png?raw=true "Example of notebook")

## Contributing

Create branches named `issue-X` where `X` is the number of the issue.
Rebase instead of merge.

## License

[MIT](LICENSE.txt)
