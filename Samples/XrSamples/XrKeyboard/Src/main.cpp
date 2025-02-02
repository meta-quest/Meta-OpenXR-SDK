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
Content     :   Simple test app to test openxr keyboard extension
Created     :   Dec 2020
Authors     :   Federico Schliemann
Language    :   C++
Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

*******************************************************************************/

#include <openxr/openxr.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <sstream>

#include "XrApp.h"

#include "Input/AxisRenderer.h"
#include "Input/ControllerRenderer.h"
#include "Input/HandMaskRenderer.h"
#include "Input/HandRenderer.h"
#include "Input/KeyboardRenderer.h"
#include "Input/TinyUI.h"
#include "Render/GeometryRenderer.h"
#include "Render/SimpleBeamRenderer.h"

/// add logging
#define XRLOG ALOG

#include "xr_hand_helper.h"
#include "xr_keyboard_helper.h"
#include "xr_passthrough_helper.h"
#include "xr_render_model_helper.h"

class XrKeyboardApp : public OVRFW::XrApp {
   private:
    static constexpr std::string_view kSampleExplanation =
        "The Tracked Keyboard is the physical keyboard in virtual space.    \n"
        "                                                                   \n"
        "When this component sees a keyboard device in the physical         \n"
        "world, it will try to match that physical keyboard to one of       \n"
        "the user-configured virtual keyboard models. If the match          \n"
        "succeeds, the keyboard is “tracked”, and a virtual keyboard        \n"
        "model will render in the position and orientation of the           \n"
        "physical keyboard.                                                 \n"
        "                                                                   \n"
        "You will need to setup your device with a connected or remote      \n"
        "tracked keyboard. You can do this from Quest Settings.             ";

   public:
    XrKeyboardApp() : OVRFW::XrApp() {
        BackgroundColor = OVR::Vector4f(0.60f, 0.95f, 0.4f, 1.0f);
    }

    // Returns a list of OpenXr extensions needed for this app
    virtual std::vector<const char*> GetExtensions() override {
        std::vector<const char*> extensions = XrApp::GetExtensions();
        /// add keyboard extensions
        for (const auto& kbdExtension : XrKeyboardHelper::RequiredExtensionNames()) {
            extensions.push_back(kbdExtension);
        }
        /// add hand extensions
        for (const auto& handExtension : XrHandHelper::RequiredExtensionNames()) {
            extensions.push_back(handExtension);
        }
        /// add passthrough extensions
        for (const auto& passthroughExtension : XrPassthroughHelper::RequiredExtensionNames()) {
            extensions.push_back(passthroughExtension);
        }
        /// add render model extensions
        for (const auto& renderModelExtension : XrRenderModelHelper::RequiredExtensionNames()) {
            extensions.push_back(renderModelExtension);
        }

        /// add composition alpha blend
        extensions.push_back(XR_FB_COMPOSITION_LAYER_ALPHA_BLEND_EXTENSION_NAME);

        /// log all extensions
        ALOG("XrKeyboardApp requesting extensions:");
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
        handL_ = std::make_unique<XrHandHelper>(GetInstance(), true);
        OXR(handL_->GetLastError());
        handR_ = std::make_unique<XrHandHelper>(GetInstance(), false);
        OXR(handR_->GetLastError());

        /// keyboard tracking
        keyboard_ = std::make_unique<XrKeyboardHelper>(GetInstance());
        OXR(keyboard_->GetLastError());

        /// passthrough
        passthrough_ = std::make_unique<XrPassthroughHelper>(GetInstance(), GetCurrentSpace());
        OXR(passthrough_->GetLastError());

        /// render model
        renderModel_ = std::make_unique<XrRenderModelHelper>(GetInstance());
        OXR(renderModel_->GetLastError());

        /// Build UI
        CreateSampleDescriptionPanel();
        bigText_ = ui_.AddLabel("OpenXR Keyboard", {0.1f, -0.25f, -2.0f}, {1300.0f, 100.0f});
        systemText_ = ui_.AddLabel("No System Keyboard", {0.1f, -0.5f, -2.0f}, {1300.0f, 100.0f});
        ui_.AddToggleButton(
            "Use Passthrough Window",
            "Use Model with Passthrough Hands",
            &usePassthroughWindow_,
            {-0.7f, 0.0f, -1.9f},
            {450.0f, 100.0f});
        connectionRequiredButton_ = ui_.AddButton(
            "Toggle Require Keyboard Connected", {-0.7f, 0.25f, -1.9f}, {450.0f, 100.0f}, [this]() {
                bool currentState = keyboard_->RequireKeyboardConnectedToTrack();
                bool newState = !currentState;
                SetSwitchConnectionRequired(bigText_, connectionRequiredButton_, newState);
            });

        SetSwitchConnectionRequired(bigText_, connectionRequiredButton_, true);

        useRemoteKeyboardButton_ = ui_.AddButton(
            "Toggle Local or Remote Keyboard", {-0.7f, 0.5f, -1.9f}, {450.0f, 100.0f}, [this]() {
                bool currentState = keyboard_->UseRemoteKeyboard();
                bool newState = !currentState;
                SetSwitchUseRemoteKeyboard(
                    bigText_, connectionRequiredButton_, useRemoteKeyboardButton_, newState);
            });

        SetSwitchUseRemoteKeyboard(
            bigText_, connectionRequiredButton_, useRemoteKeyboardButton_, false);

        trackingRequiredButton_ = ui_.AddButton(
            "Show Untracked Keyboard", {-0.7f, 0.75f, -1.9f}, {450.0f, 100.0f}, [this]() {
                bool currentState = keyboard_->TrackingRequired();
                bool newState = !currentState;
                SetTrackingRequired(
                    bigText_, connectionRequiredButton_, trackingRequiredButton_, newState);
            });

        SetTrackingRequired(bigText_, connectionRequiredButton_, trackingRequiredButton_, true);

        return true;
    }

