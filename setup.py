from setuptools import setup

setup(name='jupyter_c_kernel',
      version='1.2.1',
      description='Minimalistic C kernel for Jupyter',
      author='Brendan Rius',
      author_email='ping@brendan-rius.com',
      url='https://github.com/brendanrius/jupyter-c-kernel/',
      download_url='https://github.com/brendanrius/jupyter-c-kernel/tarball/1.2.1',
      packages=['jupyter_c_kernel'],
      scripts=['jupyter_c_kernel/install_c_kernel'],
      keywords=['jupyter', 'notebook', 'kernel', 'c']
      )
