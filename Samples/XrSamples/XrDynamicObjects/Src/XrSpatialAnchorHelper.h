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
Filename    :   XrSpatialAnchorHelper.h
Content     :   Helper inteface for openxr XR_FB_spatial_entity and related extensions
Created     :   September 2023
Authors     :   Adam Bengis, Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#pragma once

#include <unordered_set>

#include <openxr/openxr.h>

#include "XrHelper.h"

class XrSpatialAnchorHelper : public XrHelper {
   public:
    static std::vector<const char*> RequiredExtensionNames();

   public:
    explicit XrSpatialAnchorHelper(XrInstance instance);

    void SessionInit(XrSession session) override;
    void SessionEnd() override;
    void Update(XrSpace currentSpace, XrTime predictedDisplayTime) override;

    bool HandleXrEvents(const XrEventDataBaseHeader* baseEventHeader);

    void SetSupportedSemanticLabels(std::string supportedLabels);

    bool RequestQueryAnchors(XrAsyncRequestIdFB& requestId);
    bool RequestQueryAnchorsWithComponentEnabled(
        XrSpaceComponentTypeFB componentType,
        XrAsyncRequestIdFB& requestId);
    bool RequestSetComponentStatus(
        XrSpace space,
        XrSpaceComponentTypeFB type,
        bool enabled,
        XrAsyncRequestIdFB& requestId);

    bool IsRequestCompleted(XrAsyncRequestIdFB requestId) const;
    std::vector<XrSpace> GetSpatialAnchors(std::function<bool(XrSpace)> filter = {}) const;
    bool IsComponentSupported(XrSpace space, XrSpaceComponentTypeFB type) const;
    bool GetComponentStatus(XrSpace space, XrSpaceComponentTypeFB type, bool& enabled) const;
    bool GetSemanticLabels(XrSpace space, std::string& labels) const;
    bool GetBoundingBox3D(XrSpace space, XrRect3DfFB& boundingBox3D) const;
    bool GetBoundingBox2D(XrSpace space, XrRect2Df& boundingBox2D) const;

   private:
    XrSession Session = XR_NULL_HANDLE;

    // XR_FB_scene
    PFN_xrGetSpaceBoundingBox3DFB XrGetSpaceBoundingBox3DFB = nullptr;
    PFN_xrGetSpaceBoundingBox2DFB XrGetSpaceBoundingBox2DFB = nullptr;
    PFN_xrGetSpaceSemanticLabelsFB XrGetSpaceSemanticLabelsFB = nullptr;

    // XR_FB_spatial_entity
    PFN_xrEnumerateSpaceSupportedComponentsFB XrEnumerateSpaceSupportedComponentsFB = nullptr;
    PFN_xrSetSpaceComponentStatusFB XrSetSpaceComponentStatusFB = nullptr;
    PFN_xrGetSpaceComponentStatusFB XrGetSpaceComponentStatusFB = nullptr;

    // XR_FB_spatial_entity_query
    PFN_xrQuerySpacesFB XrQuerySpacesFB = nullptr;
    PFN_xrRetrieveSpaceQueryResultsFB XrRetrieveSpaceQueryResultsFB = nullptr;

    std::string SupportedLabels;

    XrAsyncRequestIdFB QuerySpacesRequestId = 0;
    std::unordered_set<XrAsyncRequestIdFB> RequestIds;

    std::vector<XrSpaceQueryResultFB> Anchors;
};
