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

Filename    :   ParticleSystem.h
Content     :   A simple particle system for System Activities.
Created     :   October 12, 2015
Authors     :   Jonathan E. Wright

************************************************************************************/

#pragma once

#include "OVR_Math.h"
#include "OVR_TypesafeNumber.h"

#include "FrameParams.h"
#include "Render/GlGeometry.h"
#include "Render/GlProgram.h"
#include "Render/SurfaceRender.h"
#include "OVR_FileSys.h"

#include "EaseFunctions.h"

#include <cstdint>
#include <vector>
#include <string>

namespace OVRFW {

class ovrTextureAtlas;

struct particleDerived_t {
    OVR::Vector3f Pos;
    OVR::Vector4f Color;
    float Orientation; // roll angle in radians
    float Scale;
    uint16_t SpriteIndex;
};

struct particleSort_t {
    int ActiveIndex;
    float DistanceSq;
};

//==============================================================
// ovrParticleSystem
class ovrParticleSystem {
   public:
    enum ovrParticleIndex { INVALID_PARTICLE_INDEX = -1 };

    using handle_t = OVR::TypesafeNumberT<int32_t, ovrParticleIndex, INVALID_PARTICLE_INDEX>;

    ovrParticleSystem();
    virtual ~ovrParticleSystem();

    // specify sprite locations as a regular grid
    void Init(
        size_t maxParticles,
        const ovrTextureAtlas* atlas,
        const ovrGpuState& gpuState,
        bool const sortParticles);

    void Frame(
        const OVRFW::ovrApplFrameIn& frame,
        const ovrTextureAtlas* textureAtlas,
        const OVR::Matrix4f& centerEyeViewMatrix);

    void Shutdown();

    void RenderEyeView(
        OVR::Matrix4f const& viewMatrix,
        OVR::Matrix4f const& projectionMatrix,
        std::vector<ovrDrawSurface>& surfaceList) const;

    handle_t AddParticle(
        const OVRFW::ovrApplFrameIn& frame,
        const OVR::Vector3f& initialPosition,
        const float initialOrientation,
        const OVR::Vector3f& initialVelocity,
        const OVR::Vector3f& acceleration,
        const OVR::Vector4f& initialColor,
        const ovrEaseFunc easeFunc,
        const float rotationRate,
        const float scale,
        const float lifeTime,
        const uint16_t spriteIndex);

    void UpdateParticle(
        const OVRFW::ovrApplFrameIn& frame,
        const handle_t handle,
        const OVR::Vector3f& position,
        const float orientation,
        const OVR::Vector3f& velocity,
        const OVR::Vector3f& acceleration,
        const OVR::Vector4f& color,
        const ovrEaseFunc easeFunc,
        const float rotationRate,
        const float scale,
        const float lifeTime,
        const uint16_t spriteIndex);

    void RemoveParticle(const handle_t handle);

    static ovrGpuState GetDefaultGpuState();

   private:
    void CreateGeometry(const int maxParticles);

    int GetMaxParticles() const {
        return SurfaceDef.geo.vertexCount / 4;
    }

    class ovrParticle {
       public:
        // empty constructor so we don't pay the price for double initialization
        ovrParticle() {}

        double StartTime; // time particle was created, negative means this particle is invalid
        float LifeTime; // time particle should die
        OVR::Vector3f InitialPosition; // initial position of the particle
        float InitialOrientation; // initial orientation of the particle
        OVR::Vector3f InitialVelocity; // initial velocity of the particle
        OVR::Vector3f HalfAcceleration; // 1/2 the initial acceleration of the particle
        OVR::Vector4f InitialColor; // initial color of the particle
        float RotationRate; // rotation of the particle
        float InitialScale; // initial scale of the particle
        uint16_t SpriteIndex; // index of the sprite for this particle
        ovrEaseFunc EaseFunc; // parametric function used to compute alpha
    };

    size_t maxParticles_; // maximum allowd particles
    std::vector<ovrParticle> particles_; // all active particles
    std::vector<handle_t> freeParticles_; // indices of free particles
    std::vector<handle_t> activeParticles_; // indices of active particles
    std::vector<particleDerived_t> derived_;
    std::vector<particleSort_t> sortIndices_;
    std::vector<uint8_t> packedAttr_;
    OVRFW::VertexAttribs attr_;
    GlProgram Program;
    ovrSurfaceDef SurfaceDef;
    OVR::Matrix4f ModelMatrix;
    bool SortParticles;
};

} // namespace OVRFW
