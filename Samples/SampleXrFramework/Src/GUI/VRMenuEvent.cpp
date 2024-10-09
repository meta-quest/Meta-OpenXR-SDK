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

Filename    :   VRMenuEvent.cpp
Content     :   Event class for menu events.
Created     :   June 23, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#include "VRMenuEvent.h"

namespace OVRFW {

char const* VRMenuEvent::EventTypeNames[] = {
    "VRMENU_EVENT_FOCUS_GAINED",
    "VRMENU_EVENT_FOCUS_LOST",
    "VRMENU_EVENT_TOUCH_DOWN",
    "VRMENU_EVENT_TOUCH_UP",
    "VRMENU_EVENT_TOUCH_RELATIVE",
    "VRMENU_EVENT_TOUCH_ABSOLUTE",
    "VRMENU_EVENT_SWIPE_FORWARD",
    "VRMENU_EVENT_SWIPE_BACK",
    "VRMENU_EVENT_SWIPE_UP",
    "VRMENU_EVENT_SWIPE_DOWN",
    "VRMENU_EVENT_FRAME_UPDATE",
    "VRMENU_EVENT_SUBMIT_FOR_RENDERING",
    "VRMENU_EVENT_RENDER",
    "VRMENU_EVENT_OPENING",
    "VRMENU_EVENT_OPENED",
    "VRMENU_EVENT_CLOSING",
    "VRMENU_EVENT_CLOSED",
    "VRMENU_EVENT_INIT",
    "VRMENU_EVENT_SELECTED",
    "VRMENU_EVENT_DESELECTED",
    "VRMENU_EVENT_UPDATE_OBJECT",
    "VRMENU_EVENT_SWIPE_COMPLETE",
    "VRMENU_EVENT_ITEM_ACTION_COMPLETE"};

} // namespace OVRFW
