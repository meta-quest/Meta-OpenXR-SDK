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
Filename    :   main.cpp
Content     :   Sample test app to test openxr XR_META_dynamic_object_tracker extension
Created     :   August 2023
Authors     :   Adam Bengis, Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#include <OVR_Math.h>
#include <XrApp.h>

#include <Input/AxisRenderer.h>
#include <Input/ControllerRenderer.h>
#include <Input/HandRenderer.h>
#include <Input/TinyUI.h>
#include <Render/GeometryRenderer.h>
#include <Render/SimpleBeamRenderer.h>

#include "XrDynamicObjectTrackerHelper.h"
#include "XrHandHelper.h"
#include "XrPassthroughHelper.h"
#include "XrSpatialAnchorHelper.h"

class XrDynamicObjectsApp : public OVRFW::XrApp {
   public:
    XrDynamicObjectsApp() = default;

    // Returns a list of OpenXr extensions needed for this app
    std::vector<const char*> GetExtensions() override {
        std::vector<const char*> extensions = XrApp::GetExtensions();
        // Add hand extensions
        for (const auto& handExtension : XrHandHelper::RequiredExtensionNames()) {
            extensions.push_back(handExtension);
        }
        // Add dynamic object tracker extension
        for (const auto& dynamicObjectTrackerExtension :
             XrDynamicObjectTrackerHelper::RequiredExtensionNames()) {
            extensions.push_back(dynamicObjectTrackerExtension);
        }
        // Add spatial anchor extensions
        for (const auto& spatialAnchorExtension : XrSpatialAnchorHelper::RequiredExtensionNames()) {
            extensions.push_back(spatialAnchorExtension);
        }
        // Add passthrough extensions
        for (const auto& passthroughExtension : XrPassthroughHelper::RequiredExtensionNames()) {
            extensions.push_back(passthroughExtension);
        }

        // Log all extensions
        ALOG("XrDynamicObjectsApp requesting extensions:");
        for (const auto& e : extensions) {
            ALOG("   --> %s", e);
        }

        return extensions;
    }

    // Must return true if the application initializes successfully
    bool AppInit(const xrJava* context) override {
        if (!UI.Init(context, GetFileSys(), false)) {
            ALOGE("TinyUI::Init FAILED.");
            return false;
        }

        // Hand tracking
        HandsExtensionAvailable = ExtensionsArePresent(XrHandHelper::RequiredExtensionNames());
        if (HandsExtensionAvailable) {
            HandL = std::make_unique<XrHandHelper>(GetInstance(), true);
            HandR = std::make_unique<XrHandHelper>(GetInstance(), false);
        }

        // Dynamic object tracking
        if (ExtensionsArePresent(XrDynamicObjectTrackerHelper::RequiredExtensionNames())) {
            DynamicObjectTracker = std::make_unique<XrDynamicObjectTrackerHelper>(GetInstance());
        }

        // Spatial anchor
        if (ExtensionsArePresent(XrSpatialAnchorHelper::RequiredExtensionNames())) {
            SpatialAnchor = std::make_unique<XrSpatialAnchorHelper>(GetInstance());
        }

        // Passthrough
        if (ExtensionsArePresent(XrPassthroughHelper::RequiredExtensionNames())) {
            Passthrough = std::make_unique<XrPassthroughHelper>(GetInstance());
        }

        return true;
    }

    void AppShutdown(const xrJava* context) override {
        Passthrough.reset();
        SpatialAnchor.reset();
        DynamicObjectTracker.reset();
        HandL.reset();
        HandR.reset();

        UIInitialized = false;
        HandsExtensionAvailable = false;

        UI.Shutdown();
        OVRFW::XrApp::AppShutdown(context);
    }

