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
Filename    :   KeyboardRenderer.h
Content     :   A one stop for rendering keyboards
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
#include "Render/SurfaceRender.h"
#include "OVR_FileSys.h"
#include "OVR_Math.h"

#if defined(ANDROID)
#define XR_USE_GRAPHICS_API_OPENGL_ES 1
#define XR_USE_PLATFORM_ANDROID 1
#else
#include "unknwn.h"
#define XR_USE_GRAPHICS_API_OPENGL 1
#define XR_USE_PLATFORM_WIN32 1
#endif

#include <openxr/openxr.h>
#include <meta_openxr_preview/openxr_oculus_helpers.h>
#include <openxr/openxr_platform.h>

namespace OVRFW {

class KeyboardRenderer {
   public:
    KeyboardRenderer() = default;
    ~KeyboardRenderer() = default;

    bool Init(std::vector<uint8_t>& keyboardBuffer);
    void Shutdown();
    void Update(const OVR::Posef& pose, const OVR::Vector3f& scale = OVR::Vector3f(1.0, 1.0, 1.0f));
    void Render(std::vector<ovrDrawSurface>& surfaceList);

   public:
    OVR::Vector3f SpecularLightDirection;
    OVR::Vector3f SpecularLightColor;
    OVR::Vector3f AmbientLightColor;
    float Opacity = 1.0f;
    bool ShowModel = true;

   private:
    float AlphaBlendFactor = 1.0f;
    GlProgram ProgKeyboard;
    ModelFile* KeyboardModel = nullptr;
    GlTexture KeyboardTextureSolid;
    OVR::Matrix4f Transform;
};

} // namespace OVRFW
