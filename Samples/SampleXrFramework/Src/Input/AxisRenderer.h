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

Filename    :   AxisRenderer.h
Content     :   A rendering component for axis
Created     :   September 2020
Authors     :   Federico Schliemann

************************************************************************************/

#pragma once

#include <vector>

#include "OVR_Math.h"
#include "FrameParams.h"
#include "Render/SurfaceRender.h"
#include "Render/GlProgram.h"

namespace OVRFW {

class ovrAxisRenderer {
   public:
    ovrAxisRenderer() = default;
    ~ovrAxisRenderer() = default;

    bool Init(size_t count = 64, float size = 0.025f);
    void Shutdown();
    void Update(const std::vector<OVR::Posef>& points);
    void Update(const OVR::Posef* points, size_t count);
    void Render(
        const OVR::Matrix4f& worldMatrix,
        const OVRFW::ovrApplFrameIn& in,
        OVRFW::ovrRendererOutput& out);

   private:
    float AxisSize;
    GlProgram ProgAxis;
    ovrSurfaceDef AxisSurfaceDef;
    ovrDrawSurface AxisSurface;
    std::vector<OVR::Matrix4f> TransformMatrices;
    GlBuffer InstancedBoneUniformBuffer;
    size_t Count;
};

} // namespace OVRFW
