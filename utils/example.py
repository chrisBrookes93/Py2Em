from py2_em import Py2Emulator
import sys

python_home = sys.argv[1] if len(sys.argv) == 2 else None

# Initialize the Py2 interpreter
print('--> Initializing py2_em\n')
Py2Emulator.initialize(py2_home=python_home)

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
