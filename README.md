# Py2Em
This is a Python library that emulates a Python2.7 interpreter, allowing you to execute Python code under 
Python2.7, from Python3. This is achieved by embedding a Python2 interpreter into a Python3 C extension module.

The C extension module dynamically loads the Python2 Shared Object file into memory (using ```dlopen```, ```dlsym```, 
```dlclose``` and then invokes the necessary functions to setup a Python interpreter. 
It is done this way to get around an issue whereby you need to include ```Python.h``` from both version of Python, 
leading to function name conflicts.

## Environment Requirements
* Linux with Python2 & Python3 installed
* GCC
* ```libpython2.7.so``` present in the linker (ld) search path 

### Specific Environment
As this project is at the first MVP stage, this has only been setup and used in a very specific environment.
In developing this I have experienced an ungodly amount of Seg Faults, and I have little confidence that this will work in many other situations.
Therefore, I have provided an exact set of instructions to get this library working on the environment it was written on:
* Install [Ubuntu 20.04.2 Desktop (amd64)](http://old-releases.ubuntu.com/releases/focal/ubuntu-20.04.2-desktop-amd64.iso)
* Update packages via ```> sudo apt update```
* Checkout this repo
* Install via ```Py2Em> sudo pip3 install .```
* Test via ```Py2Em> python3.8 test.py```

## Usage
The library exposes three functions:
* ```Init()```
* ```Py2_Exec(str)```
* ```Finalize()```

Basic Usage:
```python
import Py2Em
import sys

# Initialize the Py2 interpreter
print('--> Initializing Py2Em')
Py2Em.Init()

print('--> Normal interpreter version is: ')
print(sys.version)

print('--> Py2Em interpreter version is: ')
Py2Em.Py2_Exec('import sys;print(sys.version)')

print('--> Normal interpreter\'s answer to "10/3" is: ')
print(10 / 3)

print('--> Py2Em interpreter\'s answer to "10/3" is: ')
Py2Em.Py2_Exec('print(10 / 3)')

print('Finalizing Py2Em')
Py2Em.Finalize()
```
Produces the following output:

```
--> Initializing Py2Em
Loading Python Binary...Complete
Initializing Python...
Complete

--> Normal interpreter version is: 
3.8.5 (default, Jul 28 2020, 12:59:40) [GCC 9.3.0]

--> Py2Em interpreter version is: 
2.7.18 (default, Aug  4 2020, 11:16:42) [GCC 9.3.0]

--> Normal interpreter's answer to "10/3" is: 
3.3333333333333335

--> Py2Em interpreter's answer to "10/3" is: 
3

Finalizing Py2Em
```

## Contributions
All contributions are welcome and appreciated, feel free to create an issue, fork this repo and raise pull requests.

## Next Steps
* Add an insane amount of error handling/checking to the C extension.
* Substitute ```printf()``` in the C extension for proper Python logging.
* Add the ability to  retrieve values back from a string execution.
* Add the ability to execute complex Python code (```eval()``` as well as ```exec()```).
* Add the ability to share locals() and globals() between the two interpreters.
* Add support for execution of Python files.
* Test/add support for other Linux environments.
* Add support for MacOS and Windows.

## Resources
* https://docs.python.org/2.7/c-api/veryhigh.html
* https://docs.python.org/3/extending/extending.html