    void CreateSampleDescriptionPanel() {
        // Panel to provide sample description to the user for context
        auto descriptionLabel = ui_.AddLabel(
            static_cast<std::string>(kSampleExplanation), {0.56f, 0.4f, -2.0f}, {760.0f, 360.0f});

        // Align and size the description text for readability
        OVRFW::VRMenuFontParms fontParams{};
        fontParams.Scale = 0.5f;
        fontParams.AlignHoriz = OVRFW::HORIZONTAL_LEFT;
        descriptionLabel->SetFontParms(fontParams);
        descriptionLabel->SetTextLocalPosition({-0.7f, 0, 0});
    }

    virtual void AppShutdown(const xrJava* context) override {
        handL_ = nullptr;
        handR_ = nullptr;
        keyboard_ = nullptr;
        passthrough_ = nullptr;
        renderModel_ = nullptr;

        OVRFW::XrApp::AppShutdown(context);
        ui_.Shutdown();
    }

    virtual bool SessionInit() override {
        /// Use LocalSpace instead of Stage Space.
        CurrentSpace = LocalSpace;
        /// Init session bound objects
        if (false == controllerRenderL_.Init(true)) {
            ALOG("SessionInit::Init L controller renderer FAILED.");
            return false;
        }
        if (false == controllerRenderR_.Init(false)) {
            ALOG("SessionInit::Init R controller renderer FAILED.");
            return false;
        }
        beamRenderer_.Init(GetFileSys(), nullptr, OVR::Vector4f(1.0f), 1.0f);

        /// hands
        handL_->SessionInit(GetSession());
        handR_->SessionInit(GetSession());
        /// keyboard
        keyboard_->SessionInit(GetSession());
        /// passthrough
        passthrough_->SetDefaultSpace(GetCurrentSpace());
        passthrough_->SessionInit(GetSession());
        /// render model
        renderModel_->SessionInit(GetSession());

        /// rendering
        axisRenderer_.Init();
        handRendererL_.Init(&handL_->Mesh(), handL_->IsLeft());
        handRendererR_.Init(&handR_->Mesh(), handR_->IsLeft());
        handMaskRendererL_.Init(handL_->IsLeft() /*leftHand*/);
        handMaskRendererR_.Init(handR_->IsLeft() /*leftHand*/);

        gr_.Init(OVRFW::BuildTesselatedQuadDescriptor(
            4, 4, false /*twoSided*/)); // quad in XY plane, facing +Z
        gr_.DiffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
        gr_.ChannelControl = {0, 1, 0, 1};
        gr_.AmbientLightColor = {1, 1, 1};
        gr_.BlendMode = GL_FUNC_REVERSE_SUBTRACT;

        /// configure hand mask rendering
        handMaskRendererL_.RenderInverseSubtract = true;
        handMaskRendererR_.RenderInverseSubtract = true;
        handMaskRendererL_.UseBorderFade = false;
        handMaskRendererR_.UseBorderFade = false;

        /// hand presence
        ovrFramebuffer_Create(GetSession(), &presenceBuffer_, GL_SRGB8_ALPHA8, 256, 256, 1);

        return true;
    }

