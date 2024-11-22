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
Filename    :   XrDynamicObjectTrackerHelper.cpp
Content     :   Helper inteface for openxr XR_META_dynamic_object_tracker extension
Created     :   September 2023
Authors     :   Adam Bengis, Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#include "XrDynamicObjectTrackerHelper.h"

#include <Misc/Log.h>
#include <openxr/openxr.h>

std::vector<const char*> XrDynamicObjectTrackerHelper::RequiredExtensionNames() {
    return {XR_METAX1_DYNAMIC_OBJECT_TRACKER_EXTENSION_NAME};
}

XrDynamicObjectTrackerHelper::XrDynamicObjectTrackerHelper(XrInstance instance)
    : XrHelper(instance) {
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrCreateDynamicObjectTrackerMETAX1",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrCreateDynamicObjectTrackerMETAX1)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrDestroyDynamicObjectTrackerMETAX1",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrDestroyDynamicObjectTrackerMETAX1)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrSetDynamicObjectTrackedClassesMETAX1",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrSetDynamicObjectTrackedClassesMETAX1)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrGetSpaceDynamicObjectDataMETAX1",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrGetSpaceDynamicObjectDataMETAX1)));
}

void XrDynamicObjectTrackerHelper::SessionInit(XrSession session) {
    Session = session;
}

void XrDynamicObjectTrackerHelper::SessionEnd() {
    Session = XR_NULL_HANDLE;
}

void XrDynamicObjectTrackerHelper::Update(XrSpace currentSpace, XrTime predictedDisplayTime) {}

bool XrDynamicObjectTrackerHelper::HandleXrEvents(const XrEventDataBaseHeader* baseEventHeader) {
    bool handled = false;
    switch (baseEventHeader->type) {
        case XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_TRACKER_CREATE_RESULT_METAX1: {
            const auto createResult =
                reinterpret_cast<const XrEventDataDynamicObjectTrackerCreateResultMETAX1*>(
                    baseEventHeader);
            // Make sure this belongs to our tracker
            if (createResult->handle == DynamicObjectTracker) {
                if (createResult->result != XR_SUCCESS) {
                    ALOGE("Dynamic object tracker create result: %d", createResult->result);
                    DynamicObjectTracker = XR_NULL_HANDLE;
                    State = TrackerState::Empty;
                } else {
                    State = TrackerState::Created;
                }
                handled = true;
            }
        } break;
        case XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_SET_TRACKED_CLASSES_RESULT_METAX1: {
            const auto setTrackedClassesResult =
                reinterpret_cast<const XrEventDataDynamicObjectSetTrackedClassesResultMETAX1*>(
                    baseEventHeader);
            // Make sure this belongs to our tracker
            if (setTrackedClassesResult->handle == DynamicObjectTracker) {
                if (setTrackedClassesResult->result != XR_SUCCESS) {
                    ALOGE(
                        "Dynamic object set tracked classes result: %d",
                        setTrackedClassesResult->result);
                    State = TrackerState::Created;
                } else {
                    State = TrackerState::Initialized;
                }
                handled = true;
            }
        } break;
        default: {
        }
    }

    return handled;
}

XrDynamicObjectTrackerHelper::TrackerState XrDynamicObjectTrackerHelper::GetTrackerState() const {
    return State;
}

bool XrDynamicObjectTrackerHelper::CreateDynamicObjectTracker() {
    if (State != TrackerState::Empty) {
        ALOGE("Cannot create tracker more than once");
        return false;
    }

    XrDynamicObjectTrackerCreateInfoMETAX1 createInfo{
        XR_TYPE_DYNAMIC_OBJECT_TRACKER_CREATE_INFO_METAX1};
    XrResult result =
        XrCreateDynamicObjectTrackerMETAX1(Session, &createInfo, &DynamicObjectTracker);
    if (result != XR_SUCCESS) {
        ALOGE("xrCreateDynamicObjectTrackerMETAX1 failed, error %d", result);
        return false;
    }

    State = TrackerState::Creating;
    return true;
}

bool XrDynamicObjectTrackerHelper::DestroyDynamicObjectTracker() {
    if (State == TrackerState::Empty) {
        ALOGE("No tracker available");
        return false;
    }

    XrResult result = XrDestroyDynamicObjectTrackerMETAX1(DynamicObjectTracker);
    if (result != XR_SUCCESS) {
        ALOGE("xrDestroyDynamicObjectTrackerMETAX1 failed, error %d", result);
        return false;
    }

    DynamicObjectTracker = XR_NULL_HANDLE;
    State = TrackerState::Empty;
    return true;
}

bool XrDynamicObjectTrackerHelper::SetDynamicObjectTrackedClasses(
    const std::vector<XrDynamicObjectClassMETAX1>& trackedClasses) {
    if (State != TrackerState::Created) {
        ALOGE("No tracker available");
        return false;
    }

    XrDynamicObjectTrackedClassesSetInfoMETAX1 setInfo{
        XR_TYPE_DYNAMIC_OBJECT_TRACKED_CLASSES_SET_INFO_METAX1};
    setInfo.classCount = trackedClasses.size();
    setInfo.classes = trackedClasses.data();
    XrResult result = XrSetDynamicObjectTrackedClassesMETAX1(DynamicObjectTracker, &setInfo);
    if (result != XR_SUCCESS) {
        ALOGE("xrSetDynamicObjectTrackedClassesMETAX1 failed, error %d", result);
        return false;
    }

    State = TrackerState::Initializing;
    return true;
}

bool XrDynamicObjectTrackerHelper::GetDynamicObjectClass(
    XrSpace space,
    XrDynamicObjectClassMETAX1& classType) const {
    XrDynamicObjectDataMETAX1 dynamicObjectData{XR_TYPE_DYNAMIC_OBJECT_DATA_METAX1};
    XrResult result = XrGetSpaceDynamicObjectDataMETAX1(Session, space, &dynamicObjectData);
    if (XR_FAILED(result)) {
        ALOGE("xrGetSpaceDynamicObjectDataMETAX1 failed, error %d", result);
        return false;
    }

    classType = dynamicObjectData.classType;
    return true;
}

bool XrDynamicObjectTrackerHelper::IsSupported() const {
    XrSystemGetInfo systemGetInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId systemId;
    XrResult result = xrGetSystem(Instance, &systemGetInfo, &systemId);
    if (result != XR_SUCCESS) {
        if (result == XR_ERROR_FORM_FACTOR_UNAVAILABLE) {
            ALOGE(
                "Failed to get system; the specified form factor is not available. Is your headset connected?");
        } else {
            ALOGE("xrGetSystem failed, error %d", result);
        }
        exit(1);
    }

    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    XrSystemDynamicObjectTrackerPropertiesMETAX1 dynamicObjectTrackerProps{
        XR_TYPE_SYSTEM_DYNAMIC_OBJECT_TRACKER_PROPERTIES_METAX1};
    systemProperties.next = &dynamicObjectTrackerProps;
    result = xrGetSystemProperties(Instance, systemId, &systemProperties);
    if (result != XR_SUCCESS) {
        ALOGE("xrGetSystemProperties failed, error %d", result);
        return false;
    }

    return (dynamicObjectTrackerProps.supportsDynamicObjectTracker == XR_TRUE);
}
