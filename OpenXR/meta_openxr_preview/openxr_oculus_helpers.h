/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
