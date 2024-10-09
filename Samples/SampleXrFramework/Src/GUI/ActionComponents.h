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

#pragma once

#include "VRMenuComponent.h"
#include "VRMenu.h"

namespace OVRFW {

class VRMenu;

//==============================================================
// OvrButton_OnUp
// This is a generic component that forwards a touch up to a menu (normally its owner)
class OvrButton_OnUp : public VRMenuComponent_OnTouchUp {
   public:
    static const int TYPE_ID = 1010;

    OvrButton_OnUp(VRMenu* menu, VRMenuId_t const buttonId)
        : VRMenuComponent_OnTouchUp(), Menu(menu), ButtonId(buttonId) {}

    void SetID(VRMenuId_t newButtonId) {
        ButtonId = newButtonId;
    }

    virtual int GetTypeId() const {
        return TYPE_ID;
    }

   private:
    virtual eMsgStatus OnEvent_Impl(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);

   private:
    VRMenu* Menu; // menu that holds the button
    VRMenuId_t ButtonId; // id of the button this control handles
};

} // namespace OVRFW
