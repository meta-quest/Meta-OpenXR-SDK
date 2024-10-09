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
/*******************************************************************************

Filename    :   Main.cpp
Content     :   Simple test app to test openxr hand tracking wide motion mode api
Language    :   C++
Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

*******************************************************************************/

#include <openxr/openxr.h>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <sstream>

#include "XrApp.h"

#include "Input/HandRenderer.h"
#include "Input/AxisRenderer.h"
#include "Input/TinyUI.h"
#include "Render/GeometryRenderer.h"
#include "Render/SimpleBeamRenderer.h"

/// add logging
#define XRLOG ALOG

#include "xr_hand_helper.h"

class XrHandTrackingWideMotionModeApp : public OVRFW::XrApp {
    static constexpr std::string_view kSampleExplanation =
        "Wide Motion Mode:                                                  \n"
        "                                                                   \n"
        "This sample allows you to compare between normal hands             \n"
        "and Wide Motion Mode hands.  It renders two sets of                \n"
        "tracked hands:                                                     \n"
        "* Red are the default hand tracking hands.                         \n"
        "* Green are the wide motion mode hands.                            \n"
        "Click the buttons to toggle rendering of the hand types.           \n"
        "                                                                   \n"
        "The easiest test case to see the difference is to put your hands   \n"
        "under a desk in front of you.                                      \n"
        "                                                                   \n"
        "Red will disappear, while green remains. \n";

   public:
    XrHandTrackingWideMotionModeApp() : OVRFW::XrApp() {
        BackgroundColor = OVR::Vector4f(0.60f, 0.95f, 0.4f, 1.0f);
    }

    // Returns a list of OpenXr extensions needed for this app
    virtual std::vector<const char*> GetExtensions() override {
        std::vector<const char*> extensions = XrApp::GetExtensions();
        /// add hand extensions
        for (const auto& handExtension : XrHandHelper::RequiredExtensionNames()) {
            extensions.push_back(handExtension);
        }

        /// add composition alpha blend
        extensions.push_back(XR_FB_COMPOSITION_LAYER_ALPHA_BLEND_EXTENSION_NAME);

        /// log all extensions
        ALOG("XrHandTrackingWideMotionModeApp requesting extensions:");
        for (const auto& e : extensions) {
            ALOG("   --> %s", e);
        }

        return extensions;
    }

    // Must return true if the application initializes successfully.
    virtual bool AppInit(const xrJava* context) override {
        if (false == ui_.Init(context, GetFileSys())) {
            ALOG("TinyUI::Init FAILED.");
            return false;
        }

        /// hand tracking
        hand_Base_L_ = std::make_unique<XrHandHelper>(
            GetInstance(), true, XrHandHelper::HandType::HANDTYPE_BASE);
        OXR(hand_Base_L_->GetLastError());
        hand_Base_R_ = std::make_unique<XrHandHelper>(
            GetInstance(), false, XrHandHelper::HandType::HANDTYPE_BASE);
        OXR(hand_Base_R_->GetLastError());

        hand_WMM_L_ = std::make_unique<XrHandHelper>(
            GetInstance(), true, XrHandHelper::HandType::HANDTYPE_WMM);
        OXR(hand_WMM_L_->GetLastError());
        hand_WMM_R_ = std::make_unique<XrHandHelper>(
            GetInstance(), false, XrHandHelper::HandType::HANDTYPE_WMM);
        OXR(hand_WMM_R_->GetLastError());

        /// Build UI
        CreateSampleDescriptionPanel();

        bigText_ = ui_.AddLabel("OpenXR Wide Motion Mode", {0.1f, -0.5f, -2.0f}, {1300.0f, 100.0f});
        bigText_->SetSurfaceColor(0, {0.0f, 0.0f, 1.0f, 1.0f});

        renderLabelBaseHandsButton_ = ui_.AddLabel("X", {-0.51f, 0.25f, -2.01f}, {550.0f, 110.0f});
        renderLabelBaseHandsButton_->SetSurfaceColor(0, {1.0f, 0.0f, 0.0f, 1.0f});

        renderBaseHandsButton_ = ui_.AddButton(
            "Stop Rendering Base Hands", {-0.5f, 0.25f, -2.0f}, {500.0f, 100.0f}, [this]() {
                renderBaseHands_ = !renderBaseHands_;
                if (renderBaseHands_) {
                    renderBaseHandsButton_->SetText("Stop Rendering Base Hands");
                    renderLabelBaseHandsButton_->SetSurfaceColor(0, {1.0f, 0.0f, 0.0f, 1.0f});
                    renderLabelBaseHandsButton_->SetText("X");
                } else {
                    renderBaseHandsButton_->SetText("Render Base Hands");
                    renderLabelBaseHandsButton_->SetSurfaceColor(0, {0.75f, 0.0f, 0.0f, 0.25f});
                    renderLabelBaseHandsButton_->SetText("");
                }
            });

        renderLabelWideMotionModeHandsButton_ =
            ui_.AddLabel("X", {-0.51f, 0.0, -2.01f}, {550.0f, 110.0f});
        renderLabelWideMotionModeHandsButton_->SetSurfaceColor(0, {0.0f, 1.0f, 0.0f, 1.0f});

        renderWideMotionModeHandsButton_ = ui_.AddButton(
            "Stop Rendering Wide Motion Mode Hands",
            {-0.5f, 0.0f, -2.0f},
            {500.0f, 100.0f},
            [this]() {
                renderWideMotionModeHands_ = !renderWideMotionModeHands_;
                if (renderWideMotionModeHands_) {
                    renderWideMotionModeHandsButton_->SetText(
                        "Stop Rendering Wide Motion Mode Hands");
                    renderLabelWideMotionModeHandsButton_->SetSurfaceColor(
                        0, {0.0f, 1.0f, 0.0f, 1.0f});
                    renderLabelWideMotionModeHandsButton_->SetText("X");
                } else {
                    renderWideMotionModeHandsButton_->SetText("Render Wide Motion Mode Hands");
                    renderLabelWideMotionModeHandsButton_->SetSurfaceColor(
                        0, {0.0f, 0.75f, 0.0f, 0.25f});
                    renderLabelWideMotionModeHandsButton_->SetText("");
                }
            });

        return true;
    }

