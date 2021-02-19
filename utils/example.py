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
Py2Emulator.exec('import sys;print(sys.version)')
print()

print('--> Normal interpreter\'s answer to "10/3" is: ')
print(10 / 3)
print()

print('--> py2_em interpreter\'s answer to "10/3" is: ')
Py2Emulator.exec('print(10 / 3)')
print()

print('Finalizing py2_em')
Py2Emulator.finalize()
