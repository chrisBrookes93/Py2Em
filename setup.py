from distutils.core import setup, Extension

native_module = Extension('Py2Em', sources=['py2_em.c'])

setup(
	name='Py2Em',
	version='1.0',
	description='Execute Python code in an emulated Python2.7 terminal (within Python3)',
	ext_modules=[native_module]
)