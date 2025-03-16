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

#include "SoundLimiter.h"
#include "GuiSys.h"

#include "OVR_Std.h"

#include "System.h"

namespace OVRFW {

//==============================
// ovrSoundLimiter::PlaySound
void ovrSoundLimiter::PlaySoundEffect(
    OvrGuiSys& guiSys,
    char const* soundName,
    double const limitSeconds) {
    double curTime = GetTimeInSeconds();
    double t = curTime - LastPlayTime;
    // LOG_WITH_TAG( "VrMenu", "PlaySoundEffect( '%s', %.2f ) - t == %.2f : %s", soundName,
    // limitSeconds, t, t >= limitSeconds ? "PLAYING" : "SKIPPING" );
    if (t >= limitSeconds) {
        guiSys.GetSoundEffectPlayer().Play(soundName);
        LastPlayTime = curTime;
    }
}

void ovrSoundLimiter::PlayMenuSound(
    OvrGuiSys& guiSys,
    char const* appendKey,
    char const* soundName,
    double const limitSeconds) {
    char overrideSound[1024];
    OVR::OVR_sprintf(overrideSound, 1024, "%s_%s", appendKey, soundName);

    if (guiSys.GetSoundEffectPlayer().Has(overrideSound)) {
        PlaySoundEffect(guiSys, overrideSound, limitSeconds);
    } else {
        PlaySoundEffect(guiSys, soundName, limitSeconds);
    }
}

} // namespace OVRFW