    virtual void SessionEnd() override {
        /// hands
        handL_->SessionEnd();
        handR_->SessionEnd();
        /// keyboard
        keyboard_->SessionEnd();
        /// passthrough
        passthrough_->SessionEnd();
        /// render model
        renderModel_->SessionEnd();

        controllerRenderL_.Shutdown();
        controllerRenderR_.Shutdown();
        beamRenderer_.Shutdown();
        axisRenderer_.Shutdown();
        handRendererL_.Shutdown();
        handRendererR_.Shutdown();
        handMaskRendererL_.Shutdown();
        handMaskRendererR_.Shutdown();
        keyboardRenderer_.Shutdown();

        ovrFramebuffer_Destroy(&presenceBuffer_);
    }

    // Update state
    virtual void Update(const OVRFW::ovrApplFrameIn& in) override {
        XrSpace currentSpace = GetCurrentSpace();
        XrTime predictedDisplayTime = ToXrTime(in.PredictedDisplayTime);

        /// Toggle visualizations together
        if (!usePassthroughWindow_) {
            /// render hand presence using a hand mask layer
            renderRealPassthroughLayerUnder_ = false;
            renderRealPassthroughLayerOver_ = true;
        } else {
            /// render hand presence using a planar layer and locally rendered cutout
            renderRealPassthroughLayerUnder_ = true;
            renderRealPassthroughLayerOver_ = false;
        }

        /// render model
        renderModel_->Update(currentSpace, predictedDisplayTime);
        /// hands
        handL_->Update(currentSpace, predictedDisplayTime);
        handR_->Update(currentSpace, predictedDisplayTime);
        /// Keyboard
        keyboard_->Update(currentSpace, predictedDisplayTime);

        if (keyboard_->GetAndClearSystemKeyboardStateChanged()) {
            // update the render model
            if (keyboard_->SystemKeyboardExists()) {
                std::vector<uint8_t> keyboardBuffer =
                    renderModel_->LoadRenderModel(keyboard_->UseRemoteKeyboard());
                if (keyboardBuffer.size() > 0) {
                    ALOG("### Keyboard Render Model Size: %u", (uint32_t)keyboardBuffer.size());
                    keyboardRenderer_.Init(keyboardBuffer);
                } else {
                    ALOG("### Failed to Load keyboard Render Model");
                }
            } else {
                ALOG("### no system keyboard, clearing model");
            }

            // rebuild the system keyboard string
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;

            if (keyboard_->UseRemoteKeyboard()) {
                ss << "(REMOTE) ";
            } else {
                ss << "(LOCAL) ";
            }

            if (keyboard_->SystemKeyboardExists()) {
                ss << "System Keyboard: " << keyboard_->SystemKeyboardDesc().trackedKeyboardId
                   << ": " << keyboard_->SystemKeyboardDesc().name << "    "
                   << (keyboard_->SystemKeyboardConnected() ? "Connected" : "Not Connected");
                if (keyboard_->TrackingSystemKeyboard()) {
                    ss << " TRACKING";
                } else {
                    ss << " not TRACKING";
                }
            } else {
                ss << "No System Keyboard";
            }
            if (systemText_ != nullptr) {
                systemText_->SetText(ss.str().c_str());
            }
        }

        if (keyboard_->IsLocationActive()) {
            renderKeyboard_ = true;
            auto location = keyboard_->Location();
            std::vector<OVR::Posef> keyboardPoses;
            // Tracked joints and computed joints can all be valid
            XrSpaceLocationFlags isValid =
                XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;
            if ((location.locationFlags & isValid) != 0) {
                /// render a box
                pose_ = FromXrPosef(location.pose);
                dimensions_ = FromXrVector3f(keyboard_->Size());
                /// add center
                keyboardPoses.push_back(pose_);
                /// add corners
                OVR::Posef point;
                point = OVR::Posef::Identity();
                point.Translation.x += dimensions_.x / 2.0f;
                point.Translation.z += dimensions_.z / 2.0f;
                keyboardPoses.push_back(pose_ * point);
                point = OVR::Posef::Identity();
                point.Translation.x += dimensions_.x / 2.0f;
                point.Translation.z -= dimensions_.z / 2.0f;
                keyboardPoses.push_back(pose_ * point);
                point = OVR::Posef::Identity();
                point.Translation.x -= dimensions_.x / 2.0f;
                point.Translation.z += dimensions_.z / 2.0f;
                keyboardPoses.push_back(pose_ * point);
                point = OVR::Posef::Identity();
                point.Translation.x -= dimensions_.x / 2.0f;
                point.Translation.z -= dimensions_.z / 2.0f;
                keyboardPoses.push_back(pose_ * point);
            }
            axisRenderer_.Update(keyboardPoses);

            /// update cut out plane pose
            OVR::Posef planePose = pose_;
            planePose.Translation.y -= 0.02f;
            planePose.Rotation *= OVR::Quatf({1.0f, 0.0f, 0.0f}, OVR::DegreeToRad(-90.0f));
            gr_.SetPose(planePose);
            const float padding = 0.1f; // provide some padding
            gr_.SetScale(
                {(dimensions_.x * 0.5f) + padding, (dimensions_.z * 0.5f) + padding, 1.0f});
            gr_.Update();
        } else {
            renderKeyboard_ = false;
        }

        /// Compute fade heuristics
        bool isKeyboardStale = ComputeKeyboardOpacity(in.PredictedDisplayTime);
        if (!isKeyboardStale) {
            /// force keyboard render at LKG, do opacity fade
            renderKeyboard_ = true;
        }

        if (renderKeyboard_) {
            /// render keyboard model
            keyboardRenderer_.Update(pose_);

            /// render passthrough
            OVR::Vector3f scale{1, 1, 1};
            if (renderRealPassthroughLayerUnder_) {
                /// TODO: size the passthrough layer based on coverage of hands
                /// instead of hard coded 3x of keyboard size
                /// Underlay layer needs some padding
                scale.x *= 3.0f;
                scale.z *= 3.0f;
            }

            /// Send the transform to the planar layer, (hand layer ignores it)
            OVR::Posef passthroughPose = pose_;
            if (usePassthroughWindow_) {
                passthroughPose.Translation.y += (dimensions_.y / 2.0f);
            }
            passthrough_->SetTransform(
                ToXrPosef(passthroughPose), ToXrVector3f(dimensions_ * scale));
        }

        /// passthrough
        if (!passthrough_->Update(currentSpace, predictedDisplayTime)) {
            ALOG("passthrough_->Update() FAILED.");
            OXR(passthrough_->GetLastError());
        }

        controllerRenderL_.Update(in.LeftRemotePose);
        controllerRenderR_.Update(in.RightRemotePose);

        if (handL_->AreLocationsActive()) {
            handRendererL_.Update(handL_->Joints(), handL_->RenderScale());
            handMaskRendererL_.Update(
                in.HeadPose,
                FromXrPosef(handL_->Joints()[XR_HAND_JOINT_WRIST_EXT].pose),
                handRendererL_.Transforms(),
                handL_->RenderScale());
        }

        if (handR_->AreLocationsActive()) {
            handRendererR_.Update(handR_->Joints(), handR_->RenderScale());
            handMaskRendererR_.Update(
                in.HeadPose,
                FromXrPosef(handR_->Joints()[XR_HAND_JOINT_WRIST_EXT].pose),
                handRendererR_.Transforms(),
                handR_->RenderScale());
        }

        /// UI
        ui_.HitTestDevices().clear();

        if (handR_->AreLocationsActive()) {
            ui_.AddHitTestRay(FromXrPosef(handR_->AimPose()), handR_->IndexPinching());
        } else if (in.RightRemoteTracked) {
            const bool didPinch = in.RightRemoteIndexTrigger > 0.25f;
            ui_.AddHitTestRay(in.RightRemotePointPose, didPinch);
        }

        if (handL_->AreLocationsActive()) {
            ui_.AddHitTestRay(FromXrPosef(handL_->AimPose()), handL_->IndexPinching());
        } else if (in.LeftRemoteTracked) {
            const bool didPinch = in.LeftRemoteIndexTrigger > 0.25f;
            ui_.AddHitTestRay(in.LeftRemotePointPose, didPinch);
        }

        ui_.Update(in);
        beamRenderer_.Update(in, ui_.HitTestDevices());
    }

