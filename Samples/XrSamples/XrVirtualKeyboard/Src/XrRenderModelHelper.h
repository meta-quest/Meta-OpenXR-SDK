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
Filename    :   XrRenderModelHelper.h
Content     :   Helper inteface for openxr XR_FB_render_model extensions
Created     :   October 2022
Authors     :   Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#pragma once

#include <map>

#include "XrHelper.h"

class XrRenderModelHelper : public XrHelper {
   public:
    static std::vector<const char*> RequiredExtensionNames();

   public:
    explicit XrRenderModelHelper(XrInstance instance);

    /// XrHelper Interface
    bool SessionInit(XrSession session) override;
    bool SessionEnd() override;
    bool Update(XrSpace currentSpace, XrTime predictedDisplayTime) override;

    XrRenderModelKeyFB TryGetRenderModelKey(const char* modelPath);
    std::vector<uint8_t> LoadRenderModel(XrRenderModelKeyFB modelKey);

   private:
    void LazyInitialize();

    XrSession session_ = XR_NULL_HANDLE;

    PFN_xrEnumerateRenderModelPathsFB xrEnumerateRenderModelPathsFB_ = nullptr;
    PFN_xrGetRenderModelPropertiesFB xrGetRenderModelPropertiesFB_ = nullptr;
    PFN_xrLoadRenderModelFB xrLoadRenderModelFB_ = nullptr;

    std::map<std::string, XrRenderModelPropertiesFB> properties_;

    bool isInitialized_ = false;
};