    bool SessionInit() override {
        // Use LocalSpace instead of Stage Space
        CurrentSpace = LocalSpace;

        // Init session bound objects
        if (!ControllerRenderL.Init(true)) {
            ALOGE("SessionInit::Init L controller renderer FAILED.");
            return false;
        }
        if (!ControllerRenderR.Init(false)) {
            ALOGE("SessionInit::Init R controller renderer FAILED.");
            return false;
        }
        BeamRenderer.Init(GetFileSys(), nullptr, OVR::Vector4f(1.0f), 1.0f);
        ParticleSystem.Init(10, nullptr, OVRFW::ovrParticleSystem::GetDefaultGpuState(), false);

        if (HandsExtensionAvailable) {
            HandL->SessionInit(GetSession());
            HandR->SessionInit(GetSession());
            HandRendererL.Init(&HandL->GetMesh(), HandL->IsLeftHand());
            HandRendererR.Init(&HandR->GetMesh(), HandR->IsLeftHand());
        }
        if (DynamicObjectTracker) {
            DynamicObjectTracker->SessionInit(GetSession());
        }
        if (SpatialAnchor) {
            SpatialAnchor->SessionInit(GetSession());
        }
        if (Passthrough) {
            Passthrough->SessionInit(GetSession());
            PassthroughLayer = Passthrough->CreateProjectedLayer();
            Passthrough->Start();
        }

        AxisRenderer.Init();

        GeometryRenderer.Init(OVRFW::BuildTesselatedQuadDescriptor(
            4, 4, false /*twoSided*/)); // quad in XY plane, facing +Z

        GeometryRenderer.DiffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
        GeometryRenderer.ChannelControl = {0, 1, 0, 1};
        GeometryRenderer.AmbientLightColor = {1, 1, 1};
        GeometryRenderer.BlendMode = GL_FUNC_REVERSE_SUBTRACT;

        return true;
    }

    void SessionEnd() override {
        GeometryRenderer.Shutdown();
        AxisRenderer.Shutdown();

        if (Passthrough) {
            Passthrough->DestroyGeometryInstance(PassthroughWindow);
            Passthrough->DestroyLayer(PassthroughLayer);
            Passthrough->SessionEnd();
        }
        if (SpatialAnchor) {
            SpatialAnchor->SessionEnd();
        }
        if (DynamicObjectTracker) {
            DynamicObjectTracker->DestroyDynamicObjectTracker();
            DynamicObjectTracker->SessionEnd();
        }
        if (HandsExtensionAvailable) {
            HandL->SessionEnd();
            HandR->SessionEnd();
            HandRendererL.Shutdown();
            HandRendererR.Shutdown();
        }

        ControllerRenderL.Shutdown();
        ControllerRenderR.Shutdown();
        ParticleSystem.Shutdown();
        BeamRenderer.Shutdown();
    }

