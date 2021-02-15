import unittest
from py2_em import Py2Emulator


class IntegrationTests(unittest.TestCase):

    def setUp(self):
        try:
            # To ensure the interpreter is not initialized when the test starts
            Py2Emulator.finalize()
        except RuntimeError:
            pass

    def test_is_not_initialized(self):
        expected_error = 'Python Interpreter is not initialized.'
        self.assertRaisesRegex(RuntimeError,
                               expected_error,
                               Py2Emulator.py2_exec,
                               'print("hello world")')

    def test_correct_case(self):
        Py2Emulator.initialize()
        Py2Emulator.py2_exec('print("hello world")')
        Py2Emulator.finalize()

    def test_finalize_before_initialize(self):
        expected_error = 'Python Interpreter is not Initialized'
        self.assertRaisesRegex(RuntimeError,
                               expected_error,
                               Py2Emulator.finalize)

    def test_multiple_init_finalize(self):
        Py2Emulator.initialize()
        Py2Emulator.py2_exec('print("hello world")')
        Py2Emulator.finalize()
        Py2Emulator.initialize()
        Py2Emulator.py2_exec('print("hello world")')
        Py2Emulator.finalize()
        Py2Emulator.initialize()
        Py2Emulator.py2_exec('print("hello world")')
        Py2Emulator.finalize()

    def test_as_context_manager(self):
        with Py2Emulator() as py2_emulator:
             py2_emulator.exec('print("hello world")')

    def test_incorrect_so_path(self):
        expected_error = 'cannot open shared object file: No such file or directory'
        self.assertRaisesRegex(RuntimeError,
                               expected_error,
                               Py2Emulator.initialize,
                               'NOT_EXIST.so')
