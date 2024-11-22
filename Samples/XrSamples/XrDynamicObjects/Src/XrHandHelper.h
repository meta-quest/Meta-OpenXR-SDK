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
Filename    :   XrHandHelper.h
Content     :   Helper Inteface for openxr hand extensions
Created     :   September 2023
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/
#pragma once

#include "XrHelper.h"

class XrHandHelper : public XrHelper {
   public:
    static std::vector<const char*> RequiredExtensionNames();

   public:
    XrHandHelper(XrInstance instance, bool isLeft);
    ~XrHandHelper() override;

    void SessionInit(XrSession session) override;
    void SessionEnd() override;
    void Update(XrSpace currentSpace, XrTime predictedDisplayTime) override;

   public:
    bool IsLeftHand() const {
        return IsLeft;
    }
    const XrHandTrackingMeshFB& GetMesh() const {
        return Mesh;
    }
    const XrHandTrackingScaleFB& GetScale() const {
        return Scale;
    }
    const XrPosef* GetJointBindPoses() const {
        return JointBindPoses;
    }
    const XrHandJointEXT* GetJointParents() const {
        return JointParents;
    }
    const XrHandJointLocationEXT* GetJoints() const {
        return JointLocations;
    }
    float GetRenderScale() const {
        return Scale.sensorOutput;
    }
    bool IsTracking() const {
        return (HandTracker != XR_NULL_HANDLE);
    }
    bool AreLocationsActive() const {
        return IsTracking() && (Locations.isActive);
    }
    bool IsPositionValid() const {
        return JointLocations[XR_HAND_JOINT_PALM_EXT].locationFlags &
            XR_SPACE_LOCATION_POSITION_VALID_BIT;
    }
    const XrPosef& GetAimPose() const {
        return AimState.aimPose;
    }
    const XrPosef& GetWristRootPose() const {
        return JointLocations[XR_HAND_JOINT_WRIST_EXT].pose;
    }
    bool GetIndexPinching() const {
        return (AimState.status & XR_HAND_TRACKING_AIM_INDEX_PINCHING_BIT_FB) != 0;
    }
    const XrHandTrackingAimStateFB& GetAimState() const {
        return AimState;
    }

    XrPosef GetScaledJointPose(XrHandJointEXT joint) const;
    void ModifyWristRoot(const XrPosef& wristRootPose);

   private:
    bool IsLeft = false;
    /// Hands - extension functions
    PFN_xrCreateHandTrackerEXT XrCreateHandTrackerExt = nullptr;
    PFN_xrDestroyHandTrackerEXT XrDestroyHandTrackerExt = nullptr;
    PFN_xrLocateHandJointsEXT XrLocateHandJointsExt = nullptr;
    /// Hands - FB mesh rendering extensions
    PFN_xrGetHandMeshFB XrGetHandMeshFb = nullptr;
    /// Hands - tracker handles
    XrHandTrackerEXT HandTracker = XR_NULL_HANDLE;
    /// Hands - data buffers
    XrHandJointLocationEXT JointLocations[XR_HAND_JOINT_COUNT_EXT];
    XrPosef JointBindPoses[XR_HAND_JOINT_COUNT_EXT];
    XrHandJointEXT JointParents[XR_HAND_JOINT_COUNT_EXT];
    float JointRadii[XR_HAND_JOINT_COUNT_EXT];
    /// mesh storage
    XrHandTrackingMeshFB Mesh{XR_TYPE_HAND_TRACKING_MESH_FB};
    std::vector<XrVector3f> VertexPositions;
    std::vector<XrVector3f> VertexNormals;
    std::vector<XrVector2f> VertexUVs;
    std::vector<XrVector4sFB> VertexBlendIndices;
    std::vector<XrVector4f> VertexBlendWeights;
    std::vector<int16_t> Indices;
    /// extension nodes
    /// scale
    XrHandTrackingScaleFB Scale{XR_TYPE_HAND_TRACKING_SCALE_FB};
    /// aim
    XrHandTrackingAimStateFB AimState{XR_TYPE_HAND_TRACKING_AIM_STATE_FB};
    /// joint locations *before* applying hand scale
    XrHandJointLocationsEXT Locations{XR_TYPE_HAND_JOINT_LOCATIONS_EXT};
    // cached matrix for applying hand scale to joint locations
    OVR::Matrix4f MatJointScaledFromUnscaled;
};
