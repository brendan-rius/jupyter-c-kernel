# C kernel for Jupyter

This project was forked from [https://github.com/brendan-rius/jupyter-c-kernel](brendan-rius/jupyter-c-kernel) as that project seems to have been abandoned. (PR is pending)

This project includes fixes to many issues reported in [https://github.com/brendan-rius/jupyter-c-kernel](brendan-rius/jupyter-c-kernel), as well as the following additional features:

* Option for buffered output to mimic command line behaviour (useful for teaching, default is on)
* Command line input via `scanf` and `getchar`
* Support for `C89`/`ANSI C` (all newer versions were already supported and still are)

Following limitations compared to command line execution exist:

* Input is always buffered due to limitations of the jupyter interface
* When using `-ansi` or `-std=C89`, glibc still has to support at least `C99` for the interfacing with jupyter (this should not be an issue on an OS made after 2000)

## Use with Docker (recommended)

* `docker pull xaverklemenschits/jupyter-c-kernel`
* `docker run -p 8888:8888 xaverklemenschits/jupyter-c-kernel`
* Copy the given URL containing the token, and browse to it. For instance:

 ```bash
 Copy/paste this URL into your browser when you connect for the first time,
 to login with a token:
    http://localhost:8888/?token=66750c80bd0788f6ba15760aadz53beb9a9fb4cf8ac15ce8
 ```

## Manual installation

Works only on Linux and OS X. Windows is not supported yet. If you want to use this project on Windows, please use Docker.

* Make sure you have the following requirements installed:
  * gcc
  * jupyter
  * python 3
  * pip

### Step-by-step

```bash
git clone https://github.com/XaverKlemenschits/jupyter-c-kernel.git
cd jupyter-c-kernel
pip install -e .  # for system install: sudo install .
cd jupyter_c_kernel && install_c_kernel --user # for sys install: sudo install_c_kernel
# now you can start the notebook
jupyter notebook
```

## Example of notebook

![Example of notebook](example-notebook.png?raw=true "Example of notebook")

## Custom compilation flags

You can use custom compilation flags like so:

![Custom compulation flag](custom_flags.png?raw=true "Example of notebook using custom compilation flags")

Here, the `-lm` flag is passed so you can use the math library.

## Contributing

The docker image installs the kernel in editable mode, meaning that you can
change the code in real-time in Docker. For that, just run the docker box like
that:

```bash
git clone https://github.com/XaverKlemenschits/jupyter-c-kernel.git
cd jupyter-c-kernel
docker build -t myName/jupyter .
docker run -v $(pwd):/tmp/jupyter_c_kernel/ -p 8888:8888 myName/jupyter
```

This clones the source, run the kernel, and binds the current folder (the one
you just cloned) to the corresponding folder in Docker.
Now, if you change the source, it will be reflected in [http://localhost:8888](http://localhost:8888)
instantly. Do not forget to click "restart" the kernel on the page as it does
not auto-restart.

## License

[MIT](LICENSE.txt)
