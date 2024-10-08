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

#pragma once

#include <cstdint>
#include <cassert>
#include "OVR_Math.h"

namespace OVRFW {

enum ovrEaseFunc : uint8_t {
    NONE, // just use initial color and alpha values
    IN_OUT_LINEAR, // ease color and alpha in and out over life time linear
    IN_OUT_CUBIC, // ease color and alpha in and out over life time cubic
    IN_OUT_QUADRIC, // ease color and alpha in and out over life time quadratic
    ALPHA_IN_OUT_LINEAR, // ease alpha channel in and out over life time linear
    ALPHA_IN_OUT_CUBIC, // ease alpha channel in and out over life time cubic
    ALPHA_IN_OUT_QUADRIC, // ease alpha channel in and out over life time quadratic
    MAX
};

template <typename T>
T EaseInOut_Linear(const T t) {
    const T HALF = static_cast<T>(0.5);
    const T ONE = static_cast<T>(1.0);
    const T TWO = static_cast<T>(2.0);

    if (t <= HALF) {
        return TWO * t;
    }

    return ONE - (TWO * (t - HALF));
}

// t must be between 0 and 1
// The return value approaches 1 as t approaches 0.5 and
// begins to approach 0 again as t goes from 0.5 to 1
template <typename T>
T EaseInOut_Quadratic(const T t) {
    const T HALF = static_cast<T>(0.5);
    const T ONE = static_cast<T>(1.0);
    const T TWO = static_cast<T>(2.0);

    if (t <= HALF) {
        return TWO * t * t;
    }

    return ONE - (TWO * (t - HALF) * (t - HALF));
}

// t must be between 0 and 1
// The return value is always 0 to 1.
// The output changes slowly near 0.0, most rapidly at 0.5 and slowly again near 1.0
template <typename T>
T EaseInOut_Quadratic2(const T t) {
    const T HALF = static_cast<T>(0.5);
    const T ONE = static_cast<T>(1.0);
    const T TWO = static_cast<T>(2.0);

    if (t <= HALF) {
        return TWO * t * t;
    }

    T const t2 = 1.0f - t;
    return ONE - (TWO * (t2 * t2));
}

template <typename T>
T EaseInOut_Cubic(T t) {
    const T HALF = static_cast<T>(0.5);
    const T ONE = static_cast<T>(1.0);
    const T TWO = static_cast<T>(2.0);

    if (t <= HALF) {
        return TWO * t * t * t;
    }
    t -= HALF;
    return ONE - (TWO * t * t * t);
}

template <typename T>
T EaseIn_Quadratic(T t) {
    return t * t;
}

// y = x^3
template <typename T>
T EaseIn_Cubic(T t) {
    return t * t * t;
}

// y = 1.0 - ( ( 1.0 - x )^3 )
template <typename T>
T EaseIn_CubicInverted(T t) {
    const T ZERO = static_cast<T>(0.0);
    const T ONE = static_cast<T>(1.0);
    assert(t >= ZERO && t <= ONE);
    T invT = ONE - t;
    return ONE - (invT * invT * invT);
}

typedef OVR::Vector4f (*EaseFunction_t)(const OVR::Vector4f& c, const float t);

extern EaseFunction_t EaseFunctions[ovrEaseFunc::MAX];

} // namespace OVRFW
