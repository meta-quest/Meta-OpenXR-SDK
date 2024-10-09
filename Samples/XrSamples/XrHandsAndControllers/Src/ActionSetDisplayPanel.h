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
#include <openxr/openxr.h>

#include "XrApp.h"
#include "Input/TinyUI.h"
#include "GUI/VRMenuObject.h"

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
    void AddFloatAction(XrAction action, const char* actionName);
    void AddVec2Action(XrAction action, const char* actionName);
    void AddPoseAction(XrAction action, const char* actionName);

    void Update();
    void UpdateAllLabelRotation(OVR::Quatf const& rot);

   private:
    std::pair<VRMenuObject*, VRMenuObject*> CreateActionLabel(const char* actionName);
    std::string ListBoundSources(XrAction action);
    OVR::Vector3f GetNextLabelLocation();
    OVR::Vector3f GetNextStateLabelLocation();

    // OVRFW::VRMenuObject* backgroundPane_{};
    std::vector<std::pair<XrAction, VRMenuObject*>> boolActions_{};
    std::vector<std::pair<XrAction, VRMenuObject*>> floatActions_{};
    std::vector<std::pair<XrAction, VRMenuObject*>> vec2Actions_{};
    std::vector<std::pair<XrAction, VRMenuObject*>> poseActions_{};
    std::vector<VRMenuObject*> labels_{};
    XrSession Session;
    XrInstance Instance;
    OVRFW::TinyUI* ui_;

    OVR::Vector3f topLeftLocation_;
    int elements_{0};
    static constexpr float kHeaderHeight_{0.15};
    static constexpr float kElementGap_{0.65};

    static constexpr float widthPx_{600};
    static constexpr float heightPx_{500};
    static constexpr float width_{widthPx_ * VRMenuObject::DEFAULT_TEXEL_SCALE};
    static constexpr float height_{heightPx_ * VRMenuObject::DEFAULT_TEXEL_SCALE};
};
