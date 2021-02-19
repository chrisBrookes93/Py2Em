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

PyObject *MarshalBoolPy2ToPy3(PyObject *py2Obj)
{
	PyObjectIsTrueFunc pPyObjectIsTrue;
	pPyObjectIsTrue = (PyObjectIsTrueFunc)GetPy2Func("PyObject_IsTrue");
	if (!pPyObjectIsTrue)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	return pPyObjectIsTrue ? Py_True : Py_False;
}

PyObject *MarshalFloatPy2ToPy3(PyObject *py2Obj)
{
	PyFloatAsDoubleFunc pFloatAsDouble;
	pFloatAsDouble = (PyFloatAsDoubleFunc)GetPy2Func("PyFloat_AsDouble");
	if (!pFloatAsDouble)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	double dFloatVal = pFloatAsDouble(py2Obj);
	return Py_BuildValue("d", dFloatVal);
}

PyObject *MarshalComplexPy2ToPy3(PyObject *py2Obj)
{
	PyComplexRealAsDoubleFunc pComplexRealAsDouble;
	PyComplexImagAsDoubleFunc pComplexImagAsDouble;
	pComplexRealAsDouble = (PyComplexRealAsDoubleFunc)GetPy2Func("PyComplex_RealAsDouble");
	pComplexImagAsDouble = (PyComplexImagAsDoubleFunc)GetPy2Func("PyComplex_ImagAsDouble");

	if (!pComplexRealAsDouble || !pComplexImagAsDouble)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	double dRealVal = pComplexRealAsDouble(py2Obj);
	double dImagVal = pComplexImagAsDouble(py2Obj);

	PyObject *pComplex;
	pComplex = PyComplex_FromDoubles(dRealVal, dImagVal);
	return pComplex;
	//return Py_BuildValue("D", pComplex);
}


PyObject *MarshalStringPy2ToPy3(PyObject *py2Obj)
{
	PyStringAsStringFunc pPyStringAsString; 
	pPyStringAsString = (PyStringAsStringFunc)GetPy2Func("PyString_AsString");

	if (!pPyStringAsString)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	
	char *cstr_val;
	cstr_val = pPyStringAsString(py2Obj);
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
	printf("return data type is:  %s\n", typeName);	

	if (strcmp(typeName, "int") == 0 || strcmp(typeName, "long") == 0)
	{
		return MarshalLongPy2toPy3(py2Obj);		
	}
	if (strcmp(typeName, "float") == 0)
	{
		return MarshalFloatPy2ToPy3(py2Obj);

	}
	if (strcmp(typeName, "complex") == 0)
	{
		return MarshalComplexPy2ToPy3(py2Obj);

	}
	else if (strcmp(typeName, "list") == 0)
	{
		return MarshalListPy2ToPy3(py2Obj);
	}
	else if (strcmp(typeName, "bool") == 0)
	{
		return MarshalBoolPy2ToPy3(py2Obj);
	}
	else if (strcmp(typeName, "NoneType") == 0)
	{
		return Py_BuildValue("");
	}
	else if (strcmp(typeName, "str") == 0)
	{
		return MarshalStringPy2ToPy3(py2Obj);
	}

	else
	{
		PyErr_WarnFormat(PyExc_Warning, 1, "Failed to marshal data type '%s', returning it as a str()", typeName);
		PyObjectStrFunc pPyObjectStr;
		pPyObjectStr = (PyObjectStrFunc)GetPy2Func("PyObject_Str");
		if (!pPyObjectStr)
		{
			// GetPy2Func() will have set the error
			return NULL;
		}

		PyObject *pStr = pPyObjectStr(py2Obj);
		return MarshalStringPy2ToPy3(pStr);
	}
}