    // Render eye buffers while running
    virtual void Render(const OVRFW::ovrApplFrameIn& in, OVRFW::ovrRendererOutput& out) override {
        /// Render plane
        if (usePassthroughWindow_ && renderKeyboard_) {
            gr_.Render(out.Surfaces);
        }

        /// configure hand mask rendering
        const float kMaskSize = usePassthroughWindow_ ? 0.0125f : 0.0175f;
        handMaskRendererL_.AlphaMaskSize = kMaskSize;
        handMaskRendererR_.AlphaMaskSize = kMaskSize;

        /// Render UI
        ui_.Render(in, out);

        /// Render keyboard
        if (renderKeyboard_) {
            /// render axis to show tracking status
            if (keyboard_->IsLocationActive()) {
                axisRenderer_.Render(OVR::Matrix4f(), in, out);
            }

            /// Always render the keyboard at the LKG location ...
            keyboardRenderer_.ShowModel = !usePassthroughWindow_;
            /// Opacity fade when losing tracking
            keyboardRenderer_.Opacity = keyboardOpacity_;
            keyboardRenderer_.Render(out.Surfaces);
        }

        /// Render beams
        beamRenderer_.Render(in, out);

        /// Rendering a blend of hand mask and solid hands
        /// keep hands semi-transparent for regular mask, solid for passthrough window
        const float HandMaskBlendFactor = usePassthroughWindow_ ? 1.0f : 0.4f;
        const float HandFadeIntensity = usePassthroughWindow_ ? 1.0f : 0.75f;
        const float HandBlendFactor = 1.0f;

        renderPassthroughHands_ = false;
        if (handL_->AreLocationsActive() && handL_->IsPositionValid()) {
            /// Determine hand / mask blending
            /// based on proximity to keyboard heuristic
            const float MaskBlend = ComputeHandBlendFactor(
                FromXrPosef(handL_->Joints()[XR_HAND_JOINT_MIDDLE_TIP_EXT].pose).Translation,
                pose_.Translation);

            /// only render hand mask when we are using a planar layer
            if (renderKeyboard_ && usePassthroughWindow_) {
                /// ... and render the cutout mask
                handMaskRendererL_.LayerBlend = (MaskBlend)*HandMaskBlendFactor;
                handMaskRendererL_.FadeIntensity = HandFadeIntensity;
                handMaskRendererL_.Render(out.Surfaces);
            }

            /// Render solid Hands
            handRendererL_.Confidence = (1.0f - MaskBlend) * HandBlendFactor;
            handRendererL_.Render(out.Surfaces);

            /// Render passthrough hands if either hand is within visible range
            if (MaskBlend > 0.0f) {
                renderPassthroughHands_ = true;
            }
        } else if (in.LeftRemoteTracked) {
            // Only render controller when hands are not rendered
            // Note: Hand tracking can drive controller positions as well.
            controllerRenderL_.Render(out.Surfaces);
        }

        if (handR_->AreLocationsActive() && handR_->IsPositionValid()) {
            /// Determine hand / mask blending
            /// based on proximity to keyboard heuristic
            const float MaskBlend = ComputeHandBlendFactor(
                FromXrPosef(handR_->Joints()[XR_HAND_JOINT_MIDDLE_TIP_EXT].pose).Translation,
                pose_.Translation);

            /// only render hand mask when we are using a planar layer
            if (renderKeyboard_ && usePassthroughWindow_) {
                /// ... and render the cutout mask
                handMaskRendererR_.LayerBlend = (MaskBlend)*HandMaskBlendFactor;
                handMaskRendererR_.FadeIntensity = HandFadeIntensity;
                handMaskRendererR_.Render(out.Surfaces);
            }

            /// Render solid Hands
            handRendererR_.Confidence = (1.0f - MaskBlend) * HandBlendFactor;
            handRendererR_.Render(out.Surfaces);

            /// Render passthrough hands if either hand is within visible range
            if (MaskBlend > 0.0f) {
                renderPassthroughHands_ = true;
            }
        } else if (in.RightRemoteTracked) {
            // Only render controller when hands are not rendered
            // Note: Hand tracking can drive controller positions as well.
            controllerRenderR_.Render(out.Surfaces);
        }
    }

