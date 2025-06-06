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
Created     :
Authors     :
Language    :   C++
Copyright:  Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

*******************************************************************************/

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Anchor.h"
#include "CubesRenderer.h"
#include "HexStringHelper.h"
#include "OVR_JSON.h"
#include "SimpleListUI.h"
#include "XrApp.h"
#include "Input/TinyUI.h"
#include "Input/ControllerRenderer.h"
#include "Misc/Log.h"
#include "Render/SimpleBeamRenderer.h"

#include <meta_openxr_preview/openxr_oculus_helpers.h>
#include <openxr/openxr.h>

#define DECLARE_FUNCTION_PTR(functionName) PFN_##functionName functionName = nullptr;
#define HOOKUP_FUNCTION_PTR(functionName) \
    OXR(xrGetInstanceProcAddr(GetInstance(), #functionName, (PFN_xrVoidFunction*)(&functionName)));
#define VALIDATE_FUNCTION_PTR(functionName)                                              \
    if (functionName == nullptr) {                                                       \
        Logs->AppendRow("Skipping: this app does not currently support " #functionName); \
        return;                                                                          \
    }
#define K_MAX_PERSISTENT_SPACES 32

class XrColocationDiscoveryApp : public OVRFW::XrApp {
   public:
    XrColocationDiscoveryApp() : OVRFW::XrApp() {
        BackgroundColor = OVR::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
        // Seed the random number generator with the current time, rand() is called in GenerateRandomGroupUUID()
        // and GenerateRandomColor()
        srand((unsigned int)time(nullptr));
    }

    // Must return true if the application initializes successfully.
    virtual bool AppInit(const xrJava* context) override {
        if (false == Ui.Init(context, GetFileSys())) {
            ALOG("TinyUI::Init FAILED.");
            return false;
        }
        AnchorsRenderer = std::make_unique<CubesRenderer>();

        HookExtensions();
        /// Build UI
        Logs = std::make_unique<SimpleListUI>(
            Ui, OVR::Vector3f(0.2f, 1.0f, -2.0f), OVR::Vector2f(700.0f, 1000.0f), 10, "Logs");

        Results = std::make_unique<SimpleListUI>(
            Ui, OVR::Vector3f(1.7f, 1.0f, -2.0f), OVR::Vector2f(700.0f, 1000.0f), 10, "");

        Banner = Ui.AddLabel(
            "Click \"A\" on your right controller to place a shared anchor.\n"
            "Once the shared anchor is created, you may start an advertisement to other nearby devices.\n"
            "When nearby devices discover you, they will attempt to localize your shared anchor.",
            OVR::Vector3f(0.2f, 2.2f, -2.0f),
            OVR::Vector2f(1400.0f, 150.0f));

        const OVR::Vector2f buttonSize = {400.0f, 150.0f};
        const float buttonXPos = -1.0f;
        const float buttonZPos = -2.0f;

        StartDiscoveryButton = Ui.AddButton(
            "Start Colocation Discovery", {buttonXPos, 1.9f, buttonZPos}, buttonSize, [this]() {
                StartDiscovery();
            });
        StopDiscoveryButton = Ui.AddButton(
            "Stop Colocation Discovery", {buttonXPos, 1.9f, buttonZPos}, buttonSize, [this]() {
                StopDiscovery();
            });
        StartAdvertisementButton = Ui.AddButton(
            "Start Colocation Advertisement", {buttonXPos, 1.6f, buttonZPos}, buttonSize, [this]() {
                StartColocationAdvertisement();
            });
        StopAdvertisementButton = Ui.AddButton(
            "Stop Colocation Advertisement", {buttonXPos, 1.6f, buttonZPos}, buttonSize, [this]() {
                StopColocationAdvertisement();
            });

        return true;
    }

    virtual void AppShutdown(const xrJava* context) override {
        OVRFW::XrApp::AppShutdown(context);
        Ui.Shutdown();
        AnchorsRenderer->Shutdown();
    }

    virtual void PreProjectionAddLayer(xrCompositorLayerUnion* layers, int& layerCount) override {
        AddPassthroughLayer(layers, layerCount);
    }

    // Returns a list of OpenXR extensions requested for this app
    // Note that the sample framework will filter out any extension
    // that is not listed as supported.
    virtual std::vector<const char*> GetExtensions() override {
        std::vector<const char*> extensions = XrApp::GetExtensions();
        extensions.push_back(XR_FB_PASSTHROUGH_EXTENSION_NAME);
        extensions.push_back(XR_META_COLOCATION_DISCOVERY_EXTENSION_NAME);
        extensions.push_back(XR_FB_SPATIAL_ENTITY_EXTENSION_NAME);
        extensions.push_back(XR_FB_SPATIAL_ENTITY_QUERY_EXTENSION_NAME);
        extensions.push_back(XR_META_SPATIAL_ENTITY_SHARING_EXTENSION_NAME);
        extensions.push_back(XR_META_SPATIAL_ENTITY_GROUP_SHARING_EXTENSION_NAME);

        return extensions;
    }

    void AddPassthroughLayer(xrCompositorLayerUnion* layers, int& layerCount) {
        if (PassthroughLayer != XR_NULL_HANDLE) {
            XrCompositionLayerPassthroughFB passthroughLayer = {
                XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB};
            passthroughLayer.layerHandle = PassthroughLayer;
            passthroughLayer.flags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
            passthroughLayer.space = XR_NULL_HANDLE;
            layers[layerCount++].Passthrough = passthroughLayer;
        }
    }

    virtual bool SessionInit() override {
        /// Init session bound objects
        if (false == ControllerRenderL.Init(true)) {
            ALOG("AppInit::Init L controller renderer FAILED.");
            return false;
        }
        if (false == ControllerRenderR.Init(false)) {
            ALOG("AppInit::Init R controller renderer FAILED.");
            return false;
        }
        BeamRenderer.Init(GetFileSys(), nullptr, OVR::Vector4f(1.0f), 1.0f);

        // Create and start passthrough
        {
            XrPassthroughCreateInfoFB ptci{XR_TYPE_PASSTHROUGH_CREATE_INFO_FB};
            XrResult result;
            OXR(result = xrCreatePassthroughFB(GetSession(), &ptci, &Passthrough));

            if (XR_SUCCEEDED(result)) {
                ALOGV("Creating passthrough layer");
                XrPassthroughLayerCreateInfoFB plci{XR_TYPE_PASSTHROUGH_LAYER_CREATE_INFO_FB};
                plci.passthrough = Passthrough;
                plci.purpose = XR_PASSTHROUGH_LAYER_PURPOSE_RECONSTRUCTION_FB;
                OXR(xrCreatePassthroughLayerFB(GetSession(), &plci, &PassthroughLayer));

                ALOGV("Setting passthrough style");
                XrPassthroughStyleFB style{XR_TYPE_PASSTHROUGH_STYLE_FB};
                OXR(xrPassthroughLayerResumeFB(PassthroughLayer));
                style.textureOpacityFactor = 0.5f;
                style.edgeColor = {0.0f, 0.0f, 0.0f, 0.0f};
                OXR(xrPassthroughLayerSetStyleFB(PassthroughLayer, &style));
            } else {
                ALOGV("Create passthrough failed");
            }
            if (result != XR_ERROR_FEATURE_UNSUPPORTED) {
                OXR(result = xrPassthroughStartFB(Passthrough));
            }
        }
        return true;
    }

    virtual void SessionEnd() override {
        ControllerRenderL.Shutdown();
        ControllerRenderR.Shutdown();
        BeamRenderer.Shutdown();
    }

    // Update state
    virtual void Update(const OVRFW::ovrApplFrameIn& in) override {
        Ui.HitTestDevices().clear();

        if (in.LeftRemoteTracked) {
            ControllerRenderL.Update(in.LeftRemotePose);
            const bool didPinch = in.LeftRemoteIndexTrigger > 0.5f;
            Ui.AddHitTestRay(in.LeftRemotePointPose, didPinch);
        }
        if (in.RightRemoteTracked) {
            ControllerRenderR.Update(in.RightRemotePose);
            const bool didPinch = in.RightRemoteIndexTrigger > 0.5f;
            Ui.AddHitTestRay(in.RightRemotePointPose, didPinch);
        }

        Ui.Update(in);
        BeamRenderer.Update(in, Ui.HitTestDevices());

        if (in.Clicked(in.kButtonA)) {
            ALOG("\'A\' button is clicked, creating a shared anchor");
            CreateAndShareAnchor(in.RightRemotePose, ToXrTime(in.PredictedDisplayTime));
        }

        StartAdvertisementButton->SetVisible(false);
        StopAdvertisementButton->SetVisible(false);

        StartDiscoveryButton->SetVisible(IsStartDiscoveryButtonEnabled);
        StopDiscoveryButton->SetVisible(!IsStartDiscoveryButtonEnabled);

        if (MyAnchor.has_value() && IsAnchorShared) {
            StartAdvertisementButton->SetVisible(IsStartAdvertisementButtonEnabled);
            StopAdvertisementButton->SetVisible(!IsStartAdvertisementButtonEnabled);
        }

        UpdateAnchorPoses(in);
    }

    void UpdateAnchorPoses(const OVRFW::ovrApplFrameIn& in) {
        // Locate the space
        // If anchor was placed, just update the anchor location
        // Updating it regularly will prevent drift
        std::vector<AnchorPose> poses;
        if (MyAnchor.has_value() && MyAnchor.value().Space.has_value()) {
            const auto& [uuid, space, color] = MyAnchor.value();
            XrSpaceLocation persistedAnchorLoc = {XR_TYPE_SPACE_LOCATION};
            XrResult res = XR_SUCCESS;
            OXR(res = xrLocateSpace(
                    space.value(),
                    GetCurrentSpace(),
                    ToXrTime(in.PredictedDisplayTime),
                    &persistedAnchorLoc));
            if (res == XR_SUCCESS) {
                if (ValidateLocationFlags(persistedAnchorLoc.locationFlags)) {
                    const OVR::Posef localFromPersistedAnchor =
                        FromXrPosef(persistedAnchorLoc.pose);
                    poses.push_back(AnchorPose{localFromPersistedAnchor, color});
                } else {
                    ALOGE(
                        "Failed to locate anchor pose of uuid %s : invalid locationFlags %#lx",
                        uuid.c_str(),
                        (long)persistedAnchorLoc.locationFlags);
                }
            } else {
                ALOGE("Failed locate anchor pose of uuid: %s", uuid.c_str());
            }
        }

        for (const auto& [uuid, anchor] : ReceivedAnchors) {
            const auto& [_, space, color] = anchor;
            if (!space.has_value())
                continue;
            XrSpaceLocation persistedAnchorLoc = {XR_TYPE_SPACE_LOCATION};
            XrResult res = XR_SUCCESS;
            OXR(res = xrLocateSpace(
                    space.value(),
                    GetCurrentSpace(),
                    ToXrTime(in.PredictedDisplayTime),
                    &persistedAnchorLoc));
            if (res == XR_SUCCESS) {
                if (ValidateLocationFlags(persistedAnchorLoc.locationFlags)) {
                    const OVR::Posef localFromPersistedAnchor =
                        FromXrPosef(persistedAnchorLoc.pose);
                    poses.push_back(AnchorPose{localFromPersistedAnchor, color});
                } else {
                    ALOGE(
                        "Failed to locate anchor pose of uuid %s : invalid locationFlags %#lx",
                        uuid.c_str(),
                        (long)persistedAnchorLoc.locationFlags);
                }
            } else {
                ALOGE("Failed locate anchor pose of uuid: %s", uuid.c_str());
            }
        }
        AnchorsRenderer->UpdatePoses(poses);
    }

    // Render eye buffers while running
    virtual void Render(const OVRFW::ovrApplFrameIn& in, OVRFW::ovrRendererOutput& out) override {
        /// Render UI
        Ui.Render(in, out);

        /// Render controllers
        if (in.LeftRemoteTracked) {
            ControllerRenderL.Render(out.Surfaces);
        }
        if (in.RightRemoteTracked) {
            ControllerRenderR.Render(out.Surfaces);
        }

        /// Render beams last, since they render with transparency (alpha blending)
        BeamRenderer.Render(in, out);

        /// Render placed anchors
        AnchorsRenderer->Render(out.Surfaces);
    }

    bool IsComponentSupported(XrSpace space, XrSpaceComponentTypeFB type) {
        uint32_t numComponents = 0;
        OXR(xrEnumerateSpaceSupportedComponentsFB(space, 0, &numComponents, nullptr));
        std::vector<XrSpaceComponentTypeFB> components(numComponents);
        OXR(xrEnumerateSpaceSupportedComponentsFB(
            space, numComponents, &numComponents, components.data()));

        bool supported = false;
        for (uint32_t c = 0; c < numComponents; ++c) {
            if (components[c] == type) {
                supported = true;
                break;
            }
        }
        return supported;
    }

    virtual void HandleXrEvents() override {
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
                    const XrEventDataPerfSettingsEXT* perfSettingsEvent =
                        (XrEventDataPerfSettingsEXT*)(baseEventHeader);
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
                    const XrEventDataSessionStateChanged* sessionStateChangedEvent =
                        (XrEventDataSessionStateChanged*)(baseEventHeader);
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: %d for session %p at time %f",
                        sessionStateChangedEvent->state,
                        (void*)sessionStateChangedEvent->session,
                        FromXrTime(sessionStateChangedEvent->time));

                    switch (sessionStateChangedEvent->state) {
                        case XR_SESSION_STATE_FOCUSED:
                            Focused = true;
                            break;
                        case XR_SESSION_STATE_VISIBLE:
                            Focused = false;
                            break;
                        case XR_SESSION_STATE_READY:
                            HandleSessionStateChanges(sessionStateChangedEvent->state);
                            break;
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

                case XR_TYPE_EVENT_DATA_START_COLOCATION_DISCOVERY_COMPLETE_META: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_START_COLOCATION_DISCOVERY_COMPLETE_META");
                    const XrEventDataStartColocationDiscoveryCompleteMETA* started =
                        (XrEventDataStartColocationDiscoveryCompleteMETA*)(baseEventHeader);
                    if (started->result == XR_SUCCESS) {
                        Logs->AppendRow("Start Colocation Discovery complete");
                    } else {
                        Logs->AppendRow("Start Colocation Discovery failed with code: " + std::to_string(started->result));
                        IsStartDiscoveryButtonEnabled = true;
                    }
                } break;
                case XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_RESULT_META: {
                    ALOGV("xrPollEvent: received XR_TYPE_EVENT_COLOCATION_DISCOVERY_RESULT_META");
                    const XrEventDataColocationDiscoveryResultMETA* result =
                        (XrEventDataColocationDiscoveryResultMETA*)(baseEventHeader);
                    auto advertisementUuid =
                        HexStringHelper::UuidToHexString(result->advertisementUuid);
                    Logs->AppendRow("Advertisement UUID: " + advertisementUuid);
                    auto metadata = std::string((char*)(result->buffer), result->bufferSize);

                    const auto root = OVR::JSON::Parse(metadata.c_str());

                    std::string groupUuidStr = root->GetItemByName("group_uuid")->GetStringValue();
                    Results->AppendRow("Discovered Group: " + groupUuidStr);
                    std::string anchorUuidStr = root->GetItemByName("anchor_uuid")->GetStringValue();
                    Results->AppendRow(std::string("Discovered Anchor: ").append(anchorUuidStr));
                    const auto anchorColor = root->GetItemByName("anchor_color");
                    OVR::Vector4f colorVec = OVR::Vector4f(
                        anchorColor->GetItemByName("x")->GetDoubleValue(),
                        anchorColor->GetItemByName("y")->GetDoubleValue(),
                        anchorColor->GetItemByName("z")->GetDoubleValue(),
                        anchorColor->GetItemByName("w")->GetDoubleValue());

                    ReceivedAnchors[anchorUuidStr] = Anchor{anchorUuidStr, std::nullopt, colorVec};
                    XrUuidEXT groupUuid;
                    HexStringHelper::HexStringToUuid(groupUuidStr, groupUuid);
                    QueryAnchors(groupUuid);
                } break;
                case XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_COMPLETE_META: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_COMPLETE_META");
                    const XrEventDataColocationDiscoveryCompleteMETA* complete =
                        (XrEventDataColocationDiscoveryCompleteMETA*)(baseEventHeader);
                    if (complete->result == XR_SUCCESS) {
                        Logs->AppendRow("Colocation Discovery Complete");
                    } else {
                        Logs->AppendRow(
                            "Colocation Discovery Complete with code: " +
                            std::to_string(complete->result));
                    }
                    IsStartDiscoveryButtonEnabled = true;
                } break;
                case XR_TYPE_EVENT_DATA_STOP_COLOCATION_DISCOVERY_COMPLETE_META: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_STOP_COLOCATION_DISCOVERY_COMPLETE_META");
                    const XrEventDataStopColocationDiscoveryCompleteMETA* complete =
                        (XrEventDataStopColocationDiscoveryCompleteMETA*)(baseEventHeader);
                    if (complete->result == XR_SUCCESS) {
                        Logs->AppendRow("Stop Colocation Discovery Complete");
                        IsStartDiscoveryButtonEnabled = true;
                    } else {
                        Logs->AppendRow("Stop Colocation Discovery Failed to Complete");
                        IsStartDiscoveryButtonEnabled = false;
                    }
                } break;
                case XR_TYPE_EVENT_DATA_START_COLOCATION_ADVERTISEMENT_COMPLETE_META: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_ENABLE_COLOCATION_ADVERTISEMENT_COMPLETE_META");
                    const XrEventDataStartColocationAdvertisementCompleteMETA* complete =
                        (XrEventDataStartColocationAdvertisementCompleteMETA*)(baseEventHeader);
                    if (complete->result == XR_SUCCESS) {
                        Logs->AppendRow("Start Colocation Advertisement complete");
                        Logs->AppendRow(
                            "Advertisement UUID: " +
                            HexStringHelper::UuidToHexString(complete->advertisementUuid));
                        IsStartAdvertisementButtonEnabled = false;
                    } else {
                        Logs->AppendRow("Start Colocation Advertisement failed with code: " + std::to_string(complete->result));
                        IsStartAdvertisementButtonEnabled = true;
                    }
                } break;
                case XR_TYPE_EVENT_DATA_COLOCATION_ADVERTISEMENT_COMPLETE_META: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_COLOCATION_ADVERTISEMENT_COMPLETE_META");
                    const XrEventDataColocationAdvertisementCompleteMETA* complete =
                        (XrEventDataColocationAdvertisementCompleteMETA*)(baseEventHeader);
                    if (complete->result == XR_SUCCESS) {
                        Logs->AppendRow("Colocation Advertisement Complete");
                    } else {
                        Logs->AppendRow(
                            "Colocation Advertisement Complete with code: " +
                            std::to_string(complete->result));
                    }
                    IsStartAdvertisementButtonEnabled = true;
                } break;
                case XR_TYPE_EVENT_DATA_STOP_COLOCATION_ADVERTISEMENT_COMPLETE_META: {
                    ALOG(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_STOP_COLOCATION_ADVERTISEMENT_COMPLETE_META");
                    const XrEventDataStopColocationAdvertisementCompleteMETA* complete =
                        (XrEventDataStopColocationAdvertisementCompleteMETA*)(baseEventHeader);
                    if (complete->result == XR_SUCCESS) {
                        Logs->AppendRow("Stop Colocation Advertisement complete");
                        IsStartAdvertisementButtonEnabled = true;
                    } else {
                        Logs->AppendRow("Stop Colocation Advertisement failed with code: " + std::to_string(complete->result));
                        IsStartAdvertisementButtonEnabled = false;
                    }
                } break;
                case XR_TYPE_EVENT_DATA_SPATIAL_ANCHOR_CREATE_COMPLETE_FB: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_SPATIAL_ANCHOR_CREATE_COMPLETE_FB");
                    const XrEventDataSpatialAnchorCreateCompleteFB* createAnchorResult =
                        (XrEventDataSpatialAnchorCreateCompleteFB*)(baseEventHeader);
                    if (createAnchorResult->result != XR_SUCCESS) {
                        Logs->AppendRow("Failed to create spatial anchor!!!");
                        break;
                    }
                    ALOGV("Success to create spatial anchor");
                    XrSpace space = createAnchorResult->space;
                    if (space == XR_NULL_HANDLE) {
                        ALOG("Failed to create spatial anchor:  Generated anchor's space is null");
                        Logs->AppendRow(
                            "Failed to create spatial anchor: Generated anchor's space is null");
                        break;
                    }
                    auto spaceUuid = HexStringHelper::UuidToHexString(createAnchorResult->uuid);
                    auto color = GenerateRandomColor();
                    MyAnchor = Anchor{spaceUuid, space, color};

                    if (IsComponentSupported(space, XR_SPACE_COMPONENT_TYPE_STORABLE_FB)) {
                        XrSpaceComponentStatusSetInfoFB request = {
                            XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB,
                            nullptr,
                            XR_SPACE_COMPONENT_TYPE_STORABLE_FB,
                            XR_TRUE,
                            0};

                        XrAsyncRequestIdFB requestId;
                        XrResult res = xrSetSpaceComponentStatusFB(space, &request, &requestId);
                        if (res == XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB) {
                            ALOGV("SpatialAnchorXr: Space component STORABLE already set!");
                        }
                    } else {
                        Logs->AppendRow("Error: Anchor is not storable");
                        MyAnchor = std::nullopt;
                        return;
                    }

                    if (IsComponentSupported(space, XR_SPACE_COMPONENT_TYPE_SHARABLE_FB)) {
                        XrSpaceComponentStatusSetInfoFB request = {
                            XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB,
                            nullptr,
                            XR_SPACE_COMPONENT_TYPE_SHARABLE_FB,
                            XR_TRUE,
                            0};

                        XrAsyncRequestIdFB requestId;
                        XrResult res = xrSetSpaceComponentStatusFB(space, &request, &requestId);
                        if (res == XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB) {
                            ALOGV("SpatialAnchorXr: Space component SHARABLE already set!");
                        }
                    } else {
                        Logs->AppendRow("Error: Anchor is not sharable");
                        MyAnchor = std::nullopt;
                        return;
                    }

                    ShareAnchor(space);
                } break;

                case XR_TYPE_EVENT_DATA_SPACE_SET_STATUS_COMPLETE_FB: {
                    const XrEventDataSpaceSetStatusCompleteFB* enableResult =
                        (XrEventDataSpaceSetStatusCompleteFB*)(baseEventHeader);
                    if (enableResult->result == XR_SUCCESS) {
                        ALOGV(
                            "xrPollEvent: XR_TYPE_EVENT_DATA_SPACE_SET_STATUS_COMPLETE_FB success");
                        return;
                    }
                    ALOGV("xrPollEvent: XR_TYPE_EVENT_DATA_SPACE_SET_STATUS_COMPLETE_FB failed");

                } break;

                case XR_TYPE_EVENT_DATA_SHARE_SPACES_COMPLETE_META: {
                    ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_SHARE_SPACES_COMPLETE_META");
                    const XrEventDataShareSpacesCompleteMETA* shareResult =
                        (XrEventDataShareSpacesCompleteMETA*)(baseEventHeader);
                    if (shareResult->result == XR_SUCCESS) {
                        Logs->AppendRow("Successfully shared anchor!");
                        IsAnchorShared = true;
                    } else {
                        Logs->AppendRow(
                            "Share Space failed with code: " + std::to_string(shareResult->result));
                        MyAnchor = std::nullopt;
                        GroupUuid = std::nullopt;
                    }
                } break;

                case XR_TYPE_EVENT_DATA_SPACE_QUERY_RESULTS_AVAILABLE_FB: {
                    ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_SPACE_QUERY_RESULTS_AVAILABLE_FB");
                    const auto resultsAvailable =
                        (XrEventDataSpaceQueryResultsAvailableFB*)baseEventHeader;

                    XrResult res = XR_SUCCESS;
                    XrSpaceQueryResultsFB queryResults{XR_TYPE_SPACE_QUERY_RESULTS_FB};
                    queryResults.resultCapacityInput = 0;
                    queryResults.resultCountOutput = 0;
                    queryResults.results = nullptr;

                    VALIDATE_FUNCTION_PTR(xrRetrieveSpaceQueryResultsFB)
                    res = xrRetrieveSpaceQueryResultsFB(
                        Session, resultsAvailable->requestId, &queryResults);
                    if (res != XR_SUCCESS) {
                        Logs->AppendRow(
                            "Retrieve query results failed with code: " + std::to_string(res));
                        break;
                    } else {
                        ALOGV("xrRetrieveSpaceQueryResultsFB: success");
                    }

                    std::vector<XrSpaceQueryResultFB> results(queryResults.resultCountOutput);
                    queryResults.resultCapacityInput = results.size();
                    queryResults.results = results.data();

                    res = xrRetrieveSpaceQueryResultsFB(
                        Session, resultsAvailable->requestId, &queryResults);
                    if (res != XR_SUCCESS) {
                        Logs->AppendRow(
                            "Retrieve query results failed with code: " + std::to_string(res));
                        break;
                    }

                    for (uint32_t i = 0; i < queryResults.resultCountOutput; ++i) {
                        auto& result = results[i];

                        if (IsComponentSupported(
                                result.space, XR_SPACE_COMPONENT_TYPE_LOCATABLE_FB)) {
                            XrSpaceComponentStatusSetInfoFB request = {
                                XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB,
                                nullptr,
                                XR_SPACE_COMPONENT_TYPE_LOCATABLE_FB,
                                XR_TRUE,
                                0};
                            XrAsyncRequestIdFB requestId;
                            res = xrSetSpaceComponentStatusFB(result.space, &request, &requestId);
                            if (res == XR_SUCCESS ||
                                res == XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB) {
                                auto spaceUuid = HexStringHelper::UuidToHexString(result.uuid);
                                ReceivedAnchors[spaceUuid].Space = result.space;
                            }
                        }

                        if (IsComponentSupported(
                                result.space, XR_SPACE_COMPONENT_TYPE_STORABLE_FB)) {
                            XrSpaceComponentStatusSetInfoFB request = {
                                XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB,
                                nullptr,
                                XR_SPACE_COMPONENT_TYPE_STORABLE_FB,
                                XR_TRUE,
                                0};
                            XrAsyncRequestIdFB requestId;
                            res = xrSetSpaceComponentStatusFB(result.space, &request, &requestId);
                            if (res == XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB) {
                                ALOGV(
                                    "xrPollEvent: Storable component was already enabled for Space uuid: %s",
                                    HexStringHelper::UuidToHexString(result.uuid).c_str());
                            }
                        }

                        if (IsComponentSupported(
                                result.space, XR_SPACE_COMPONENT_TYPE_SHARABLE_FB)) {
                            XrSpaceComponentStatusSetInfoFB request = {
                                XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB,
                                nullptr,
                                XR_SPACE_COMPONENT_TYPE_SHARABLE_FB,
                                XR_TRUE,
                                0};
                            XrAsyncRequestIdFB requestId;
                            res = xrSetSpaceComponentStatusFB(result.space, &request, &requestId);
                            if (res == XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB) {
                                ALOGV(
                                    "xrPollEvent: Sharable component was already enabled for Space uuid: %s",
                                    HexStringHelper::UuidToHexString(result.uuid).c_str());
                            }
                        }
                    }
                } break;
                case XR_TYPE_EVENT_DATA_SPACE_QUERY_COMPLETE_FB: {
                    ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_SPACE_QUERY_COMPLETE_FB");
                } break;

                default:
                    ALOGV("xrPollEvent: Unknown event");
                    break;
            }
        }
    }

    void ClearResults() {
        Results->SetHeader("");
        Results->Clear();
    }

    std::string GetLastUpdatedJsonStr() {
        auto json = OVR::JSON::CreateObject();
        json->AddStringItem("group_uuid", HexStringHelper::UuidToHexString(GroupUuid.value()).c_str());
        json->AddStringItem("anchor_uuid", MyAnchor.value().Uuid.c_str());
        auto anchorColor = OVR::JSON::CreateObject();
        anchorColor->AddNumberItem("x", MyAnchor.value().Color.x);
        anchorColor->AddNumberItem("y", MyAnchor.value().Color.y);
        anchorColor->AddNumberItem("z", MyAnchor.value().Color.z);
        anchorColor->AddNumberItem("w", MyAnchor.value().Color.w);
        json->AddItem("anchor_color", anchorColor);
        return json->PrintValue(0, false);
    }

    void StartColocationAdvertisement() {
        VALIDATE_FUNCTION_PTR(xrStartColocationAdvertisementMETA)
        IsStartAdvertisementButtonEnabled = false;
        ALOGV("Start Colocation Advertisement");
        XrColocationAdvertisementStartInfoMETA info{
            XR_TYPE_COLOCATION_ADVERTISEMENT_START_INFO_META};
        auto metadataJson = GetLastUpdatedJsonStr();
        std::vector<uint8_t> metadata(metadataJson.size());
        std::memcpy(metadata.data(), metadataJson.data(), sizeof(uint8_t) * metadataJson.size());
        info.buffer = metadata.data();
        info.bufferSize = metadata.size();
        XrAsyncRequestIdFB requestId;
        XrResult r;
        OXR(r = xrStartColocationAdvertisementMETA(GetSession(), &info, &requestId));

        if (r == XR_SUCCESS) {
            Logs->AppendRow("Enabling Colocation Advertisement...");
        } else {
            Logs->AppendRow("Failed to call Start Colocation Advertisement: " + std::to_string(r));
            IsStartAdvertisementButtonEnabled = true;
        }
    }

    void StopColocationAdvertisement() {
        VALIDATE_FUNCTION_PTR(xrStopColocationAdvertisementMETA)
        IsStartAdvertisementButtonEnabled = true;
        ALOGV("StopColocationAdvertisement");
        XrAsyncRequestIdFB requestId;
        XrResult r;
        OXR(r = xrStopColocationAdvertisementMETA(GetSession(), nullptr, &requestId));

        if (r == XR_SUCCESS) {
            Logs->AppendRow("Disabling Colocation Advertisement...");
        } else {
            Logs->AppendRow("Failed to call Stop Colocation Advertisement: " + std::to_string(r));
            IsStartAdvertisementButtonEnabled = false;
        }
    }

    void StartDiscovery() {
        VALIDATE_FUNCTION_PTR(xrStartColocationDiscoveryMETA)
        IsStartDiscoveryButtonEnabled = false;
        XrAsyncRequestIdFB requestId;
        XrResult r;
        OXR(r = xrStartColocationDiscoveryMETA(GetSession(), nullptr, &requestId));
        if (r == XR_SUCCESS) {
            Logs->AppendRow("Starting Colocation Discovery...");
            ClearResults();
            Results->SetHeader("Discovered Advertisements");
        } else {
            Logs->AppendRow("Failed to start Colocation Discovery: " + std::to_string(r));
            IsStartDiscoveryButtonEnabled = true;
        }
    }

    void StopDiscovery() {
        VALIDATE_FUNCTION_PTR(xrStopColocationDiscoveryMETA)
        IsStartDiscoveryButtonEnabled = true;
        XrAsyncRequestIdFB requestId;
        XrResult r;
        OXR(r = xrStopColocationDiscoveryMETA(GetSession(), nullptr, &requestId));
        if (r == XR_SUCCESS) {
            Logs->AppendRow("Stopping Colocation Discovery...");
        } else {
            Logs->AppendRow("Failed to stop Colocation Discovery: " + std::to_string(r));
            IsStartDiscoveryButtonEnabled = false;
        }
    }

    void CreateAndShareAnchor(const OVR::Posef& pose, XrTime predictedDisplayTime) {
        if (MyAnchor.has_value()) {
            Logs->AppendRow("Skipping: Already created anchor.");
            return;
        }

        VALIDATE_FUNCTION_PTR(xrCreateSpatialAnchorFB)
        Logs->AppendRow("Creating shared anchor");

        XrSpatialAnchorCreateInfoFB anchorCreateInfo = {XR_TYPE_SPATIAL_ANCHOR_CREATE_INFO_FB};
        anchorCreateInfo.space = GetCurrentSpace();
        anchorCreateInfo.poseInSpace = ToXrPosef(pose);
        anchorCreateInfo.time = predictedDisplayTime;
        XrAsyncRequestIdFB createRequest;
        XrResult r;
        OXR(r = xrCreateSpatialAnchorFB(GetSession(), &anchorCreateInfo, &createRequest));
        if (r != XR_SUCCESS) {
            Logs->AppendRow("Failed to share anchors: " + std::to_string(r));
            return;
        }
        ALOGV("Place Spatial Anchor initiated.");
    }

    void ShareAnchor(XrSpace anchor) {
        VALIDATE_FUNCTION_PTR(xrShareSpacesMETA)

        XrUuidEXT groupuuid = GenerateRandomGroupUUID();
        Logs->AppendRow(
            "Sharing anchor to group UUID: " + HexStringHelper::UuidToHexString(groupuuid));

        XrShareSpacesRecipientGroupsMETA recipientGroupInfo = {
            XR_TYPE_SHARE_SPACES_RECIPIENT_GROUPS_META, nullptr};
        recipientGroupInfo.groupCount = 1;
        recipientGroupInfo.groups = &groupuuid;

        XrShareSpacesInfoMETA info = {XR_TYPE_SHARE_SPACES_INFO_META};
        info.spaceCount = 1;
        info.spaces = &anchor;

        info.recipientInfo = (const XrShareSpacesRecipientBaseHeaderMETA*)&recipientGroupInfo;
        XrAsyncRequestIdFB shareRequestId;

        XrResult r;
        OXR(r = xrShareSpacesMETA(GetSession(), &info, &shareRequestId));
        if (r == XR_SUCCESS) {
            GroupUuid = groupuuid;
        } else {
            Logs->AppendRow("Failed to share anchors: " + std::to_string(r));
            MyAnchor = std::nullopt;
        }
    }

    void QueryAnchors(XrUuidEXT groupUuid) {
        VALIDATE_FUNCTION_PTR(xrQuerySpacesFB)

        Logs->AppendRow(
            "Querying anchors from group: " + HexStringHelper::UuidToHexString(groupUuid));

        XrSpaceStorageLocationFilterInfoFB locationFilterInfo = {
            XR_TYPE_SPACE_STORAGE_LOCATION_FILTER_INFO_FB,
            nullptr,
            XR_SPACE_STORAGE_LOCATION_CLOUD_FB};
        XrSpaceGroupUuidFilterInfoMETA filterInfo = {XR_TYPE_SPACE_GROUP_UUID_FILTER_INFO_META};
        filterInfo.groupUuid = groupUuid;
        filterInfo.next = &locationFilterInfo;

        XrSpaceQueryInfoFB info = {
            XR_TYPE_SPACE_QUERY_INFO_FB,
            nullptr,
            XR_SPACE_QUERY_ACTION_LOAD_FB,
            K_MAX_PERSISTENT_SPACES,
            0,
            (XrSpaceFilterInfoBaseHeaderFB*)&filterInfo,
            nullptr};

        XrAsyncRequestIdFB requestId;
        XrResult r;
        OXR(r = xrQuerySpacesFB(GetSession(), (XrSpaceQueryInfoBaseHeaderFB*)&info, &requestId));

        if (r == XR_SUCCESS) {
            ALOGV("Query Spatial Anchor initiated.");
        } else {
            Logs->AppendRow("Failed to Query anchors: " + std::to_string(r));
        }
    }

    void HookExtensions() {
        /// Passthrough
        HOOKUP_FUNCTION_PTR(xrCreatePassthroughFB)
        HOOKUP_FUNCTION_PTR(xrDestroyPassthroughFB)
        HOOKUP_FUNCTION_PTR(xrCreatePassthroughLayerFB)
        HOOKUP_FUNCTION_PTR(xrDestroyPassthroughLayerFB)
        HOOKUP_FUNCTION_PTR(xrPassthroughLayerResumeFB)
        HOOKUP_FUNCTION_PTR(xrPassthroughLayerPauseFB)
        HOOKUP_FUNCTION_PTR(xrPassthroughLayerSetStyleFB)
        HOOKUP_FUNCTION_PTR(xrPassthroughStartFB)
        HOOKUP_FUNCTION_PTR(xrPassthroughPauseFB)

        // Spatial anchor
        HOOKUP_FUNCTION_PTR(xrEnumerateSpaceSupportedComponentsFB)
        HOOKUP_FUNCTION_PTR(xrSetSpaceComponentStatusFB)
        HOOKUP_FUNCTION_PTR(xrCreateSpatialAnchorFB)
        HOOKUP_FUNCTION_PTR(xrGetSpaceComponentStatusFB)

        // Colocation Discovery
        HOOKUP_FUNCTION_PTR(xrStopColocationAdvertisementMETA)
        HOOKUP_FUNCTION_PTR(xrStartColocationAdvertisementMETA)
        HOOKUP_FUNCTION_PTR(xrStartColocationDiscoveryMETA)
        HOOKUP_FUNCTION_PTR(xrStopColocationDiscoveryMETA)
        HOOKUP_FUNCTION_PTR(xrShareSpacesMETA)
        HOOKUP_FUNCTION_PTR(xrRetrieveSpaceQueryResultsFB)
        HOOKUP_FUNCTION_PTR(xrQuerySpacesFB)
    }

    bool ValidateLocationFlags(XrSpaceLocationFlags flags) {
        return (flags & XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT) != 0 &&
            (flags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT) != 0;
    }

    XrUuidEXT GenerateRandomGroupUUID() {
        XrUuidEXT groupuuid;

        for (uint8_t i = 0; i < XR_UUID_SIZE; i++) {
            groupuuid.data[i] = rand();
        }

        return groupuuid;
    }

    OVR::Vector4f GenerateRandomColor() {
        return {
            (float)rand() / (float)RAND_MAX,
            (float)rand() / (float)RAND_MAX,
            (float)rand() / (float)RAND_MAX,
            1.0f};
    }

   private:
    // Passthrough layer
    XrPassthroughFB Passthrough = XR_NULL_HANDLE;
    XrPassthroughLayerFB PassthroughLayer = XR_NULL_HANDLE;

    // Passthrough
    DECLARE_FUNCTION_PTR(xrCreatePassthroughFB)
    DECLARE_FUNCTION_PTR(xrDestroyPassthroughFB)
    DECLARE_FUNCTION_PTR(xrCreatePassthroughLayerFB)
    DECLARE_FUNCTION_PTR(xrDestroyPassthroughLayerFB)
    DECLARE_FUNCTION_PTR(xrPassthroughLayerResumeFB)
    DECLARE_FUNCTION_PTR(xrPassthroughLayerPauseFB)
    DECLARE_FUNCTION_PTR(xrPassthroughLayerSetStyleFB)
    DECLARE_FUNCTION_PTR(xrPassthroughStartFB)
    DECLARE_FUNCTION_PTR(xrPassthroughPauseFB)

    // Spatial anchor
    DECLARE_FUNCTION_PTR(xrEnumerateSpaceSupportedComponentsFB)
    DECLARE_FUNCTION_PTR(xrSetSpaceComponentStatusFB)
    DECLARE_FUNCTION_PTR(xrCreateSpatialAnchorFB)
    DECLARE_FUNCTION_PTR(xrGetSpaceComponentStatusFB)

    // Colocation Discovery
    DECLARE_FUNCTION_PTR(xrStopColocationAdvertisementMETA)
    DECLARE_FUNCTION_PTR(xrStartColocationAdvertisementMETA)
    DECLARE_FUNCTION_PTR(xrStartColocationDiscoveryMETA)
    DECLARE_FUNCTION_PTR(xrStopColocationDiscoveryMETA)
    DECLARE_FUNCTION_PTR(xrShareSpacesMETA)
    DECLARE_FUNCTION_PTR(xrRetrieveSpaceQueryResultsFB)
    DECLARE_FUNCTION_PTR(xrQuerySpacesFB)

    std::unique_ptr<IAnchorsRenderer> AnchorsRenderer;
    OVRFW::ControllerRenderer ControllerRenderL;
    OVRFW::ControllerRenderer ControllerRenderR;
    OVRFW::TinyUI Ui;
    OVRFW::SimpleBeamRenderer BeamRenderer;
    std::vector<OVRFW::ovrBeamRenderer::handle_t> Beams;

    std::optional<Anchor> MyAnchor = std::nullopt;
    std::unordered_map<std::string, Anchor> ReceivedAnchors;
    std::optional<XrUuidEXT> GroupUuid;

    // UI Menu
    std::unique_ptr<SimpleListUI> Logs;
    std::unique_ptr<SimpleListUI> Results;
    OVRFW::VRMenuObject* Banner;
    OVRFW::VRMenuObject* StartDiscoveryButton;
    OVRFW::VRMenuObject* StopDiscoveryButton;
    OVRFW::VRMenuObject* StartAdvertisementButton;
    OVRFW::VRMenuObject* StopAdvertisementButton;

    bool IsStartDiscoveryButtonEnabled = true;
    bool IsStartAdvertisementButtonEnabled = true;
    bool IsAnchorShared = false;
};

ENTRY_POINT(XrColocationDiscoveryApp)
