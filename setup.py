from setuptools import setup, Extension, find_packages

native_module = Extension('_py2_em', sources=['src/ext/_py2_em.c'])

with open('README.md', 'r') as fp:
	long_description = fp.read()

setup(
	name='py2_em',
	version='1.0',
	description='Execute Python code in an emulated Python2.7 terminal (within Python3)',
	ext_modules=[native_module],
	author='Chris Brookes',
	author_email='chris_b_856@hotmail.com',
	long_description=long_description,
	license='MIT',
	url='https://github.com/chrisBrookes93/Py2Em',
	python_requires='>=3',
	classifiers=[
		'Development Status :: 1 - Planning',
		'Programming Language :: Python :: 3',
		'License :: OSI Approved :: MIT License',
		'Operating System :: POSIX :: Linux'
	],
	packages=find_packages(where='src'),
	package_dir={'': 'src'},
	extras_require={
		'dev': ['pytest==6.2.2']
	}
	)