    float ComputeHandBlendFactor(const OVR::Vector3f& hand, const OVR::Vector3f& keyboard) {
        const OVR::Vector3f v = hand - keyboard;
        const float l = v.Length();

        /// simple blend in ranger
        const float kHiThreshold = 0.20f;
        const float kLoThreshold = 0.10f;
        if (l > kHiThreshold) {
            /// too far
            return 0.0f;
        } else if (l > kLoThreshold) {
            /// blend
            return (kHiThreshold - l) / (kHiThreshold - kLoThreshold);
        } else {
            /// near
            return 1.0f;
        }
    }

    bool ComputeKeyboardOpacity(double predictedDisplayTimeInSeconds) {
        /// How soon before the keyboard begins to fade due to not being tracked.
        static const double kStaleDelaySeconds = 1.0;
        static const double kStaleFadeDurationSeconds = 0.5;

        bool isTrackingFullyStale = false;
        if (!keyboard_->IsLocationActive()) {
            // The keyboard is not tracked this frame.
            double elapsedTime = predictedDisplayTimeInSeconds - lastTrackedTimeSeconds_;
            if (elapsedTime < kStaleDelaySeconds) {
                keyboardOpacity_ = 1.0f;
            } else {
                const double elapsedPostDelayTime = elapsedTime - kStaleDelaySeconds;
                isTrackingFullyStale = elapsedPostDelayTime > kStaleFadeDurationSeconds;
                keyboardOpacity_ = 1.0f -
                    std::clamp(float(elapsedPostDelayTime / kStaleFadeDurationSeconds), 0.0f, 1.0f);
            }
        } else {
            // keyboard is tracked
            keyboardOpacity_ = 1.0f;
            lastTrackedTimeSeconds_ = predictedDisplayTimeInSeconds;
        }
        return isTrackingFullyStale;
    }

