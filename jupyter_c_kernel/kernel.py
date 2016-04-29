from threading import Thread

import time
from ipykernel.kernelbase import Kernel
import subprocess
import tempfile
import os
from Queue import Queue, Empty


class CKernel(Kernel):
    implementation = 'jupyter_c_kernel'
    implementation_version = '1.0'
    language = 'c'
    language_version = 'C11'
    language_info = {'name': 'c',
                     'mimetype': 'text/plain',
                     'file_extension': 'c'}
    banner = "C kernel.\n" \
             "Uses gcc, compiles in C11, and creates source code files and executables in temporary folder.\n"

    def __init__(self, *args, **kwargs):
        super(CKernel, self).__init__(*args, **kwargs)
        self.files = []

    def cleanup_files(self):
        """Remove all the temporary files created by the kernel"""
        for file in self.files:
            os.remove(file)

    def new_temp_file(self, **kwargs):
        """Create a new temp file to be deleted when the kernel shuts down"""
        # We don't want the file to be deleted when closed, but only when the kernel stops
        kwargs['delete'] = False
        kwargs['mode'] = 'w'
        file = tempfile.NamedTemporaryFile(**kwargs)
        self.files.append(file.name)
        return file

    @staticmethod
    def launch_process(cmd):
        """Execute a command and returns the return code, stdout and stderr"""
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        stdout_queue = Queue()
        stdout_thread = Thread(target=CKernel.enqueue_output, args=(p.stdout, stdout_queue))
        stdout_thread.daemon = True
        stdout_thread.start()

        stderr_queue = Queue()
        stderr_thread = Thread(target=CKernel.enqueue_output, args=(p.stderr, stderr_queue))
        stderr_thread.daemon = True
        stderr_thread.start()

        return p, stdout_queue, stderr_queue

    @staticmethod
    def compile_with_gcc(source_filename, binary_filename):
        args = ['gcc', source_filename, '-std=c11', '-o', binary_filename]
        return CKernel.launch_process(args)

    @staticmethod
    def enqueue_output(out, queue):
        for line in iter(out.readline, b''):
            queue.put(line)
        out.close()

    def do_execute(self, code, silent, store_history=True,
                   user_expressions=None, allow_stdin=False):
        a = raw_input()
        with self.new_temp_file(suffix='.c') as source_file:
            source_file.write(code)
            source_file.flush()
            with self.new_temp_file(suffix='.out') as binary_file:
                p, stdout_queue, stderr_queue = self.compile_with_gcc(source_file.name, binary_file.name)
                while p.poll() is None:
                    self.log.error("Waiting for compilation to end")
                    try:
                        stdout = stdout_queue.get_nowait()
                    except Empty:
                        self.log.error("stdout empty")
                    else:
                        self.log.error("stdout contains: {}".format(stdout))
                        self.send_response(self.iopub_socket, 'stream', {'name': 'stdout', 'text': stdout})
                    try:
                        stderr = stderr_queue.get_nowait()
                    except Empty:
                        self.log.error("stderr empty")
                    else:
                        self.log.error("stderr contains: {}".format(stderr))
                        self.send_response(self.iopub_socket, 'stream', {'name': 'stderr', 'text': stderr})

                if p.returncode != 0:  # Compilation failed
                    stream_content = {'name': 'stderr',
                                      'text': "[C kernel] GCC exited with code {}, the executable will not be executed".format(
                                              p.returncode)}
                    self.send_response(self.iopub_socket, 'stream', stream_content)
                    return {'status': 'ok', 'execution_count': self.execution_count, 'payload': [],
                            'user_expressions': {}}

        p, stdout_queue, stderr_queue = CKernel.launch_process([binary_file.name])
        while p.poll() is None:
            self.log.error("Waiting for execution to end")
            try:
                stdout = stdout_queue.get_nowait()
            except Empty:
                self.log.error("stdout empty")
            else:
                self.log.error("stdout contains: {}".format(stdout))
                self.send_response(self.iopub_socket, 'stream', {'name': 'stdout', 'text': stdout})
            try:
                stderr = stderr_queue.get_nowait()
            except Empty:
                self.log.error("stderr empty")
            else:
                self.log.error("stderr contains: {}".format(stderr))
                self.send_response(self.iopub_socket, 'stream', {'name': 'stderr', 'text': stderr})

        if p.returncode != 0:
            stderr_queue += "[C kernel] Executable exited with code {}".format(p.returncode)
        return {'status': 'ok', 'execution_count': self.execution_count, 'payload': [], 'user_expressions': {}}

    def do_shutdown(self, restart):
        """Cleanup the created source code files and executables when shutting down the kernel"""
        self.cleanup_files()
