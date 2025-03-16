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
#include "ActionSetDisplayPanel.h"
#include "Render/BitmapFont.h"


#include <openxr/openxr.h>
#include <sstream>
#include <string>

ActionSetDisplayPanel::ActionSetDisplayPanel(
    std::string title,
    XrSession session,
    XrInstance instance,
    OVRFW::TinyUI* ui,
    OVR::Vector3f topLeftLocation)
    : Session{session}, Instance{instance}, Ui{ui}, TopLeftLocation{topLeftLocation} {
    auto label = Ui->AddLabel(
        title, GetNextLabelLocation() + OVR::Vector3f{0, kHeaderHeight, 0.00}, {kWidthPx, 45.0f});
    Labels.push_back(label);
}

void ActionSetDisplayPanel::AddBoolAction(XrAction action, const char* actionName) {
    auto labelPair = CreateActionLabel(actionName);
    auto headerLabel = labelPair.first;
    auto actionStateLabel = labelPair.second;
    BoolActions.push_back({action, headerLabel, actionStateLabel, actionName});
    Labels.push_back(headerLabel);
}

std::pair<VRMenuObject*, VRMenuObject*> ActionSetDisplayPanel::CreateActionLabel(
    const char* actionName) {
    auto label = Ui->AddLabel(actionName, GetNextLabelLocation(), {kWidthPx, 45.0f});
    auto stateLabel = Ui->AddLabel("state", GetNextStateLabelLocation(), {kWidthPx, 250.0f});

    OVRFW::VRMenuFontParms fontParams{};
    fontParams.Scale = 0.5f;
    fontParams.AlignVert = OVRFW::VERTICAL_CENTER;
    fontParams.AlignHoriz = OVRFW::HORIZONTAL_LEFT;
    label->SetFontParms(fontParams);
    label->SetTextLocalPosition({-0.45f * kWidth, 0, 0});
    stateLabel->SetFontParms(fontParams);
    stateLabel->SetTextLocalPosition({-0.47f * kWidth, -0.02f * kHeight, 0});

    Elements++;
    return std::make_pair(label, stateLabel);
}

OVR::Vector3f ActionSetDisplayPanel::GetNextLabelLocation() {
    return TopLeftLocation +
        OVR::Vector3f{kWidth * 0.5f, -Elements * kElementGap - kHeaderHeight, 0.01};
}

OVR::Vector3f ActionSetDisplayPanel::GetNextStateLabelLocation() {
    return GetNextLabelLocation() + OVR::Vector3f{0.0, -kElementGap * 0.5f, 0.0};
}

void ActionSetDisplayPanel::Update() {
    for (auto& boolAction : BoolActions) {
        XrAction action = boolAction.Action;
        VRMenuObject* header = boolAction.HeaderMenu;
        VRMenuObject* label = boolAction.StateMenu;
        auto& totalCount = boolAction.TotalCount;
        auto& showActiveStateCountdown = boolAction.ShowActiveStateCountdown;

        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = action;
        getInfo.subactionPath = XR_NULL_PATH;
        XrActionStateBoolean state{XR_TYPE_ACTION_STATE_BOOLEAN};
        OXR(xrGetActionStateBoolean(Session, &getInfo, &state));

        const bool microgestureEventDetected = state.changedSinceLastSync && state.currentState;
        if (microgestureEventDetected) {
            ++totalCount;
        }
        showActiveStateCountdown = microgestureEventDetected
            ? kShowActiveState
            : (showActiveStateCountdown > 0 ? showActiveStateCountdown - 1 : 0);

        label->SetText(
            "currentState:            %s\n"
            "isActive:                 %s\n"
            "changedSinceLastSync:  %s\n"
            "lastChangeTime:         %ldms\n\n"
            "num events:              %d\n",
            state.currentState ? "True" : "False",
            state.isActive ? "True" : "False",
            state.changedSinceLastSync ? "True" : "False",
            state.lastChangeTime / (1000 * 1000), // convert from ns to ms
            totalCount);
        label->SetSurfaceColor(
            0,
            showActiveStateCountdown > 0 ? OVR::Vector4f(0.05f, 0.5f, 0.05f, 0.6f)
                                         : OVR::Vector4f(0.05f, 0.05f, 0.05f, 1.0f));
        label->SetSelected(state.currentState);

        header->SetSurfaceColor(
            0,
            showActiveStateCountdown > 0 ? OVR::Vector4f(0.2f, 0.5f, 0.2f, 0.6f)
                                         : OVR::Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
        header->SetSelected(state.currentState);
            }
}
