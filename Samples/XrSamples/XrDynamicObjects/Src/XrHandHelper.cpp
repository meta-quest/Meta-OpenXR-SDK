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
Filename    :   XrHandHelper.cpp
Content     :   Helper Inteface for openxr hand extensions
Created     :   September 2023
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#include "XrHandHelper.h"

std::vector<const char*> XrHandHelper::RequiredExtensionNames() {
    return {
        XR_EXT_HAND_TRACKING_EXTENSION_NAME,
        XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME,
        XR_FB_HAND_TRACKING_AIM_EXTENSION_NAME};
}

XrHandHelper::XrHandHelper(XrInstance instance, bool isLeft) : XrHelper(instance), IsLeft(isLeft) {
    /// Hook up extensions for hand tracking
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrCreateHandTrackerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrCreateHandTrackerExt)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrDestroyHandTrackerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrDestroyHandTrackerExt)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrLocateHandJointsEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrLocateHandJointsExt)));
    /// Hook up extensions for hand rendering
    OXR(xrGetInstanceProcAddr(
        instance, "xrGetHandMeshFB", reinterpret_cast<PFN_xrVoidFunction*>(&XrGetHandMeshFb)));
}

XrHandHelper::~XrHandHelper() {
    XrCreateHandTrackerExt = nullptr;
    XrDestroyHandTrackerExt = nullptr;
    XrLocateHandJointsExt = nullptr;
    XrGetHandMeshFb = nullptr;
};

void XrHandHelper::SessionInit(XrSession session) {
    if (XrCreateHandTrackerExt) {
        XrHandTrackerCreateInfoEXT createInfo{XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT};
        createInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
        createInfo.hand = IsLeft ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT;
        OXR(XrCreateHandTrackerExt(session, &createInfo, &HandTracker));

        if (XrGetHandMeshFb) {
            Mesh.type = XR_TYPE_HAND_TRACKING_MESH_FB;
            Mesh.next = nullptr;
            Mesh.jointCapacityInput = 0;
            Mesh.jointCountOutput = 0;
            Mesh.vertexCapacityInput = 0;
            Mesh.vertexCountOutput = 0;
            Mesh.indexCapacityInput = 0;
            Mesh.indexCountOutput = 0;
            OXR(XrGetHandMeshFb(HandTracker, &Mesh));

            /// update sizes
            Mesh.jointCapacityInput = Mesh.jointCountOutput;
            Mesh.vertexCapacityInput = Mesh.vertexCountOutput;
            Mesh.indexCapacityInput = Mesh.indexCountOutput;
            VertexPositions.resize(Mesh.vertexCapacityInput);
            VertexNormals.resize(Mesh.vertexCapacityInput);
            VertexUVs.resize(Mesh.vertexCapacityInput);
            VertexBlendIndices.resize(Mesh.vertexCapacityInput);
            VertexBlendWeights.resize(Mesh.vertexCapacityInput);
            Indices.resize(Mesh.indexCapacityInput);

            /// skeleton
            Mesh.jointBindPoses = JointBindPoses;
            Mesh.jointParents = JointParents;
            Mesh.jointRadii = JointRadii;
            /// mesh
            Mesh.vertexPositions = VertexPositions.data();
            Mesh.vertexNormals = VertexNormals.data();
            Mesh.vertexUVs = VertexUVs.data();
            Mesh.vertexBlendIndices = VertexBlendIndices.data();
            Mesh.vertexBlendWeights = VertexBlendWeights.data();
            Mesh.indices = Indices.data();
            /// get mesh
            OXR(XrGetHandMeshFb(HandTracker, &Mesh));
        }
    }
}

void XrHandHelper::SessionEnd() {
    if (XrDestroyHandTrackerExt) {
        OXR(XrDestroyHandTrackerExt(HandTracker));
    }
}

void XrHandHelper::Update(XrSpace currentSpace, XrTime predictedDisplayTime) {
    if (XrLocateHandJointsExt) {
        /// aim
        AimState.next = nullptr;
        /// scale
        Scale.next = &AimState;
        Scale.sensorOutput = 1.0f;
        Scale.currentOutput = 1.0f;
        Scale.overrideHandScale = XR_TRUE;
        Scale.overrideValueInput = 1.00f;
        /// locations
        Locations.next = &Scale;
        Locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
        Locations.jointLocations = JointLocations;
        XrHandJointsLocateInfoEXT locateInfo{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
        locateInfo.baseSpace = currentSpace;
        locateInfo.time = predictedDisplayTime;

        MatJointScaledFromUnscaled = OVR::Matrix4f::Identity();

        OXR(XrLocateHandJointsExt(HandTracker, &locateInfo, &Locations));
        OVR::Matrix4f rootMat = OVR::Matrix4f(FromXrPosef(GetWristRootPose()));
        OVR::Matrix4f scaleMat = OVR::Matrix4f::Scaling(GetRenderScale());
        MatJointScaledFromUnscaled = rootMat * scaleMat * rootMat.Inverted();
    }
}

XrPosef XrHandHelper::GetScaledJointPose(XrHandJointEXT joint) const {
    OVR::Posef jointPoseWorldUnscaled = FromXrPosef(JointLocations[joint].pose);
    OVR::Matrix4f scaled = MatJointScaledFromUnscaled * OVR::Matrix4f(jointPoseWorldUnscaled);
    return ToXrPosef(OVR::Posef(scaled));
}

void XrHandHelper::ModifyWristRoot(const XrPosef& wristRootPose) {
    auto rootPose = FromXrPosef(GetWristRootPose());
    auto modifiedPose = FromXrPosef(wristRootPose);
    if (!rootPose.IsEqual(modifiedPose)) {
        const OVR::Matrix4f rootMatrix = OVR::Matrix4f(rootPose);
        const OVR::Matrix4f rootMatrixOffset = OVR::Matrix4f(modifiedPose);
        for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; i++) {
            // Apply offset to hand joints
            auto j = OVR::Matrix4f(FromXrPosef(JointLocations[i].pose));
            JointLocations[i].pose = ToXrPosef(OVR::Posef(rootMatrixOffset + (j - rootMatrix)));
        }
    }
}
