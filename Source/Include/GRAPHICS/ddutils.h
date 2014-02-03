///////////////////////////////////////////////
// File: "ddutils.h"
//
// Author: Jesse Edmunds (JE)
//
// Date Created: 11/4/2002
//
// Purpose: various direct draw utils
///////////////////////////////////////////////

#pragma once

// safe COM object release macro
#define SAFE_RELEASE(p) if (p) { p->Release(); p = NULL; }