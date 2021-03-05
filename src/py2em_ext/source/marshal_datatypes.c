#include "marshal_datatypes.h"

/**
* Marshals a Python2 PyObject* list into a Python3 PyObject* list
*
* @param py2List Python2 list to marshal
* @return Python3 PyObject list
**/
PyObject* MarshalListPy2ToPy3(PyObject* py2List)
{
	PyObject* pPy2Iterator, * pPy2Item;
	PyObject* pPy3List, * pPy3Item;

	// Get the size of the list
	Py_ssize_t listLen = PY2_PyList_Size(py2List);
	Log("Marshalling a list of size %d\n", listLen);

	// Initialize the new list
	pPy3List = PY3_PyList_New(listLen);
	if (!pPy3List)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to create a new Python3 list.");
		return NULL;
	}

	// Get an iterator to the py2 list
	pPy2Iterator = PY2_PyObject_GetIter(py2List);
	if (!pPy2Iterator)
	{
		PY3_Py_XDECREF(pPy3List);
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to get an iterator to the Python2 List");
		return NULL;
	}

	int index = 0;

	while ((pPy2Item = PY2_PyIter_Next(pPy2Iterator)))
	{
		Log("Marshalling element %d\n", index);
		// Marshal the current item to a Py3 object and add to the new list
		pPy3Item = MarshalObjectPy2ToPy3(pPy2Item);
		PY2_Py_XDECREF(pPy2Item);

		if (!pPy3Item)
		{
			// MarshalObjectPy2ToPy3() should have set the error
			PY2_Py_XDECREF(pPy2Iterator);
			PY3_Py_XDECREF(pPy3List);
			return NULL;
		}

		// Note that PyList_SetItem() steals the reference of the item being added so we don't PY3_Py_XDECREF() on success
		if (PY3_PyList_SetItem(pPy3List, index, pPy3Item) != ERROR_SUCCESS)
		{
			PY2_Py_XDECREF(pPy2Iterator);
			PY3_Py_XDECREF(pPy3Item);
			PY3_Py_XDECREF(pPy3List);
			PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to set item in the Python3 list.");
			return NULL;
		}

		index++;
	}

	PY2_Py_XDECREF(pPy2Iterator);

	return pPy3List;
}

/**
* Marshals a Python2 PyObject* set into a Python3 PyObject* set
*
* @param pPy2Obj Python2 set to marshal
* @return Python3 PyObject set
**/
PyObject* MarshalSetPy2ToPy3(PyObject* pPy2Obj)
{
	Log("Marshalling a set\n");
	PyObject* pPy2Item;
	PyObject* pPy3Item;
	PyObject* pPy2Iterator;
	PyObject* pPy3Set;

	// Initialize the new set
	pPy3Set = PY3_PySet_New(NULL);
	if (!pPy3Set)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to create a new Python3 set.");
		return NULL;
	}

	// Get an iterator to the py2 set
	pPy2Iterator = PY2_PyObject_GetIter(pPy2Obj);
	if (!pPy2Iterator)
	{
		PY3_Py_XDECREF(pPy3Set);
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to get an iterator to the Python2 set");
		return NULL;
	}

	int index = 0;

	while ((pPy2Item = PY2_PyIter_Next(pPy2Iterator)))
	{
		Log("Marshalling element %d\n", index);
		// Marshal the current item to a Py3 object and add to the new list
		pPy3Item = MarshalObjectPy2ToPy3(pPy2Item);
		PY2_Py_XDECREF(pPy2Item);

		if (!pPy3Item)
		{
			// MarshalObjectPy2ToPy3() should have set the error
			PY2_Py_XDECREF(pPy2Iterator);
			PY3_Py_XDECREF(pPy3Set);
			return NULL;
		}

		if (PY3_PySet_Add(pPy3Set, pPy3Item) != ERROR_SUCCESS)
		{
			PY2_Py_XDECREF(pPy2Iterator);
			PY3_Py_XDECREF(pPy3Item);
			PY3_Py_XDECREF(pPy3Set);
			PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to set item in the Python3 set.");
			return NULL;
		}
		PY3_Py_XDECREF(pPy3Item);

		index++;
	}
	PY2_Py_XDECREF(pPy2Iterator);

	return pPy3Set;
}

