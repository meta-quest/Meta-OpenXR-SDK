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

Filename    :   SurfaceRender.h
Content     :   Optimized OpenGL rendering path
Created     :   August 9, 2013
Authors     :   John Carmack

************************************************************************************/

#pragma once

#include <vector>
#include <string>

#include "OVR_Math.h"

#include "GlGeometry.h"
#include "GpuState.h"
#include "GlProgram.h"
#include "GlBuffer.h"
#include "GlTexture.h"

namespace OVRFW {

struct ovrSurfaceDef {
    ovrSurfaceDef() : numInstances(1) {}

    // Name from the model file, can be used to control surfaces with code.
    // May be multiple semi-colon separated names if multiple source meshes
    // were merged into one surface.
    std::string surfaceName;

    // There is a space savings to be had with triangle strips
    // if primitive restart is supported, but it is a net speed
    // loss on most architectures.  Adreno docs still recommend,
    // so it might be worth trying.
    GlGeometry geo;

    // state + program + uniforms
    ovrGraphicsCommand graphicsCommand;

    // Number of instances to be rendered  (0 or 1 denotes no instancing)
    int numInstances;
};

struct ovrDrawCounters {
    ovrDrawCounters()
        : numElements(0),
          numDrawCalls(0),
          numProgramBinds(0),
          numParameterUpdates(0),
          numTextureBinds(0),
          numBufferBinds(0) {}

    int numElements;
    int numDrawCalls;
    int numProgramBinds;
    int numParameterUpdates; // MVP, etc
    int numTextureBinds;
    int numBufferBinds;
};

struct ovrDrawSurface {
    ovrDrawSurface() : surface(NULL) {}

    ovrDrawSurface(const OVR::Matrix4f& modelMatrix_, const ovrSurfaceDef* surface_)
        : modelMatrix(modelMatrix_), surface(surface_) {}

    ovrDrawSurface(const ovrSurfaceDef* surface_) : surface(surface_) {}

    void Clear() {
        modelMatrix = OVR::Matrix4f();
        surface = NULL;
    }

    OVR::Matrix4f modelMatrix;
    const ovrSurfaceDef* surface;
};

class ovrSurfaceRender {
   public:
    ovrSurfaceRender();
    ~ovrSurfaceRender();

    // Requires an active GL context.
    void Init();
    void Shutdown();

    // Draws a list of surfaces in order.
    // Any sorting or culling should be performed before calling.
    ovrDrawCounters RenderSurfaceList(
        const std::vector<ovrDrawSurface>& surfaceList,
        const OVR::Matrix4f& viewMatrix,
        const OVR::Matrix4f& projectionMatrix,
        const int eye);

   private:
    // Returns the index of the updated SceneMatrices UBO.
    int UpdateSceneMatrices(
        const OVR::Matrix4f* viewMatrix,
        const OVR::Matrix4f* projectionMatrix,
        const int numMatrices);

   private:
    // Use a ring-buffer to avoid rendering hazards with potential update
    // of the SceneMatrices UBO multiple times per frame.
    static const int MAX_SCENEMATRICES_UBOS = 8;
    int CurrentSceneMatricesIdx;
    GlBuffer SceneMatrices[MAX_SCENEMATRICES_UBOS]; // ubo for storing per-view scene matrices which
                                                    // are common to the framework render programs
                                                    // and do not change frequently.

    OVR::Matrix4f CachedViewMatrix[GlProgram::MAX_VIEWS];
    OVR::Matrix4f CachedProjectionMatrix[GlProgram::MAX_VIEWS];
};

// Set this true for log spew from BuildDrawSurfaceList and RenderSurfaceList.
extern bool LogRenderSurfaces;

} // namespace OVRFW
