FROM jupyter/minimal-notebook
MAINTAINER Brendan Rius <ping@brendan-rius.com>

USER root

COPY ./ /home/$NB_USER/.jupyter/jupyter_c_kernel/
RUN pip install /home/$NB_USER/.jupyter/jupyter_c_kernel/
RUN jupyter-kernelspec install /home/$NB_USER/.jupyter/jupyter_c_kernel/