    void CreateSampleDescriptionPanel() {
        // Panel to provide sample description to the user for context
        auto descriptionLabel = ui_.AddLabel(
            static_cast<std::string>(kSampleExplanation), {1.15f, 0.25f, -1.8f}, {750.0f, 400.0f});

        // Align and size the description text for readability
        OVRFW::VRMenuFontParms fontParams{};
        fontParams.Scale = 0.5f;
        fontParams.AlignHoriz = OVRFW::HORIZONTAL_LEFT;
        descriptionLabel->SetFontParms(fontParams);
        descriptionLabel->SetTextLocalPosition({-0.65f, 0, 0});

        // Tilt the description billboard 45 degrees towards the user
        descriptionLabel->SetLocalRotation(
            OVR::Quat<float>::FromRotationVector({0, OVR::DegreeToRad(-30.0f), 0}));
        descriptionLabel->SetSurfaceColor(0, {0.0f, 0.0f, 1.0f, 1.0f});
    }

    virtual void AppShutdown(const xrJava* context) override {
        hand_Base_L_ = nullptr;
        hand_Base_R_ = nullptr;
        hand_WMM_L_ = nullptr;
        hand_WMM_R_ = nullptr;

        OVRFW::XrApp::AppShutdown(context);
        ui_.Shutdown();
    }