    void AddPassthroughLayer(xrCompositorLayerUnion* layers, int& layerCount) {
        /// Add the blending layer as a chain to the passthrough one
        /// NOTE: this needs to be static or otherwise stay around when this call executes
        /// or the actual call to xrEndFrame will get garbage memory
        static XrCompositionLayerAlphaBlendFB alphaBlend{XR_TYPE_COMPOSITION_LAYER_ALPHA_BLEND_FB};
        alphaBlend.next = nullptr;
        alphaBlend.srcFactorColor = XR_BLEND_FACTOR_ONE_FB;
        alphaBlend.dstFactorColor = XR_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA_FB;
        alphaBlend.srcFactorAlpha = XR_BLEND_FACTOR_ONE_FB;
        alphaBlend.dstFactorAlpha = XR_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA_FB;

        /// Add the passthrough layer
        XrCompositionLayerPassthroughFB passthroughLayer{XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB};
        passthroughLayer.next = &alphaBlend;
        passthroughLayer.layerHandle = GetLayer();
        if (passthroughLayer.layerHandle != XR_NULL_HANDLE) {
            layers[layerCount++].Passthrough = passthroughLayer;
            /// ALOG("adding XrCompositionLayerPassthroughFB SUCCESS.");
        } else {
            ALOG("adding XrCompositionLayerPassthroughFB FAILED.");
        }
    }

    virtual void PreProjectionAddLayer(xrCompositorLayerUnion* layers, int& layerCount) override {
        if (!renderKeyboard_) {
            return;
        }
        /// add passthrough layer first
        if (renderRealPassthroughLayerUnder_) {
            AddPassthroughLayer(layers, layerCount);
        }
    }

    virtual void PostProjectionAddLayer(xrCompositorLayerUnion* layers, int& layerCount) override {
        if (!renderKeyboard_ || !renderPassthroughHands_) {
            return;
        }
        /// add passthrough layer
        if (renderRealPassthroughLayerOver_) {
            AddPassthroughLayer(layers, layerCount);
        }
    }

    const XrPassthroughLayerFB& GetLayer() const {
        return usePassthroughWindow_ ? passthrough_->GetProjectedLayer()
                                     : passthrough_->GetHandsLayer();
    }

    void SetTrackingRequired(
        OVRFW::VRMenuObject* bigText,
        OVRFW::VRMenuObject* connectionRequiredButton,
        OVRFW::VRMenuObject* toggleButton,
        bool state) {
        if (state) {
            keyboard_->SetTrackingRequired(true);
            toggleButton->SetSurfaceColor(0, OVR::Vector4f(0.25f, 0.25f, 1.0f, 1.0f));
            toggleButton->SetText("Show Untracked Keyboard");
        } else {
            // set connection required to false since if we aren't tracking we are likely in debug
            // mode and connection isn't necessary
            SetSwitchConnectionRequired(bigText, connectionRequiredButton, false);
            keyboard_->SetTrackingRequired(false);
            toggleButton->SetSurfaceColor(0, OVR::Vector4f(1.0f, 0.25f, 0.25f, 1.0f));
            toggleButton->SetText("Do Not Show Untracked Keyboard");
        }
    }

