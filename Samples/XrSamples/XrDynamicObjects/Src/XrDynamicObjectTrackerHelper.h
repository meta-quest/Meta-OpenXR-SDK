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
Filename    :   XrDynamicObjectTrackerHelper.h
Content     :   Helper inteface for openxr XR_META_dynamic_object_tracker extension
Created     :   September 2023
Authors     :   Adam Bengis, Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#pragma once

#include <meta_openxr_preview/meta_dynamic_object_tracker.h>
#include <meta_openxr_preview/meta_dynamic_object_keyboard.h>

#include "XrHelper.h"

class XrDynamicObjectTrackerHelper : public XrHelper {
   public:
    static std::vector<const char*> RequiredExtensionNames();

   public:
    enum class TrackerState {
        Empty,
        Creating,
        Created,
        Initializing,
        Initialized,
    };

   public:
    explicit XrDynamicObjectTrackerHelper(XrInstance instance);

    void SessionInit(XrSession session) override;
    void SessionEnd() override;
    void Update(XrSpace currentSpace, XrTime predictedDisplayTime) override;

    bool HandleXrEvents(const XrEventDataBaseHeader* baseEventHeader);

    TrackerState GetTrackerState() const;

    bool CreateDynamicObjectTracker();
    bool DestroyDynamicObjectTracker();
    bool SetDynamicObjectTrackedClasses(
        const std::vector<XrDynamicObjectClassMETA>& trackedClasses);
    bool GetDynamicObjectClass(XrSpace space, XrDynamicObjectClassMETA& classType) const;

    bool IsSupported() const;

   private:
    XrSession Session = XR_NULL_HANDLE;
    XrDynamicObjectTrackerMETA DynamicObjectTracker = XR_NULL_HANDLE;

    PFN_xrCreateDynamicObjectTrackerMETA XrCreateDynamicObjectTrackerMETA = nullptr;
    PFN_xrDestroyDynamicObjectTrackerMETA XrDestroyDynamicObjectTrackerMETA = nullptr;
    PFN_xrSetDynamicObjectTrackedClassesMETA XrSetDynamicObjectTrackedClassesMETA = nullptr;
    PFN_xrGetSpaceDynamicObjectDataMETA XrGetSpaceDynamicObjectDataMETA = nullptr;

    TrackerState State = TrackerState::Empty;
};
