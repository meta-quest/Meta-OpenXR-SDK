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

Filename    :   HandRenderer.h
Content     :   A one stop for rendering hands
Created     :   April 2020
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

class HandRenderer {
   public:
    HandRenderer() = default;
    ~HandRenderer() = default;

    bool Init(const XrHandTrackingMeshFB* mesh, bool leftHand);
    void Shutdown();
    void Update(const XrHandJointLocationEXT* joints, const float scale = 1.0f);
    void Render(std::vector<ovrDrawSurface>& surfaceList);

    bool IsLeftHand() const {
        return isLeftHand;
    }
    const std::vector<OVR::Matrix4f>& Transforms() const {
        return TransformMatrices;
    }

   public:
    OVR::Vector3f SpecularLightDirection;
    OVR::Vector3f SpecularLightColor;
    OVR::Vector3f AmbientLightColor;
    OVR::Vector3f GlowColor;
    float Confidence;
    float Solidity;

   private:
    bool isLeftHand;
    GlProgram ProgHand;
    ovrSurfaceDef HandSurfaceDef;
    ovrDrawSurface HandSurface;
    std::vector<OVR::Matrix4f> TransformMatrices;
    std::vector<OVR::Matrix4f> BindMatrices;
    std::vector<OVR::Matrix4f> SkinMatrices;
    GlBuffer SkinUniformBuffer;
};

} // namespace OVRFW