    void SetSwitchConnectionRequired(
        OVRFW::VRMenuObject* bigText,
        OVRFW::VRMenuObject* toggleButton,
        bool state) {
        if (state) {
            bigText->SetText("Open XR Keyboard: Connection Required");
            keyboard_->SetRequireKeyboardConnectedToTrack(true);
            toggleButton->SetSurfaceColor(0, OVR::Vector4f(0.25f, 0.25f, 1.0f, 1.0f));
        } else {
            bigText->SetText("Open XR Keyboard: no Connection Required");
            keyboard_->SetRequireKeyboardConnectedToTrack(false);
            toggleButton->SetSurfaceColor(0, OVR::Vector4f(1.0f, 0.25f, 0.25f, 1.0f));
        }
    }

    void SetSwitchUseRemoteKeyboard(
        OVRFW::VRMenuObject* bigText,
        OVRFW::VRMenuObject* connectionRequiredButton,
        OVRFW::VRMenuObject* toggleButton,
        bool state) {
        if (state) {
            // Set the connction required to false because at the current time, remote will always
            // be unconnected.
            SetSwitchConnectionRequired(bigText, connectionRequiredButton, false);
            keyboard_->SetUseRemoteKeyboard(true);
            toggleButton->SetSurfaceColor(0, OVR::Vector4f(1.0f, 0.25f, 0.25f, 1.0f));
        } else {
            keyboard_->SetUseRemoteKeyboard(false);
            toggleButton->SetSurfaceColor(0, OVR::Vector4f(0.25f, 0.25f, 1.0f, 1.0f));
        }
    }

   public:
   private:
    OVRFW::ControllerRenderer controllerRenderL_;
    OVRFW::ControllerRenderer controllerRenderR_;
    OVRFW::TinyUI ui_;
    OVRFW::SimpleBeamRenderer beamRenderer_;
    std::vector<OVRFW::ovrBeamRenderer::handle_t> beams_;

    /// hands - xr interface
    std::unique_ptr<XrHandHelper> handL_;
    std::unique_ptr<XrHandHelper> handR_;
    /// hands - rendering
    OVRFW::HandRenderer handRendererL_;
    OVRFW::HandRenderer handRendererR_;
    OVRFW::HandMaskRenderer handMaskRendererL_;
    OVRFW::HandMaskRenderer handMaskRendererR_;

    /// keyboard - xr interface
    std::unique_ptr<XrKeyboardHelper> keyboard_;
    /// keyboard - rendering
    OVRFW::ovrAxisRenderer axisRenderer_;
    OVRFW::KeyboardRenderer keyboardRenderer_;
    /// keyboard - hand presence rendering
    OVR::Posef pose_;
    OVR::Vector3f dimensions_;
    ovrFramebuffer presenceBuffer_;

    /// passthrough - xr interface
    std::unique_ptr<XrPassthroughHelper> passthrough_;

    /// render model - xr interface
    std::unique_ptr<XrRenderModelHelper> renderModel_;

    /// control
    bool renderRealPassthroughLayerUnder_ = true;
    bool renderRealPassthroughLayerOver_ = false;

    /// state
    bool renderKeyboard_ = false;
    bool renderPassthroughHands_ = false;
    bool usePassthroughWindow_ = false;

    /// geometry renderer for passthrough cutout
    OVRFW::GeometryRenderer gr_;

    // system keyboard info text;
    OVRFW::VRMenuObject* bigText_ = nullptr;
    OVRFW::VRMenuObject* systemText_ = nullptr;
    OVRFW::VRMenuObject* connectionRequiredButton_ = nullptr;
    OVRFW::VRMenuObject* useRemoteKeyboardButton_ = nullptr;
    OVRFW::VRMenuObject* trackingRequiredButton_ = nullptr;

    /// fade heuristic to match VrShell
    float keyboardOpacity_ = 0.0f;
    double lastTrackedTimeSeconds_ = 0.0;
};

ENTRY_POINT(XrKeyboardApp)
