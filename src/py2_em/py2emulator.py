import _py2_em
import platform

EXEC_MODE_EVAL = 0
EXEC_MODE_EXEC = 1


class Py2Emulator:

    def __enter__(self, py2_binary_path=None, py2_home=None):
        """
        Initializes the Python2 Emulator

        :param py2_binary_path: Path to the Python2 binary file. If not a full path the string must be resolvable in
        the search path. Defaults to libpython2.7.so on Linux and Python27.dll on Windows
        :type py2_binary_path: str
        :param py2_home: Path to the directory containing the python2 executable. This is used to find the Python2
        site packages.
        :type py2_home: str
        """
        self.initialize(py2_binary_path, py2_home)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Finalizes the Python2 Emulator
        """
        self.finalize()

    @staticmethod
    def _get_default_binary_name():
        """
        Set the default Python2 binary name based on the OS

        :return: Python2 binary name for the given OS
        :rtype: str
        """
        if platform.system().lower() == 'windows':
            return 'Python27.dll'
        else:
            return 'libpython2.7.so'

    @staticmethod
    def _get_default_python_home():
        """
        Resolves a default PYTHONHOME value based on the OS

        :return: Default PYTHONHOME value based on the OS
        :rtype: str
        """
        if platform.system().lower() == 'windows':
            return 'C:\\Python27'
        else:
            return None

    @staticmethod
    def is_initialized():
        """
        Returns a bool indicating if the interpreter is initialized
        """        
        return _py2_em.IsInitialized()

    @staticmethod
    def initialize(py2_binary_path=None, py2_home=None):
        """
        Initializes the Python2 Emulator

        :param py2_binary_path: Path to the Python2 binary file. If not a full path the string must be resolvable in
        the search path. Defaults to libpython2.7.so on Linux and Python27.dll on Windows
        :type py2_binary_path: str
        :param py2_home: Path to the directory containing the python2 executable. This is used to find the Python2
        site packages.
        :type py2_home: str
        """
        if not py2_binary_path:
            py2_binary_path = Py2Emulator._get_default_binary_name()

        if not py2_home:
            py2_home = Py2Emulator._get_default_python_home()

        _py2_em.Initialize(py2_binary_path, py2_home)

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
