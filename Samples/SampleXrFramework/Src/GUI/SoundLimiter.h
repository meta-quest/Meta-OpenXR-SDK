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

Filename    :   SoundLimiter.cpp
Content     :   Utility class for limiting how often sounds play.
Created     :   June 23, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#pragma once

namespace OVRFW {

class OvrGuiSys;

//==============================================================
// ovrSoundLimiter
class ovrSoundLimiter {
   public:
    ovrSoundLimiter() : LastPlayTime(0.0) {}

    void PlaySoundEffect(OvrGuiSys& guiSys, char const* soundName, double const limitSeconds);
    // Checks if menu specific sounds exists before playing the default vrappframework sound passed
    // in.
    void PlayMenuSound(
        OvrGuiSys& guiSys,
        char const* menuName,
        char const* soundName,
        double const limitSeconds);

   private:
    double LastPlayTime;
};

} // namespace OVRFW
