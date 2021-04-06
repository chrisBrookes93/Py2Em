# Py2Em
Py2Em is library that emulates a Python2 interpreter, allowing you to execute Python code under 
Python2, from Python3. This is achieved by embedding a Python2 interpreter into a Python3 C extension module.
You can then execute and evaluate (equivalent to [```exec()```](https://docs.python.org/3/library/functions.html#exec) 
and [```eval()```](https://docs.python.org/3/library/functions.html#eval)) code in Python2 by marshalling a 
call from Python3.

## Why the need?
You may be asking yourself... *"Why not just port your code to Python3?"*...and you would be correct!. 
This was designed with a specific use case in mind, a project in which many hundred Python2 code snippets 
are executed dynamically. 
It would be a huge task to port all of these to Python3, so Python2 emulation provides a suitable workaround (for now).

## Implementation Details
The C extension module dynamically loads the main Python2 binary file (e.g. ```libpython2.7.so``` on Linux or 
```Python2.7.dll``` on Windows) into memory (using 
[```dlopen()```](https://man7.org/linux/man-pages/man3/dlopen.3.html)) / [```LoadLibrary()```](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya), 
and then invokes the necessary 
functions to initialize a Python2 interpreter. 
The dynamic loading is done to get around an issue whereby you need to include ```Python.h``` from both versions of Python, 
leading to function name conflicts, and the linker choosing one or the other.

Once the Python2 interpreter has been initialized, calls are made to ```PyRun_String()``` from the loaded Python2 
binary everytime something is executed on the emulator. 
The result of the execution is marshalled from a Python2 ```PyObject *``` to a Python3 ```PyObject *```, providing seamless integration into your application. 
Currently, only built-in types can be marshalled (i.e. ```int```, ```float```, ```long```, ```complex```, ```str```, 
```bool```, ```list```, ```set```, ```dict```, ```tuple```). Anything else, the ```str()``` value will be returned instead.

## Environment Requirements

Current official support is Ubuntu, Debian and Windows. 
#### Ubuntu & Debian
* Python3 installed 
* Python2-dev installed (e.g. ```libpython2.7-dev```).
* GCC

#### Windows
* Python2 and Python3 installed.
* Microsoft Visual C++

## Usage
#### Initializing and Finalizing the Python2 Interpreter
To setup the Python2 Interpreter, import ```Py2Emulator``` from ```py2_em``` and then statically call ```initialize()```.
Once you are finished with your Interpreter, close it by calling ```finalize()```. This is a global interpreter, and 
you can only have one active at once.

```python
from py2_em import Py2Emulator
Py2Emulator.initialize()

Py2Emulator.eval('10 / 3')

Py2Emulator.finalize()
```
You can also use Py2Emulator as a Context Manager:
```python
with Py2Emulator() as py2_emulator:
     py2_emulator.eval('10 / 3')

```

The ```initialize()``` function takes two optional arguments:
* ```py2_binary_path``` - This is defaulted to ```libpython2.7.so``` on Linux and ```python27.dll``` on Windows. 
If you have an irregular setup, or are using pre-Python2.7, you can use this to direct the library to the Python binary. 
This value can either a full path, or a filename (as long as the file can be found in the search path).
 * ```py2_home``` - This is defaulted to empty on Linux and ```C:\Python27``` on Windows. 
 This argument allows you to set the [```PYTHONHOME```](https://docs.python.org/2.7/using/cmdline.html#envvar-PYTHONHOME) 
 variable which impacts Python's search path.
 Python's search path generation behaves differently between Linux and Windows. 
 It must be set on Windows and is recommended to leave empty on Linux. 
 Only set if you are on Windows and your Python2 installation is not ```C:\Python27```.


#### Executing and Evaluating
Once initialized, you can execute and evaluate Python expressions via two functions:
* ```Py2Emulator.exec(str)```
* ```Py2Emulator.eval(str)```

These behave the same was as [```exec()```](https://docs.python.org/3/library/functions.html#exec) and 
[```eval()```](https://docs.python.org/3/library/functions.html#eval) in Python, but will execute the code in the Python2 emulator.

 


Example Usage:
```python
from py2_em import Py2Emulator
import sys

print('--> Initializing py2_em\n')
Py2Emulator.initialize()

print('--> Normal interpreter version is: ')
print(sys.version + '\n')

print('--> py2_em interpreter version is: ')
Py2Emulator.exec('import sys')
print(Py2Emulator.eval('sys.version') + '\n')

print('--> Normal interpreter\'s answer to "10/3" is: ')
print(str(10 / 3) + '\n')

print('--> py2_em interpreter\'s answer to "10/3" is: ')
print(Py2Emulator.eval('str(10 / 3)') + '\n')

print('Finalizing py2_em')
Py2Emulator.finalize()
```
This produces the following output:

```
--> Initializing Py2Em

--> Normal interpreter version is: 
3.8.5 (default, Jul 28 2020, 12:59:40) [GCC 9.3.0]

--> Py2Em interpreter version is: 
2.7.18 (default, Aug  4 2020, 11:16:42) [GCC 9.3.0]

--> Normal interpreter's answer to "10/3" is: 
3.3333333333333335

--> Py2Em interpreter's answer to "10/3" is: 
3

--> Finalizing Py2Em
```

## Troubleshooting and Workarounds
If any Python errors occur in the Python2 emulator, a ```RuntimeError``` will be raised in Python3.
If any unhandled errors occur in the C extension itself then it is possible that it will Seg Fault and crash the process. 
In aid in tracking down any issue you may encounter, you can enable logging by setting a value of ```1``` to the ```LOGGING_ON``` macro
in the ```setup.py``` and then reinstalling the package.

#### Unicode Input
This package doesn't support passing unicode strings into ```exec()``` or ```eval()```. 
If you need to deal with characters outside of the ASCII range in your input then consider some kind of encoding. e.g.

```python
import base64
expected_val = '<string with unicode characters>'
b64_data = base64.b64encode(expected_val.encode('utf-8')).decode('utf-8')
input_val = "a = base64.b64decode(u'{}')".format(b64_data)

with Py2Emulator() as py2em:
    py2em.exec("import base64")
    py2em.exec(input_val)
    py2em.exec('print(a)')
    actual_val = py2em.eval('a')
    assert actual_val == expected_val
```

## Contributions
All contributions are welcome and appreciated, feel free to create an issue, fork this repo and raise pull requests.

## Further Improvements
* Add support for MacOS and other Linux distributions
* Add the ability to share ```locals()``` and ```globals()``` between the interpreters.
* Add support for execution of Python files.

## Resources
* https://docs.python.org/2.7/c-api/veryhigh.html
* https://docs.python.org/3/extending/extending.html