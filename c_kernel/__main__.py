from ipykernel.kernelapp import IPKernelApp
from .kernel import CKernel
IPKernelApp.launch_instance(kernel_class=CKernel)
