import os
import unittest
import warnings

from py2_em import Py2Emulator

ULLONG_MAX = 18446744073709551615
LLONG_MIN = -9223372036854775807
LONG_MAX = 2147483647


class IntegrationTests(unittest.TestCase):

    def setUp(self):
        if os.environ.get('PY2_HOME'):
            self.py2_home = os.environ['PY2_HOME']
        else:
            self.py2_home = None

        if Py2Emulator.is_initialized():
            Py2Emulator.finalize()

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
        Py2Emulator.initialize(py2_home=self.py2_home)
        Py2Emulator.exec('import sys')
        version = Py2Emulator.eval('sys.version')
        self.assertIsInstance(version, str)
        self.assertTrue(len(version) > 0)

    def test_is_python2(self):
        Py2Emulator.initialize(py2_home=self.py2_home)
        Py2Emulator.exec('import sys')
        version = Py2Emulator.eval('sys.version')
        self.assertRegex(version, '2.[\\d]{1}.[\\d]{1,2}')

    def test_finalize_is_not_initialized(self):
        expected_error = 'Interpreter is not Initialized.'

        with warnings.catch_warnings(record=True) as warn:
            warnings.simplefilter('always')
            Py2Emulator.finalize()
            self.assertEqual(1, len(warn))
            self.assertRegexpMatches(str(warn[0].message), expected_error)

    def test_finalize_correct_case(self):
        Py2Emulator.initialize(py2_home=self.py2_home)
        self.assertTrue(Py2Emulator.is_initialized())
        Py2Emulator.exec('import sys')
        Py2Emulator.finalize()
        self.assertFalse(Py2Emulator.is_initialized())

    def test_multiple_initialize(self):
        Py2Emulator.initialize(py2_home=self.py2_home)
        self.assertTrue(Py2Emulator.is_initialized())
        self.assertEqual(2, Py2Emulator.eval('1+1'))

        with warnings.catch_warnings(record=True) as warn:
            warnings.simplefilter('always')
            Py2Emulator.initialize(py2_home=self.py2_home)
            self.assertEqual(1, len(warn))
            self.assertRegexpMatches(str(warn[0].message), 'Interpreter is already Initialized.')

        self.assertTrue(Py2Emulator.is_initialized())
        self.assertEqual(3, Py2Emulator.eval('1+2'))
        Py2Emulator.finalize()

    def test_multiple_finalize(self):
        Py2Emulator.initialize(py2_home=self.py2_home)
        self.assertTrue(Py2Emulator.is_initialized())
        self.assertEqual(2, Py2Emulator.eval('1+1'))
        Py2Emulator.finalize()

        self.assertFalse(Py2Emulator.is_initialized())
        with warnings.catch_warnings(record=True) as warn:
            warnings.simplefilter('always')
            Py2Emulator.finalize()
            self.assertEqual(1, len(warn))
            self.assertRegexpMatches(str(warn[0].message), 'Interpreter is not Initialized.')

        self.assertFalse(Py2Emulator.is_initialized())
        with warnings.catch_warnings(record=True) as warn:
            warnings.simplefilter('always')
            Py2Emulator.finalize()
            self.assertEqual(1, len(warn))
            self.assertRegexpMatches(str(warn[0].message), 'Interpreter is not Initialized.')

    def test_as_context_manager(self):
        with Py2Emulator(py2_home=self.py2_home) as py2em:
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
        with Py2Emulator(py2_home=self.py2_home) as py2em:
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
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            py2em.exec(custom_class)
            py2em.exec('div_class = DivClass(10)')
            div_value = py2em.eval('div_class.do_div(3)')
            self.assertEqual(3, div_value)

    def test_eval_return_int(self):
        expected_val = 1337
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval('1337')

        self.assertIsInstance(actual_val, int)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_long(self):
        expected_val = 999999999999999

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, int)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_ulong(self):
        expected_val = ULLONG_MAX - 10

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, int)
        self.assertEqual(expected_val, actual_val)

    def test_eval_positive_overflow_handled_gracefully(self):
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            self.assertRaisesRegex(RuntimeError,
                                   'Long value overflow. Value is larger than an unsigned long long.',
                                   py2em.eval,
                                   str(ULLONG_MAX + 10))

    def test_eval_negative_int(self):
        expected_val = -1337

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, int)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_negative_long(self):
        expected_val = -999999999999999

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, int)
        self.assertEqual(expected_val, actual_val)

    def test_eval_underflow_handled_gracefully(self):
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            self.assertRaisesRegex(RuntimeError,
                                   'Long value overflow/underflow. Value does not fit in a long long',
                                   py2em.eval,
                                   str(LLONG_MIN - 10))

    def test_eval_return_float(self):
        expected_val = float(1.34)

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, float)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_float_precision(self):
        expected_val = float(1.123456789123456789123456789)

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, float)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_complex(self):
        expected_val = 2+3j

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, complex)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_str(self):
        expected_val = 'abcdefghijklmnopqrstuvwxyzABCDEF'

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval('"{}"'.format(expected_val))

        self.assertIsInstance(actual_val, str)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_unicode(self):
        import base64
        expected_val = 'µ, ж, з, к, л'
        b64_data = base64.b64encode(expected_val.encode('utf-8')).decode('utf-8')
        input_val = "a = base64.b64decode(u'{}')".format(b64_data)
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            py2em.exec("import base64")
            py2em.exec(input_val)
            py2em.exec('print(type(a))')
            actual_val = py2em.eval('a')

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_none(self):

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(None))

        self.assertIsNone(actual_val)

    def test_eval_return_bool(self):
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            should_be_true = py2em.eval(str(True))
            should_be_false = py2em.eval(str(False))

        self.assertIsInstance(should_be_true, bool)
        self.assertTrue(should_be_true)
        self.assertIsInstance(should_be_false, bool)
        self.assertFalse(should_be_false)

    def test_eval_return_class(self):
        expected_val = "<class 'collections.OrderedDict'>"

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            py2em.exec('from collections import OrderedDict')
            actual_val = py2em.eval('OrderedDict')

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_class_instance(self):
        expected_regex = "_sre.SRE_Pattern object at"

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            py2em.exec('import re')
            actual_val = py2em.eval("re.compile('blah')")

        self.assertRegex(actual_val, expected_regex)

    def test_eval_return_list(self):
        expected_val = ['a', 1, 3.3, u'µµ', [1], {'a': 1}, set([124])]

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertEqual(expected_val, actual_val)

    def test_eval_return_set(self):
        expected_val = set([1, 1, 2, 4])

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))
            print(actual_val)

        self.assertIsInstance(actual_val, set)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_tuple(self):
        expected_val = ('a', 2, 3.14)

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, tuple)
        self.assertEqual(expected_val, actual_val)

    def test_eval_return_dict(self):
        expected_val = {'a': 1, 'b': 'b', 'c': 3.14}

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, dict)
        self.assertDictEqual(expected_val, actual_val)

    def test_eval_deeply_nested_structure(self):
        expected_val = {'a': 1, 'c': [[[[[[[[[[[[[[(1, 2, {'d': 0, 'f': set([123, 456, 1.34, "a"])})]]]]]]]]]]]]]]}
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            actual_val = py2em.eval(str(expected_val))

        self.assertIsInstance(actual_val, dict)
        self.assertDictEqual(expected_val, actual_val)

    def test_invalid_syntax_handled_gracefully(self):
        thrown1 = False
        thrown2 = False
        with Py2Emulator(py2_home=self.py2_home) as py2em:
            try:
                py2em.exec('red lorry')
            except RuntimeError as e:
                self.assertIn('red lorry', str(e))
                thrown1 = True

            try:
                py2em.eval('yellow lorry')
            except RuntimeError as e:
                self.assertIn('yellow lorry', str(e))
                thrown2 = True

            self.assertTrue(thrown1)
            self.assertTrue(thrown2)
            self.assertEqual(3, py2em.eval('1 + 2'))

    def test_invalid_python_home(self):
        with self.assertRaisesRegex(ImportError, 'Py2 Interpreter failed to import the \'site\' module. Have you set '
                                                 'the py2_home correctly\\?. Exception caught'):
            Py2Emulator.initialize(py2_home='C:/NOT_EXIST')

    def test_invalid_py2_binary(self):
        with self.assertRaisesRegex(RuntimeError, 'Failed to find Python2 binary at'):
            Py2Emulator.initialize(py2_home=self.py2_home, py2_binary_path='C:\\NOT_EXIST')

    def test_import_stdlibs(self):
        import_ls = ['import base64',
                     'import os',
                     'import sys',
                     'from collections import OrderedDict']

        with Py2Emulator(py2_home=self.py2_home) as py2em:
            for stmt in import_ls:
                py2em.exec(stmt)
