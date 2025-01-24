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
Filename    :   XrSpatialAnchorHelper.cpp
Content     :   Helper inteface for openxr XR_FB_spatial_entity and related extensions
Created     :   September 2023
Authors     :   Adam Bengis, Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#include "XrSpatialAnchorHelper.h"

#include <Misc/Log.h>

std::vector<const char*> XrSpatialAnchorHelper::RequiredExtensionNames() {
    return {
        XR_FB_SCENE_EXTENSION_NAME,
        XR_FB_SPATIAL_ENTITY_EXTENSION_NAME,
        XR_FB_SPATIAL_ENTITY_QUERY_EXTENSION_NAME,
        XR_FB_SPATIAL_ENTITY_STORAGE_EXTENSION_NAME};
}

XrSpatialAnchorHelper::XrSpatialAnchorHelper(XrInstance instance) : XrHelper(instance) {
    // XR_FB_scene
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrGetSpaceBoundingBox3DFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrGetSpaceBoundingBox3DFB)));
        OXR(xrGetInstanceProcAddr(
        instance,
        "xrGetSpaceBoundingBox2DFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrGetSpaceBoundingBox2DFB)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrGetSpaceSemanticLabelsFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrGetSpaceSemanticLabelsFB)));

    // XR_FB_spatial_entity
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrEnumerateSpaceSupportedComponentsFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrEnumerateSpaceSupportedComponentsFB)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrSetSpaceComponentStatusFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrSetSpaceComponentStatusFB)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrGetSpaceComponentStatusFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrGetSpaceComponentStatusFB)));

    // XR_FB_spatial_entity_query
    OXR(xrGetInstanceProcAddr(
        instance, "xrQuerySpacesFB", reinterpret_cast<PFN_xrVoidFunction*>(&XrQuerySpacesFB)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrRetrieveSpaceQueryResultsFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrRetrieveSpaceQueryResultsFB)));
}

void XrSpatialAnchorHelper::SessionInit(XrSession session) {
    Session = session;
}

void XrSpatialAnchorHelper::SessionEnd() {
    Session = XR_NULL_HANDLE;
}

void XrSpatialAnchorHelper::Update(XrSpace currentSpace, XrTime predictedDisplayTime) {}

bool XrSpatialAnchorHelper::HandleXrEvents(const XrEventDataBaseHeader* baseEventHeader) {
    bool handled = false;
    switch (baseEventHeader->type) {
        case XR_TYPE_EVENT_DATA_SPACE_QUERY_RESULTS_AVAILABLE_FB: {
            const auto resultsAvailable =
                reinterpret_cast<const XrEventDataSpaceQueryResultsAvailableFB*>(baseEventHeader);
            if (resultsAvailable->requestId != QuerySpacesRequestId) {
                ALOGE(
                    "Unexpected request id %ull for xrQuerySpacesFB results, ignoring...",
                    resultsAvailable->requestId);
                break;
            }

            // This is for our request, mark event handled regardless of whether retrieval succeeds
            handled = true;

            XrSpaceQueryResultsFB queryResults{XR_TYPE_SPACE_QUERY_RESULTS_FB};
            XrResult result =
                XrRetrieveSpaceQueryResultsFB(Session, resultsAvailable->requestId, &queryResults);
            if (result != XR_SUCCESS) {
                ALOGE("xrRetrieveSpaceQueryResultsFB failed, error %d", result);
                break;
            }

            Anchors.clear();
            Anchors.resize(queryResults.resultCountOutput);

            queryResults.resultCapacityInput = Anchors.size();
            queryResults.resultCountOutput = 0;
            queryResults.results = Anchors.data();

            result =
                XrRetrieveSpaceQueryResultsFB(Session, resultsAvailable->requestId, &queryResults);
            if (result != XR_SUCCESS) {
                ALOGE("xrRetrieveSpaceQueryResultsFB failed, error %d", result);
                break;
            }

            ALOG(
                "xrQuerySpacesFB results received: %u, request id %ull",
                queryResults.resultCountOutput,
                resultsAvailable->requestId);
        } break;
        case XR_TYPE_EVENT_DATA_SPACE_QUERY_COMPLETE_FB: {
            const auto queryComplete =
                reinterpret_cast<const XrEventDataSpaceQueryCompleteFB*>(baseEventHeader);
            if (queryComplete->requestId != QuerySpacesRequestId) {
                ALOGE(
                    "Unexpected request id %ull for xrQuerySpacesFB complete, ignoring...",
                    queryComplete->requestId);
                return false;
            }
            ALOG("xrQuerySpacesFB completed, request id %ull", queryComplete->requestId);
            QuerySpacesRequestId = 0;
            handled = true;
        } break;
        case XR_TYPE_EVENT_DATA_SPACE_SET_STATUS_COMPLETE_FB: {
            const auto setStatusComplete =
                reinterpret_cast<const XrEventDataSpaceSetStatusCompleteFB*>(baseEventHeader);
            ALOG(
                "xrSetSpaceComponentStatusFB completed, type %d, result %s, request id %ull",
                setStatusComplete->componentType,
                setStatusComplete->result == XR_SUCCESS ? "success" : "fail",
                setStatusComplete->requestId);
            RequestIds.erase(setStatusComplete->requestId);
            handled = true;
        } break;
        default: {
        }
    }

    return handled;
}

