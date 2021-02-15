# Py2Em
Py2Em is library that emulates a Python2 interpreter, allowing you to execute Python code under 
Python2, from Python3. This is achieved by embedding a Python2 interpreter into a Python3 C extension module.
It currently only allows you to execute code under Python2, but not return anything (equivalent of an ```exec()```), 
but this will be extended shortly to provide a rich API. At this point, this repo contains an MVP that one may 
extend/tailor to their needs.

## Why the need?
You may be asking yourself... *"Why not just port your code to Python3?"*. This was designed with a specific use case in
mind, specifically a project in which many hundred Python2 code snippets are executed dynamically. It would be a huge 
task to port all of these to Python3, so Python2 emulation provides a suitable workaround (for now).

## Implementation Details
The C extension module dynamically loads the Python2 Shared Object file (```libpython2.so```) into memory (using ```dlopen```, ```dlsym```, 
```dlclose```), and then invokes the necessary functions to setup a Python interpreter. 
This is done to get around an issue whereby you need to include ```Python.h``` from both versions of Python, 
leading to function name conflicts, and the linker simply links to the first shared library that's specified at compile time.

## Environment Requirements
* Linux with Python2.7 & Python3 installed
* ```libpython2.7-dev``` package installed (this creates ```libpython2.7.so```)
* GCC

#### Ubuntu 20.04 Setup Instructions
This project has only ever been tested on Ubuntu 20.04.

Steps:
* Install [Ubuntu 20.04.2 Desktop (amd64)](http://old-releases.ubuntu.com/releases/focal/ubuntu-20.04.2-desktop-amd64.iso)
* Update packages via ```> sudo apt update```
* Install dependencies ```> sudo apt install git python2.7 libpython2.7-dev python3-pip```
* Checkout and install via ```Py2Em> sudo pip3 install .```
* Test via ```Py2Em> python3.8 utils/example.py```. You should see the same output as in the Usage section below.

## Usage
The ```Py2Emulator``` class exposes three static functions:
* ```initialize()```
* ```py2_exec(str)```
* ```finalize()```

Basic Usage:
```python
from py2_em import Py2Emulator
import sys

# Initialize the Py2 interpreter
print('--> Initializing py2_em')
Py2Emulator.initialize()
print()

print('--> Normal interpreter version is: ')
print(sys.version)
print()

print('--> py2_em interpreter version is: ')
Py2Emulator.py2_exec('import sys;print(sys.version)')
print()

print('--> Normal interpreter\'s answer to "10/3" is: ')
print(10 / 3)
print()

print('--> py2_em interpreter\'s answer to "10/3" is: ')
Py2Emulator.py2_exec('print(10 / 3)')
print()

print('--> Finalizing py2_em')
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

```Py2Emulator``` has also been implemented as context manager:
```python
with Py2Emulator() as py2_emulator:
     py2_emulator.exec('print("hello world")')
```

## Contributions
All contributions are welcome and appreciated, feel free to create an issue, fork this repo and raise pull requests.

## Next Steps
* Add more error handling/checking to the C extension.
* Add the ability to retrieve values back from a string execution.
* Add the ability to execute complex Python code (```eval()``` as well as ```exec()```).
* Add the ability to share ```locals()``` and ```globals()``` between the interpreters.
* Add support for execution of Python files.
* Test/add support for other Linux environments.
* Add support for MacOS and Windows.

## Resources
* https://docs.python.org/2.7/c-api/veryhigh.html
* https://docs.python.org/3/extending/extending.html