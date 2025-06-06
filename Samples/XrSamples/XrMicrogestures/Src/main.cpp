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
#include <cstdint>

#include "GUI/VRMenuObject.h"
#include "XrApp.h"
#include "ActionSetDisplayPanel.h"
#include "SkyboxRenderer.h"
#include "EnvironmentRenderer.h"
#include "Input/TinyUI.h"
#include "Render/SimpleBeamRenderer.h"
#include "Input/HandRenderer.h"

// For expressiveness; use _m rather than f literals when we mean meters
constexpr float operator"" _m(long double meters) {
    return static_cast<float>(meters);
}
constexpr float operator"" _m(unsigned long long meters) {
    return static_cast<float>(meters);
}

class XrMicrogesturesApp : public OVRFW::XrApp {
   public:
    XrMicrogesturesApp() : OVRFW::XrApp() {
        BackgroundColor = OVR::Vector4f(0.75f, 0.75f, 0.75f, 1.0f);

        // Disable framework input management, letting this sample explicitly
        // call xrSyncActions() every frame; which includes control over which
        // ActionSet to set as active each frame
        SkipInputHandling = true;
    }

    // Return a list of OpenXR extensions needed for this app
    virtual std::vector<const char*> GetExtensions() override {
        std::vector<const char*> extensions = XrApp::GetExtensions();
        extensions.push_back(XR_EXT_HAND_TRACKING_EXTENSION_NAME);
        extensions.push_back(XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME);
        extensions.push_back(XR_FB_HAND_TRACKING_AIM_EXTENSION_NAME);
        extensions.push_back(XR_EXT_HAND_INTERACTION_EXTENSION_NAME);
        extensions.push_back(XR_META_HAND_TRACKING_MICROGESTURES_EXTENSION_NAME);
        return extensions;
    }