void XrSpatialAnchorHelper::SetSupportedSemanticLabels(std::string supportedLabels) {
    SupportedLabels = std::move(supportedLabels);
}

bool XrSpatialAnchorHelper::RequestQueryAnchors(XrAsyncRequestIdFB& requestId) {
    if (QuerySpacesRequestId != 0) {
        // Only allow one query in-flight at a time
        ALOGW("xrQuerySpacesFB request already sent.");
        return false;
    }

    XrSpaceQueryInfoFB queryInfo{
        XR_TYPE_SPACE_QUERY_INFO_FB,
        nullptr,
        XR_SPACE_QUERY_ACTION_LOAD_FB,
        100 /*maxResultCount*/,
        XR_NO_DURATION,
        nullptr,
        nullptr};
    XrResult result = XrQuerySpacesFB(
        Session,
        reinterpret_cast<XrSpaceQueryInfoBaseHeaderFB*>(&queryInfo),
        &QuerySpacesRequestId);
    if (result != XR_SUCCESS) {
        ALOGE("xrQuerySpacesFB failed, error %d", result);
        return false;
    }

    ALOG("xrQuerySpacesFB request sent, request id %ull", QuerySpacesRequestId);

    requestId = QuerySpacesRequestId;
    return true;
}

bool XrSpatialAnchorHelper::RequestQueryAnchorsWithComponentEnabled(
    XrSpaceComponentTypeFB componentType,
    XrAsyncRequestIdFB& requestId) {
    if (QuerySpacesRequestId != 0) {
        // Only allow one query in-flight at a time
        ALOGW("xrQuerySpacesFB request already sent.");
        return false;
    }

    XrSpaceStorageLocationFilterInfoFB storageLocationFilterInfo{
        XR_TYPE_SPACE_STORAGE_LOCATION_FILTER_INFO_FB, nullptr, XR_SPACE_STORAGE_LOCATION_LOCAL_FB};
    XrSpaceComponentFilterInfoFB componentFilterInfo{
        XR_TYPE_SPACE_COMPONENT_FILTER_INFO_FB, &storageLocationFilterInfo, componentType};
    XrSpaceQueryInfoFB queryInfo{
        XR_TYPE_SPACE_QUERY_INFO_FB,
        nullptr,
        XR_SPACE_QUERY_ACTION_LOAD_FB,
        100 /*maxResultCount*/,
        XR_NO_DURATION,
        reinterpret_cast<XrSpaceFilterInfoBaseHeaderFB*>(&componentFilterInfo),
        nullptr};
    XrResult result = XrQuerySpacesFB(
        Session,
        reinterpret_cast<XrSpaceQueryInfoBaseHeaderFB*>(&queryInfo),
        &QuerySpacesRequestId);
    if (result != XR_SUCCESS) {
        ALOGE("xrQuerySpacesFB failed, error %d", result);
        return false;
    }

    ALOG(
        "xrQuerySpacesFB request sent with component filter %d, request id %ull",
        static_cast<int>(componentType),
        QuerySpacesRequestId);

    requestId = QuerySpacesRequestId;
    return true;
}

bool XrSpatialAnchorHelper::RequestSetComponentStatus(
    XrSpace space,
    XrSpaceComponentTypeFB type,
    bool enabled,
    XrAsyncRequestIdFB& requestId) {
    if (!IsComponentSupported(space, type)) {
        ALOGE("Anchor does not support component %d.", type);
        return false;
    }

    XrSpaceComponentStatusSetInfoFB setInfo{
        XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB, nullptr, type, enabled, 0};
    XrAsyncRequestIdFB reqId;
    XrResult result = XrSetSpaceComponentStatusFB(space, &setInfo, &reqId);
    if (XR_FAILED(result)) {
        ALOGE("xrSetSpaceComponentStatusFB failed, error %d", result);
        return false;
    }

    ALOG("xrSetSpaceComponentStatusFB request sent for component %d, request id %ull", type, reqId);

    RequestIds.insert(reqId);
    requestId = reqId;
    return true;
}