/**
* Marshals a Python2 PyObject* dict into a Python3 PyObject* dict
*
* @param pPy2Obj Python2 dict to marshal
* @return Python3 PyObject dict
**/
PyObject* MarshalDictPy2ToPy3(PyObject* pPy2Obj)
{
	Log("Marshalling a dict\n");
	// Initialize the new dict
	PyObject* pPy3Dict;
	PyObject* pPy2Key, * pPy2Value;
	PyObject* pPy3Key, * pPy3Value;
	Py_ssize_t index = 0;

	pPy3Dict = PY3_PyDict_New();
	if (!pPy3Dict)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to create a new Python3 dict.");
		return NULL;
	}

	// pPy2Key & pPy2Value are borrowed, no need to DECREF
	while (PY2_PyDict_Next(pPy2Obj, &index, &pPy2Key, &pPy2Value))
	{
		Log("Dict item %d\n", index);

		pPy3Key = MarshalObjectPy2ToPy3(pPy2Key);
		if (!pPy3Key)
		{
			// MarshalObjectPy2ToPy3() should have set the error
			PY3_Py_XDECREF(pPy3Dict);
			return NULL;
		}

		pPy3Value = MarshalObjectPy2ToPy3(pPy2Value);
		if (!pPy3Value)
		{
			// MarshalObjectPy2ToPy3() should have set the error
			PY3_Py_XDECREF(pPy3Key);
			PY3_Py_XDECREF(pPy3Dict);
			return NULL;
		}

		if (PY3_PyDict_SetItem(pPy3Dict, pPy3Key, pPy3Value) != ERROR_SUCCESS)
		{
			PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to set item in the Python3 dict.");
			PY3_Py_XDECREF(pPy3Key);
			PY3_Py_XDECREF(pPy3Value);
			PY3_Py_XDECREF(pPy3Dict);
			return NULL;
		}
	}
	return pPy3Dict;
}

/**
* Marshals a Python2 PyObject* tuple into a Python3 PyObject* tuple
*
* @param pPy2Obj Python2 tuple to marshal
* @return Python3 PyObject tuple
**/
PyObject* MarshalTuplePy2ToPy3(PyObject* pPy2Obj)
{
	PyObject* pPy2Iterator, * pPy2Item;
	PyObject* pPy3Tuple, * pPy3Item;

	// Get the size of the tuple
	Py_ssize_t tupleLen = PY2_PyTuple_Size(pPy2Obj);
	Log("Marshalling a tuple of size %d\n", tupleLen);

	// Initialize the new list
	pPy3Tuple = PY3_PyTuple_New(tupleLen);
	if (!pPy3Tuple)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to create a new Python3 tuple.");
		return NULL;
	}

	// Get an iterator to the py2 list
	pPy2Iterator = PY2_PyObject_GetIter(pPy2Obj);
	if (!pPy2Iterator)
	{
		PY3_Py_XDECREF(pPy3Tuple);
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to get an iterator to the Python2 tuple");
		return NULL;
	}
	int index = 0;

	while ((pPy2Item = PY2_PyIter_Next(pPy2Iterator)))
	{
		Log("Marshalling element %d\n", index);
		// Marshal the current item to a Py3 object and add to the new list
		pPy3Item = MarshalObjectPy2ToPy3(pPy2Item);
		PY2_Py_XDECREF(pPy2Item);

		if (!pPy3Item)
		{
			// MarshalObjectPy2ToPy3() should have set the error
			PY2_Py_XDECREF(pPy2Iterator);
			PY3_Py_XDECREF(pPy3Tuple);
			return NULL;
		}

		// Note that PyTuple_SetItem() steals the reference of the item being added so we don't PY3_Py_XDECREF() on success
		if (PY3_PyTuple_SetItem(pPy3Tuple, index, pPy3Item) != ERROR_SUCCESS)
		{
			PY2_Py_XDECREF(pPy2Iterator);
			PY3_Py_XDECREF(pPy3Tuple);
			PY3_Py_XDECREF(pPy3Item);
			PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to set item in the Python3 tuple.");
			return NULL;
		}

		index++;
	}

	PY2_Py_XDECREF(pPy2Iterator);

	return pPy3Tuple;
}

