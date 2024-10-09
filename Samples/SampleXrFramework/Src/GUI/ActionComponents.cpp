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

Filename    :   ActionComponents.h
Content     :   Misc. VRMenu Components to handle actions
Created     :   September 12, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#include "ActionComponents.h"
#include "Misc/Log.h"

namespace OVRFW {

//==============================
// OvrButton_OnUp::OnEvent_Impl
eMsgStatus OvrButton_OnUp::OnEvent_Impl(
    OvrGuiSys& guiSys,
    ovrApplFrameIn const& vrFrame,
    VRMenuObject* self,
    VRMenuEvent const& event) {
    assert(event.EventType == VRMENU_EVENT_TOUCH_UP);
    ALOG("Button id %lli clicked", ButtonId.Get());
    Menu->OnItemEvent(guiSys, vrFrame, ButtonId, event);
    return MSG_STATUS_CONSUMED;
}

} // namespace OVRFW
