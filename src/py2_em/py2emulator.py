import _py2_em


class Py2Emulator:

    def __enter__(self, libpython_path='libpython2.7.so'):
        """
        Initializes the Python2 Emulator

        :param libpython_path: Path to the Python Library .so file. If not a full path the string must be resolvable on
        the LD search path
        :type libpython_path: str
        """
        Py2Emulator.initialize(libpython_path)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Finalizes the Python2 Emulator
        :param exc_type:
        :param exc_val:
        :param exc_tb:
        """
        Py2Emulator.finalize()

    def exec(self, eval_str):
        """
        Not static version of py2_exec, for if you're using this class as a context manager

        :param eval_str: Python string to execute
        :type eval_str: str
        """
        _py2_em.Py2_Exec(eval_str)

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
    def py2_exec(eval_str):
        """
        Executes a Python string in the Python2 emulator interpreter. Equivalent to an exec()

        :param eval_str: Python string to execute
        :type eval_str: str
        """
        _py2_em.Py2_Exec(eval_str)

    @staticmethod
    def finalize():
        """
        Finalizes the Python2 Emulator
        """
        _py2_em.Finalize()