    void HandleXrEvents() override {
        XrEventDataBuffer eventDataBuffer = {};

        // Poll for events
        for (;;) {
            XrEventDataBaseHeader* baseEventHeader = (XrEventDataBaseHeader*)(&eventDataBuffer);
            baseEventHeader->type = XR_TYPE_EVENT_DATA_BUFFER;
            baseEventHeader->next = NULL;
            XrResult r;
            OXR(r = xrPollEvent(Instance, &eventDataBuffer));
            if (r != XR_SUCCESS) {
                break;
            }

            if (DynamicObjectTracker && DynamicObjectTracker->HandleXrEvents(baseEventHeader)) {
                continue;
            }
            if (SpatialAnchor && SpatialAnchor->HandleXrEvents(baseEventHeader)) {
                continue;
            }

            switch (baseEventHeader->type) {
                case XR_TYPE_EVENT_DATA_EVENTS_LOST:
                    ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_EVENTS_LOST event");
                    break;
                case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
                    ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING event");
                    break;
                case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED event");
                    break;
                case XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT: {
                    const auto perfSettingsEvent =
                        reinterpret_cast<const XrEventDataPerfSettingsEXT*>(baseEventHeader);
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT event: type %d subdomain %d : level %d -> level %d",
                        perfSettingsEvent->type,
                        perfSettingsEvent->subDomain,
                        perfSettingsEvent->fromLevel,
                        perfSettingsEvent->toLevel);
                } break;
                case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING event");
                    break;
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                    const auto sessionStateChangedEvent =
                        reinterpret_cast<const XrEventDataSessionStateChanged*>(baseEventHeader);
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: %d for session %p at time %f",
                        sessionStateChangedEvent->state,
                        reinterpret_cast<void*>(sessionStateChangedEvent->session),
                        FromXrTime(sessionStateChangedEvent->time));

                    switch (sessionStateChangedEvent->state) {
                        case XR_SESSION_STATE_FOCUSED:
                            Focused = true;
                            break;
                        case XR_SESSION_STATE_VISIBLE:
                            Focused = false;
                            break;
                        case XR_SESSION_STATE_READY:
                        case XR_SESSION_STATE_STOPPING:
                            HandleSessionStateChanges(sessionStateChangedEvent->state);
                            break;
                        case XR_SESSION_STATE_EXITING:
                            ShouldExit = true;
                            break;
                        default:
                            break;
                    }
                } break;
                default:
                    ALOGV("xrPollEvent: Unknown event");
                    break;
            }
        }
    }

    void Update(const OVRFW::ovrApplFrameIn& in) override {
        InitializeUI();

        XrSpace currentSpace = GetCurrentSpace();
        XrTime predictedDisplayTime = ToXrTime(in.PredictedDisplayTime);

        if (HandsExtensionAvailable) {
            HandL->Update(currentSpace, predictedDisplayTime);
            HandR->Update(currentSpace, predictedDisplayTime);
        }

        UpdateDynamicObjectTracker(currentSpace, predictedDisplayTime);
        UpdateSpatialAnchor(currentSpace, predictedDisplayTime);
        UpdatePassthrough(currentSpace, predictedDisplayTime);
        UpdateUIHitTests(in);

        // Hands
        if (HandsExtensionAvailable) {
            if (HandL->AreLocationsActive()) {
                HandRendererL.Update(HandL->GetJoints(), HandL->GetRenderScale());
            }
            if (HandR->AreLocationsActive()) {
                HandRendererR.Update(HandR->GetJoints(), HandR->GetRenderScale());
            }
        }

        // Controllers
        if (in.LeftRemoteTracked) {
            ControllerRenderL.Update(in.LeftRemotePose);
        }
        if (in.RightRemoteTracked) {
            ControllerRenderR.Update(in.RightRemotePose);
        }
    }

    void Render(const OVRFW::ovrApplFrameIn& in, OVRFW::ovrRendererOutput& out) override {
        if (KeyboardTracking) {
            AxisRenderer.Render(OVR::Matrix4f(), in, out);
            GeometryRenderer.Render(out.Surfaces);
        }

        UI.Render(in, out);

        if (HandsExtensionAvailable && HandL->AreLocationsActive() && HandL->IsPositionValid()) {
            const float transparency = ComputeHandTransparency(
                FromXrPosef(HandL->GetJoints()[XR_HAND_JOINT_MIDDLE_TIP_EXT].pose).Translation);
            HandRendererL.Confidence = transparency;
            HandRendererL.Render(out.Surfaces);
        } else if (in.LeftRemoteTracked) {
            ControllerRenderL.Render(out.Surfaces);
        }

        if (HandsExtensionAvailable && HandR->AreLocationsActive() && HandR->IsPositionValid()) {
            const float transparency = ComputeHandTransparency(
                FromXrPosef(HandR->GetJoints()[XR_HAND_JOINT_MIDDLE_TIP_EXT].pose).Translation);
            HandRendererR.Confidence = transparency;
            HandRendererR.Render(out.Surfaces);
        } else if (in.RightRemoteTracked) {
            ControllerRenderR.Render(out.Surfaces);
        }

        // Render beams last for proper blending
        ParticleSystem.Frame(in, nullptr, out.FrameMatrices.CenterView);
        ParticleSystem.RenderEyeView(
            out.FrameMatrices.CenterView, out.FrameMatrices.EyeProjection[0], out.Surfaces);
        BeamRenderer.Render(in, out);
    }

    void PreProjectionAddLayer(xrCompositorLayerUnion* layers, int& layerCount) override {
        static XrCompositionLayerAlphaBlendFB alphaBlend{XR_TYPE_COMPOSITION_LAYER_ALPHA_BLEND_FB};
        alphaBlend.next = nullptr;
        alphaBlend.srcFactorColor = XR_BLEND_FACTOR_ONE_FB;
        alphaBlend.dstFactorColor = XR_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA_FB;
        alphaBlend.srcFactorAlpha = XR_BLEND_FACTOR_ONE_FB;
        alphaBlend.dstFactorAlpha = XR_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA_FB;

        // Add the passthrough layer
        if (PassthroughLayer != XR_NULL_HANDLE) {
            XrCompositionLayerPassthroughFB passthroughLayer{
                XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB};
            passthroughLayer.next = &alphaBlend;
            passthroughLayer.layerHandle = PassthroughLayer;
            layers[layerCount++].Passthrough = passthroughLayer;
        }
    }

   private:
    enum class HitTestRayDeviceNums {
        LeftHand,
        LeftRemote,
        RightHand,
        RightRemote,
    };

    bool ExtensionsArePresent(const std::vector<const char*>& extensionList) const {
        const auto extensionProperties = GetXrExtensionProperties();
        bool foundAllExtensions = true;
        for (const auto& extension : extensionList) {
            bool foundExtension = false;
            for (const auto& extensionProperty : extensionProperties) {
                if (!strcmp(extension, extensionProperty.extensionName)) {
                    foundExtension = true;
                    break;
                }
            }
            if (!foundExtension) {
                foundAllExtensions = false;
                break;
            }
        }
        return foundAllExtensions;
    }

    void InitializeUI() {
        if (UIInitialized) {
            return;
        }

        UIInitialized = true;
        TextLabel = UI.AddLabel("Dynamic Objects Sample", {0.0f, 0.5f, -1.5f}, {600.0f, 50.0f});

        StatusTextLabel = UI.AddLabel("", {0.0f, 0.1f, -1.5f}, {600.0f, 320.0f});
        OVRFW::VRMenuFontParms fontParms = StatusTextLabel->GetFontParms();
        fontParms.AlignHoriz = OVRFW::HORIZONTAL_LEFT;
        fontParms.AlignVert = OVRFW::VERTICAL_BASELINE;
        fontParms.WrapWidth = 1.1f;
        fontParms.MaxLines = 10;
        StatusTextLabel->SetFontParms(fontParms);
        StatusTextLabel->SetTextLocalPosition({-0.55f, 0.25f, 0.0f});

        StatusText += "DynamicObjectTracker ";
        if (DynamicObjectTracker) {
            StatusText += DynamicObjectTracker->IsSupported() ? "supported\n" : "unsupported\n";
        } else {
            StatusText += "unavailable\n";
        }
        StatusText +=
            "SpatialAnchor " + std::string(SpatialAnchor ? "available\n" : "unavailable\n");
        StatusText += "Passthrough " + std::string(Passthrough ? "available\n" : "unavailable\n");
        StatusTextLabel->SetText(StatusText.c_str());
    }

    void EnableButton(OVRFW::VRMenuObject* button) {
        button->SetSurfaceColor(0, UI.BackgroundColor);
        button->RemoveFlags(OVRFW::VRMENUOBJECT_DONT_HIT_ALL);
    }

    void DisableButton(OVRFW::VRMenuObject* button) {
        button->SetSurfaceColor(0, {0.1f, 0.1f, 0.1f, 1.0f});
        button->AddFlags(OVRFW::VRMENUOBJECT_DONT_HIT_ALL);
    }

    OVRFW::ovrParticleSystem::handle_t AddParticle(
        const OVRFW::ovrApplFrameIn& in,
        const OVR::Vector3f& position) {
        return ParticleSystem.AddParticle(
            in,
            position,
            0.0f,
            OVR::Vector3f(0.0f),
            OVR::Vector3f(0.0f),
            BeamRenderer.PointerParticleColor,
            OVRFW::ovrEaseFunc::NONE,
            0.0f,
            0.03f,
            0.1f,
            0);
    }

    void UpdateUIHitTests(const OVRFW::ovrApplFrameIn& in) {
        UI.HitTestDevices().clear();
        ParticleSystem.RemoveParticle(LeftControllerPoint);
        ParticleSystem.RemoveParticle(RightControllerPoint);

        // The controller actions are still triggered with hand tracking
        if (HandsExtensionAvailable && HandL->IsPositionValid()) {
            UpdateRemoteTrackedUIHitTest(
                FromXrPosef(HandL->GetAimPose()),
                HandL->GetIndexPinching() ? 1.0f : 0.0f,
                HandL.get(),
                HitTestRayDeviceNums::LeftHand);
        } else if (in.LeftRemoteTracked) {
            UpdateRemoteTrackedUIHitTest(
                in.LeftRemotePointPose,
                in.LeftRemoteIndexTrigger,
                HandL.get(),
                HitTestRayDeviceNums::LeftRemote);
            LeftControllerPoint = AddParticle(in, in.LeftRemotePointPose.Translation);
        }

        if (HandsExtensionAvailable && HandR->IsPositionValid()) {
            UpdateRemoteTrackedUIHitTest(
                FromXrPosef(HandR->GetAimPose()),
                HandR->GetIndexPinching() ? 1.0f : 0.0f,
                HandR.get(),
                HitTestRayDeviceNums::RightHand);
        } else if (in.RightRemoteTracked) {
            UpdateRemoteTrackedUIHitTest(
                in.RightRemotePointPose,
                in.RightRemoteIndexTrigger,
                HandR.get(),
                HitTestRayDeviceNums::RightRemote);
            RightControllerPoint = AddParticle(in, in.RightRemotePointPose.Translation);
        }

        UI.Update(in);
        BeamRenderer.Update(in, UI.HitTestDevices());
    }

    void UpdateRemoteTrackedUIHitTest(
        const OVR::Posef& remotePose,
        float remoteIndexTrigger,
        XrHandHelper* hand,
        HitTestRayDeviceNums device) {
        const bool didPinch = remoteIndexTrigger > 0.25f;
        UI.AddHitTestRay(remotePose, didPinch, static_cast<int>(device));
    }

    void UpdateDynamicObjectTracker(XrSpace currentSpace, XrTime predictedDisplayTime) {
        if (!DynamicObjectTracker) {
            return;
        }

        DynamicObjectTracker->Update(currentSpace, predictedDisplayTime);

        // Check if dynamic object tracker is supported
        if (!DynamicObjectTracker->IsSupported()) {
            DynamicObjectTracker.reset();
            return;
        }

        const auto trackerState = DynamicObjectTracker->GetTrackerState();
        if (trackerState == XrDynamicObjectTrackerHelper::TrackerState::Empty) {
            // Create tracker if it is uninitialized
            if (DynamicObjectTracker->CreateDynamicObjectTracker()) {
                StatusText += "Creating DynamicObjectTracker...\n";
                StatusTextLabel->SetText(StatusText.c_str());
                return;
            } else {
                StatusText += "Failed to create DynamicObjectTracker!\n";
                StatusTextLabel->SetText(StatusText.c_str());
                DynamicObjectTracker.reset();
                return;
            }
        }

        if (trackerState == XrDynamicObjectTrackerHelper::TrackerState::Created) {
            // Set object class that we want to track
            const std::vector<XrDynamicObjectClassMETA> trackedClasses = {
                XR_DYNAMIC_OBJECT_CLASS_KEYBOARD_META};
            if (DynamicObjectTracker->SetDynamicObjectTrackedClasses(trackedClasses)) {
                StatusText += "Looking for: KEYBOARD\n";
                StatusTextLabel->SetText(StatusText.c_str());
                return;
            } else {
                StatusText += "Failed to set tracked classes: KEYBOARD\n";
                StatusTextLabel->SetText(StatusText.c_str());
                DynamicObjectTracker.reset();
                return;
            }
        }

        if (trackerState == XrDynamicObjectTrackerHelper::TrackerState::Initialized) {
            // Tracker ready, if we don't have a keyboard then try looking for objects
            // Currently we update our anchor at interval to avoid the bounding box miscmatching
            if (SpatialAnchor && predictedDisplayTime > NextQueryAnchorTime) {
                if (SpatialAnchor->RequestQueryAnchorsWithComponentEnabled(
                        XR_SPACE_COMPONENT_TYPE_DYNAMIC_OBJECT_DATA_META, QueryAnchorRequestId)) {
                    StatusText += NextQueryAnchorTime == 0 ? "Querying spatial anchors" : ".";
                    StatusTextLabel->SetText(StatusText.c_str());
                } else {
                    StatusText += "Failed to query spatial anchors!\n";
                    StatusTextLabel->SetText(StatusText.c_str());
                }
                // If no anchor is found, retry after a short delay.
                NextQueryAnchorTime = predictedDisplayTime + ToXrTime(1.0);
            }
        }
    }

    void UpdateSpatialAnchor(XrSpace currentSpace, XrTime predictedDisplayTime) {
        if (!SpatialAnchor) {
            return;
        }

        SpatialAnchor->Update(currentSpace, predictedDisplayTime);

        // Wait for pending requests to finish
        if (QueryAnchorRequestId != 0 && SpatialAnchor->IsRequestCompleted(QueryAnchorRequestId)) {
            QueryAnchorRequestId = 0;
        }
        if (EnableLocatableRequestId != 0 &&
            SpatialAnchor->IsRequestCompleted(EnableLocatableRequestId)) {
            EnableLocatableRequestId = 0;
        }
        if (QueryAnchorRequestId != 0 || EnableLocatableRequestId != 0) {
            return;
        }

        // Check if we have a detected keyboard
        KeyboardSpace = DetectKeyboard();
        if (KeyboardSpace == XR_NULL_HANDLE) {
            return;
        }

        // We have a keyboard, make sure it's locatable
        bool keyboardLocatable = false;
        if (!SpatialAnchor->GetComponentStatus(
                KeyboardSpace, XR_SPACE_COMPONENT_TYPE_LOCATABLE_FB, keyboardLocatable)) {
            ALOGE("KeyboardSpace is not locatable!");
            return;
        }

        // Start tracking the keyboard if it is untracked
        if (!keyboardLocatable) {
            StatusText += "Locating KeyboardSpace...\n";
            StatusTextLabel->SetText(StatusText.c_str());
            SpatialAnchor->RequestSetComponentStatus(
                KeyboardSpace,
                XR_SPACE_COMPONENT_TYPE_LOCATABLE_FB,
                true,
                EnableLocatableRequestId);
            return;
        }

        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrResult result =
            xrLocateSpace(KeyboardSpace, GetLocalSpace(), predictedDisplayTime, &spaceLocation);
        if (XR_FAILED(result)) {
            ALOGE("xrLocateSpace failed for KeyboardSpace, error %d", result);
            return;
        }

        static bool logKeyboardTracking = true;
        if (spaceLocation.locationFlags == 0) {
            // we didn't get a valid pose, reset state and keep trying!
            logKeyboardTracking = true;
            KeyboardTracking = false;
            KeyboardSpace = XR_NULL_HANDLE;
            return;
        }

        if (logKeyboardTracking) {
            StatusText += "Tracking KeyboardSpace...\n";
            StatusTextLabel->SetText(StatusText.c_str());
            logKeyboardTracking = false;
        }

        KeyboardTracking = true;
        KeyboardPose = FromXrPosef(spaceLocation.pose);
        SpatialAnchor->GetBoundingBox3D(KeyboardSpace, KeyboardBoundingBox3D);
        SpatialAnchor->GetBoundingBox2D(KeyboardSpace, KeyboardBoundingBox2D);
    }

    void UpdatePassthrough(XrSpace currentSpace, XrTime predictedDisplayTime) {
        if (!Passthrough) {
            return;
        }

        Passthrough->Update(currentSpace, predictedDisplayTime);

        // If keyboard is not tracking, we are done
        if (!KeyboardTracking) {
            return;
        }

        // Create passthrough window if we don't already have one
        if (PassthroughWindow == XR_NULL_HANDLE) {
            const std::vector<XrVector3f> kSquareVertices = {
                XrVector3f{-0.5f, -0.5f, 0.0f},
                XrVector3f{-0.5f, 0.5f, 0.0f},
                XrVector3f{0.5f, 0.5f, 0.0f},
                XrVector3f{0.5f, -0.5f, 0.0f}};
            const std::vector<uint32_t> kSquareIndices = {0, 1, 2, 2, 3, 0};
            PassthroughWindow = Passthrough->CreateGeometryInstance(
                PassthroughLayer, GetCurrentSpace(), kSquareVertices, kSquareIndices);
        }

        // Update cutoff transform
        if (PassthroughWindow != XR_NULL_HANDLE) {
            const OVR::Vector3f kb3DExtent = OVR::Vector3f(
                KeyboardBoundingBox3D.extent.width,
                KeyboardBoundingBox3D.extent.height,
                KeyboardBoundingBox3D.extent.depth);
                
            // multiplying by 1.25 because when we generated the Tesselated Quad, we used a 4x4 grid
            // of verts. and BuildTesselatedQuadDescriptor will transition from opaque on the outer
            // ring of verts to transparent on the next inner ring of verts. i.e. 25% of each
            // dimension will be feathered, so we need to scale the quad by 1.25 so that the actual
            // keyboard part is unfeathered. Note: this is highly dependant on the implementation of
            // and the call to BuildTesselatedQuadDescriptor, so if that changes this will break.
            constexpr float kPassThrough3DScaleFeatheringMultiplier = 1.25f;
            const OVR::Vector3f passthrough3DScale = kb3DExtent * kPassThrough3DScaleFeatheringMultiplier;

            Passthrough->SetGeometryInstanceTransform(
                PassthroughWindow,
                predictedDisplayTime,
                ToXrPosef(KeyboardPose),
                ToXrVector3f(passthrough3DScale));

            // update the feathered cutout shape
            GeometryRenderer.SetPose(KeyboardPose);
            GeometryRenderer.SetScale({passthrough3DScale.x / 2, passthrough3DScale.y / 2, 1.0f});
            GeometryRenderer.Update();


            // update rgb axis markers on keyboard corners using the 2d bounding box for demonstration 
            // purposes, but we could use the 3d bounding box as well
            const auto kb2DOffset = OVR::Vector3f{KeyboardBoundingBox2D.offset.x, KeyboardBoundingBox2D.offset.y, 0.0f};
            const auto kbOffsetPose = KeyboardPose * OVR::Posef(OVR::Quatf(), kb2DOffset);

            const auto& kb2DExtent = KeyboardBoundingBox2D.extent;

            std::vector<OVR::Posef> poses;
            // Center
            poses.push_back(KeyboardPose);
            // XY plane corners on face of keyboard
            OVR::Posef point = OVR::Posef::Identity();
            poses.push_back(kbOffsetPose * point);
            point.Translation.x = kb2DExtent.width;
            point.Translation.y = 0;
            poses.push_back(kbOffsetPose * point);
            point.Translation.x = 0;
            point.Translation.y = kb2DExtent.height;
            poses.push_back(kbOffsetPose * point);
            point.Translation.x = kb2DExtent.width;
            point.Translation.y = kb2DExtent.height;
            poses.push_back(kbOffsetPose * point);
            AxisRenderer.Update(poses);
        }
    }

    XrSpace DetectKeyboard() {
        if (KeyboardSpace != XR_NULL_HANDLE) {
            // Already have a keyboard, we are done
            return KeyboardSpace;
        }

        if (!SpatialAnchor) {
            // Can't detect if spatial anchor is not available
            return XR_NULL_HANDLE;
        }

        // Filter by dynamic object class KEYBOARD
        const auto& results = SpatialAnchor->GetSpatialAnchors([&](XrSpace space) {
            if (SpatialAnchor->IsComponentSupported(
                    space, XR_SPACE_COMPONENT_TYPE_DYNAMIC_OBJECT_DATA_META)) {
                XrDynamicObjectClassMETA dynamicObjectClass;
                if (DynamicObjectTracker->GetDynamicObjectClass(space, dynamicObjectClass) &&
                    dynamicObjectClass == XR_DYNAMIC_OBJECT_CLASS_KEYBOARD_META) {
                    return true;
                }
            }
            return false;
        });
        if (results.empty()) {
            // Found nothing
            return XR_NULL_HANDLE;
        }

        KeyboardSpace = results.at(0);
        StatusText += "\nKeyboard detected!\n";
        StatusTextLabel->SetText(StatusText.c_str());
        return KeyboardSpace;
    }

    float ComputeHandTransparency(const OVR::Vector3f& handPoint) {
        // Define test box to use XY from bounding box, but cutout plane
        // sits on max Z so define a custom range from that
        const float boxMinX = KeyboardBoundingBox3D.offset.x;
        const float boxMinY = KeyboardBoundingBox3D.offset.y;
        const float boxMaxX = boxMinX + KeyboardBoundingBox3D.extent.width;
        const float boxMaxY = boxMinY + KeyboardBoundingBox3D.extent.height;
        const float boxMinZ = KeyboardBoundingBox3D.offset.z + KeyboardBoundingBox3D.extent.depth;
        const float boxMaxZ = boxMinZ + 0.1f;

        // Closest point to test box
        const OVR::Vector3f localHandPoint = KeyboardPose.InverseTransform(handPoint);
        const OVR::Vector3f closestHandPoint{
            OVR::OVRMath_Clamp(localHandPoint.x, boxMinX, boxMaxX),
            OVR::OVRMath_Clamp(localHandPoint.y, boxMinY, boxMaxY),
            OVR::OVRMath_Clamp(localHandPoint.z, boxMinZ, boxMaxZ)};

        // Apply falloff based on distance
        const OVR::Vector3f v = localHandPoint - closestHandPoint;
        const float distance = v.Length();
        const float kHiThreshold = 0.1f;
        const float kLoThreshold = 0.0f;
        if (distance < kLoThreshold) {
            return 0.0f;
        }
        if (distance < kHiThreshold) {
            return 1.0f - ((kHiThreshold - distance) / (kHiThreshold - kLoThreshold));
        }
        return 1.0f;
    }

   private:
    bool HandsExtensionAvailable = false;
    bool UIInitialized = false;

    // XR interface
    std::unique_ptr<XrHandHelper> HandL;
    std::unique_ptr<XrHandHelper> HandR;
    std::unique_ptr<XrDynamicObjectTrackerHelper> DynamicObjectTracker;
    std::unique_ptr<XrSpatialAnchorHelper> SpatialAnchor;
    std::unique_ptr<XrPassthroughHelper> Passthrough;

    // hands/controllers - rendering
    OVRFW::HandRenderer HandRendererL;
    OVRFW::HandRenderer HandRendererR;
    OVRFW::ControllerRenderer ControllerRenderL;
    OVRFW::ControllerRenderer ControllerRenderR;

    // UI
    OVRFW::TinyUI UI;
    OVRFW::SimpleBeamRenderer BeamRenderer;
    std::vector<OVRFW::ovrBeamRenderer::handle_t> Beams;
    OVRFW::ovrParticleSystem ParticleSystem;
    OVRFW::ovrParticleSystem::handle_t LeftControllerPoint;
    OVRFW::ovrParticleSystem::handle_t RightControllerPoint;

    OVRFW::VRMenuObject* TextLabel = nullptr;
    OVRFW::VRMenuObject* StatusTextLabel = nullptr;
    std::string StatusText;

    XrTime NextQueryAnchorTime = 0;
    bool KeyboardTracking = false;

    XrAsyncRequestIdFB QueryAnchorRequestId = 0;
    XrAsyncRequestIdFB EnableLocatableRequestId = 0;
    XrSpace KeyboardSpace = XR_NULL_HANDLE;
    OVR::Posef KeyboardPose = OVR::Posef::Identity();
    XrRect3DfFB KeyboardBoundingBox3D = {};
    XrRect2Df KeyboardBoundingBox2D = {};

    XrPassthroughLayerFB PassthroughLayer = XR_NULL_HANDLE;
    XrGeometryInstanceFB PassthroughWindow = XR_NULL_HANDLE;

    OVRFW::ovrAxisRenderer AxisRenderer;
    OVRFW::GeometryRenderer GeometryRenderer;
};

ENTRY_POINT(XrDynamicObjectsApp)
