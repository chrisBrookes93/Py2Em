import unittest
from py2_em import Py2Emulator


class IntegrationTests(unittest.TestCase):

    def test_is_not_initialized(self):
        expected_error = 'Python Interpreter is not Initialized. You need to call Init\\(\\) first.'
        self.assertRaisesRegex(RuntimeError,
                               expected_error,
                               Py2Emulator.py2_exec,
                               'print("hello world")')

    def test_correct_case(self):
        Py2Emulator.initialize()
        Py2Emulator.py2_exec('print("hello world")')
        Py2Emulator.finalize()

    def test_finalize_before_initialize(self):
        expected_error = 'Python Interpreter is not Initialized.'
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
             Py2Emulator.py2_exec('print("hello world")')
