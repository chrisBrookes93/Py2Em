import unittest
from py2_em import Py2Emulator


class IntegrationTests(unittest.TestCase):

    def setUp(self):
        try:
            Py2Emulator.finalize()
        except RuntimeError:
            pass

    def test_exec_is_not_initialized(self):
        expected_error = 'Interpreter is not Initialized.'
        self.assertRaisesRegex(RuntimeError,
                               expected_error,
                               Py2Emulator.exec,
                               'print("hello world")')

    def test_eval_is_not_initialized(self):
        expected_error = 'Interpreter is not Initialized.'
        self.assertRaisesRegex(Exception,
                               expected_error,
                               Py2Emulator.eval,
                               '1+1')

    def test_exec_correct_case(self):
        Py2Emulator.initialize()
        Py2Emulator.exec('import sys')
        version = Py2Emulator.eval('sys.version')
        self.assertIsInstance(version, str)
        self.assertTrue(len(version) > 0)

    def test_is_python2(self):
        Py2Emulator.initialize()
        Py2Emulator.exec('import sys')
        version = Py2Emulator.eval('sys.version')
        self.assertRegexpMatches(version, '2.[\\d]{1}.[\\d]{1,2}')

    def test_finalize_is_not_initialized(self):
        expected_error = 'Interpreter is not Initialized.'
        self.assertRaisesRegex(RuntimeError,
                               expected_error,
                               Py2Emulator.finalize)

    def test_finalize_correct_case(self):
        Py2Emulator.initialize()
        self.assertTrue(Py2Emulator.is_initialized())
        Py2Emulator.exec('import sys')
        Py2Emulator.finalize()
        self.assertFalse(Py2Emulator.is_initialized())

    def test_multiple_initialize(self):
        Py2Emulator.initialize()
        self.assertTrue(Py2Emulator.is_initialized())
        self.assertEqual(2, Py2Emulator.eval('1+1'))
        Py2Emulator.initialize()
        self.assertTrue(Py2Emulator.is_initialized())
        self.assertEqual(3, Py2Emulator.eval('1+2'))
        Py2Emulator.finalize()

    def test_multiple_finalize(self):
        Py2Emulator.initialize()
        self.assertTrue(Py2Emulator.is_initialized())
        self.assertEqual(2, Py2Emulator.eval('1+1'))
        Py2Emulator.finalize()
        self.assertFalse(Py2Emulator.is_initialized())
        self.assertRaises(RuntimeError, Py2Emulator.finalize)
        self.assertFalse(Py2Emulator.is_initialized())
        self.assertRaises(RuntimeError, Py2Emulator.finalize)
        self.assertFalse(Py2Emulator.is_initialized())

    def test_as_context_manager(self):
        with Py2Emulator() as py2em:
            self.assertTrue(py2em.is_initialized())
            py2em.exec('import sys')
            version = py2em.eval('sys.version')
            self.assertIsInstance(version, str)
        self.assertFalse(py2em.is_initialized())

    def test_exec_func_then_eval(self):
        div_func = """
def custom_div(num_a, num_b):
    return num_a / num_b
"""
        with Py2Emulator() as py2em:
            py2em.exec(div_func)
            div_value = py2em.eval('custom_div(10, 3)')
            self.assertEqual(3, div_value)

    def test_exec_class_then_eval(self):
        custom_class = """
class DivClass:

    def __init__(self, num_a):
        self._num_a = num_a

    def do_div(self, num_b):
        return self._num_a / num_b

"""
        with Py2Emulator() as py2em:
            py2em.exec(custom_class)
            py2em.exec('div_class = DivClass(10)')
            div_value = py2em.eval('div_class.do_div(3)')
            self.assertEqual(3, div_value)
