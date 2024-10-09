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

Filename    :   SkeletonRenderer.h
Content     :   A rendering component for sample skeletons
Created     :   April 2020
Authors     :   Federico Schliemann

************************************************************************************/

#pragma once

#include <memory>
#include <vector>

#include "Input/Skeleton.h"
#include "OVR_FileSys.h"
#include "Render/TextureAtlas.h"
#include "Render/BeamRenderer.h"
#include "Render/ParticleSystem.h"
#include "Render/GlProgram.h"

#include "AxisRenderer.h"

namespace OVRFW {

typedef std::vector<std::pair<ovrParticleSystem::handle_t, ovrBeamRenderer::handle_t>>
    jointHandles_t;

class ovrSkeletonRenderer {
   public:
    ovrSkeletonRenderer() = default;
    ~ovrSkeletonRenderer() = default;

    bool Init(OVRFW::ovrFileSys* FileSys);
    void Shutdown();
    void Render(
        const OVR::Matrix4f& worldMatrix,
        const std::vector<ovrJoint>& joints,
        const ovrApplFrameIn& in,
        OVRFW::ovrRendererOutput& out);

   public:
    OVR::Vector4f BoneColor;
    float JointRadius;
    float BoneWidth;
    bool DrawAxis;

   protected:
    void ResetBones(jointHandles_t& handles);
    void RenderBones(
        const ovrApplFrameIn& frame,
        const OVR::Matrix4f& worldMatrix,
        const std::vector<ovrJoint>& joints,
        jointHandles_t& handles,
        const OVR::Vector4f& boneColor,
        const float jointRadius,
        const float boneWidth);

   private:
    std::unique_ptr<OVRFW::ovrParticleSystem> ParticleSystem;
    std::unique_ptr<OVRFW::ovrBeamRenderer> BeamRenderer;
    std::unique_ptr<OVRFW::ovrTextureAtlas> SpriteAtlas;
    std::unique_ptr<OVRFW::ovrTextureAtlas> BeamAtlas;
    jointHandles_t JointHandles;
    OVRFW::ovrAxisRenderer AxisRenderer;
};

} // namespace OVRFW
