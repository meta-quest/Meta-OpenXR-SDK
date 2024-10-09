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

Filename    :   EaseFunctions.h
Content     :   Functions for blending alpha over time.
Created     :   October 12, 2015
Authors     :   Jonathan E. Wright

************************************************************************************/

#include "EaseFunctions.h"
using OVR::Vector4f;

namespace OVRFW {

static Vector4f EaseFunc_None(const Vector4f& c, const float /*t*/) {
    return c;
}

static Vector4f EaseFunc_InOut_Linear(const Vector4f& c, const float t) {
    const float s = EaseInOut_Linear(t);
    return Vector4f(c.x * s, c.y * s, c.z * s, c.w * s);
}

static Vector4f EaseFunc_InOut_Cubic(const Vector4f& c, const float t) {
    const float s = EaseInOut_Cubic(t);
    return Vector4f(c.x * s, c.y * s, c.z * s, c.w * s);
}

static Vector4f EaseFunc_InOut_Quadratic(const Vector4f& c, const float t) {
    const float s = EaseInOut_Quadratic(t);
    return Vector4f(c.x * s, c.y * s, c.z * s, c.w * s);
}

static Vector4f EaseFunc_Alpha_InOut_Linear(const Vector4f& c, const float t) {
    return Vector4f(c.x, c.y, c.z, c.w * EaseInOut_Linear(t));
}

static Vector4f EaseFunc_Alpha_InOut_Cubic(const Vector4f& c, const float t) {
    return Vector4f(c.x, c.y, c.z, c.w * EaseInOut_Cubic(t));
}

static Vector4f EaseFunc_Alpha_InOut_Quadratic(const Vector4f& c, const float t) {
    return Vector4f(c.x, c.y, c.z, c.w * EaseInOut_Quadratic(t));
}

EaseFunction_t EaseFunctions[ovrEaseFunc::MAX] = {
    EaseFunc_None,
    EaseFunc_InOut_Linear,
    EaseFunc_InOut_Cubic,
    EaseFunc_InOut_Quadratic,
    EaseFunc_Alpha_InOut_Linear,
    EaseFunc_Alpha_InOut_Cubic,
    EaseFunc_Alpha_InOut_Quadratic};

} // namespace OVRFW
