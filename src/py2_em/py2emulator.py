import _py2_em


class Py2Emulator:

    def __enter__(self, libpython_path='libpython2.7.so', prog_name=None, python_home=None):
        """
        Initializes the Python2 Emulator

        :param libpython_path: Path to the Python Library .so file
        :type libpython_path: str
        :param prog_name: Optional name of the program that is passed into Py_SetProgramName()
        :type prog_name: str
        :param python_home: Optional python home directory. This is passed into Py_SetPythonHome()
        :type python_home: str
        """
        Py2Emulator.initialize(libpython_path, prog_name, python_home)

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Finalizes the Python2 Emulator
        :param exc_type:
        :param exc_val:
        :param exc_tb:
        :return:
        """
        Py2Emulator.finalize()

    @staticmethod
    def initialize(libpython_path='libpython2.7.so', prog_name=None, python_home=None):
        """
       Initializes the Python2 Emulator

       :param libpython_path: Path to the Python Library .so file
       :type libpython_path: str
       :param prog_name: Optional name of the program that is passed into Py_SetProgramName()
       :type prog_name: str
       :param python_home: Optional python home directory. This is passed into Py_SetPythonHome()
       :type python_home: str
       """
        _py2_em.Init()

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
