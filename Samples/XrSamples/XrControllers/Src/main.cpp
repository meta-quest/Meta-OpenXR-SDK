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
Content     :   OpenXR sample app to showcase the use of controller and haptics extensions
Created     :   Dec 2020
Authors     :   -
Language    :   C++

*******************************************************************************/

#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <openxr/openxr.h>

#include <meta_openxr_preview/extx1_haptic_parametric.h>

#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "XrApp.h"

/// Haptic effects
const float kScrollBuffer[]{1, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
float reducingIntensity[]{1, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
float increasingIntensity[]{0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
float constantIntensity[]{0.5};

// PCM Haptic API
PFN_xrGetDeviceSampleRateFB xrGetDeviceSampleRateFB = nullptr;

#include "Input/SkeletonRenderer.h"
#include "Input/ControllerRenderer.h"
#include "Input/TinyUI.h"
#include "Input/AxisRenderer.h"
#include "Render/SimpleBeamRenderer.h"


class XrControllersApp : public OVRFW::XrApp {
   public:
    XrControllersApp() : OVRFW::XrApp() {
        BackgroundColor = OVR::Vector4f(0.60f, 0.95f, 0.4f, 1.0f);
        OpenXRVersion = XR_API_VERSION_1_1;
    }

    /**
Function to create PCM samples from an array of amplitudes, frequency and duration
**/
    std::vector<float>
    createPCMSamples(float freq, int amplitudeCount, float* amplitudes, float durationSecs) {
        int numSamples = static_cast<int>(
            2000 * FromXrTime(durationSecs)); // samples consumed per sec * duration in secs

        // Initialize result
        std::vector<float> result(numSamples);

        // Init data for sample loop
        float srcSample = 0;
        float srcStep = static_cast<float>(amplitudeCount) / numSamples;
        float currentCycle = 0.0;
        float dt = 1.0 / 2000;

        // Compute samples
        for (int i = 0; i < numSamples; i++) {
            float intPart; // trash
            const float cycleTime = std::modf(currentCycle, &intPart);
            int srcIdx = static_cast<int>(srcSample);
            float srcAmplitude = amplitudes[srcIdx];

            // Compute sample value
            float base = std::sin(cycleTime * M_PI * 2.0);
            float sample = base * srcAmplitude;
            result[i] = sample;

            // Step
            currentCycle += freq * dt;
            srcSample += srcStep;
        }
        return result;
    }

    bool SupportsParametricHaptics() {
        XrSystemHapticParametricPropertiesEXTX1 systemHapticParametricProperties{XR_TYPE_SYSTEM_HAPTIC_PARAMETRIC_PROPERTIES_EXTX1};
        XrSystemProperties systemProperties = {XR_TYPE_SYSTEM_PROPERTIES, &systemHapticParametricProperties};
        OXR(xrGetSystemProperties(Instance, GetSystemId(), &systemProperties));
        if (systemHapticParametricProperties.supportsParametricHaptics == XR_FALSE) {
            ALOGE("System does not support parametric haptics");
            return false;
        }
        return true;
    }

    // Returns a list of OpenXr extensions needed for this app
    virtual std::vector<const char*> GetExtensions() override {
        std::vector<const char*> extensions = XrApp::GetExtensions();
        extensions.push_back(XR_FB_HAPTIC_AMPLITUDE_ENVELOPE_EXTENSION_NAME);
        extensions.push_back(XR_FB_HAPTIC_PCM_EXTENSION_NAME);
        extensions.push_back(XR_EXTX1_HAPTIC_PARAMETRIC_EXTENSION_NAME);
                return extensions;
    }

    // Returns a map from interaction profile paths to vectors of suggested bindings.
    // xrSuggestInteractionProfileBindings() is called once for each interaction profile path in the
    // returned map.
    // Apps are encouraged to suggest bindings for every device/interaction profile they support.
    // Overridden to add support for the touch_pro interaction profile
    std::unordered_map<XrPath, std::vector<XrActionSuggestedBinding>> GetSuggestedBindings(
        XrInstance instance) override {
        //    …/input/trackpad/x
        //    …/input/trackpad/y
        //    …/input/trackpad/force
        //    …/input/stylus/force
        //    …/input/trigger/curl
        //    …/input/trigger/slide
        //    …/output/trigger_haptic
        //    …/output/thumb_haptic

        XrPath handSubactionPaths[2] = {LeftHandPath, RightHandPath};

        trackpadForceAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_FLOAT_INPUT,
            "the_trackpad_force",
            nullptr,
            2,
            handSubactionPaths);
        triggerForceAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_FLOAT_INPUT,
            "trigger_force",
            nullptr,
            2,
            handSubactionPaths);
        stylusForceAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_FLOAT_INPUT,
            "the_stylus_force",
            nullptr,
            2,
            handSubactionPaths);
        triggerCurlAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_FLOAT_INPUT,
            "the_trigger_curl",
            nullptr,
            2,
            handSubactionPaths);
        triggerSlideAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_FLOAT_INPUT,
            "the_trigger_slide",
            nullptr,
            2,
            handSubactionPaths);

        /// haptics
        mainHapticAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_VIBRATION_OUTPUT,
            "the_main_haptic",
            nullptr,
            2,
            handSubactionPaths);
        triggerHapticAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_VIBRATION_OUTPUT,
            "the_trigger_haptic",
            nullptr,
            2,
            handSubactionPaths);
        thumbHapticAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_VIBRATION_OUTPUT,
            "the_thumb_haptic",
            nullptr,
            2,
            handSubactionPaths);

        // Proximity
        triggerProxAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "trigger_prox",
            nullptr,
            2,
            handSubactionPaths);
        thumbFbProxAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "thumb_fb_prox",
            nullptr,
            2,
            handSubactionPaths);
        thumbMetaProxAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "thumb_meta_prox",
            nullptr,
            2,
            handSubactionPaths);

        // Trigger Value
        triggerValueAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "trigger_value",
            nullptr,
            2,
            handSubactionPaths);

        // Trigger Touch
        triggerTouchAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "trigger_touch",
            nullptr,
            2,
            handSubactionPaths);

        // Squeeze Value
        squeezeValueAction_ = CreateAction(
            BaseActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "squeeze_value",
            nullptr,
            2,
            handSubactionPaths);

        XrPath touchInteractionProfile = XR_NULL_PATH;
        OXR(xrStringToPath(
            instance, "/interaction_profiles/meta/touch_controller_quest_2", &touchInteractionProfile));

        XrPath touchProInteractionProfile = XR_NULL_PATH;
        OXR(xrStringToPath(
            instance,
            "/interaction_profiles/meta/touch_pro_controller",
            &touchProInteractionProfile));
        XrPath touchPlusInteractionProfile = XR_NULL_PATH;
        OXR(xrStringToPath(
            instance,
            "/interaction_profiles/meta/touch_plus_controller",
            &touchPlusInteractionProfile));

        std::vector<XrActionSuggestedBinding> baseTouchBindings{};

        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(AimPoseAction, "/user/hand/left/input/aim/pose"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(AimPoseAction, "/user/hand/right/input/aim/pose"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(GripPoseAction, "/user/hand/left/input/grip/pose"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(GripPoseAction, "/user/hand/right/input/grip/pose"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(JoystickAction, "/user/hand/left/input/thumbstick"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(JoystickAction, "/user/hand/right/input/thumbstick"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(thumbstickClickAction, "/user/hand/left/input/thumbstick/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(thumbstickClickAction, "/user/hand/right/input/thumbstick/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(IndexTriggerAction, "/user/hand/left/input/trigger/value"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(IndexTriggerAction, "/user/hand/right/input/trigger/value"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(GripTriggerAction, "/user/hand/left/input/squeeze/value"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(GripTriggerAction, "/user/hand/right/input/squeeze/value"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ButtonAAction, "/user/hand/right/input/a/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ButtonBAction, "/user/hand/right/input/b/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ButtonXAction, "/user/hand/left/input/x/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ButtonYAction, "/user/hand/left/input/y/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ButtonMenuAction, "/user/hand/left/input/menu/click"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ThumbStickTouchAction, "/user/hand/left/input/thumbstick/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ThumbStickTouchAction, "/user/hand/right/input/thumbstick/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ThumbRestTouchAction, "/user/hand/left/input/thumbrest/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(ThumbRestTouchAction, "/user/hand/right/input/thumbrest/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(TriggerTouchAction, "/user/hand/left/input/trigger/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(TriggerTouchAction, "/user/hand/right/input/trigger/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(mainHapticAction_, "/user/hand/left/output/haptic"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(mainHapticAction_, "/user/hand/right/output/haptic"));

        // Thumb rest proximity
        baseTouchBindings.emplace_back(ActionSuggestedBinding(
            thumbFbProxAction_, "/user/hand/left/input/thumb_resting_surfaces/proximity"));
        baseTouchBindings.emplace_back(ActionSuggestedBinding(
            thumbFbProxAction_, "/user/hand/right/input/thumb_resting_surfaces/proximity"));

        // Trigger Value
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(triggerValueAction_, "/user/hand/left/input/trigger/value"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(triggerValueAction_, "/user/hand/right/input/trigger/value"));

        // Trigger Touch
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(triggerTouchAction_, "/user/hand/left/input/trigger/touch"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(triggerTouchAction_, "/user/hand/right/input/trigger/touch"));

        // Trigger Proximity
        baseTouchBindings.emplace_back(ActionSuggestedBinding(
            triggerProxAction_, "/user/hand/left/input/trigger/proximity"));
        baseTouchBindings.emplace_back(ActionSuggestedBinding(
            triggerProxAction_, "/user/hand/right/input/trigger/proximity"));

        // Squeeze Value
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(squeezeValueAction_, "/user/hand/left/input/squeeze/value"));
        baseTouchBindings.emplace_back(
            ActionSuggestedBinding(squeezeValueAction_, "/user/hand/right/input/squeeze/value"));

        // Copy(construct) base paths since these interaction profiles are similar
        // NOTE: Only bindings that are different from touch controller will be listed here.
        // Bindings that also exist on touch controller will be copied from baseTouchBindings
        std::vector<XrActionSuggestedBinding> touchProBindings(baseTouchBindings);

        // We are assuming that every touch binding exists for touch pro here
        // if that is not the case they need to be removed from the vector

        touchProBindings.emplace_back(
            ActionSuggestedBinding(trackpadForceAction_, "/user/hand/left/input/thumbrest/force"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(trackpadForceAction_, "/user/hand/right/input/thumbrest/force"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(stylusForceAction_, "/user/hand/left/input/stylus/force"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(stylusForceAction_, "/user/hand/right/input/stylus/force"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(triggerCurlAction_, "/user/hand/left/input/trigger_curl/value"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(triggerCurlAction_, "/user/hand/right/input/trigger_curl/value"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(triggerSlideAction_, "/user/hand/left/input/trigger_slide/value"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(triggerSlideAction_, "/user/hand/right/input/trigger_slide/value"));
        touchProBindings.emplace_back(ActionSuggestedBinding(
            triggerHapticAction_, "/user/hand/left/output/haptic_trigger"));
        touchProBindings.emplace_back(ActionSuggestedBinding(
            triggerHapticAction_, "/user/hand/right/output/haptic_trigger"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(thumbHapticAction_, "/user/hand/left/output/haptic_thumb"));
        touchProBindings.emplace_back(
            ActionSuggestedBinding(thumbHapticAction_, "/user/hand/right/output/haptic_thumb"));

        // Copy(construct) base paths since these interaction profiles are similar
        // NOTE: Only bindings that are different from touch controller will be listed here.
        // Bindings that also exist on touch controller will be copied from baseTouchBindings
        std::vector<XrActionSuggestedBinding> touchPlusBindings(baseTouchBindings);
        touchPlusBindings.emplace_back(
            ActionSuggestedBinding(triggerForceAction_, "/user/hand/left/input/trigger/force"));
        touchPlusBindings.emplace_back(
            ActionSuggestedBinding(triggerForceAction_, "/user/hand/right/input/trigger/force"));
        touchPlusBindings.emplace_back(
            ActionSuggestedBinding(triggerCurlAction_, "/user/hand/left/input/trigger_curl/value"));
        touchPlusBindings.emplace_back(
            ActionSuggestedBinding(triggerCurlAction_, "/user/hand/right/input/trigger_curl/value"));
        touchPlusBindings.emplace_back(ActionSuggestedBinding(
            triggerSlideAction_, "/user/hand/left/input/trigger_slide/value"));
        touchPlusBindings.emplace_back(ActionSuggestedBinding(
            triggerSlideAction_, "/user/hand/right/input/trigger_slide/value"));

        std::unordered_map<XrPath, std::vector<XrActionSuggestedBinding>> allSuggestedBindings;
        allSuggestedBindings[touchInteractionProfile] = baseTouchBindings;
        allSuggestedBindings[touchProInteractionProfile] = touchProBindings;
        allSuggestedBindings[touchPlusInteractionProfile] = touchPlusBindings;
        return allSuggestedBindings;
    }

    // Must return true if the application initializes successfully.
    virtual bool AppInit(const xrJava* context) override {
        if (false == ui_.Init(context, GetFileSys())) {
            ALOG("TinyUI::Init FAILED.");
            return false;
        }

        /// Hook up extensions

        /// Build UI
        bigText_ =
            ui_.AddLabel("OpenXR Controllers Sample", {0.0f, -0.8f, -1.9f}, {1300.0f, 100.0f});

        OVR::Vector2f size = {200.0f, 100.0f};
        OVR::Vector3f position = {+0.0f, 0.5f, -1.9f};
        OVR::Vector3f positionL = {-0.4f, 0.5f, -1.9f};
        OVR::Vector3f positionR = {+0.4f, 0.5f, -1.9f};
        const float dh = 0.2f;
        position.y += dh;
        positionL.y += dh;
        positionR.y += dh;
        ui_.AddLabel("Trigger Force", position, size);
        triggerForceLText_ = ui_.AddLabel("trf L 0.0", positionL, size);
        triggerForceRText_ = ui_.AddLabel("trf R 0.0", positionR, size);
        position.y += dh;
        positionL.y += dh;
        positionR.y += dh;
        ui_.AddLabel("Track Force", position, size);
        trackpadForceLText_ = ui_.AddLabel("tf L 0.0", positionL, size);
        trackpadForceRText_ = ui_.AddLabel("tf R 0.0", positionR, size);
        position.y += dh;
        positionL.y += dh;
        positionR.y += dh;
        ui_.AddLabel("Stylus Force", position, size);
        stylusForceLText_ = ui_.AddLabel("tf L 0.0", positionL, size);
        stylusForceRText_ = ui_.AddLabel("tf R 0.0", positionR, size);
        position.y += dh;
        positionL.y += dh;
        positionR.y += dh;
        ui_.AddLabel("Trigger Curl", position, size);
        triggerCurlLText_ = ui_.AddLabel("tf L 0.0", positionL, size);
        triggerCurlRText_ = ui_.AddLabel("tf R 0.0", positionR, size);
        position.y += dh;
        positionL.y += dh;
        positionR.y += dh;
        ui_.AddLabel("Trigger Slide", position, size);
        squeezeCurlLText_ = ui_.AddLabel("tf L 0.0", positionL, size);
        squeezeCurlRText_ = ui_.AddLabel("tf R 0.0", positionR, size);
        // Proximity
        positionL.y += dh;
        positionR.y += dh;
        position.y += dh;
        ui_.AddLabel("Trigger Prox", position, size);
        triggerProxLText_ = ui_.AddLabel("trProx L 0.0", positionL, size);
        triggerProxRText_ = ui_.AddLabel("trProx R 0.0", positionR, size);

        positionL.y += dh * 3 / 4.0f;
        positionR.y += dh * 3 / 4.0f;
        position.y += dh;
        ui_.AddLabel("Thumb Prox", position, size);
        const OVR::Vector2f halfSize{size.x, size.y / 2.0f};
        thumbFBProxLText_ = ui_.AddLabel("_FB: 0", positionL, halfSize);
        thumbFBProxRText_ = ui_.AddLabel("_FB: 0", positionR, halfSize);
        positionL.y += dh / 2.0;
        positionR.y += dh / 2.0;
        thumbMetaProxLText_ = ui_.AddLabel("_META: 0", positionL, halfSize);
        thumbMetaProxRText_ = ui_.AddLabel("_META: 0", positionR, halfSize);

        positionL.y += dh * 3 / 4.0f;
        positionR.y += dh * 3 / 4.0f;
        position.y += dh;
        ui_.AddLabel("Trigger Value", position, size);
        triggerValueLText_ = ui_.AddLabel("trVal L 0.0", positionL, size);
        triggerValueRText_ = ui_.AddLabel("trVal R 0.0", positionR, size);

        positionL.y += dh;
        positionR.y += dh;
        position.y += dh;
        ui_.AddLabel("Trigger Touch", position, size);
        triggerTouchLText_ = ui_.AddLabel("trTouch L 0.0", positionL, size);
        triggerTouchRText_ = ui_.AddLabel("trTouch R 0.0", positionR, size);

        positionL.y += dh;
        positionR.y += dh;
        position.y += dh;
        ui_.AddLabel("Squeeze Value", position, size);
        squeezeValueLText_ = ui_.AddLabel("sqVal L 0.0", positionL, size);
        squeezeValueRText_ = ui_.AddLabel("sqVal R 0.0", positionR, size);

        ipText_ = ui_.AddLabel("Interaction Profiles", {0.0f, 0.5f, -1.9f}, {600.0f, 100.0f});

        ui_.AddButton("Haptic Main S", {-0.8f, 0.5f, -1.9f}, size, [this]() {
            VibrateController(
                mainHapticAction_, LeftHandPath, XR_MIN_HAPTIC_DURATION, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Main S", {+0.8f, 0.5f, -1.9f}, size, [this]() {
            VibrateController(
                mainHapticAction_, RightHandPath, XR_MIN_HAPTIC_DURATION, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Main M", {-1.2f, 0.5f, -1.9f}, size, [this]() {
            VibrateController(mainHapticAction_, LeftHandPath, 0.1f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Main M", {+1.2f, 0.5f, -1.9f}, size, [this]() {
            VibrateController(mainHapticAction_, RightHandPath, 0.1f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Main L", {-1.6f, 0.5f, -1.9f}, size, [this]() {
            VibrateController(mainHapticAction_, LeftHandPath, 1.0f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Main L", {+1.6f, 0.5f, -1.9f}, size, [this]() {
            VibrateController(mainHapticAction_, RightHandPath, 1.0f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Trigger", {-0.8f, 0.7f, -1.9f}, size, [this]() {
            VibrateController(triggerHapticAction_, LeftHandPath, 0.1f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Trigger", {+0.8f, 0.7f, -1.9f}, size, [this]() {
            VibrateController(triggerHapticAction_, RightHandPath, 0.1f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Thumb", {-0.8f, 0.9f, -1.9f}, size, [this]() {
            VibrateController(thumbHapticAction_, LeftHandPath, 0.1f, 157.0f, 1.0f);
        });
        ui_.AddButton("Haptic Thumb", {+0.8f, 0.9f, -1.9f}, size, [this]() {
            VibrateController(thumbHapticAction_, RightHandPath, 0.1f, 157.0f, 1.0f);
        });
        position.y += dh;
        ui_.AddToggleButton("Lag On", "Lag Off", &delayUI_, position, size);

        // Left Hand
        const float sampleDurationBuffered = 0.002f; // 2ms
        position = {-1.2f, 0.7f, -1.9f};
        ui_.AddButton("AE Scroll", position, size, [this, sampleDurationBuffered]() {
            VibrateControllerAmplitude(
                mainHapticAction_,
                LeftHandPath,
                kScrollBuffer,
                std::size(kScrollBuffer),
                sampleDurationBuffered * std::size(kScrollBuffer));
        });
        position.x -= 0.4f;
        float aeBufferSimple[500]; // 1sec
        for (int i = 0; i < 500; i++) {
            aeBufferSimple[i] = 0.1;
        }
        ui_.AddButton("AE 0.5s\n(Downsample)", position, size, [this, aeBufferSimple]() {
            VibrateControllerAmplitude(
                mainHapticAction_, LeftHandPath, aeBufferSimple, std::size(aeBufferSimple), 0.5f);
        });

        position.x -= 0.4f;
        ui_.AddButton("Parametric", position, size, [this] {
            VibrateControllerParametric(mainHapticAction_, LeftHandPath);
        });

        // Right Hand
        position = {+1.2f, 0.7f, -1.9f};
        ui_.AddButton("AE Scroll", position, size, [this, sampleDurationBuffered]() {
            VibrateControllerAmplitude(
                mainHapticAction_,
                RightHandPath,
                kScrollBuffer,
                std::size(kScrollBuffer),
                sampleDurationBuffered * std::size(kScrollBuffer));
        });
        position.x += 0.4f;
        float aeBufferSingle[2] = {1, 0.5f};
        ui_.AddButton("AE 1s (Upsample)", position, size, [this, aeBufferSingle]() {
            VibrateControllerAmplitude(
                mainHapticAction_, RightHandPath, aeBufferSingle, std::size(aeBufferSingle), 1.0f);
        });

        position.x += 0.4f;
        ui_.AddButton("Parametric", position, size, [this] {
            VibrateControllerParametric(mainHapticAction_, RightHandPath);
        });

        position = {+0.0f, 0.5f, -1.9f};
        position.y -= dh;
        pcmHapticText_ = ui_.AddLabel("PCM Haptic\n[SR: 0.0]", position, size);

        float pcmBufferSimple[5000];
        for (int i = 0; i < 5000; i++) {
            if (i < 2500) {
                pcmBufferSimple[i] = 0.8f;
            } else {
                pcmBufferSimple[i] = -0.8f;
            }
        }
        // Right Controller
        position.x += 0.4f;
        float sampleRate = 2000.0f;
        std::vector<float> decayingSineWave =
            createPCMSamples(40, std::size(reducingIntensity), reducingIntensity, ToXrTime(2));

        ui_.AddButton(
            "Decaying sine\nwave", position, size, [this, decayingSineWave, sampleRate]() {
                VibrateControllerPCM(
                    mainHapticAction_,
                    RightHandPath,
                    decayingSineWave.data(),
                    decayingSineWave.size(),
                    sampleRate);
            });

        auto copyReducingSineWave =
            createPCMSamples(40, std::size(reducingIntensity), reducingIntensity, ToXrTime(1));
        auto copyIncreasingSineWave =
            createPCMSamples(40, std::size(increasingIntensity), increasingIntensity, ToXrTime(1));

        std::vector<float> decayingSineWaveLong;
        for (int i = 0; i < 5; ++i) {
            decayingSineWaveLong.insert(
                decayingSineWaveLong.end(),
                copyReducingSineWave.begin(),
                copyReducingSineWave.end());
            decayingSineWaveLong.insert(
                decayingSineWaveLong.end(),
                copyIncreasingSineWave.begin(),
                copyIncreasingSineWave.end());
        }

        position.x += 0.4f;
        ui_.AddButton(
            "Long wave (10s)", position, size, [this, decayingSineWaveLong, sampleRate]() {
                VibrateControllerPCM(
                    mainHapticAction_,
                    RightHandPath,
                    decayingSineWaveLong.data(),
                    decayingSineWaveLong.size(),
                    sampleRate);
            });

        std::vector<float> sineWave =
            createPCMSamples(157, std::size(constantIntensity), constantIntensity, ToXrTime(1));

        position.x += 0.4f;
        ui_.AddButton("Wave 1s", position, size, [this, sineWave, sampleRate]() {
            VibrateControllerPCM(
                mainHapticAction_, RightHandPath, sineWave.data(), sineWave.size(), sampleRate);
        });

        position.x += 0.4f;
        sampleRate = 1000.0f;
        ui_.AddButton("Upsampled Wave\n2s", position, size, [this, sineWave, sampleRate]() {
            VibrateControllerPCM(
                mainHapticAction_, RightHandPath, sineWave.data(), sineWave.size(), sampleRate);
        });

        position.x += 0.4f;
        sampleRate = 4000.0f;
        ui_.AddButton("Downsampled\nWave\n0.5s", position, size, [this, sineWave, sampleRate]() {
            VibrateControllerPCM(
                mainHapticAction_, RightHandPath, sineWave.data(), sineWave.size(), sampleRate);
        });

        // Left Controller
        position.x -= 2.4f;
        sampleRate = 2000.0f;

        ui_.AddButton(
            "Decaying sine\nwave 1s", position, size, [this, decayingSineWave, sampleRate]() {
                VibrateControllerPCM(
                    mainHapticAction_,
                    LeftHandPath,
                    decayingSineWave.data(),
                    decayingSineWave.size(),
                    sampleRate);
            });

        position.x -= 0.4f;
        ui_.AddButton(
            "Long wave (10s)", position, size, [this, decayingSineWaveLong, sampleRate]() {
                VibrateControllerPCM(
                    mainHapticAction_,
                    LeftHandPath,
                    decayingSineWaveLong.data(),
                    decayingSineWaveLong.size(),
                    sampleRate);
            });

        position.x -= 0.4f;
        // 2s Sine wave
        float singleIntensity[]{1};
        sineWave = createPCMSamples(157, std::size(singleIntensity), singleIntensity, ToXrTime(2));

        ui_.AddButton("Wave 2s", position, size, [this, sineWave, sampleRate]() {
            VibrateControllerPCM(
                mainHapticAction_, LeftHandPath, sineWave.data(), sineWave.size(), sampleRate);
        });

        position.x -= 0.4f;
        sampleRate = 1500.0f;
        ui_.AddButton("Upsampled Wave\n2.67s", position, size, [this, sineWave, sampleRate]() {
            VibrateControllerPCM(
                mainHapticAction_, LeftHandPath, sineWave.data(), sineWave.size(), sampleRate);
        });

        position.x -= 0.4f;
        sampleRate = 3000.0f;
        ui_.AddButton("Downsampled\nWave\n1.3s", position, size, [this, sineWave, sampleRate]() {
            VibrateControllerPCM(
                mainHapticAction_, LeftHandPath, sineWave.data(), sineWave.size(), sampleRate);
        });

        // Playing haptics on both controllers by passing XR_NULL_PATH in subActionPath
        position = {+0.0f, +0.1f, -1.9f};
        ui_.AddButton("Haptic Main (both)", position, size, [this]() {
            VibrateController(mainHapticAction_, XR_NULL_PATH, 1.0f, 157.0f, 0.5f);
        });
        OXR(xrGetInstanceProcAddr(
            GetInstance(),
            "xrGetDeviceSampleRateFB",
            (PFN_xrVoidFunction*)(&xrGetDeviceSampleRateFB)));

        // both triggers
        position.x -= 0.4f;
        ui_.AddButton("Thumb (2s, both)", position, size, [this]() {
            VibrateController(thumbHapticAction_, XR_NULL_PATH, 2.0f, 157.0f, 0.25f);
        });
        // both grips
        position.x -= 0.4f;
        ui_.AddButton("Trigger (2s, both)", position, size, [this]() {
            VibrateController(triggerHapticAction_, XR_NULL_PATH, 2.0f, 157.0f, 0.25f);
        });
        position = {+0.0f, 0.1f, -1.9f};
        position.x += 0.4f;
        ui_.AddButton("Thumb (2s, right)", position, size, [this]() {
            VibrateController(thumbHapticAction_, RightHandPath, 2.0f, 157.0f, 0.25f);
        });
        // both grips
        position.x += 0.4f;
        ui_.AddButton("Trigger (2s, right)", position, size, [this]() {
            VibrateController(triggerHapticAction_, RightHandPath, 2.0f, 157.0f, 0.25f);
        });

        position = {+0.0f, -0.1f, -1.9f};
        ui_.AddButton("Stop BOTH Main", position, size, [this]() {
            StopHapticEffect(mainHapticAction_, XR_NULL_PATH);
        });

        position.x -= 0.4f;
        ui_.AddButton("Stop Left Main", position, size, [this]() {
            StopHapticEffect(mainHapticAction_, LeftHandPath);
        });

        position.x += 2 * 0.4f;
        ui_.AddButton("Stop Right Main", position, size, [this]() {
            StopHapticEffect(mainHapticAction_, RightHandPath);
        });

        position = {+0.0f, -0.3f, -1.9f};
        ui_.AddButton("Stop BOTH Thumb", position, size, [this]() {
            StopHapticEffect(thumbHapticAction_, XR_NULL_PATH);
        });

        position.x -= 0.4f;
        ui_.AddButton("Stop Left Thumb", position, size, [this]() {
            StopHapticEffect(thumbHapticAction_, LeftHandPath);
        });

        position.x += 2 * 0.4f;
        ui_.AddButton("Stop Right Thumb", position, size, [this]() {
            StopHapticEffect(thumbHapticAction_, RightHandPath);
        });

        position = {+0.0f, -0.5f, -1.9f};
        ui_.AddButton("Stop BOTH Trigger", position, size, [this]() {
            StopHapticEffect(triggerHapticAction_, XR_NULL_PATH);
        });
        
        position.x -= 0.4f;
        ui_.AddButton("Stop Left Trigger", position, size, [this]() {
            StopHapticEffect(triggerHapticAction_, LeftHandPath);
        });

        position.x += 2 * 0.4f;
        ui_.AddButton("Stop Right Trigger", position, size, [this]() {
            StopHapticEffect(triggerHapticAction_, RightHandPath);
        });

        return true;
    }

    virtual void AppShutdown(const xrJava* context) override {
        /// unhook extensions

        OVRFW::XrApp::AppShutdown(context);
        ui_.Shutdown();
    }

    virtual bool SessionInit() override {
        /// Use LocalSpace instead of Stage Space.
        CurrentSpace = LocalSpace;
        /// Init session bound objects
        if (false == controllerRenderL_.Init(true)) {
            ALOG("AppInit::Init L controller renderer FAILED.");
            return false;
        }
        if (false == controllerRenderR_.Init(false)) {
            ALOG("AppInit::Init R controller renderer FAILED.");
            return false;
        }
        beamRenderer_.Init(GetFileSys(), nullptr, OVR::Vector4f(1.0f), 1.0f);

        /// enumerate all actions
        EnumerateActions();

        return true;
    }

    virtual void SessionEnd() override {
        controllerRenderL_.Shutdown();
        controllerRenderR_.Shutdown();
        beamRenderer_.Shutdown();
    }

    // Update state
    virtual void Update(const OVRFW::ovrApplFrameIn& in) override {
        /// Update Input
        {
            /// Trigger Force
            triggerForceL_ = GetActionStateFloat(triggerForceAction_, LeftHandPath).currentState;
            triggerForceR_ = GetActionStateFloat(triggerForceAction_, RightHandPath).currentState;
            /// TrackPad Force
            trackpadForceL_ = GetActionStateFloat(trackpadForceAction_, LeftHandPath).currentState;
            trackpadForceR_ = GetActionStateFloat(trackpadForceAction_, RightHandPath).currentState;
            /// Stylus Force
            stylusForceL_ = GetActionStateFloat(stylusForceAction_, LeftHandPath).currentState;
            stylusForceR_ = GetActionStateFloat(stylusForceAction_, RightHandPath).currentState;
            /// Trigger Curl
            triggerCurlL_ = GetActionStateFloat(triggerCurlAction_, LeftHandPath).currentState;
            triggerCurlR_ = GetActionStateFloat(triggerCurlAction_, RightHandPath).currentState;
            /// Squeeze Curl
            squeezeCurlL_ = GetActionStateFloat(triggerSlideAction_, LeftHandPath).currentState;
            squeezeCurlR_ = GetActionStateFloat(triggerSlideAction_, RightHandPath).currentState;
            /// Proximity
            triggerProxL_ = GetActionStateBoolean(triggerProxAction_, LeftHandPath).currentState;
            triggerProxR_ = GetActionStateBoolean(triggerProxAction_, RightHandPath).currentState;
            thumbFBProxL_ = GetActionStateBoolean(thumbFbProxAction_, LeftHandPath).currentState;
            thumbFBProxR_ = GetActionStateBoolean(thumbFbProxAction_, RightHandPath).currentState;
            // same as above on touch plus
            thumbMetaProxL_ =
                GetActionStateBoolean(thumbMetaProxAction_, LeftHandPath).currentState;
            thumbMetaProxR_ =
                GetActionStateBoolean(thumbMetaProxAction_, RightHandPath).currentState;
            /// Trigger Value
            triggerValueL_ = GetActionStateBoolean(triggerValueAction_, LeftHandPath).currentState;
            triggerValueR_ = GetActionStateBoolean(triggerValueAction_, RightHandPath).currentState;

            /// Trigger Touch
            triggerTouchL_ = GetActionStateBoolean(triggerTouchAction_, LeftHandPath).currentState;
            triggerTouchR_ = GetActionStateBoolean(triggerTouchAction_, RightHandPath).currentState;
            /// Squeeze Value
            squeezeValueL_ = GetActionStateBoolean(squeezeValueAction_, LeftHandPath).currentState;
            squeezeValueR_ = GetActionStateBoolean(squeezeValueAction_, RightHandPath).currentState;
        }

        // we can only request haptic sample rate when the session is in focus
        if (Focused) {
            XrHapticActionInfo hai = {XR_TYPE_HAPTIC_ACTION_INFO, nullptr};
            hai.action = mainHapticAction_;
            hai.subactionPath = LeftHandPath;
            OXR(xrGetDeviceSampleRateFB(Session, &hai, &leftDeviceSampleRate_));

            hai.action = mainHapticAction_;
            hai.subactionPath = RightHandPath;
            OXR(xrGetDeviceSampleRateFB(Session, &hai, &rightDeviceSampleRate_));
        }

        // once per A button press
        const auto buttonA = GetActionStateBoolean(ButtonAAction);
        if (buttonA.currentState == XR_TRUE && buttonA.changedSinceLastSync == XR_TRUE) {
            // Trigger PCM haptics: simple sine wave
            std::vector<float> sineWave =
                createPCMSamples(157, std::size(constantIntensity), constantIntensity, ToXrTime(1));
            VibrateControllerPCM(
                mainHapticAction_, RightHandPath, sineWave.data(), sineWave.size(), 2000.0f);
        }

        // once per B button press
        const auto buttonB = GetActionStateBoolean(ButtonBAction);
        if (buttonB.currentState == XR_TRUE && buttonB.changedSinceLastSync == XR_TRUE) {
            // Trigger AE haptics
            float aeBufferSimple[500]; // 1sec
            for (int i = 0; i < 500; i++) {
                aeBufferSimple[i] = 0.1;
            }

            VibrateControllerAmplitude(
                mainHapticAction_,
                RightHandPath,
                aeBufferSimple,
                std::size(aeBufferSimple),
                0.002f * std::size(aeBufferSimple));
        }

        // once per X button press
        const auto buttonX = GetActionStateBoolean(ButtonXAction);
        if (buttonX.currentState == XR_TRUE && buttonX.changedSinceLastSync == XR_TRUE) {
            // Trigger Localized(thumb) haptics
            VibrateController(thumbHapticAction_, LeftHandPath, 0.1f, 157.0f, 1.0f);
        }

        // once per Y button press
        const auto buttonY = GetActionStateBoolean(ButtonYAction);
        if (buttonY.currentState == XR_TRUE && buttonY.changedSinceLastSync == XR_TRUE) {
            // Trigger Localized(trigger) haptics
            VibrateController(triggerHapticAction_, LeftHandPath, 0.1f, 157.0f, 1.0f);
        }

        ui_.HitTestDevices().clear();

        if (in.LeftRemoteTracked) {
            controllerRenderL_.Update(in.LeftRemotePose);
            const bool didPinch = in.LeftRemoteIndexTrigger > 0.25f;
            ui_.AddHitTestRay(in.LeftRemotePointPose, didPinch);
        }
        if (in.RightRemoteTracked) {
            controllerRenderR_.Update(in.RightRemotePose);
            const bool didPinch = in.RightRemoteIndexTrigger > 0.25f;
            ui_.AddHitTestRay(in.RightRemotePointPose, didPinch);
        }

        /// Update labels
        {
            XrInteractionProfileState lIpState{XR_TYPE_INTERACTION_PROFILE_STATE};
            OXR(xrGetCurrentInteractionProfile(Session, LeftHandPath, &lIpState));
            XrInteractionProfileState rIpState{XR_TYPE_INTERACTION_PROFILE_STATE};
            OXR(xrGetCurrentInteractionProfile(Session, LeftHandPath, &rIpState));

            char lBuf[XR_MAX_PATH_LENGTH];
            uint32_t written = 0;
            if (lIpState.interactionProfile != XR_NULL_PATH) {
                OXR(xrPathToString(
                    Instance, lIpState.interactionProfile, XR_MAX_PATH_LENGTH, &written, lBuf));
            }
            if (written == 0) {
                strcpy(lBuf, "<none>");
            }

            char rBuf[XR_MAX_PATH_LENGTH];
            written = 0;
            if (rIpState.interactionProfile != XR_NULL_PATH) {
                OXR(xrPathToString(
                    Instance, rIpState.interactionProfile, XR_MAX_PATH_LENGTH, &written, rBuf));
            }
            if (written == 0) {
                strcpy(rBuf, "<none>");
            }

            std::stringstream ss;
            ss << "Left IP: " << lBuf << std::endl;
            ss << "Right IP: " << rBuf;
            ipText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerForceL_;
            triggerForceLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerForceR_;
            triggerForceRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << trackpadForceL_;
            trackpadForceLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << trackpadForceR_;
            trackpadForceRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << stylusForceL_;
            stylusForceLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << stylusForceR_;
            stylusForceRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerCurlL_;
            triggerCurlLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerCurlR_;
            triggerCurlRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << squeezeCurlL_;
            squeezeCurlLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << squeezeCurlR_;
            squeezeCurlRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << "PCM Haptic\n[SR: ";
            ss << std::setprecision(1) << std::fixed;
            ss << leftDeviceSampleRate_.sampleRate << ", ";
            ss << rightDeviceSampleRate_.sampleRate << "]";
            pcmHapticText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerProxL_;
            triggerProxLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerProxR_;
            triggerProxRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << "_FB: " << thumbFBProxL_;
            thumbFBProxLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << "_FB: " << thumbFBProxR_;
            thumbFBProxRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << "_META: " << thumbMetaProxL_;
            thumbMetaProxLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << "_META: " << thumbMetaProxR_;
            thumbMetaProxRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerValueL_;
            triggerValueLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerValueR_;
            triggerValueRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerTouchL_;
            triggerTouchLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << triggerTouchR_;
            triggerTouchRText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << squeezeValueL_;
            squeezeValueLText_->SetText(ss.str().c_str());
        }
        {
            std::stringstream ss;
            ss << std::setprecision(4) << std::fixed;
            ss << squeezeValueR_;
            squeezeValueRText_->SetText(ss.str().c_str());
        }

        /*
         */

        ui_.Update(in);
        beamRenderer_.Update(in, ui_.HitTestDevices());

        /// Add some deliberate lag to the app
        if (delayUI_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    }

    // Render eye buffers while running
    virtual void Render(const OVRFW::ovrApplFrameIn& in, OVRFW::ovrRendererOutput& out) override {
        /// Render UI
        ui_.Render(in, out);
        /// Render controllers
        if (in.LeftRemoteTracked) {
            controllerRenderL_.Render(out.Surfaces);
        }
        if (in.RightRemoteTracked) {
            controllerRenderR_.Render(out.Surfaces);
        }

        /// Render beams
        beamRenderer_.Render(in, out);
    }

    void EnumerateActions() {
        // Enumerate actions
        XrPath actionPathsBuffer[16];
        char stringBuffer[256];
        XrAction actionsToEnumerate[] = {
            /// new actions
            triggerForceAction_,
            thumbMetaProxAction_,
            trackpadForceAction_,
            stylusForceAction_,
            triggerCurlAction_,
            triggerSlideAction_,
            /// existing actions form base class
            IndexTriggerAction,
            GripTriggerAction,
            triggerProxAction_,
            thumbFbProxAction_,
            triggerValueAction_,
            triggerTouchAction_,
            squeezeValueAction_,
        };
        for (size_t i = 0; i < sizeof(actionsToEnumerate) / sizeof(actionsToEnumerate[0]); ++i) {
            XrBoundSourcesForActionEnumerateInfo enumerateInfo = {
                XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
            enumerateInfo.action = actionsToEnumerate[i];

            // Get Count
            uint32_t countOutput = 0;
            OXR(xrEnumerateBoundSourcesForAction(
                Session, &enumerateInfo, 0 /* request size */, &countOutput, nullptr));
            ALOGV(
                "xrEnumerateBoundSourcesForAction action=%lld count=%u",
                (long long)enumerateInfo.action,
                countOutput);

            if (countOutput < 16) {
                OXR(xrEnumerateBoundSourcesForAction(
                    Session, &enumerateInfo, 16, &countOutput, actionPathsBuffer));
                for (uint32_t a = 0; a < countOutput; ++a) {
                    XrInputSourceLocalizedNameGetInfo nameGetInfo = {
                        XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};
                    nameGetInfo.sourcePath = actionPathsBuffer[a];
                    nameGetInfo.whichComponents = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                        XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                        XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

                    uint32_t stringCount = 0u;
                    OXR(xrGetInputSourceLocalizedName(
                        Session, &nameGetInfo, 0, &stringCount, nullptr));
                    if (stringCount < 256) {
                        OXR(xrGetInputSourceLocalizedName(
                            Session, &nameGetInfo, 256, &stringCount, stringBuffer));
                        char pathStr[256];
                        uint32_t strLen = 0;
                        OXR(xrPathToString(
                            Instance,
                            actionPathsBuffer[a],
                            (uint32_t)sizeof(pathStr),
                            &strLen,
                            pathStr));
                        ALOGV(
                            "Xr##  -> path = %lld `%s` -> `%s`",
                            (long long)actionPathsBuffer[a],
                            pathStr,
                            stringBuffer);
                    }
                }
            }
        }
    }

    void VibrateController(
        const XrAction& action,
        const XrPath& subactionPath,
        float duration,
        float frequency,
        float amplitude) {
        // fire haptics using output action
        XrHapticVibration v{XR_TYPE_HAPTIC_VIBRATION, nullptr};
        v.amplitude = amplitude;
        v.duration = ToXrTime(duration);
        v.frequency = frequency;
        XrHapticActionInfo hai = {XR_TYPE_HAPTIC_ACTION_INFO, nullptr};
        hai.action = action;
        hai.subactionPath = subactionPath;
        OXR(xrApplyHapticFeedback(Session, &hai, (const XrHapticBaseHeader*)&v));
    }

    void VibrateControllerAmplitude(
        const XrAction& action,
        const XrPath& subactionPath,
        const float* envelope,
        const size_t envelopeSize,
        const float durationSecs) {
        /// fill in the amplitude buffer
        std::vector<float> amplitudes(envelope, envelope + envelopeSize);
        // fire haptics using output action
        XrHapticAmplitudeEnvelopeVibrationFB v{
            XR_TYPE_HAPTIC_AMPLITUDE_ENVELOPE_VIBRATION_FB, nullptr};
        v.duration = ToXrTime(durationSecs);
        v.amplitudeCount = (uint32_t)envelopeSize;
        v.amplitudes = amplitudes.data();
        XrHapticActionInfo hai = {XR_TYPE_HAPTIC_ACTION_INFO, nullptr};
        hai.action = action;
        hai.subactionPath = subactionPath;
        OXR(xrApplyHapticFeedback(Session, &hai, (const XrHapticBaseHeader*)&v));
    }

    void VibrateControllerPCM(
        const XrAction& action,
        const XrPath& subactionPath,
        const float* buffer,
        const size_t bufferSize,
        float sampleRate) {
        // stream and sleep on a separate thread,
        // so that we don't lock up the entire app
        std::thread t([action, subactionPath, buffer, bufferSize, sampleRate, this]() {
        /// fill in the amplitude buffer
        std::vector<float> pcmBuffer(bufferSize);
        for (size_t i = 0; i < bufferSize; ++i) {
            pcmBuffer[i] = buffer[i];
        }
        // fire haptics using output action
        XrHapticPcmVibrationFB v{XR_TYPE_HAPTIC_PCM_VIBRATION_FB, nullptr};
        v.sampleRate = sampleRate;
        v.bufferSize = bufferSize;
        v.buffer = pcmBuffer.data();
        uint32_t samplesUsed = 0;
        v.samplesConsumed = &samplesUsed;
        v.append = XR_FALSE;
        XrHapticActionInfo hai = {XR_TYPE_HAPTIC_ACTION_INFO, nullptr};
        hai.action = action;
        hai.subactionPath = subactionPath;
        OXR(xrApplyHapticFeedback(Session, &hai, (const XrHapticBaseHeader*)&v));
        samplesUsed = *(v.samplesConsumed);
        ALOG("Initial Haptics PCM Buffer Count Output: %d", samplesUsed);
        uint32_t totalSamplesUsed = samplesUsed;
        while (totalSamplesUsed < bufferSize) {
            ALOG("TotalSamplesUsed: %d", totalSamplesUsed);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            int newBufferSize = bufferSize - totalSamplesUsed;
            pcmBuffer.resize(newBufferSize);
            for (int i = 0; i < newBufferSize; ++i) {
                pcmBuffer[i] = buffer[i + totalSamplesUsed];
            }
            v.bufferSize = newBufferSize;
            v.buffer = pcmBuffer.data();
            v.append = XR_TRUE;
            OXR(xrApplyHapticFeedback(Session, &hai, (const XrHapticBaseHeader*)&v));
            samplesUsed = *(v.samplesConsumed);
            if (samplesUsed == 0) {
                ALOG("No samples used; stopping logging.");
                break;
            }
            totalSamplesUsed += samplesUsed;
            ALOG("Haptics PCM Buffer Count Output: %d", *(v.samplesConsumed));
        }
        });
        t.detach();
    }

    void VibrateControllerParametric(
        const XrAction& action,
        const XrPath& subactionPath)
    {
        if (!SupportsParametricHaptics()) {
            return;
        }

        const std::vector<XrHapticParametricPointEXTX1> amplitudePoints{{
            {0, 0.0}, {100000000, 1.0}, {270000000, 1.0}, {480000000, 0.7}, {750000000, 0.6},
            {1000000000, 1.0}}};
         const std::vector<XrHapticParametricPointEXTX1> frequencyPoints{{
            {0, 1.0}, {1000000000, 0.0}}};
         const std::vector<XrHapticParametricTransientEXTX1> transients{{
            {600000000, 1.0, 1.0}}};

        XrHapticParametricVibrationEXTX1 parametricVibration{
            XR_TYPE_HAPTIC_PARAMETRIC_VIBRATION_EXTX1};
        parametricVibration.amplitudePointCount = amplitudePoints.size();
        parametricVibration.amplitudePoints = amplitudePoints.data();
        parametricVibration.frequencyPointCount = frequencyPoints.size();
        parametricVibration.frequencyPoints = frequencyPoints.data();
        parametricVibration.transientCount = transients.size();
        parametricVibration.transients = transients.data();
        parametricVibration.minFrequencyHz = XR_FREQUENCY_UNSPECIFIED;
        parametricVibration.maxFrequencyHz = XR_FREQUENCY_UNSPECIFIED;
        parametricVibration.streamFrameType = XR_HAPTIC_PARAMETRIC_STREAM_FRAME_TYPE_NONE_EXTX1;
        XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
        hapticActionInfo.action = action;
        hapticActionInfo.subactionPath = subactionPath;
        OXR(xrApplyHapticFeedback(
            Session, &hapticActionInfo, reinterpret_cast<const XrHapticBaseHeader*>(&parametricVibration)));
    }

    void StopHapticEffect(const XrAction& action, const XrPath& subactionPath) {
        XrHapticActionInfo hai = {XR_TYPE_HAPTIC_ACTION_INFO, nullptr};
        hai.action = action;
        hai.subactionPath = subactionPath;
        OXR(xrStopHapticFeedback(Session, &hai));
    }

   public:
   private:
    OVRFW::ControllerRenderer controllerRenderL_;
    OVRFW::ControllerRenderer controllerRenderR_;
    OVRFW::TinyUI ui_;
    OVRFW::SimpleBeamRenderer beamRenderer_;
    std::vector<OVRFW::ovrBeamRenderer::handle_t> beams_;

    OVRFW::VRMenuObject* bigText_ = nullptr;
    OVRFW::VRMenuObject* ipText_ = nullptr;
    XrAction triggerForceAction_ = XR_NULL_HANDLE;
    float triggerForceL_ = 0.0f;
    float triggerForceR_ = 0.0f;
    OVRFW::VRMenuObject* triggerForceLText_ = nullptr;
    OVRFW::VRMenuObject* triggerForceRText_ = nullptr;

    XrAction trackpadForceAction_ = XR_NULL_HANDLE;
    float trackpadForceL_ = 0.0f;
    float trackpadForceR_ = 0.0f;
    OVRFW::VRMenuObject* trackpadForceLText_ = nullptr;
    OVRFW::VRMenuObject* trackpadForceRText_ = nullptr;

    XrAction stylusForceAction_ = XR_NULL_HANDLE;
    float stylusForceL_ = 0.0f;
    float stylusForceR_ = 0.0f;
    OVRFW::VRMenuObject* stylusForceLText_ = nullptr;
    OVRFW::VRMenuObject* stylusForceRText_ = nullptr;

    XrAction triggerCurlAction_ = XR_NULL_HANDLE;
    float triggerCurlL_ = 0.0f;
    float triggerCurlR_ = 0.0f;
    OVRFW::VRMenuObject* triggerCurlLText_ = nullptr;
    OVRFW::VRMenuObject* triggerCurlRText_ = nullptr;

    XrAction triggerSlideAction_ = XR_NULL_HANDLE;
    float squeezeCurlL_ = 0.0f;
    float squeezeCurlR_ = 0.0f;
    OVRFW::VRMenuObject* squeezeCurlLText_ = nullptr;
    OVRFW::VRMenuObject* squeezeCurlRText_ = nullptr;

    XrDevicePcmSampleRateGetInfoFB rightDeviceSampleRate_{
        XR_TYPE_DEVICE_PCM_SAMPLE_RATE_GET_INFO_FB};
    XrDevicePcmSampleRateGetInfoFB leftDeviceSampleRate_{
        XR_TYPE_DEVICE_PCM_SAMPLE_RATE_GET_INFO_FB};
    OVRFW::VRMenuObject* pcmHapticText_ = nullptr;

    XrAction mainHapticAction_ = XR_NULL_HANDLE;
    XrAction triggerHapticAction_ = XR_NULL_HANDLE;
    XrAction thumbHapticAction_ = XR_NULL_HANDLE;

    // Proximity
    XrAction triggerProxAction_ = XR_NULL_HANDLE;
    bool triggerProxL_ = false;
    bool triggerProxR_ = false;
    OVRFW::VRMenuObject* triggerProxLText_ = nullptr;
    OVRFW::VRMenuObject* triggerProxRText_ = nullptr;

    XrAction thumbFbProxAction_ = XR_NULL_HANDLE;
    bool thumbFBProxL_ = false;
    bool thumbFBProxR_ = false;
    OVRFW::VRMenuObject* thumbFBProxLText_ = nullptr;
    OVRFW::VRMenuObject* thumbFBProxRText_ = nullptr;
    XrAction thumbMetaProxAction_ = XR_NULL_HANDLE;
    bool thumbMetaProxL_ = false;
    bool thumbMetaProxR_ = false;
    OVRFW::VRMenuObject* thumbMetaProxLText_ = nullptr;
    OVRFW::VRMenuObject* thumbMetaProxRText_ = nullptr;

    // Trigger Value
    XrAction triggerValueAction_ = XR_NULL_HANDLE;
    bool triggerValueL_ = false;
    bool triggerValueR_ = false;
    OVRFW::VRMenuObject* triggerValueLText_ = nullptr;
    OVRFW::VRMenuObject* triggerValueRText_ = nullptr;

    // Trigger Touch
    XrAction triggerTouchAction_ = XR_NULL_HANDLE;
    bool triggerTouchL_ = false;
    bool triggerTouchR_ = false;
    OVRFW::VRMenuObject* triggerTouchLText_ = nullptr;
    OVRFW::VRMenuObject* triggerTouchRText_ = nullptr;

    // Squeeze Value
    XrAction squeezeValueAction_ = XR_NULL_HANDLE;
    bool squeezeValueL_ = false;
    bool squeezeValueR_ = false;
    OVRFW::VRMenuObject* squeezeValueLText_ = nullptr;
    OVRFW::VRMenuObject* squeezeValueRText_ = nullptr;

    /// UI lag
    bool delayUI_ = false;
};

ENTRY_POINT(XrControllersApp)
