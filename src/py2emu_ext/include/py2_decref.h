/* 
Re-implementation of Python2's reference decrement and destructor code. 
Since they're implement as macros rather than functions, we cannot invoke them from the loaded binary, so we instead replicate them.

All Macros have been prefixed with a 'PY2_' to avoid name conflicts. It's likely the case that these are exactly the same in Python3, but that may not always be the case.

Sourced from Python2.7.18/include/object.h
*/
#ifndef PY2_DECREF_h__
#define PY2_DECREF_h__

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#define PY2__Py_DEC_REFTOTAL
#define PY2__Py_REF_DEBUG_COMMA
#define PY2__Py_CHECK_REFCNT(OP)    /* a semicolon */;
#define PY2__Py_INC_TPFREES(OP)
#define PY2__Py_COUNT_ALLOCS_COMMA

#define PY2_Py_REFCNT(ob)           (((PyObject*)(ob))->ob_refcnt)
#define PY2_Py_TYPE(ob)             (((PyObject*)(ob))->ob_type)

#define PY2__Py_Dealloc(op) (                             \
    PY2__Py_INC_TPFREES(op) PY2__Py_COUNT_ALLOCS_COMMA    \
    (*PY2_Py_TYPE(op)->tp_dealloc)((PyObject *)(op)))

#define PY2_Py_DECREF(op)                                 \
    do {                                                  \
        if (PY2__Py_DEC_REFTOTAL  PY2__Py_REF_DEBUG_COMMA \
        --((PyObject*)(op))->ob_refcnt != 0)              \
            PY2__Py_CHECK_REFCNT(op)                      \
        else                                              \
        PY2__Py_Dealloc((PyObject *)(op));                \
    } while (0)

#define PY2_Py_XDECREF(op) do { if ((op) == NULL) ; else PY2_Py_DECREF(op); } while (0)


#endif // PY2_DECREF_h__
