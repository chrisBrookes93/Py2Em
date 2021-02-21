import _py2_em

EXEC_MODE_EVAL = 0
EXEC_MODE_EXEC = 1


class Py2Emulator:

    def __enter__(self, libpython_path='libpython2.7.so'):
        """
        Initializes the Python2 Emulator

        :param libpython_path: Path to the Python Library .so file. If not a full path the string must be resolvable on
        the LD search path
        :type libpython_path: str
        """
        self.initialize(libpython_path)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Finalizes the Python2 Emulator
        """
        self.finalize()

    @staticmethod
    def is_initialized():
        """
        Returns a bool indicating if the interpreter is initialized
        """        
        return _py2_em.IsInitialized()

    @staticmethod
    def initialize(libpython_path='libpython2.7.so'):
        """
        Initializes the Python2 Emulator

        :param libpython_path: Path to the Python Library .so file. If not a full path the string must be resolvable on
        the LD search path
        :type libpython_path: str
        """
        _py2_em.Initialize(libpython_path)

    @staticmethod
    def exec(exec_str):
        """
        Executes a Python string in the Python2 emulator interpreter. Equivalent to an exec()

        :param exec_str: Python string to execute
        :type exec_str: str
        """
        return _py2_em.Py2_Run(exec_str, EXEC_MODE_EXEC)

    @staticmethod
    def eval(eval_str):
        """
        Evaluates a Python string in the Python2 emulator interpreter. Equivalent to an exec()

        :param eval_str: Python string to evaluate
        :type eval_str: str
        """
        return _py2_em.Py2_Run(eval_str, EXEC_MODE_EVAL)

    @staticmethod
    def finalize():
        """
        Finalizes the Python2 Emulator
        """
        _py2_em.Finalize()
