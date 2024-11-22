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
#include <map>
#include "XrApp.h"
#include "Input/TinyUI.h"
#include "GUI/VRMenuObject.h"

#include <openxr/openxr.h>

using OVRFW::VRMenuObject;

class ActionSetDisplayPanel {
   public:
    ActionSetDisplayPanel(
        std::string title,
        XrSession session,
        XrInstance instance,
        OVRFW::TinyUI* ui,
        OVR::Vector3f topLeftLocation);

    void AddBoolAction(XrAction action, const char* actionName);
    void AddPinch(const char* pinchName);

    void UpdatePinch(const char* pinchName, bool state, float value);
    void Update();

   private:
    static constexpr float kHeaderHeight{0.15};
    static constexpr float kElementGap{0.65};
    static constexpr float kWidthPx{600};
    static constexpr float kHeightPx{500};
    static constexpr float kWidth{kWidthPx * VRMenuObject::DEFAULT_TEXEL_SCALE};
    static constexpr float kHeight{kHeightPx * VRMenuObject::DEFAULT_TEXEL_SCALE};
    static constexpr uint32_t kShowActiveState{10};

    std::pair<VRMenuObject*, VRMenuObject*> CreateActionLabel(const char* actionName);
    OVR::Vector3f GetNextLabelLocation();
    OVR::Vector3f GetNextStateLabelLocation();

    struct BoolAction {
        XrAction Action;
        VRMenuObject* HeaderMenu;
        VRMenuObject* StateMenu;
        std::string ActionName;
        uint32_t TotalCount = 0;
        XrTime LastStateUpdateTime = 0;
        uint32_t ShowActiveStateCountdown = 0;
    };

    std::vector<BoolAction> BoolActions{};
    std::vector<VRMenuObject*> Labels{};
    XrSession Session;
    XrInstance Instance;
    OVRFW::TinyUI* Ui;

    OVR::Vector3f TopLeftLocation;
    int Elements{0};
};
