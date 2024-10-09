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

Filename    :   ModelCollision.h
Content     :   Basic collision detection for scene walkthroughs.
Created     :   May 2014
Authors     :   J.M.P. van Waveren

*************************************************************************************/

#pragma once

#include "OVR_Math.h"

#include <vector>
#include <string>

namespace OVRFW {

class CollisionPolytope {
   public:
    void Add(const OVR::Planef& p) {
        Planes.push_back(p);
    }

    // Returns true if the given point is inside this polytope.
    bool TestPoint(const OVR::Vector3f& p) const;

    // Returns true if the ray hits the polytope.
    // The length of the ray is clipped to the point where the ray enters the polytope.
    // Optionally the polytope boundary plane that is hit is returned.
    bool TestRay(
        const OVR::Vector3f& start,
        const OVR::Vector3f& dir,
        float& length,
        OVR::Planef* plane) const;

    // Pops the given point out of the polytope if inside.
    bool PopOut(OVR::Vector3f& p) const;

   public:
    std::string Name;
    std::vector<OVR::Planef> Planes;
};

class ModelCollision {
   public:
    // Returns true if the given point is inside solid.
    bool TestPoint(const OVR::Vector3f& p) const;

    // Returns true if the ray hits solid.
    // The length of the ray is clipped to the point where the ray enters solid.
    // Optionally the solid boundary plane that is hit is returned.
    bool TestRay(
        const OVR::Vector3f& start,
        const OVR::Vector3f& dir,
        float& length,
        OVR::Planef* plane) const;

    // Pops the given point out of any collision geometry the point may be inside of.
    bool PopOut(OVR::Vector3f& p) const;

   public:
    std::vector<CollisionPolytope> Polytopes;
};

OVR::Vector3f SlideMove(
    const OVR::Vector3f& footPos,
    const float eyeHeight,
    const OVR::Vector3f& moveDirection,
    const float moveDistance,
    const ModelCollision& collisionModel,
    const ModelCollision& groundCollisionModel);

} // namespace OVRFW