    virtual bool SessionInit() override {
        /// Use LocalSpace instead of Stage Space.
        CurrentSpace = LocalSpace;
        beamRenderer_.Init(GetFileSys(), nullptr, OVR::Vector4f(1.0f), 1.0f);

        /// hands
        hand_Base_L_->SessionInit(GetSession());
        hand_Base_R_->SessionInit(GetSession());
        hand_WMM_L_->SessionInit(GetSession());
        hand_WMM_R_->SessionInit(GetSession());

        /// rendering;
        handRenderer_Base_L_.Init(&hand_Base_L_->Mesh(), hand_Base_L_->IsLeft());
        handRenderer_Base_R_.Init(&hand_Base_R_->Mesh(), hand_Base_R_->IsLeft());
        handRenderer_WMM_L_.Init(&hand_WMM_L_->Mesh(), hand_WMM_L_->IsLeft());
        handRenderer_WMM_R_.Init(&hand_WMM_R_->Mesh(), hand_WMM_R_->IsLeft());
        axisRendererL_.Init();
        axisRendererR_.Init();

        handRenderer_Base_L_.GlowColor = OVR::Vector3f(1.0f, 0.0f, 0.0f);
        handRenderer_Base_L_.SpecularLightColor = OVR::Vector3f(1.0f, 0.15f, 0.15f) * 0.25f;
        handRenderer_Base_L_.AmbientLightColor = OVR::Vector3f(1.0f, 0.15f, 0.15f) * 1.5f;
        handRenderer_Base_R_.GlowColor = OVR::Vector3f(1.0f, 0.0f, 0.0f);
        handRenderer_Base_R_.SpecularLightColor = OVR::Vector3f(1.0f, 0.15f, 0.15f) * 0.25f;
        handRenderer_Base_R_.AmbientLightColor = OVR::Vector3f(1.0f, 0.15f, 0.15f) * 1.5f;

        handRenderer_WMM_L_.GlowColor = OVR::Vector3f(0.0f, 1.0f, 0.0f);
        handRenderer_WMM_L_.SpecularLightColor = OVR::Vector3f(0.15f, 1.0f, 0.15f) * 0.25f;
        handRenderer_WMM_L_.AmbientLightColor = OVR::Vector3f(0.15f, 1.0f, 0.15f) * 1.5f;
        handRenderer_WMM_R_.GlowColor = OVR::Vector3f(0.0f, 1.0f, 0.0f);
        handRenderer_WMM_R_.SpecularLightColor = OVR::Vector3f(0.15f, 1.0f, 0.15f) * 0.25f;
        handRenderer_WMM_R_.AmbientLightColor = OVR::Vector3f(0.15f, 1.0f, 0.15f) * 1.5f;

        return true;
    }

    virtual void SessionEnd() override {
        /// hands
        hand_Base_L_->SessionEnd();
        hand_Base_R_->SessionEnd();
        hand_WMM_L_->SessionEnd();
        hand_WMM_R_->SessionEnd();

        beamRenderer_.Shutdown();
        handRenderer_Base_L_.Shutdown();
        handRenderer_Base_R_.Shutdown();
        handRenderer_WMM_L_.Shutdown();
        handRenderer_WMM_R_.Shutdown();
        axisRendererL_.Shutdown();
        axisRendererR_.Shutdown();
    }

    // Update state
    virtual void Update(const OVRFW::ovrApplFrameIn& in) override {
        XrSpace currentSpace = GetCurrentSpace();
        XrTime predictedDisplayTime = ToXrTime(in.PredictedDisplayTime);

        /// hands
        hand_Base_L_->Update(currentSpace, predictedDisplayTime);
        hand_Base_R_->Update(currentSpace, predictedDisplayTime);
        hand_WMM_L_->Update(currentSpace, predictedDisplayTime);
        hand_WMM_R_->Update(currentSpace, predictedDisplayTime);

        if (hand_Base_L_->AreLocationsActive()) {
            handRenderer_Base_L_.Update(hand_Base_L_->Joints(), hand_Base_L_->RenderScale());
        }

        if (hand_Base_R_->AreLocationsActive()) {
            handRenderer_Base_R_.Update(hand_Base_R_->Joints(), hand_Base_R_->RenderScale());
        }

        if (hand_WMM_L_->AreLocationsActive()) {
            handRenderer_WMM_L_.Update(hand_WMM_L_->Joints(), hand_WMM_L_->RenderScale());

            std::vector<OVR::Posef> handJoints;
            for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; ++i) {
                if ((hand_WMM_L_->Joints()[i].locationFlags &
                     XR_SPACE_LOCATION_POSITION_TRACKED_BIT) == 0) {
                    const auto p = FromXrPosef(hand_WMM_L_->Joints()[i].pose);
                    handJoints.push_back(p);
                }
            }
            axisRendererL_.Update(handJoints);
        }

