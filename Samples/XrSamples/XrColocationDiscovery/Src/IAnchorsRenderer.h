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
#pragma once

#include <vector>
#include "OVR_Math.h"
#include "Render/SurfaceRender.h"

struct AnchorPose {
    OVR::Posef Pose;
    OVR::Vector4f Color;
};

class IAnchorsRenderer {
   public:
    virtual ~IAnchorsRenderer(){};
    virtual void Shutdown() = 0;
    virtual void Render(std::vector<OVRFW::ovrDrawSurface>& surfaces) = 0;
    virtual void UpdatePoses(const std::vector<AnchorPose>& poses) = 0;

   private:
    virtual void Add(const OVR::Vector4f& colorOptions) = 0;
};