bool XrSpatialAnchorHelper::IsRequestCompleted(XrAsyncRequestIdFB requestId) const {
    if (QuerySpacesRequestId == requestId) {
        return false;
    }
    if (RequestIds.find(requestId) != RequestIds.end()) {
        return false;
    }
    return true;
}

std::vector<XrSpace> XrSpatialAnchorHelper::GetSpatialAnchors(
    std::function<bool(XrSpace)> filter) const {
    std::vector<XrSpace> results;
    for (const auto& anchor : Anchors) {
        if (filter && filter(anchor.space)) {
            results.emplace_back(anchor.space);
        }
    }
    return results;
}

bool XrSpatialAnchorHelper::IsComponentSupported(XrSpace space, XrSpaceComponentTypeFB type) const {
    uint32_t numComponents = 0;
    OXR(XrEnumerateSpaceSupportedComponentsFB(space, 0, &numComponents, nullptr));
    std::vector<XrSpaceComponentTypeFB> components(numComponents);
    OXR(XrEnumerateSpaceSupportedComponentsFB(
        space, numComponents, &numComponents, components.data()));
    const bool supported =
        std::find(components.begin(), components.end(), type) != components.end();
    return supported;
}

bool XrSpatialAnchorHelper::GetComponentStatus(
    XrSpace space,
    XrSpaceComponentTypeFB type,
    bool& enabled) const {
    if (!IsComponentSupported(space, type)) {
        ALOGE("Anchor does not support component %d.", type);
        return false;
    }

    XrSpaceComponentStatusFB status{XR_TYPE_SPACE_COMPONENT_STATUS_FB};
    XrResult result = XrGetSpaceComponentStatusFB(space, type, &status);
    if (XR_FAILED(result)) {
        ALOGE("XrGetSpaceComponentStatusFB failed, error %d", result);
        return false;
    }

    enabled = (status.enabled && !status.changePending);
    return true;
}

bool XrSpatialAnchorHelper::GetSemanticLabels(XrSpace space, std::string& labels) const {
    if (!IsComponentSupported(space, XR_SPACE_COMPONENT_TYPE_SEMANTIC_LABELS_FB)) {
        ALOGE("Anchor does not support semantic label component.");
        return false;
    }

    XrSemanticLabelsSupportInfoFB supportInfo{
        XR_TYPE_SEMANTIC_LABELS_SUPPORT_INFO_FB,
        nullptr,
        XR_SEMANTIC_LABELS_SUPPORT_MULTIPLE_SEMANTIC_LABELS_BIT_FB,
        SupportedLabels.c_str()};
    XrSemanticLabelsFB semanticLabels{XR_TYPE_SEMANTIC_LABELS_FB};
    if (!SupportedLabels.empty()) {
        semanticLabels.next = &supportInfo;
    }
    XrResult result = XrGetSpaceSemanticLabelsFB(Session, space, &semanticLabels);
    if (XR_FAILED(result)) {
        ALOGE("xrGetSpaceSemanticLabelsFB failed, error %d", result);
        return false;
    }

    std::vector<char> labelData(semanticLabels.bufferCountOutput);
    semanticLabels.bufferCapacityInput = labelData.size();
    semanticLabels.buffer = labelData.data();

    result = XrGetSpaceSemanticLabelsFB(Session, space, &semanticLabels);
    if (XR_FAILED(result)) {
        ALOGE("xrGetSpaceSemanticLabelsFB failed, error %d", result);
        return false;
    }

    labels.assign(semanticLabels.buffer, semanticLabels.bufferCountOutput);
    return true;
}

bool XrSpatialAnchorHelper::GetBoundingBox3D(XrSpace space, XrRect3DfFB& boundingBox3D) const {
    if (!IsComponentSupported(space, XR_SPACE_COMPONENT_TYPE_BOUNDED_3D_FB)) {
        ALOGE("Anchor does not support bounded 3D component.");
        return false;
    }

    XrResult result = XrGetSpaceBoundingBox3DFB(Session, space, &boundingBox3D);
    if (XR_FAILED(result)) {
        ALOGE("xrGetSpaceBoundingBox3DFB failed, error %d", result);
        return false;
    }
    return true;
}

bool XrSpatialAnchorHelper::GetBoundingBox2D(XrSpace space, XrRect2Df& boundingBox2D) const {
    if (!IsComponentSupported(space, XR_SPACE_COMPONENT_TYPE_BOUNDED_2D_FB)) {
        ALOGE("Anchor does not support bounded 2D component.");
        return false;
    }

    XrResult result = XrGetSpaceBoundingBox2DFB(Session, space, &boundingBox2D);
    if (XR_FAILED(result)) {
        ALOGE("xrGetSpaceBoundingBox2DFB failed, error %d", result);
        return false;
    }
    return true;
}
