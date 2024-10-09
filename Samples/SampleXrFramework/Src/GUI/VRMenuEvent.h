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
/************************************************************************************

Filename    :   VRMenuEvent.h
Content     :   Event class for menu events.
Created     :   June 23, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#pragma once

#include "VRMenuObject.h"

namespace OVRFW {

enum eVRMenuEventType {
    VRMENU_EVENT_FOCUS_GAINED, // TargetHandle is the handle of the object gaining focus, other
                               // handle is previous focus
    VRMENU_EVENT_FOCUS_LOST, // TargetHandle is the handle of the object losing focus
    VRMENU_EVENT_TOUCH_DOWN, // TargetHandle will be the focused handle
    VRMENU_EVENT_TOUCH_UP, // TargetHandle will be the focused handle
    VRMENU_EVENT_TOUCH_RELATIVE, // sent when a touch position changes, TargetHandle will be the
                                 // focused handle
    VRMENU_EVENT_TOUCH_ABSOLUTE, // sent whenever touch is down, TargetHandle will be the focused
                                 // handle
    VRMENU_EVENT_SWIPE_FORWARD, // sent for a forward swipe event, TargetHandle will be the focused
                                // handle
    VRMENU_EVENT_SWIPE_BACK, // TargetHandle will be the focused handle
    VRMENU_EVENT_SWIPE_UP, // TargetHandle will be the focused handle
    VRMENU_EVENT_SWIPE_DOWN, // TargetHandle will be the focused handle
    VRMENU_EVENT_FRAME_UPDATE, // TargetHandle should be empty
    VRMENU_EVENT_SUBMIT_FOR_RENDERING,
    VRMENU_EVENT_RENDER,
    VRMENU_EVENT_OPENING, // sent when a menu starts to open
    VRMENU_EVENT_OPENED, // sent when a menu opens
    VRMENU_EVENT_CLOSING, // sent when a menu starts to close
    VRMENU_EVENT_CLOSED, // sent when a menu closes
    VRMENU_EVENT_INIT, // sent when the owning menu initializes
    VRMENU_EVENT_SELECTED, // sent when a menu selects an object
    VRMENU_EVENT_DESELECTED, // sent when a menu deselected an object
    VRMENU_EVENT_UPDATE_OBJECT, // sent when a menu object needs to be updated
    VRMENU_EVENT_SWIPE_COMPLETE, // sent when a swipe operation has been completed and touch up has
                                 // triggered

    VRMENU_EVENT_ITEM_ACTION_COMPLETE, // item -> menu : item has completed some action

    VRMENU_EVENT_MAX
};

typedef OVR::BitFlagsT<eVRMenuEventType, uint64_t> VRMenuEventFlags_t;

enum eEventDispatchType {
    EVENT_DISPATCH_TARGET, // send only to target
    EVENT_DISPATCH_FOCUS, // send to all in focus path
    EVENT_DISPATCH_BROADCAST, // send to all
    EVENT_DISPATCH_MAX
};

//==============================================================
// VRMenuEvent
class VRMenuEvent {
   public:
    static char const* EventTypeNames[VRMENU_EVENT_MAX];

    VRMenuEvent()
        : EventType(VRMENU_EVENT_MAX), DispatchType(EVENT_DISPATCH_MAX), FloatValue(0.0f) {}

    // non-target
    VRMenuEvent(
        eVRMenuEventType const eventType,
        eEventDispatchType const dispatchType,
        menuHandle_t const& targetHandle,
        OVR::Vector3f const& floatValue,
        HitTestResult const& hitResult,
        char const* message)
        : EventType(eventType),
          DispatchType(dispatchType),
          TargetHandle(targetHandle),
          FloatValue(floatValue),
          HitResult(hitResult),
          Message(message) {}

    // non-target with other handle
    VRMenuEvent(
        eVRMenuEventType const eventType,
        eEventDispatchType const dispatchType,
        menuHandle_t const& targetHandle,
        menuHandle_t const& otherHandle,
        OVR::Vector3f const& floatValue,
        HitTestResult const& hitResult,
        char const* message)
        : EventType(eventType),
          DispatchType(dispatchType),
          TargetHandle(targetHandle),
          OtherHandle(otherHandle),
          FloatValue(floatValue),
          HitResult(hitResult),
          Message(message) {}

    eVRMenuEventType EventType;
    eEventDispatchType DispatchType;
    menuHandle_t TargetHandle; // valid only if targeted to a specific object
    menuHandle_t OtherHandle; // varies dependent on event type
    OVR::Vector3f FloatValue;
    HitTestResult HitResult;
    std::string Message;
};

} // namespace OVRFW
