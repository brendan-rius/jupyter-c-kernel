FROM jupyter/minimal-notebook
MAINTAINER Brendan Rius <ping@brendan-rius.com>

USER root

RUN mkdir /jupyter

WORKDIR /jupyter

COPY ./ jupyter_c_kernel/
RUN pip install -e jupyter_c_kernel/

RUN jupyter-kernelspec install jupyter_c_kernel/c_spec/

WORKDIR /home/$NB_USER/
