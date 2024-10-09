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

Filename    :   ModelRender.h
Content     :   Optimized OpenGL rendering path
Created     :   August 9, 2013
Authors     :   John Carmack

************************************************************************************/

#pragma once

#include "OVR_Math.h"
#include "Render/SurfaceRender.h"
#include "ModelFile.h"

#include <vector>

namespace OVRFW {
// The model surfaces are culled and added to the sorted surface list.
// Application specific surfaces from the emit list are also added to the sorted surface list.
// The surface list is sorted such that opaque surfaces come first, sorted front-to-back,
// and transparent surfaces come last, sorted back-to-front.
void BuildModelSurfaceList(
    std::vector<ovrDrawSurface>& surfaceList,
    const std::vector<ModelNodeState*>& emitNodes,
    const std::vector<ovrDrawSurface>& emitSurfaces,
    const OVR::Matrix4f& viewMatrix,
    const OVR::Matrix4f& projectionMatrix);

} // namespace OVRFW
