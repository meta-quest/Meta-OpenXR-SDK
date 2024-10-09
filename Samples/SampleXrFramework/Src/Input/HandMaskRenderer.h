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

Filename    :   HandMaskRenderer.h
Content     :   A one stop for rendering hand masks
Created     :   03/24/2020
Authors     :   Federico Schliemann

************************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <memory>

/// Sample Framework
#include "Misc/Log.h"
#include "Model/SceneView.h"
#include "Render/GlProgram.h"
#include "Render/SurfaceRender.h"

#include "OVR_Math.h"

#if defined(ANDROID)
#define XR_USE_GRAPHICS_API_OPENGL_ES 1
#define XR_USE_PLATFORM_ANDROID 1
#elif defined(WIN32)
#include <unknwn.h>
#define XR_USE_GRAPHICS_API_OPENGL 1
#define XR_USE_PLATFORM_WIN32 1
#endif // defined(ANDROID)

#include <openxr/openxr.h>
#include <meta_openxr_preview/openxr_oculus_helpers.h>
#include <openxr/openxr_platform.h>

namespace OVRFW {

class HandMaskRenderer {
   public:
    HandMaskRenderer() = default;
    ~HandMaskRenderer() = default;

    void Init(bool leftHand);
    void Shutdown();
    void Update(
        const OVR::Posef& headPose,
        const OVR::Posef& handPose,
        const std::vector<OVR::Matrix4f>& jointTransforms,
        const float handSize = 1.0f);
    void Render(std::vector<ovrDrawSurface>& surfaceList);

   public:
    float LayerBlend;
    float Falloff;
    float Intensity;
    float FadeIntensity;
    bool UseBorderFade;
    float BorderFadeSize;
    float AlphaMaskSize;
    bool RenderInverseSubtract;
    ovrSurfaceDef HandMaskSurfaceDef;

   private:
    GlProgram ProgHandMaskAlphaGradient;
    GlProgram ProgHandMaskBorderFade;
    ovrDrawSurface HandMaskSurface;
    std::vector<OVR::Matrix4f> HandMaskMatrices;
    std::vector<OVR::Vector3f> HandMaskColors;
    GlBuffer HandMaskUniformBuffer;
    GlBuffer HandColorUniformBuffer;
    bool IsLeftHand;
};

} // namespace OVRFW
