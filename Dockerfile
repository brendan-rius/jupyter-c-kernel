FROM jupyter/minimal-notebook
MAINTAINER Brendan Rius <ping@brendan-rius.com>

USER root

WORKDIR /tmp

COPY ./ jupyter_c_kernel/

RUN pip install --no-cache-dir -e jupyter_c_kernel/
RUN cd jupyter_c_kernel && python -m jupyter_c_kernel.install_c_kernel --user

WORKDIR /home/$NB_USER/

USER $NB_USER