/**
* Marshals a Python2 PyObject* number into a Python3 PyObject* number
*
* @param pPy2Obj Python2 number to marshal
* @return Python3 PyObject number
**/
PyObject* MarshalLongPy2toPy3(PyObject* pPy2Obj)
{
	Log("Marshalling a long/int\n");
	int overflow;
	// First attempt to marshal it to a long
	long cLongVal = PY2_PyLong_AsLongAndOverflow(pPy2Obj, &overflow);

	if (overflow == ERROR_OVERFLOW)
	{
		Log("Value overflowed, attempting to marshal as an unsigned long long...");
		// Overflowed a long but we know its positive, attempt a larger structure
		unsigned long long cULongLongVal = PY2_PyLong_AsUnsignedLongLong(pPy2Obj);
		if (cULongLongVal == (unsigned long long) - 1)
		{
			Log("Failed.\n");
			PY3_PyErr_SetString(PyExc_RuntimeError, "Long value overflow. Value is larger than an unsigned long long.");
			return NULL;
		}
		Log("Success.\n");
		return PY3_Py_BuildValue("K", cULongLongVal);

	}
	else if (overflow == ERROR_UNDERFLOW)
	{
		Log("Value underflowed, attempting to marshal as an long long...");
		// Underflowed a long, attempt a larger structure
		long long cLongLongVal = PY2_PyLong_AsLongLongAndOverflow(pPy2Obj, &overflow);

		if (overflow != ERROR_SUCCESS)
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

/**
* Marshals a Python2 PyObject* bool into a Python3 PyObject* bool
*
* @param pPy2Obj Python2 bool to marshal
* @return Python3 PyObject bool
**/
PyObject* MarshalBoolPy2ToPy3(PyObject* pPy2Obj)
{
	return PY2_PyObject_IsTrue(pPy2Obj) ? Py_True : Py_False;
}

/**
* Marshals a Python2 PyObject* float into a Python3 PyObject* float
*
* @param pPy2Obj Python2 float to marshal
* @return Python3 PyObject float
**/
PyObject* MarshalFloatPy2ToPy3(PyObject* pPy2Obj)
{
	double dFloatVal = PY2_PyFloat_AsDouble(pPy2Obj);
	return PY3_Py_BuildValue("d", dFloatVal);
}

/**
* Marshals a Python2 PyObject* complex into a Python3 PyObject* complex
*
* @param pPy2Obj Python2 complex to marshal
* @return Python3 PyObject complex
**/
PyObject* MarshalComplexPy2ToPy3(PyObject* pPy2Obj)
{
	double dRealVal = PY2_PyComplex_RealAsDouble(pPy2Obj);
	double dImagVal = PY2_PyComplex_ImagAsDouble(pPy2Obj);

	PyObject* pComplex;
	pComplex = PY3_PyComplex_FromDoubles(dRealVal, dImagVal);
	return pComplex;
}

/**
* Marshals a Python2 PyObject* str into a Python3 PyObject* complex
*
* @param pPy2Obj Python2 str to marshal
* @return Python3 PyObject str
**/
PyObject* MarshalStringPy2ToPy3(PyObject* pPy2Obj)
{
	char* pCStr_val;
	pCStr_val = PY2_PyString_AsString(pPy2Obj);
	if (!pCStr_val)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to marshal string to Python3");
		return NULL;
	}
	return PY3_Py_BuildValue("s", pCStr_val);
}

/**
* Marshals a Python2 PyObject* object into a Python3 PyObject* object
*
* @param pPy2Obj Python2 object to marshal
* @return Python3 PyObject object
**/
PyObject* MarshalObjectPy2ToPy3(PyObject* pPy2Obj)
{
	const char* typeName;
	typeName = pPy2Obj->ob_type->tp_name;
	Log("Data type to marshal is: %s\n", typeName);

	if (strcmp(typeName, "int") == STR_MATCH || strcmp(typeName, "long") == STR_MATCH)
	{
		return MarshalLongPy2toPy3(pPy2Obj);
	}
	if (strcmp(typeName, "float") == STR_MATCH)
	{
		return MarshalFloatPy2ToPy3(pPy2Obj);
	}
	if (strcmp(typeName, "complex") == STR_MATCH)
	{
		return MarshalComplexPy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "list") == STR_MATCH)
	{
		return MarshalListPy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "set") == STR_MATCH)
	{
		return MarshalSetPy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "dict") == STR_MATCH)
	{
		return MarshalDictPy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "tuple") == STR_MATCH)
	{
		return MarshalTuplePy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "bool") == STR_MATCH)
	{
		return MarshalBoolPy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "NoneType") == STR_MATCH)
	{
		return PY3_Py_BuildValue("");
	}
	else if (strcmp(typeName, "str") == STR_MATCH)
	{
		return MarshalStringPy2ToPy3(pPy2Obj);
	}
	else if (strcmp(typeName, "unicode") == STR_MATCH)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Unicode marshalling not supported. Consider encoding return value");
		return NULL;
	}
	else
	{
		Log("Marshaling of this type is not supported, reverting to str() instead.\n");
		PyObject* pPy2Str;
		PyObject* pRetVal;

		pPy2Str = PY2_PyObject_Str(pPy2Obj);
		if (!pPy2Str)
		{
			PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to call str() on object.");
			return NULL;
		}
		pRetVal = MarshalStringPy2ToPy3(pPy2Str);
		PY2_Py_XDECREF(pPy2Str);
		return pRetVal;
	}
}