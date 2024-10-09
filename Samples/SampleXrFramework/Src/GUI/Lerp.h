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
/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

/************************************************************************************

Filename    :   Lerp.h
Content     :	Simple floating point interpolation.
Created     :	8/25/2014
Authors     :   John Carmack

*************************************************************************************/

#pragma once

#include "OVR_Math.h"

namespace OVRFW {

class Lerp {
   public:
    Lerp() : startDomain(0.0), endDomain(0.0), startValue(0.0), endValue(0.0) {}

    void Set(double startDomain_, double startValue_, double endDomain_, double endValue_) {
        startDomain = startDomain_;
        endDomain = endDomain_;
        startValue = startValue_;
        endValue = endValue_;
    }

    double Value(double domain) const {
        const double f =
            clamp<double>((domain - startDomain) / (endDomain - startDomain), 0.0, 1.0);
        return startValue * (1.0 - f) + endValue * f;
    }

    double startDomain;
    double endDomain;
    double startValue;
    double endValue;
};

} // namespace OVRFW
