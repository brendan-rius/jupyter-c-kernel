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

The docker image installs the kernel in editable mode, meaning that you can
change the code in real-time in Docker. For that, just run the docker box like
that:

```bash
git clone https://github.com/brendan-rius/jupyter-c-kernel.git
cd jupyter-c-kernel
docker run -v $(pwd):/jupyter/jupyter_c_kernel/ -p 8888:8888 brendanrius/jupyter-c-kernel
```

This clones the source, run the kernel, and binds the current folder (the one
you just cloned) to eh corresponding folder in Docker.
Now, if you change the source, it will be reflected in [http://localhost:8888](http://localhost:8888)
instantly. Do not forget to click "restart" the kernel on the page as it does
not auto-restart.

### Version control

Create branches named `issue-X` where `X` is the number of the issue.
Rebase instead of merge.

## License

[MIT](LICENSE.txt)