    std::unordered_map<XrPath, std::vector<XrActionSuggestedBinding>> GetSuggestedBindings(
        XrInstance instance) override {
        OXR(xrStringToPath(instance, "/user/hand/left", &LeftHandPath));
        OXR(xrStringToPath(instance, "/user/hand/right", &RightHandPath));

        // Get the default bindings suggested by XrApp framework
        auto suggestedBindings = XrApp::GetSuggestedBindings(instance);
        ActionLeftHandSetMicrogestures =
            CreateActionSet(0, "left_hand_microgesture_action_set", "Left hand microgestures");
        ActionRightHandSetMicrogestures =
            CreateActionSet(0, "right_hand_microgesture_action_set", "Right hand microgestures");

        ActionLeftHandSwipeLeft = CreateAction(
            ActionLeftHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "action_swipe_left",
            "Swipe Left");
        ActionLeftHandSwipeRight = CreateAction(
            ActionLeftHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "action_swipe_right",
            "Swipe Right");
        ActionLeftHandSwipeForward = CreateAction(
            ActionLeftHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "swipe_forward",
            "Swipe Forward");
        ActionLeftHandSwipeBackward = CreateAction(
            ActionLeftHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "swipe_backward",
            "Swipe Backward");
        ActionLeftHandTapThumb = CreateAction(
            ActionLeftHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "action_tap_thumb",
            "Thumb Tap");

        ActionRightHandSwipeLeft = CreateAction(
            ActionRightHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "action_swipe_left",
            "Swipe Left");
        ActionRightHandSwipeRight = CreateAction(
            ActionRightHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "action_swipe_right",
            "Swipe Right");
        ActionRightHandSwipeForward = CreateAction(
            ActionRightHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "swipe_forward",
            "Swipe Forward");
        ActionRightHandSwipeBackward = CreateAction(
            ActionRightHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "swipe_backward",
            "Swipe Backward");
        ActionRightHandTapThumb = CreateAction(
            ActionRightHandSetMicrogestures,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "action_tap_thumb",
            "Thumb Tap");

        // EXT_hand_interaction suggested bindings
        OXR(xrStringToPath(
            instance,
            "/interaction_profiles/ext/hand_interaction_ext",
            &ExtHandInteractionProfile));

        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionLeftHandSwipeLeft, "/user/hand/left/input/swipe_left_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionLeftHandSwipeRight, "/user/hand/left/input/swipe_right_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionLeftHandSwipeForward, "/user/hand/left/input/swipe_forward_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionLeftHandSwipeBackward, "/user/hand/left/input/swipe_backward_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionLeftHandTapThumb, "/user/hand/left/input/tap_thumb_meta/click"));

        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionRightHandSwipeLeft, "/user/hand/right/input/swipe_left_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionRightHandSwipeRight, "/user/hand/right/input/swipe_right_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionRightHandSwipeForward, "/user/hand/right/input/swipe_forward_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionRightHandSwipeBackward, "/user/hand/right/input/swipe_backward_meta/click"));
        suggestedBindings[ExtHandInteractionProfile].emplace_back(ActionSuggestedBinding(
            ActionRightHandTapThumb, "/user/hand/right/input/tap_thumb_meta/click"));

        return suggestedBindings;
    }

    // Must return true if the application initializes successfully.
    virtual bool AppInit(const xrJava* context) override {
        int fontVertexBufferSize = 32 * 1024; // Custom large text buffer size for all the text
        bool updateColors = true; // Update UI colors on interaction
        if (false == Ui.Init(context, GetFileSys(), updateColors, fontVertexBufferSize)) {
            ALOG("TinyUI::Init FAILED.");
            return false;
        }

        auto fileSys = std::unique_ptr<OVRFW::ovrFileSys>(OVRFW::ovrFileSys::Create(*context));
        if (fileSys) {
            std::string environmentPath = "apk:///assets/SmallRoom.gltf.ovrscene";
            EnvironmentRenderer.Init(environmentPath, fileSys.get());
            std::string skyboxPath = "apk:///assets/Skybox.gltf.ovrscene";
            SkyboxRenderer.Init(skyboxPath, fileSys.get());
        }

        // Inspect hand tracking system properties
        XrSystemHandTrackingPropertiesEXT handTrackingSystemProperties{
            XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT};
        XrSystemProperties systemProperties{
            XR_TYPE_SYSTEM_PROPERTIES, &handTrackingSystemProperties};
        OXR(xrGetSystemProperties(GetInstance(), GetSystemId(), &systemProperties));
        if (!handTrackingSystemProperties.supportsHandTracking) {
            // The system does not support hand tracking
            ALOG("[XrMicrogestures] System does not support hand tracking");
            return false;
        } else {
            ALOG(
                "[XrMicrogestures] "
                "xrGetSystemProperties XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT OK, "
                "Initializing hand tracking...");
        }

        // Hook up extensions for hand tracking
        OXR(xrGetInstanceProcAddr(
            GetInstance(),
            "xrCreateHandTrackerEXT",
            (PFN_xrVoidFunction*)(&XrCreateHandTrackerExt)));
        OXR(xrGetInstanceProcAddr(
            GetInstance(),
            "xrDestroyHandTrackerEXT",
            (PFN_xrVoidFunction*)(&XrDestroyHandTrackerExt)));
        OXR(xrGetInstanceProcAddr(
            GetInstance(), "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)(&XrLocateHandJointsExt)));

        // Hook up extensions for hand rendering
        OXR(xrGetInstanceProcAddr(
            GetInstance(), "xrGetHandMeshFB", (PFN_xrVoidFunction*)(&XrGetHandMeshFb)));

        return true;
    }

    virtual void AppShutdown(const xrJava* context) override {
        // unhook extensions for hand tracking
        XrCreateHandTrackerExt = nullptr;
        XrDestroyHandTrackerExt = nullptr;
        XrLocateHandJointsExt = nullptr;
        XrGetHandMeshFb = nullptr;

        OVRFW::XrApp::AppShutdown(context);
        Ui.Shutdown();
    }

    virtual bool SessionInit() override {
        BeamRenderer.Init(GetFileSys(), nullptr, OVR::Vector4f(1.0f), 1.0f);

        {
            // Attach ActionSets to session
            // This is required before any call to xrSyncActions for these action sets
            std::vector<XrActionSet> actionSets{
                ActionLeftHandSetMicrogestures, ActionRightHandSetMicrogestures};
            XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
            attachInfo.countActionSets = actionSets.size();
            attachInfo.actionSets = actionSets.data();
            OXR(xrAttachSessionActionSets(Session, &attachInfo));
            // After this point all actions and bindings are final for the session
            // (calls to xrSuggestInteractionProfileBindings and xrAttachSessionActionSets fail)
        }

        {
            // Setup all the UI panels to display the state of each action
            MgActionSetPanels.insert(
                {ActionLeftHandSetMicrogestures,
                 ActionSetDisplayPanel(
                     "LEFT HAND", Session, Instance, &Ui, {-1.7_m, 2.9_m, -3.5_m})});
            MgActionSetPanels.at(ActionLeftHandSetMicrogestures)
                .AddBoolAction(ActionLeftHandSwipeLeft, "Left Hand Swipe Left");
            MgActionSetPanels.at(ActionLeftHandSetMicrogestures)
                .AddBoolAction(ActionLeftHandSwipeRight, "Left Hand Swipe Right");
            MgActionSetPanels.at(ActionLeftHandSetMicrogestures)
                .AddBoolAction(ActionLeftHandSwipeForward, "Left Hand Swipe Forward");
            MgActionSetPanels.at(ActionLeftHandSetMicrogestures)
                .AddBoolAction(ActionLeftHandSwipeBackward, "Left Hand Swipe Backward");
            MgActionSetPanels.at(ActionLeftHandSetMicrogestures)
                .AddBoolAction(ActionLeftHandTapThumb, "Left Hand Tap Thumb");

            MgActionSetPanels.insert(
                {ActionRightHandSetMicrogestures,
                 ActionSetDisplayPanel(
                     "RIGHT HAND", Session, Instance, &Ui, {0.2_m, 2.9_m, -3.5_m})});
            MgActionSetPanels.at(ActionRightHandSetMicrogestures)
                .AddBoolAction(ActionRightHandSwipeLeft, "Right Hand Swipe Left");
            MgActionSetPanels.at(ActionRightHandSetMicrogestures)
                .AddBoolAction(ActionRightHandSwipeRight, "Right Hand Swipe Right");
            MgActionSetPanels.at(ActionRightHandSetMicrogestures)
                .AddBoolAction(ActionRightHandSwipeForward, "Right Hand Swipe Forward");
            MgActionSetPanels.at(ActionRightHandSetMicrogestures)
                .AddBoolAction(ActionRightHandSwipeBackward, "Right Hand Swipe Backward");
            MgActionSetPanels.at(ActionRightHandSetMicrogestures)
                .AddBoolAction(ActionRightHandTapThumb, "Right Hand Tap Thumb");
        }

        // Hand Trackers
        if (XrCreateHandTrackerExt) {
            XrHandTrackerCreateInfoEXT createInfo{XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT};
            createInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
            createInfo.hand = XR_HAND_LEFT_EXT;
            OXR(XrCreateHandTrackerExt(GetSession(), &createInfo, &HandTrackerL));
            createInfo.hand = XR_HAND_RIGHT_EXT;
            OXR(XrCreateHandTrackerExt(GetSession(), &createInfo, &HandTrackerR));

            ALOG("xrCreateHandTrackerExt HandTrackerL=%llx", (long long)HandTrackerL);
            ALOG("xrCreateHandTrackerExt HandTrackerR=%llx", (long long)HandTrackerR);

            // Setup skinning meshes for both hands
            if (XrGetHandMeshFb) {
                for (int handIndex = 0; handIndex < 2; ++handIndex) {
                    // Alias everything for initialization
                    const bool isLeft = (handIndex == 0);
                    auto& handTracker = isLeft ? HandTrackerL : HandTrackerR;
                    auto& handRenderer = isLeft ? HandRendererL : HandRendererR;

                    // two-call pattern for mesh data
                    // call 1 - figure out sizes

                    // mesh
                    XrHandTrackingMeshFB mesh{XR_TYPE_HAND_TRACKING_MESH_FB};
                    mesh.next = nullptr;
                    // mesh - skeleton
                    mesh.jointCapacityInput = 0;
                    mesh.jointCountOutput = 0;
                    mesh.jointBindPoses = nullptr;
                    mesh.jointRadii = nullptr;
                    mesh.jointParents = nullptr;
                    // mesh - vertex
                    mesh.vertexCapacityInput = 0;
                    mesh.vertexCountOutput = 0;
                    mesh.vertexPositions = nullptr;
                    mesh.vertexNormals = nullptr;
                    mesh.vertexUVs = nullptr;
                    mesh.vertexBlendIndices = nullptr;
                    mesh.vertexBlendWeights = nullptr;
                    // mesh - index
                    mesh.indexCapacityInput = 0;
                    mesh.indexCountOutput = 0;
                    mesh.indices = nullptr;
                    // get mesh sizes
                    OXR(XrGetHandMeshFb(handTracker, &mesh));

                    // mesh storage - update sizes
                    mesh.jointCapacityInput = mesh.jointCountOutput;
                    mesh.vertexCapacityInput = mesh.vertexCountOutput;
                    mesh.indexCapacityInput = mesh.indexCountOutput;
                    // skeleton
                    std::vector<XrPosef> jointBindLocations;
                    std::vector<XrHandJointEXT> parentData;
                    std::vector<float> jointRadii;
                    jointBindLocations.resize(mesh.jointCountOutput);
                    parentData.resize(mesh.jointCountOutput);
                    jointRadii.resize(mesh.jointCountOutput);
                    mesh.jointBindPoses = jointBindLocations.data();
                    mesh.jointParents = parentData.data();
                    mesh.jointRadii = jointRadii.data();
                    // vertex
                    std::vector<XrVector3f> vertexPositions;
                    std::vector<XrVector3f> vertexNormals;
                    std::vector<XrVector2f> vertexUVs;
                    std::vector<XrVector4sFB> vertexBlendIndices;
                    std::vector<XrVector4f> vertexBlendWeights;
                    vertexPositions.resize(mesh.vertexCountOutput);
                    vertexNormals.resize(mesh.vertexCountOutput);
                    vertexUVs.resize(mesh.vertexCountOutput);
                    vertexBlendIndices.resize(mesh.vertexCountOutput);
                    vertexBlendWeights.resize(mesh.vertexCountOutput);
                    mesh.vertexPositions = vertexPositions.data();
                    mesh.vertexNormals = vertexNormals.data();
                    mesh.vertexUVs = vertexUVs.data();
                    mesh.vertexBlendIndices = vertexBlendIndices.data();
                    mesh.vertexBlendWeights = vertexBlendWeights.data();
                    // index
                    std::vector<int16_t> indices;
                    indices.resize(mesh.indexCountOutput);
                    mesh.indices = indices.data();

                    mesh.next = nullptr;

                    // get mesh data
                    OXR(XrGetHandMeshFb(handTracker, &mesh));
                    // init renderer
                    handRenderer.Init(&mesh, true);
                }
            }
        }

        return true;
    }

    virtual void SessionEnd() override {
        EnvironmentRenderer.Shutdown();
        SkyboxRenderer.Shutdown();

        // Hand Tracker
        if (XrDestroyHandTrackerExt) {
            OXR(XrDestroyHandTrackerExt(HandTrackerL));
            OXR(XrDestroyHandTrackerExt(HandTrackerR));
        }

        BeamRenderer.Shutdown();
        HandRendererL.Shutdown();
        HandRendererR.Shutdown();
    }

    // Update state
    virtual void Update(const OVRFW::ovrApplFrameIn& in) override {
        {
            // xrSyncAction
            std::vector<XrActiveActionSet> activeActionSets = {
                {ActionLeftHandSetMicrogestures}, {ActionRightHandSetMicrogestures}};

            XrActionsSyncInfo syncInfo = {XR_TYPE_ACTIONS_SYNC_INFO};
            syncInfo.countActiveActionSets = activeActionSets.size();
            syncInfo.activeActionSets = activeActionSets.data();
            OXR(xrSyncActions(Session, &syncInfo));
        }

        Ui.HitTestDevices().clear();

        // Hands
        if (XrLocateHandJointsExt) {
            // L
            XrHandTrackingScaleFB scaleL{XR_TYPE_HAND_TRACKING_SCALE_FB};
            scaleL.next = nullptr;
            scaleL.sensorOutput = 1.0f;
            scaleL.currentOutput = 1.0f;
            scaleL.overrideValueInput = 1.00f;
            scaleL.overrideHandScale = XR_FALSE;
            XrHandTrackingAimStateFB aimStateL{XR_TYPE_HAND_TRACKING_AIM_STATE_FB};
            aimStateL.next = &scaleL;
            XrHandJointVelocitiesEXT velocitiesL{XR_TYPE_HAND_JOINT_VELOCITIES_EXT};
            velocitiesL.next = &aimStateL;
            velocitiesL.jointCount = XR_HAND_JOINT_COUNT_EXT;
            velocitiesL.jointVelocities = JointVelocitiesL;
            XrHandJointLocationsEXT locationsL{XR_TYPE_HAND_JOINT_LOCATIONS_EXT};
            locationsL.next = &velocitiesL;
            locationsL.jointCount = XR_HAND_JOINT_COUNT_EXT;
            locationsL.jointLocations = JointLocationsL;
            // R
            XrHandTrackingScaleFB scaleR{XR_TYPE_HAND_TRACKING_SCALE_FB};
            scaleR.next = nullptr;
            scaleR.sensorOutput = 1.0f;
            scaleR.currentOutput = 1.0f;
            scaleR.overrideValueInput = 1.00f;
            scaleR.overrideHandScale = XR_FALSE;
            XrHandTrackingAimStateFB aimStateR{XR_TYPE_HAND_TRACKING_AIM_STATE_FB};
            aimStateR.next = &scaleR;
            XrHandJointVelocitiesEXT velocitiesR{XR_TYPE_HAND_JOINT_VELOCITIES_EXT};
            velocitiesR.next = &aimStateR;
            velocitiesR.jointCount = XR_HAND_JOINT_COUNT_EXT;
            velocitiesR.jointVelocities = JointVelocitiesR;
            XrHandJointLocationsEXT locationsR{XR_TYPE_HAND_JOINT_LOCATIONS_EXT};
            locationsR.next = &velocitiesR;
            locationsR.jointCount = XR_HAND_JOINT_COUNT_EXT;
            locationsR.jointLocations = JointLocationsR;

            XrHandJointsLocateInfoEXT locateInfoL{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
            locateInfoL.baseSpace = GetStageSpace();
            locateInfoL.time = ToXrTime(in.PredictedDisplayTime);
            OXR(XrLocateHandJointsExt(HandTrackerL, &locateInfoL, &locationsL));

            XrHandJointsLocateInfoEXT locateInfoR{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
            locateInfoR.baseSpace = GetStageSpace();
            locateInfoR.time = ToXrTime(in.PredictedDisplayTime);
            OXR(XrLocateHandJointsExt(HandTrackerR, &locateInfoR, &locationsR));

            std::vector<OVR::Posef> handJointsL;
            std::vector<OVR::Posef> handJointsR;

            // Tracked joints and computed joints can all be valid
            XrSpaceLocationFlags isValid =
                XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;

            HandTrackedL = false;
            HandTrackedR = false;

            if (locationsL.isActive) {
                for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; ++i) {
                    if ((JointLocationsL[i].locationFlags & isValid) != 0) {
                        const auto p = FromXrPosef(JointLocationsL[i].pose);
                        handJointsL.push_back(p);
                        HandTrackedL = true;
                    }
                }
                HandRendererL.Update(&JointLocationsL[0]);
            }
            if (locationsR.isActive) {
                for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; ++i) {
                    if ((JointLocationsR[i].locationFlags & isValid) != 0) {
                        const auto p = FromXrPosef(JointLocationsR[i].pose);
                        handJointsR.push_back(p);
                        HandTrackedR = true;
                    }
                }
                HandRendererR.Update(&JointLocationsR[0]);
            }
        }

        for (auto& panelPair : MgActionSetPanels) {
            panelPair.second.Update();
        }

        Ui.Update(in);
        BeamRenderer.Update(in, Ui.HitTestDevices());
    }

    // Render eye buffers while running
    virtual void Render(const OVRFW::ovrApplFrameIn& in, OVRFW::ovrRendererOutput& out) override {
        SkyboxRenderer.Render(out.Surfaces);
        EnvironmentRenderer.Render(out.Surfaces);
        Ui.Render(in, out);

        // Render hand axes
        if (HandTrackedL) {
            HandRendererL.Render(out.Surfaces);
        }
        if (HandTrackedR) {
            HandRendererR.Render(out.Surfaces);
        }
        // Render beams last, since they render with transparency (alpha blending)
        BeamRenderer.Render(in, out);
    }

   public:
    // Hands - extension functions
    PFN_xrCreateHandTrackerEXT XrCreateHandTrackerExt = nullptr;
    PFN_xrDestroyHandTrackerEXT XrDestroyHandTrackerExt = nullptr;
    PFN_xrLocateHandJointsEXT XrLocateHandJointsExt = nullptr;
    // Hands - FB mesh rendering extensions
    PFN_xrGetHandMeshFB XrGetHandMeshFb = nullptr;
    // Hands - tracker handles
    XrHandTrackerEXT HandTrackerL = XR_NULL_HANDLE;
    XrHandTrackerEXT HandTrackerR = XR_NULL_HANDLE;
    // Hands - data buffers
    XrHandJointLocationEXT JointLocationsL[XR_HAND_JOINT_COUNT_EXT];
    XrHandJointLocationEXT JointLocationsR[XR_HAND_JOINT_COUNT_EXT];
    XrHandJointVelocityEXT JointVelocitiesL[XR_HAND_JOINT_COUNT_EXT];
    XrHandJointVelocityEXT JointVelocitiesR[XR_HAND_JOINT_COUNT_EXT];

   private:
    OVRFW::EnvironmentRenderer EnvironmentRenderer;
    OVRFW::SkyboxRenderer SkyboxRenderer;
    OVRFW::TinyUI Ui;
    OVRFW::SimpleBeamRenderer BeamRenderer;

    OVRFW::HandRenderer HandRendererL;
    OVRFW::HandRenderer HandRendererR;
    bool HandTrackedL{false};
    bool HandTrackedR{false};

    XrPath ExtHandInteractionProfile{XR_NULL_PATH};
    XrPath LeftHandPath{XR_NULL_PATH};
    XrPath RightHandPath{XR_NULL_PATH};

    XrActionSet ActionLeftHandSetMicrogestures{XR_NULL_HANDLE};
    XrAction ActionLeftHandSwipeLeft{XR_NULL_HANDLE};
    XrAction ActionLeftHandSwipeRight{XR_NULL_HANDLE};
    XrAction ActionLeftHandSwipeForward{XR_NULL_HANDLE};
    XrAction ActionLeftHandSwipeBackward{XR_NULL_HANDLE};
    XrAction ActionLeftHandTapThumb{XR_NULL_HANDLE};

    XrActionSet ActionRightHandSetMicrogestures{XR_NULL_HANDLE};
    XrAction ActionRightHandSwipeLeft{XR_NULL_HANDLE};
    XrAction ActionRightHandSwipeRight{XR_NULL_HANDLE};
    XrAction ActionRightHandSwipeForward{XR_NULL_HANDLE};
    XrAction ActionRightHandSwipeBackward{XR_NULL_HANDLE};
    XrAction ActionRightHandTapThumb{XR_NULL_HANDLE};

    std::unordered_map<XrActionSet, ActionSetDisplayPanel> MgActionSetPanels{};
};

ENTRY_POINT(XrMicrogesturesApp)
