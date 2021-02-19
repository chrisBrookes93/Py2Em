#ifndef PY2EM_EXTENSION_h__
#define PY2EM_EXTENSION_h__

#include <Python.h>

/**
* Typedefs for the functions invoked in the libpython2.7.so file
**/
typedef void (*PyInitializeFunc)(void);
typedef void (*PyFinalizeFunc)(void);
typedef void (*PyErr_PrintFunc)(void);
typedef PyObject* (*PyDict_NewFunc)(void);
typedef PyObject* (*PyRun_StringFunc)(const char *, int start, PyObject *globals, PyObject *locals);

#endif // PY2EM_EXTENSION_h__r