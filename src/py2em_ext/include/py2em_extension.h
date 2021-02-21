#ifndef PY2EM_EXTENSION_h__
#define PY2EM_EXTENSION_h__

#define PY_SSIZE_T_CLEAN

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Python.h>

#include "marshal_utils.h"
#include "logging.h"
#include "py2_decref.h"
#include "marshal_datatypes.h"
#include "marshal_utils.h"

#define EXEC_MODE_EVAL 0
#define EXEC_MODE_EXEC 1

#endif // PY2EM_EXTENSION_h__
