#include "marshal_datatypes.h"


PyObject *MarshalListPy2ToPy3(PyObject *py2List)
{
	// Get the size of the list
	Py_ssize_t listLen = PY2_PyList_Size(py2List);
	Log("Marshalling a list of size %d\n", listLen);

	// Initialize the new list
	PyObject *pPy3List;
	pPy3List = PY3_PyList_New(listLen);
	
	// Get an interator to the py2 list
	PyObject *pIterator = PY2_PyObject_GetIter(py2List);
	PyObject *pPy2Item;
	PyObject *pPy3Item;
	int index = 0;

	while ((pPy2Item = PY2_PyIter_Next(pIterator)))
	{
		Log("Marshalling element %d\n", index);
		// Marshal the current item to a Py3 object and add to the new list
		pPy3Item = MarshalObjectPy2ToPy3(pPy2Item);
		if (!pPy3Item)
		{
			// TODO - clean up everything
			return NULL;
		}
		PY3_PyList_SetItem(pPy3List, index, pPy3Item);

		//PY2_Py_XDECREF(pPy2Item);

		index++;
	}
	//PY2_Py_XDECREF(pIterator);

	return pPy3List;
}

PyObject *MarshalSetPy2ToPy3(PyObject *py2Obj)
{
	Log("Marshalling a set\n");
	// Initialize the new set
	PyObject *pPy3Set;
	pPy3Set = PY3_PySet_New(NULL);
	

	// Get an interator to the py2 set
	PyObject *pPy2Iterator = PY2_PyObject_GetIter(py2Obj);
	PyObject *pPy2Item;
	PyObject *pPy3Item;
	int index = 0;

	while ((pPy2Item = PY2_PyIter_Next(pPy2Iterator)))
	{
		Log("Marshalling element %d\n", index);
		// Marshal the current item to a Py3 object and add to the new set
		pPy3Item = MarshalObjectPy2ToPy3(pPy2Item);
		if (!pPy3Item)
		{
			// TODO - clean up everything
			Log("Serialization failed :(\n");
			return NULL;
		}
		PY3_PySet_Add(pPy3Set, pPy3Item);
		//PY2_Py_XDECREF(pPy2Item);

		index++;
	}
	//PY2_Py_XDECREF(pIterator);

	return pPy3Set;
}

PyObject *MarshalLongPy2toPy3(PyObject *py2Obj)
{
	Log("Marshalling a long/int\n");
	int overflow;
	// First attempt to marshal it to a long
	long cLongVal = PY2_PyLong_AsLongAndOverflow(py2Obj, &overflow);

	if (overflow == 1)
	{
		Log("Value overflowed, attempting to marshal as an unsigned long long...");
		// Overflowed a long, attempt a larger structure
		unsigned long long cULongLongVal = PY2_PyLong_AsUnsignedLongLong(py2Obj);
		if (cULongLongVal == -1)
		{
			Log("Failed.\n");
			PY3_PyErr_SetString(PyExc_RuntimeError, "Long value overflow. Value is larger than an unsigned long long.");
			return NULL;
		}
		Log("Success.\n");
		return PY3_Py_BuildValue("K", cULongLongVal);

	}
	else if (overflow == -1)
	{
		Log("Value underflowed, attempting to marshal as an long long...");
		// Underflowed a long, attempt a larger structure
		long long cLongLongVal = PY2_PyLong_AsLongLongAndOverflow(py2Obj, &overflow);

		if (overflow != 0)
		{
			Log("Failed.\n");
			PY3_PyErr_SetString(PyExc_RuntimeError, "Long value overflow/underflow. Value does not fit in a long long");
			return NULL;		
		}
		Log("Success.\n");
		return PY3_Py_BuildValue("L", cLongLongVal);
	} 
	else
	{
		return PY3_Py_BuildValue("l", cLongVal);
	}
}

PyObject *MarshalBoolPy2ToPy3(PyObject *py2Obj)
{
	return PY2_PyObject_IsTrue(py2Obj) ? Py_True : Py_False;
}

PyObject *MarshalFloatPy2ToPy3(PyObject *py2Obj)
{
	double dFloatVal = PY2_PyFloat_AsDouble(py2Obj);
	return PY3_Py_BuildValue("d", dFloatVal);
}

PyObject *MarshalComplexPy2ToPy3(PyObject *py2Obj)
{
	double dRealVal = PY2_PyComplex_RealAsDouble(py2Obj);
	double dImagVal = PY2_PyComplex_ImagAsDouble(py2Obj);

	PyObject *pComplex;
	pComplex = PY3_PyComplex_FromDoubles(dRealVal, dImagVal);
	return pComplex;
}


PyObject *MarshalStringPy2ToPy3(PyObject *py2Obj)
{
	char *cstr_val;
	cstr_val = PY2_PyString_AsString(py2Obj);
	return PY3_Py_BuildValue("s", cstr_val);
}

PyObject *MarshalObjectPy2ToPy3(PyObject *py2Obj)
{
	PyTypeObject *pTypeObj;
	pTypeObj = py2Obj->ob_type;
	char *typeName;
	typeName = pTypeObj->tp_name;
	Log("Data type to marshal is: %s\n", typeName);	

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
	else if (strcmp(typeName, "set") == 0)
	{
		return MarshalSetPy2ToPy3(py2Obj);
	}
	else if (strcmp(typeName, "bool") == 0)
	{
		return MarshalBoolPy2ToPy3(py2Obj);
	}
	else if (strcmp(typeName, "NoneType") == 0)
	{
		return PY3_Py_BuildValue("");
	}
	else if (strcmp(typeName, "str") == 0)
	{
		return MarshalStringPy2ToPy3(py2Obj);
	}
	else
	{
		Log("Type is not supported, reverting to str() instead.\n");
		PyObject *pStr = PY2_PyObject_Str(py2Obj);
		return MarshalStringPy2ToPy3(pStr);
	}
}