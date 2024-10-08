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

Filename    :   VRMenuMgr.h
Content     :   Menuing system for VR apps.
Created     :   May 23, 2014
Authors     :   Jonathan E. Wright

*************************************************************************************/

#pragma once

#include "VRMenuObject.h"

namespace OVRFW {

class BitmapFont;
class BitmapFontSurface;
struct GlProgram;
class OvrDebugLines;

//==============================================================
// OvrVRMenuMgr
class OvrVRMenuMgr {
   public:
    friend class VRMenuObject;

    virtual ~OvrVRMenuMgr() {}

    static OvrVRMenuMgr* Create(OvrGuiSys& guiSys);
    static void Destroy(OvrVRMenuMgr*& mgr);

    // Initialize the VRMenu system
    virtual void Init(OvrGuiSys& guiSys) = 0;
    // Shutdown the VRMenu syatem
    virtual void Shutdown() = 0;

    // creates a new menu object
    virtual menuHandle_t CreateObject(VRMenuObjectParms const& parms) = 0;
    // Frees a menu object.  If the object is a child of a parent object, this will
    // also remove the child from the parent.
    virtual void FreeObject(menuHandle_t const handle) = 0;
    // Returns true if the handle is valid.
    virtual bool IsValid(menuHandle_t const handle) const = 0;
    // Return the object for a menu handle or NULL if the object does not exist or the
    // handle is invalid;
    virtual VRMenuObject* ToObject(menuHandle_t const handle) const = 0;

    // Submits the specified menu object and its children
    virtual void SubmitForRendering(
        OvrGuiSys& guiSys,
        OVR::Matrix4f const& centerViewMatrix,
        menuHandle_t const handle,
        OVR::Posef const& worldPose,
        VRMenuRenderFlags_t const& flags) = 0;

    // Call once per frame before rendering to sort surfaces.
    virtual void Finish(OVR::Matrix4f const& viewMatrix) = 0;

    virtual void AppendSurfaceList(
        OVR::Matrix4f const& centerViewMatrix,
        std::vector<ovrDrawSurface>& surfaceList) = 0;

    virtual GlProgram const* GetGUIGlProgram(eGUIProgramType const programType) const = 0;

   private:
    // Called only from VRMenuObject.
    virtual void AddComponentToDeletionList(
        menuHandle_t const ownerHandle,
        VRMenuComponent* component) = 0;
};

} // namespace OVRFW
