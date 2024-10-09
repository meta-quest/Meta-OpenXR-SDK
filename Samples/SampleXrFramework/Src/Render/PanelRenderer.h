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

Filename    :   PanelRenderer.h
Content     :   Class that manages and renders quad-based panels with custom shaders.
Created     :   September 19, 2019
Authors     :   Federico Schliemann

*************************************************************************************/

#pragma once

#include <vector>

#include "OVR_Math.h"

#include "SurfaceRender.h"
#include "GlProgram.h"

#include "FrameParams.h"

namespace OVRFW {

//==============================================================
// PanelRenderer
class ovrPanelRenderer {
   public:
    static const int NUM_DATA_POINTS = 256;

    ovrPanelRenderer()
        : ModelScale(1.0f),
          UniformChannelEnable(1.0f, 1.0f, 1.0f, 1.0f),
          UniformGraphOffset(1.0f, 0.0f),
          WritePosition(0) {
        UniformChannelData.resize(NUM_DATA_POINTS, OVR::Vector4f(0.0f));
        UniformChannelColor[0] = OVR::Vector4f(0.75f, 0.0f, 0.0f, 0.5f);
        UniformChannelColor[1] = OVR::Vector4f(0.0f, 0.75f, 0.0f, 0.5f);
        UniformChannelColor[2] = OVR::Vector4f(0.0f, 0.0f, 0.75f, 0.5f);
        UniformChannelColor[3] = OVR::Vector4f(0.5f, 0.0f, 0.5f, 0.5f);
    }
    virtual ~ovrPanelRenderer() = default;

    virtual void Init();
    virtual void Shutdown();

    virtual void Update(const OVR::Vector4f& dataPoint);
    virtual void Render(std::vector<ovrDrawSurface>& surfaceList);

    void SetPose(const OVR::Posef& pose) {
        ModelMatrix = OVR::Matrix4f(pose) * OVR::Matrix4f::Scaling(ModelScale);
    }
    void SetScale(OVR::Vector3f v) {
        ModelScale = v;
    }
    void SetChannelColor(int channel, OVR::Vector4f c) {
        UniformChannelColor[channel % 4] = c;
    }

   private:
    ovrSurfaceDef SurfaceDef;
    GlProgram Program;
    OVR::Matrix4f ModelMatrix;
    OVR::Vector3f ModelScale;

    OVR::Vector4f UniformChannelEnable;
    OVR::Vector2f UniformGraphOffset;
    std::vector<OVR::Vector4f> UniformChannelData;
    OVR::Vector4f UniformChannelColor[4];

    GlBuffer ChannelDataBuffer;
    int WritePosition;
};

} // namespace OVRFW
