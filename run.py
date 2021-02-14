import Py2Em
import sys

# Initialize the Py2 interpreter
print('--> Initializing Py2Em')
Py2Em.Init()
print()

print('--> Normal interpreter version is: ')
print(sys.version)
print()

print('--> Py2Em interpreter version is: ')
Py2Em.Py2_Exec('import sys;print(sys.version)')
print()

print('--> Normal interpreter\'s answer to "10/3" is: ')
print(10 / 3)
print()

print('--> Py2Em interpreter\'s answer to "10/3" is: ')
Py2Em.Py2_Exec('print(10 / 3)')
print()

print('Finalizing Py2Em')
Py2Em.Finalize()
