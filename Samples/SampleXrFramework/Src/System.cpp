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
/*******************************************************************************

Filename    :   System.cpp
Content     :	Global system functions.
Created     :   February 21, 2018
Authors     :   J.M.P. van Waveren, Jonathan Wright
Language    :   C++

*******************************************************************************/

#include "System.h"

#include <cstdio>
#include "time.h"
#include <string.h>

namespace OVRFW {

double GetTimeInSeconds() {
    struct timespec now;
#if !defined(WIN32)
    clock_gettime(CLOCK_MONOTONIC, &now);
#else
    timespec_get(&now, TIME_UTC);
#endif
    return (now.tv_sec * 1e9 + now.tv_nsec) * 0.000000001;
}

} // namespace OVRFW
