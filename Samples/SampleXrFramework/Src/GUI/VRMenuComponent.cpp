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

Filename    :   VRMenuComponent.h
Content     :   Menuing system for VR apps.
Created     :   June 23, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#include "VRMenuComponent.h"
#include "VRMenuMgr.h"

namespace OVRFW {

const char* VRMenuComponent::TYPE_NAME = "";

//==============================
// VRMenuComponent::OnEvent
eMsgStatus VRMenuComponent::OnEvent(
    OvrGuiSys& guiSys,
    ovrApplFrameIn const& vrFrame,
    VRMenuObject* self,
    VRMenuEvent const& event) {
    assert(self != NULL);

    //-------------------
    // do any pre work that every event handler must do
    //-------------------

    // LOG_WITH_TAG( "VrMenu", "OnEvent '%s' for '%s'", VRMenuEventTypeNames[event.EventType],
    // self->GetText().ToCStr() );

    // call the overloaded implementation
    eMsgStatus status = OnEvent_Impl(guiSys, vrFrame, self, event);

    //-------------------
    // do any post work that every event handle must do
    //-------------------

    // When new items are added to a menu, the menu sends VRMENU_EVENT_INIT again so that those
    // components will be initialized.  In order to prevent components from getting initialized
    // a second time, components clear their VRMENU_EVENT_INIT flag after each init event.
    if (event.EventType == VRMENU_EVENT_INIT) {
        EventFlags &= ~VRMenuEventFlags_t(VRMENU_EVENT_INIT);
    }

    return status;
}

} // namespace OVRFW
