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

Filename    :   VRMenuFrame.h
Content     :   Menu component for handling hit tests and dispatching events.
Created     :   June 23, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#pragma once

#include "VRMenuObject.h"
#include "VRMenuEvent.h"
#include "GazeCursor.h"
#include "SoundLimiter.h"
#include "FrameParams.h"

namespace OVRFW {

//==============================================================
// VRMenuEventHandler
class VRMenuEventHandler {
   public:
    VRMenuEventHandler();
    ~VRMenuEventHandler();

    void Frame(
        OvrGuiSys& guiSys,
        const ovrApplFrameIn& vrFrame,
        menuHandle_t const& rootHandle,
        OVR::Posef const& menuPose,
        OVR::Matrix4f const& traceMat,
        std::vector<VRMenuEvent>& events);

    void HandleEvents(
        OvrGuiSys& guiSys,
        const ovrApplFrameIn& vrFrame,
        menuHandle_t const rootHandle,
        std::vector<VRMenuEvent> const& events) const;

    void InitComponents(std::vector<VRMenuEvent>& events);
    void Opening(std::vector<VRMenuEvent>& events);
    void Opened(std::vector<VRMenuEvent>& events);
    void Closing(std::vector<VRMenuEvent>& events);
    void Closed(std::vector<VRMenuEvent>& events);

    menuHandle_t GetFocusedHandle() const {
        return FocusedHandle;
    }

   private:
    menuHandle_t FocusedHandle;

    ovrSoundLimiter GazeOverSoundLimiter;
    ovrSoundLimiter DownSoundLimiter;
    ovrSoundLimiter UpSoundLimiter;

   private:
    bool DispatchToComponents(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuEvent const& event,
        VRMenuObject* receiver) const;
    bool DispatchToPath(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuEvent const& event,
        std::vector<menuHandle_t> const& path,
        bool const log) const;
    bool BroadcastEvent(
        OvrGuiSys& guiSys,
        ovrApplFrameIn const& vrFrame,
        VRMenuEvent const& event,
        VRMenuObject* receiver) const;
};

} // namespace OVRFW
