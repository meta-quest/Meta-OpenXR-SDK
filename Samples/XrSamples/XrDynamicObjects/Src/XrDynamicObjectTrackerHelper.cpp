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
    return {
        XR_META_DYNAMIC_OBJECT_TRACKER_EXTENSION_NAME,
        XR_META_DYNAMIC_OBJECT_KEYBOARD_EXTENSION_NAME};
}

XrDynamicObjectTrackerHelper::XrDynamicObjectTrackerHelper(XrInstance instance)
    : XrHelper(instance) {
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrCreateDynamicObjectTrackerMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrCreateDynamicObjectTrackerMETA)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrDestroyDynamicObjectTrackerMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrDestroyDynamicObjectTrackerMETA)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrSetDynamicObjectTrackedClassesMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrSetDynamicObjectTrackedClassesMETA)));
    OXR(xrGetInstanceProcAddr(
        instance,
        "xrGetSpaceDynamicObjectDataMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&XrGetSpaceDynamicObjectDataMETA)));
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
        case XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_TRACKER_CREATE_RESULT_META: {
            const auto createResult =
                reinterpret_cast<const XrEventDataDynamicObjectTrackerCreateResultMETA*>(
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
        case XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_SET_TRACKED_CLASSES_RESULT_META: {
            const auto setTrackedClassesResult =
                reinterpret_cast<const XrEventDataDynamicObjectSetTrackedClassesResultMETA*>(
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

    XrDynamicObjectTrackerCreateInfoMETA createInfo{
        XR_TYPE_DYNAMIC_OBJECT_TRACKER_CREATE_INFO_META};
    XrResult result = XrCreateDynamicObjectTrackerMETA(Session, &createInfo, &DynamicObjectTracker);
    if (result != XR_SUCCESS) {
        ALOGE("xrCreateDynamicObjectTrackerMETA failed, error %d", result);
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

    XrResult result = XrDestroyDynamicObjectTrackerMETA(DynamicObjectTracker);
    if (result != XR_SUCCESS) {
        ALOGE("xrDestroyDynamicObjectTrackerMETA failed, error %d", result);
        return false;
    }

    DynamicObjectTracker = XR_NULL_HANDLE;
    State = TrackerState::Empty;
    return true;
}

bool XrDynamicObjectTrackerHelper::SetDynamicObjectTrackedClasses(
    const std::vector<XrDynamicObjectClassMETA>& trackedClasses) {
    if (State != TrackerState::Created) {
        ALOGE("No tracker available");
        return false;
    }

    XrDynamicObjectTrackedClassesSetInfoMETA setInfo{
        XR_TYPE_DYNAMIC_OBJECT_TRACKED_CLASSES_SET_INFO_META};
    setInfo.classCount = trackedClasses.size();
    setInfo.classes = trackedClasses.data();
    XrResult result = XrSetDynamicObjectTrackedClassesMETA(DynamicObjectTracker, &setInfo);
    if (result != XR_SUCCESS) {
        ALOGE("xrSetDynamicObjectTrackedClassesMETA failed, error %d", result);
        return false;
    }

    State = TrackerState::Initializing;
    return true;
}

bool XrDynamicObjectTrackerHelper::GetDynamicObjectClass(
    XrSpace space,
    XrDynamicObjectClassMETA& classType) const {
    XrDynamicObjectDataMETA dynamicObjectData{XR_TYPE_DYNAMIC_OBJECT_DATA_META};
    XrResult result = XrGetSpaceDynamicObjectDataMETA(space, &dynamicObjectData);
    if (XR_FAILED(result)) {
        ALOGE("xrGetSpaceDynamicObjectDataMETA failed, error %d", result);
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
    XrSystemDynamicObjectTrackerPropertiesMETA dynamicObjectTrackerProps{
        XR_TYPE_SYSTEM_DYNAMIC_OBJECT_TRACKER_PROPERTIES_META};
    systemProperties.next = &dynamicObjectTrackerProps;

    // specific to keyboard tracking, which we use in this sample
    XrSystemDynamicObjectKeyboardPropertiesMETA dynamicObjectKeyboardProps{
        XR_TYPE_SYSTEM_DYNAMIC_OBJECT_KEYBOARD_PROPERTIES_META};
    dynamicObjectTrackerProps.next = &dynamicObjectKeyboardProps;

    result = xrGetSystemProperties(Instance, systemId, &systemProperties);
    if (result != XR_SUCCESS) {
        ALOGE("xrGetSystemProperties failed, error %d", result);
        return false;
    }

    return (
        dynamicObjectTrackerProps.supportsDynamicObjectTracker == XR_TRUE &&
        dynamicObjectKeyboardProps.supportsDynamicObjectKeyboard == XR_TRUE);
}
