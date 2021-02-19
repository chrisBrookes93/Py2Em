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
        self.assertRegex(version, '2.[\\d]{1}.[\\d]{1,2}')

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

    def test_eval_return_int(self):
        expected_val = 1337
        with Py2Emulator() as py2em:
            actual_val = py2em.eval('1337')
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_long(self):
        expected_val = 999999999999999999

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_longer_long(self):
        expected_val = 999999999999999999999999999999999999999999

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_even_longer_long(self):
        expected_val = 999999999999999999999999999999999999999999999999999999999999999999999999999999999999

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_float(self):
        expected_val = float(1.34)

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_complex(self):
        expected_val = 2+3j

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_str(self):
        expected_val = 'abcdefghijklmnopqrstuvwxyzABCDEF'

        with Py2Emulator() as py2em:
            actual_val = py2em.eval('"{}"'.format(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_unicode(self):
        raise NotImplementedError
    #     expected_val = u'µµµµµµµ'
    #
    #     with Py2Emulator() as py2em:
    #         actual_val = py2em.eval('u"{}"'.format(expected_val))
    #
    #     self.assertEqual(expected_val, actual_val)

    def test_eval_return_none(self):
        expected_val = None

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_bool(self):
        with Py2Emulator() as py2em:
            should_be_true = py2em.eval(str(True))
            should_be_false = py2em.eval(str(False))

        self.assertIsInstance(should_be_true, bool)
        self.assertTrue(should_be_true)
        self.assertIsInstance(should_be_false, bool)
        self.assertFalse(should_be_false)

    def test_eval_return_class(self):
        expected_val = "<class 'collections.OrderedDict'>"

        with Py2Emulator() as py2em:
            py2em.exec('from collections import OrderedDict')
            actual_val = py2em.eval('OrderedDict')

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_class_instance(self):
        expected_regex = "_sre.SRE_Pattern object at"

        with Py2Emulator() as py2em:
            py2em.exec('import re')
            actual_val = py2em.eval("re.compile('blah')")

        self.assertRegex(actual_val, expected_regex)

    def test_eval_return_list(self):
        expected_val = ['a', 1, 3.3, u'µµ', [1], {'a': 1}, set([124])]

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_set(self):
        expected_val = set([1, 1, 2, 4])

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, set)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_tuple(self):
        expected_val = ('a', 2, 3.14)

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, tuple)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_dict(self):
        expected_val = {'a': 1, 'b': 'b', 'c': 3.14}

        with Py2Emulator() as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, dict)
        self.assertDictEqual(expected_val, actual_val)
