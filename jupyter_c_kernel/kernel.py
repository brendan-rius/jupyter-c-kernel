from ipykernel.kernelbase import Kernel
import subprocess
import tempfile
import os


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
    def execute_command(cmd):
        """Execute a command and returns the return code, stdout and stderr"""
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = p.communicate()
        return p.returncode, stdout.decode('utf-8'), stderr.decode('utf-8')

    @staticmethod
    def compile_with_gcc(source_filename, binary_filename):
        args = ['gcc', source_filename, '-std=c11', '-o', binary_filename]
        return CKernel.execute_command(args)

    def do_execute(self, code, silent, store_history=True,
                   user_expressions=None, allow_stdin=False):

        retcode, stdout, stderr = None, '', ''
        with self.new_temp_file(suffix='.c') as source_file:
            source_file.write(code)
            source_file.flush()
            with self.new_temp_file(suffix='.out') as binary_file:
                retcode, stdout, stderr = self.compile_with_gcc(source_file.name, binary_file.name)
                if retcode != 0:
                    stderr += "[C kernel] GCC exited with code {}, the executable will not be executed".format(retcode)
                self.log.info("GCC return code: {}".format(retcode))
                self.log.info("GCC stdout: {}".format(stdout))
                self.log.info("GCC stderr: {}".format(stderr))

        if retcode == 0:  # If the compilation succeeded
            retcode, out, err = CKernel.execute_command([binary_file.name])
            if retcode != 0:
                stderr += "[C kernel] Executable exited with code {}".format(retcode)
            self.log.info("Executable retcode: {}".format(retcode))
            self.log.info("Executable stdout: {}".format(out))
            self.log.info("Executable stderr: {}".format(err))
            stdout += out
            stderr += err
        else:
            self.log.info('Compilation failed, the program will not be executed')

        if not silent:
            stream_content = {'name': 'stderr', 'text': stderr}
            self.send_response(self.iopub_socket, 'stream', stream_content)
            stream_content = {'name': 'stdout', 'text': stdout}
            self.send_response(self.iopub_socket, 'stream', stream_content)
        return {'status': 'ok', 'execution_count': self.execution_count, 'payload': [], 'user_expressions': {}}

    def do_shutdown(self, restart):
        """Cleanup the created source code files and executables when shutting down the kernel"""
        self.cleanup_files()
