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
/************************************************************************************************
Filename    :   SimpleGlbRenderer.h
Content     :   A one stop for models from the render model extension
Created     :   April 2021
Authors     :   Federico Schliemann
Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.
************************************************************************************************/
#pragma once

#include <memory>
#include <string>
#include <vector>

/// Sample Framework
#include "Misc/Log.h"
#include "Model/SceneView.h"
#include "Render/GlProgram.h"
#include "Render/GlBuffer.h"
#include "Render/SurfaceRender.h"
#include "OVR_FileSys.h"
#include "OVR_Math.h"

namespace OVRFW {

class SimpleGlbRenderer {
   public:
    SimpleGlbRenderer() = default;
    ~SimpleGlbRenderer();

    bool Init(std::vector<uint8_t>& modelBuffer);
    void Shutdown();
    void Update(const OVR::Posef& pose);
    void Render(std::vector<ovrDrawSurface>& surfaceList);
    bool IsInitialized() const {
        return Initialized;
    }
    std::vector<OVR::Matrix4f> GetDefaultPoseTransforms() const;

   public:
    OVR::Vector3f SpecularLightDirection;
    OVR::Vector3f SpecularLightColor;
    OVR::Vector3f AmbientLightColor;
    std::unique_ptr<OVRFW::GlBuffer> jointsBuffer;
    bool UseSolidTexture = true;
    float Opacity = 1.0f;

   private:
    bool Initialized = false;
    float AlphaBlendFactor = 1.0f;
    GlProgram ProgRenderModel;
    std::unique_ptr<ModelFile> RenderModel{};
    GlTexture RenderModelTextureSolid;
    OVR::Matrix4f Transform;
    OVR::Posef GripPose = OVR::Posef::Identity();
    size_t kMaxJoints = 16;
};

} // namespace OVRFW
