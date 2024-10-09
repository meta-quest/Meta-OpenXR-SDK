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

Filename    :   DefaultComponent.h
Content     :   A default menu component that handles basic actions most menu items need.
Created     :   July 25, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#pragma once

#include "VRMenuComponent.h"
#include "Fader.h"
#include "FrameParams.h"

namespace OVRFW {

//==============================================================
// OvrDefaultComponent
class OvrDefaultComponent : public VRMenuComponent {
   public:
    static const int TYPE_ID = 1;

    OvrDefaultComponent(
        OVR::Vector3f const& hilightOffset = OVR::Vector3f(0.0f, 0.0f, 0.05f),
        float const hilightScale = 1.05f,
        float const fadeDuration = 0.125f,
        float const fadeDelay = 0.125f,
        OVR::Vector4f const& textNormalColor = OVR::Vector4f(1.0f),
        OVR::Vector4f const& textHilightColor = OVR::Vector4f(1.0f),
        bool const useSurfaceAnimator = false,
        const bool noHilight = false);

    virtual int GetTypeId() const {
        return TYPE_ID;
    }

    void SetSuppressText(bool const suppress) {
        SuppressText = suppress;
    }

   private:
    // private variables
    // We may actually want these to be static...
    ovrSoundLimiter GazeOverSoundLimiter;
    ovrSoundLimiter DownSoundLimiter;
    ovrSoundLimiter UpSoundLimiter;

    SineFader HilightFader;
    double StartFadeInTime;
    double StartFadeOutTime;
    OVR::Vector3f HilightOffset;
    float HilightScale;
    float FadeDuration;
    float FadeDelay;
    OVR::Vector4f TextNormalColor;
    OVR::Vector4f TextHilightColor;
    bool SuppressText; // true if text should not be faded in
    bool UseSurfaceAnimator; // if true, use the surface animator to select a hilight surface
                             // instead of an additive pass
    bool NoHilight; // don't change the hilight state -- something else will do it

   private:
    virtual eMsgStatus OnEvent_Impl(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);

    eMsgStatus Frame(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);
    eMsgStatus FocusGained(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);
    eMsgStatus FocusLost(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);
};

//==============================================================
// OvrSurfaceToggleComponent
// Toggles surfaced based on pair index, the current is default state, +1 is hover
class OvrSurfaceToggleComponent : public VRMenuComponent {
   public:
    static const char* TYPE_NAME;
    OvrSurfaceToggleComponent()
        : VRMenuComponent(VRMenuEventFlags_t(VRMENU_EVENT_FRAME_UPDATE)), GroupIndex(0) {}

    void SetGroupIndex(const int index) {
        GroupIndex = index;
    }
    int GetGroupIndex() const {
        return GroupIndex;
    }

    virtual const char* GetTypeName() const {
        return TYPE_NAME;
    }

   private:
    virtual eMsgStatus OnEvent_Impl(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);

    eMsgStatus Frame(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuObject* self,
        VRMenuEvent const& event);

    int GroupIndex;
};

} // namespace OVRFW
