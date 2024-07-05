// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

/************************************************************************************

Filename    :   openxr_oculus_helpers.h
Content     :   OpenXR Helper routines
Language    :   C99

*************************************************************************************/

#ifndef OPENXR_OCULUS_HELPERS_H
#define OPENXR_OCULUS_HELPERS_H 1

#include <openxr/openxr.h>
#include <xr_linear.h>

#include "math.h" // for cosf(), sinf(), tanf()

// OpenXR time values are expressed in nanseconds.
static inline double FromXrTime(const XrTime time) {
    return (time * 1e-9);
}

static inline XrTime ToXrTime(const double timeInSeconds) {
    return (timeInSeconds * 1e9);
}

#endif // OPENXR_OCULUS_HELPERS_H
