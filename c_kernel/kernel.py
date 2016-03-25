from ipykernel.kernelbase import Kernel
import subprocess
import tempfile


class CKernel(Kernel):
    implementation = 'c_kernel'
    implementation_version = '1.0'
    language = 'c'
    language_version = 'C11'
    language_info = {'name': 'c', 'mimetype': 'text/plain', 'file_extension': 'c'}
    _banner = None

    @property
    def banner(self):
        if self._banner is None:
            self._banner = subprocess.check_output(['gcc', '-v']).decode('utf-8')
        return self._banner

    def do_execute(self, code, silent, store_history=True,
                   user_expressions=None, allow_stdin=False):
        code = code.strip()
        if not code:
            return {'status': 'ok',
                    'execution_count': self.execution_count,
                    'payload': [],
                    'user_expressions': {}}

        output = '### COMPILATION ###\n'
        try:
            sourcefile = tempfile.NamedTemporaryFile(suffix='.c', delete=False)
            sourcefile.write(code)
            sourcefile.close()
            binaryfile = tempfile.NamedTemporaryFile(suffix='.out', delete=False)
            binaryfile.close()
            output += subprocess.check_output(['gcc', '-std=c11', sourcefile.name, '-o', binaryfile.name],
                                              stderr=subprocess.STDOUT).decode('utf-8')
        except subprocess.CalledProcessError as e:
            print(e)
            return {'status': 'error', 'ename': 'Compilation error', 'evalue': e.output}

        output += '\n### EXECUTION ###\n'
        try:
            output += subprocess.check_output([binaryfile.name], stderr=subprocess.STDOUT).decode('utf-8')
        except subprocess.CalledProcessError as e:
            output += e.output
        if not silent:
            stream_content = {'name': 'stdout', 'text': output}
            self.send_response(self.iopub_socket, 'stream', stream_content)
