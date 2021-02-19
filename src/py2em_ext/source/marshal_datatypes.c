#include "marshal_datatypes.h"


PyObject *MarshalListPy2ToPy3(PyObject *py2List)
{
	PyObject *iterator = PyObject_GetIter(py2List);
	PyObject *item;

	while ((item = PyIter_Next(iterator)))
	{
		printf("loop");

	}
	return NULL;
}

PyObject *MarshalLongPy2toPy3(PyObject *py2Obj)
{
	PyLongAsLongFunc pyLongAsLongPtr;
	pyLongAsLongPtr = (PyLongAsLongFunc)GetPy2Func("PyLong_AsLong");
	if (!pyLongAsLongPtr)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	long int cLongVal = pyLongAsLongPtr(py2Obj);
	return Py_BuildValue("l", cLongVal);
}

PyObject *MarshalStringPy2ToPy3(PyObject *py2Obj)
{
	printf("Enter MarshalStringPy2ToPy3\n");

	PyString_AsStringFunc pyStringAsStringPtr; 
	pyStringAsStringPtr = (PyString_AsStringFunc)GetPy2Func("PyString_AsString");
	if (!pyStringAsStringPtr)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}
	printf("About to make py2 call\n");


	char *cstr_val;
	cstr_val = pyStringAsStringPtr(py2Obj);
	printf("PyString_AsString made.\n");

	printf("cstr val: %s\n", cstr_val);

	return Py_BuildValue("s", cstr_val);
}

PyObject *MarshalObjectPy2ToPy3(PyObject *py2Obj)
{
	printf("Enter MarshalObjectPy2ToPy3");
	PyTypeObject *type;
	type = py2Obj->ob_type;
	char *typeName;
	typeName = type->tp_name;
	printf("type: %s", "f");
	return MarshalStringPy2ToPy3(py2Obj);
	/*

	if (strcmp(typeName, "int") == 0 || strcmp(typeName, "long") == 0)
	{
		printf("int or long");
		return MarshalLongPy2toPy3(pyHandle, py2Obj);		
	}
	else if (strcmp(typeName, "list") == 0)
	{
		return MarshalListPy2ToPy3(pyHandle, py2Obj);
	}
	else
	{
		// TODO - check unicode
		if (strcmp(typeName, "str") != 0)
		{
			PyErr_WarnFormat(PyExc_Warning, 1, "Failed to marshal data type '%s', treating it as a str", typeName);
		}
		printf("str");

		return MarshalStringPy2ToPy3(pyHandle, py2Obj);
	}*/
	return NULL;
}