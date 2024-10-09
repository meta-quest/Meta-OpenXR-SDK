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

Filename    :   ArmModel.h
Content     :   An arm model for the tracked remote
Created     :   2/20/2017
Authors     :   Jonathan E. Wright

************************************************************************************/

#pragma once

#include <vector>

#include "OVR_Types.h"

#include "Skeleton.h"

namespace OVRFW {

class ovrArmModel {
   public:
    enum ovrHandedness { HAND_UNKNOWN = -1, HAND_LEFT, HAND_RIGHT, HAND_MAX };

    ovrArmModel();

    void InitSkeleton(bool isLeft);

    void Update(
        const OVR::Posef& headPose,
        const OVR::Posef& remotePose,
        const ovrHandedness handedness,
        const bool recenteredController,
        OVR::Posef& outPose);

    const ovrSkeleton& GetSkeleton() const {
        return Skeleton;
    }
    ovrSkeleton& GetSkeleton() {
        return Skeleton;
    }
    const std::vector<ovrJoint>& GetTransformedJoints() const {
        return TransformedJoints;
    }

   private:
    ovrSkeleton Skeleton;
    OVR::Posef FootPose;
    std::vector<ovrJoint> TransformedJoints;

    float LastUnclampedRoll;

    float TorsoYaw; // current yaw of the torso
    bool TorsoTracksHead; // true to make the torso track the head
    bool ForceRecenter; // true to force the torso to the head yaw

    int ClavicleJointIdx;
    int ShoulderJointIdx;
    int ElbowJointIdx;
    int WristJointIdx;
};

} // namespace OVRFW
