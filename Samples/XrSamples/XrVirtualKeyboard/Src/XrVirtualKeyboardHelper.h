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
Filename    :   XrVirtualKeyboardHelper.h
Content     :   Helper inteface for openxr XR_META_virtual_keyboard extension
Created     :   April 2022
Authors     :   Juan Pablo León, Robert Memmott, Peter Chan, Brent Housen, Chiara Coetzee
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#pragma once

#include <openxr/openxr.h>

#include "XrHelper.h"

struct VirtualKeyboardLocation {
    XrPosef pose;
    float scale;
};

class XrVirtualKeyboardHelper : public XrHelper {
   public:
    static std::vector<const char*> RequiredExtensionNames();

   public:
    explicit XrVirtualKeyboardHelper(XrInstance instance);

    /// XrHelper Interface
    bool SessionInit(XrSession session) override;
    bool SessionEnd() override;
    bool Update(XrSpace currentSpace, XrTime predictedDisplayTime) override;

    bool IsSupported() const;
    bool HasVirtualKeyboard() const;

    // Creation
    bool CreateVirtualKeyboard(XrVirtualKeyboardCreateInfoMETA* createInfo);
    bool CreateVirtualKeyboardSpace(const XrVirtualKeyboardSpaceCreateInfoMETA* locationInfo);
    bool DestroyVirtualKeyboard();

    // Positioning
    bool SuggestVirtualKeyboardLocation(const XrVirtualKeyboardLocationInfoMETA* locationInfo);
    bool GetVirtualKeyboardLocation(
        XrSpace baseSpace,
        XrTime time,
        VirtualKeyboardLocation* keyboardLocation);

    // Render model
    bool ShowModel(bool visible);
    bool GetModelAnimationStates(XrVirtualKeyboardModelAnimationStatesMETA& modelAnimationStates);
    bool GetDirtyTextures(std::vector<uint64_t>& textureIds);
    bool GetTextureData(uint64_t textureId, XrVirtualKeyboardTextureDataMETA& textureData);

    // Interaction
    bool SendVirtualKeyboardInput(
        XrSpace space,
        XrVirtualKeyboardInputSourceMETA source,
        const XrPosef& pointerPose,
        bool pressed,
        XrPosef* interactorRootPose);
    bool UpdateTextContext(const std::string& textContext);

   private:
    XrSession session_ = XR_NULL_HANDLE;
    XrVirtualKeyboardMETA keyboardHandle_ = XR_NULL_HANDLE;
    XrSpace space_ = XR_NULL_HANDLE;

    PFN_xrCreateVirtualKeyboardMETA xrCreateVirtualKeyboardMETA_ = nullptr;
    PFN_xrDestroyVirtualKeyboardMETA xrDestroyVirtualKeyboardMETA_ = nullptr;
    PFN_xrCreateVirtualKeyboardSpaceMETA xrCreateVirtualKeyboardSpaceMETA_ = nullptr;
    PFN_xrSuggestVirtualKeyboardLocationMETA xrSuggestVirtualKeyboardLocationMETA_ = nullptr;
    PFN_xrGetVirtualKeyboardScaleMETA xrGetVirtualKeyboardScaleMETA_ = nullptr;
    PFN_xrSetVirtualKeyboardModelVisibilityMETA xrSetVirtualKeyboardModelVisibilityMETA_ = nullptr;
    PFN_xrGetVirtualKeyboardModelAnimationStatesMETA xrGetVirtualKeyboardModelAnimationStatesMETA_ =
        nullptr;
    PFN_xrGetVirtualKeyboardDirtyTexturesMETA xrGetVirtualKeyboardDirtyTexturesMETA_ = nullptr;
    PFN_xrGetVirtualKeyboardTextureDataMETA xrGetVirtualKeyboardTextureDataMETA_ = nullptr;
    PFN_xrSendVirtualKeyboardInputMETA xrSendVirtualKeyboardInputMETA_ = nullptr;
    PFN_xrChangeVirtualKeyboardTextContextMETA xrChangeVirtualKeyboardTextContext_ = nullptr;

    std::vector<XrVirtualKeyboardAnimationStateMETA> animationStatesBuffer_;
    std::vector<uint8_t> textureDataBuffer_;
};