        if (hand_WMM_R_->AreLocationsActive()) {
            handRenderer_WMM_R_.Update(hand_WMM_R_->Joints(), hand_WMM_R_->RenderScale());

            std::vector<OVR::Posef> handJoints;
            for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; ++i) {
                if ((hand_WMM_R_->Joints()[i].locationFlags &
                     XR_SPACE_LOCATION_POSITION_TRACKED_BIT) == 0) {
                    const auto p = FromXrPosef(hand_WMM_R_->Joints()[i].pose);
                    handJoints.push_back(p);
                }
            }
            axisRendererR_.Update(handJoints);
        }

        /// UI
        ui_.HitTestDevices().clear();
        if (hand_Base_R_->AreLocationsActive()) {
            ui_.AddHitTestRay(FromXrPosef(hand_Base_R_->AimPose()), hand_Base_R_->IndexPinching());
        } else if (in.RightRemoteTracked) {
            const bool didPinch = in.RightRemoteIndexTrigger > 0.25f;
            ui_.AddHitTestRay(in.RightRemotePointPose, didPinch);
        }

        if (hand_Base_L_->AreLocationsActive()) {
            ui_.AddHitTestRay(FromXrPosef(hand_Base_L_->AimPose()), hand_Base_L_->IndexPinching());
        } else if (in.LeftRemoteTracked) {
            const bool didPinch = in.LeftRemoteIndexTrigger > 0.25f;
            ui_.AddHitTestRay(in.LeftRemotePointPose, didPinch);
        }

        ui_.Update(in);
        beamRenderer_.Update(in, ui_.HitTestDevices());
    }

    // Render eye buffers while running
    virtual void Render(const OVRFW::ovrApplFrameIn& in, OVRFW::ovrRendererOutput& out) override {
        /// Render UI
        ui_.Render(in, out);

        /// Render beams
        beamRenderer_.Render(in, out);

        if (renderBaseHands_) {
            if (hand_Base_L_->AreLocationsActive() && hand_Base_L_->IsPositionValid()) {
                /// Render solid Hands
                handRenderer_Base_L_.Solidity = 1.0f;
                handRenderer_Base_L_.Render(out.Surfaces);
            }

            if (hand_Base_R_->AreLocationsActive() && hand_Base_R_->IsPositionValid()) {
                /// Render solid Hands
                handRenderer_Base_R_.Solidity = 1.0f;
                handRenderer_Base_R_.Render(out.Surfaces);
            }
        }

        if (renderWideMotionModeHands_) {
            if (hand_WMM_L_->AreLocationsActive() && hand_WMM_L_->IsPositionValid()) {
                /// Render solid Hands
                handRenderer_WMM_L_.Solidity = 1.0f;
                handRenderer_WMM_L_.Render(out.Surfaces);
                axisRendererL_.Render(OVR::Matrix4f(), in, out);
            }

            if (hand_WMM_R_->AreLocationsActive() && hand_WMM_R_->IsPositionValid()) {
                /// Render solid Hands
                handRenderer_WMM_R_.Solidity = 1.0f;
                handRenderer_WMM_R_.Render(out.Surfaces);
                axisRendererR_.Render(OVR::Matrix4f(), in, out);
            }
        }
    }

   public:
   private:
    OVRFW::TinyUI ui_;
    OVRFW::SimpleBeamRenderer beamRenderer_;
    std::vector<OVRFW::ovrBeamRenderer::handle_t> beams_;

    /// hands - xr interface
    std::unique_ptr<XrHandHelper> hand_Base_L_;
    std::unique_ptr<XrHandHelper> hand_Base_R_;
    std::unique_ptr<XrHandHelper> hand_WMM_L_;
    std::unique_ptr<XrHandHelper> hand_WMM_R_;
    /// hands - rendering
    OVRFW::HandRenderer handRenderer_Base_L_;
    OVRFW::HandRenderer handRenderer_Base_R_;
    OVRFW::HandRenderer handRenderer_WMM_L_;
    OVRFW::HandRenderer handRenderer_WMM_R_;
    OVRFW::ovrAxisRenderer axisRendererL_;
    OVRFW::ovrAxisRenderer axisRendererR_;

    // gui and state
    bool renderBaseHands_ = true;
    bool renderWideMotionModeHands_ = true;

    OVRFW::VRMenuObject* renderBaseHandsButton_ = nullptr;
    OVRFW::VRMenuObject* renderWideMotionModeHandsButton_ = nullptr;
    OVRFW::VRMenuObject* renderLabelBaseHandsButton_ = nullptr;
    OVRFW::VRMenuObject* renderLabelWideMotionModeHandsButton_ = nullptr;

    // info text;
    OVRFW::VRMenuObject* bigText_ = nullptr;
};

ENTRY_POINT(XrHandTrackingWideMotionModeApp)
