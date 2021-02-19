#include "marshal_datatypes.h"


PyObject *MarshalListPy2ToPy3(void *pyHandle, PyObject *py2List)
{
	PyObject *iterator = PyObject_GetIter(py2List);
	PyObject *item;

	while ((item = PyIter_Next(iterator)))
	{
		printf("loop");

	}
	return NULL;
}

PyObject *MarshalLongPy2toPy3(void *pyHandle, PyObject *py2Obj)
{
	PyLongAsLongFunc pyLongAsLongPtr;
	pyLongAsLongPtr = (PyLongAsLongFunc)GetPy2Func(pyHandle, "PyLong_AsLong");
	if (!pyLongAsLongPtr)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	long int cLongVal = pyLongAsLongPtr(py2Obj);
	return Py_BuildValue("l", cLongVal);
}

PyObject *MarshalStringPy2ToPy3(void *pyHandle, PyObject *py2Obj)
{
	PyString_AsStringFunc pyStringAsStringPtr; 
	pyStringAsStringPtr = (PyString_AsStringFunc)GetPy2Func(pyHandle, "PyString_AsString");
	if (!pyStringAsStringPtr)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	char *cstr_val;
	cstr_val = pyStringAsStringPtr(py2Obj);
	return Py_BuildValue("s", cstr_val);
}

PyObject *MarshalObjectPy2ToPy3(void *pyHandle, PyObject *py2Obj)
{
	PyTypeObject *type;
	type = py2Obj->ob_type;
	const char *typeName;
	typeName = type->tp_name;

	if (strcmp(typeName, "int") == 0 || strcmp(typeName, "long") == 0)
	{
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
		return MarshalStringPy2ToPy3(pyHandle, py2Obj);
	}
}