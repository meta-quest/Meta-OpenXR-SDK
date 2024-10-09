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

Filename    :   Ribbon.h
Content     :   Class that renders connected polygon strips from a list of points
Created     :   6/16/2017
Authors     :   Jonathan E. Wright

************************************************************************************/

#pragma once

#include <vector>

#include "OVR_Math.h"
#include "PointList.h"
#include "SurfaceRender.h"

namespace OVRFW {

//==============================================================
// ovrRibbon
class ovrRibbon {
   public:
    ovrRibbon(const ovrPointList& pointList, const float width, const OVR::Vector4f& color);
    ~ovrRibbon();

    void AddPoint(ovrPointList& pointList, const OVR::Vector3f& point);
    void Update(
        const ovrPointList& pointList,
        const OVR::Matrix4f& centerViewMatrix,
        const bool invertAlpha);
    void SetColor(const OVR::Vector4f& color);
    void SetWidth(const float width);
    void GenerateSurfaceList(std::vector<ovrDrawSurface>& surfaceList) const;

   private:
    float HalfWidth;
    OVR::Vector4f Color;
    ovrSurfaceDef Surface;
    GlTexture Texture;
};

} // namespace OVRFW
