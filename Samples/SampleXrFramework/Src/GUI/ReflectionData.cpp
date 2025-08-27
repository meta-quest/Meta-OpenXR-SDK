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

Filename    :   ReflectionData.cpp
Content     :   Data for introspection and reflection of C++ objects.
Created     :   11/16/2015
Authors     :   Jonathan E. Wright

*************************************************************************************/

// this is necessary so that offsetof() can work for private class members
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif // __clang__

#define _ALLOW_KEYWORD_MACROS
#undef private
#define private public

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

#include "ReflectionData.h"
#include "VRMenuObject.h"
#include "DefaultComponent.h"
#include "AnimComponents.h"

using OVR::BitFlagsT;
using OVR::Bounds3f;
using OVR::Matrix4f;
using OVR::Posef;
using OVR::Quatf;
using OVR::TypesafeNumberT;
using OVR::Vector2f;
using OVR::Vector2i;
using OVR::Vector3f;
using OVR::Vector4f;

namespace OVRFW {

ovrEnumInfo HorizontalJustification_Enums[] = {
    {"HORIZONTAL_LEFT", 0},
    {"HORIZONTAL_CENTER", 1},
    {"HORIZONTAL_RIGHT", 2}};
OVR_VERIFY_ARRAY_SIZE(HorizontalJustification_Enums, 3);

ovrEnumInfo VerticalJustification_Enums[] = {
    {"VERTICAL_BASELINE", 0},
    {"VERTICAL_CENTER", 1},
    {"VERTICAL_CENTER_FIXEDHEIGHT", 2},
    {"VERTICAL_TOP", 3}};
OVR_VERIFY_ARRAY_SIZE(VerticalJustification_Enums, 4);

ovrEnumInfo eContentFlags_Enums[] = {
    {"CONTENT_NONE", 0},
    {"CONTENT_SOLID", 1},
    {"CONTENT_ALL", 0x7fffffff},
};
OVR_VERIFY_ARRAY_SIZE(eContentFlags_Enums, 3);

ovrEnumInfo VRMenuObjectType_Enums[] = {
    {"VRMENU_CONTAINER", 0},
    {"VRMENU_STATIC", 1},
    {"VRMENU_BUTTON", 2},
    {"VRMENU_MAX", 3}};
OVR_VERIFY_ARRAY_SIZE(VRMenuObjectType_Enums, VRMENU_MAX + 1);

ovrEnumInfo VRMenuObjectFlag_Enums[] = {
    {"VRMENUOBJECT_FLAG_NO_FOCUS_GAINED", 0},
    {"VRMENUOBJECT_DONT_HIT_ALL", 1},
    {"VRMENUOBJECT_DONT_HIT_TEXT", 2},
    {"VRMENUOBJECT_HIT_ONLY_BOUNDS", 3},
    {"VRMENUOBJECT_BOUND_ALL", 4},
    {"VRMENUOBJECT_FLAG_POLYGON_OFFSET", 5},
    {"VRMENUOBJECT_FLAG_NO_DEPTH", 6},
    {"VRMENUOBJECT_FLAG_NO_DEPTH_MASK", 7},
    {"VRMENUOBJECT_DONT_RENDER", 8},
    {"VRMENUOBJECT_DONT_RENDER_SURFACE", 9},
    {"VRMENUOBJECT_DONT_RENDER_TEXT", 10},
    {"VRMENUOBJECT_NO_GAZE_HILIGHT", 11},
    {"VRMENUOBJECT_RENDER_HIERARCHY_ORDER", 12},
    {"VRMENUOBJECT_FLAG_BILLBOARD", 13},
    {"VRMENUOBJECT_DONT_MOD_PARENT_COLOR", 14},
    {"VRMENUOBJECT_INSTANCE_TEXT", 15}};
OVR_VERIFY_ARRAY_SIZE(VRMenuObjectFlag_Enums, VRMENUOBJECT_MAX);

ovrEnumInfo VRMenuObjectInitFlag_Enums[] = {
    {"VRMENUOBJECT_INIT_ALIGN_TO_VIEW", 0},
    {"VRMENUOBJECT_INIT_FORCE_POSITION", 1}};
OVR_VERIFY_ARRAY_SIZE(VRMenuObjectInitFlag_Enums, 2);

ovrEnumInfo SurfaceTextureType_Enums[] = {
    {"SURFACE_TEXTURE_DIFFUSE", 0},
    {"SURFACE_TEXTURE_DIFFUSE_ALPHA_DISCARD", 1},
    {"SURFACE_TEXTURE_ADDITIVE", 2},
    {"SURFACE_TEXTURE_COLOR_RAMP", 3},
    {"SURFACE_TEXTURE_COLOR_RAMP_TARGET", 4},
    {"SURFACE_TEXTURE_ALPHA_MASK", 5},
    {"SURFACE_TEXTURE_MAX", 6}};
OVR_VERIFY_ARRAY_SIZE(SurfaceTextureType_Enums, SURFACE_TEXTURE_MAX + 1);

ovrEnumInfo VRMenuId_Enums[] = {{"INVALID_MENU_ID", INT_MIN}};

ovrEnumInfo VRMenuEventType_Enums[] = {
    {"VRMENU_EVENT_FOCUS_GAINED", 0},
    {"VRMENU_EVENT_FOCUS_LOST", 1},
    {"VRMENU_EVENT_TOUCH_DOWN", 2},
    {"VRMENU_EVENT_TOUCH_UP", 3},
    {"VRMENU_EVENT_TOUCH_RELATIVE", 4},
    {"VRMENU_EVENT_TOUCH_ABSOLUTE", 5},
    {"VRMENU_EVENT_SWIPE_FORWARD", 6},
    {"VRMENU_EVENT_SWIPE_BACK", 7},
    {"VRMENU_EVENT_SWIPE_UP", 8},
    {"VRMENU_EVENT_SWIPE_DOWN", 9},
    {"VRMENU_EVENT_FRAME_UPDATE", 10},
    {"VRMENU_EVENT_SUBMIT_FOR_RENDERING", 11},
    {"VRMENU_EVENT_RENDER", 12},
    {"VRMENU_EVENT_OPENING", 13},
    {"VRMENU_EVENT_OPENED", 14},
    {"VRMENU_EVENT_CLOSING", 15},
    {"VRMENU_EVENT_CLOSED", 16},
    {"VRMENU_EVENT_INIT", 17},
    {"VRMENU_EVENT_SELECTED", 18},
    {"VRMENU_EVENT_DESELECTED", 19},
    {"VRMENU_EVENT_UPDATE_OBJECT", 20},
    {"VRMENU_EVENT_SWIPE_COMPLETE", 21},
    {"VRMENU_EVENT_ITEM_ACTION_COMPLETE", 22},
    {"VRMENU_EVENT_MAX", 23}};
OVR_VERIFY_ARRAY_SIZE(VRMenuEventType_Enums, VRMENU_EVENT_MAX + 1);

ovrEnumInfo AnimState_Enums[] = {{"ANIMSTATE_PAUSED", 0}, {"ANIMSTATE_PLAYING", 1}};
OVR_VERIFY_ARRAY_SIZE(AnimState_Enums, OvrAnimComponent::ANIMSTATE_MAX);

ovrEnumInfo EventDispatchType_Enums[] = {
    {"EVENT_DISPATCH_TARGET", 0},
    {"EVENT_DISPATCH_FOCUS", 1},
    {"EVENT_DISPATCH_BROADCAST", 2}};
OVR_VERIFY_ARRAY_SIZE(EventDispatchType_Enums, EVENT_DISPATCH_MAX);

template <typename T>
T* CreateObject(void* placementBuffer) {
    if (placementBuffer != nullptr) {
        return new (placementBuffer) T();
    }
    return new T();
}

void* Create_OvrDefaultComponent(void* placementBuffer) {
    return CreateObject<OvrDefaultComponent>(placementBuffer);
}

void* Create_OvrSurfaceAnimComponent(void* placementBuffer) {
    return OvrSurfaceAnimComponent::Create(placementBuffer);
}

void* Create_VRMenuSurfaceParms(void* placementBuffer) {
    return CreateObject<VRMenuSurfaceParms>(placementBuffer);
}

void* Create_VRMenuObjectParms(void* placementBuffer) {
    return CreateObject<VRMenuObjectParms>(placementBuffer);
}

void* Create_ovrSoundLimiter(void* placementBuffer) {
    return CreateObject<ovrSoundLimiter>(placementBuffer);
}

void* Create_string(void* placementBuffer) {
    return CreateObject<std::string>(placementBuffer);
}

void* Create_bool(void* placementBuffer) {
    return CreateObject<bool>(placementBuffer);
}

void* Create_float(void* placementBuffer) {
    return CreateObject<float>(placementBuffer);
}

void* Create_double(void* placementBuffer) {
    return CreateObject<double>(placementBuffer);
}

void* Create_int(void* placementBuffer) {
    return CreateObject<int>(placementBuffer);
}

template <class ItemClass>
void ResizeArray(void* arrayPtr, const int newSize) {
    std::vector<ItemClass>& a = *static_cast<std::vector<ItemClass>*>(arrayPtr);
    a.resize(newSize);
}

template <class ItemClass>
void ResizePtrArray(void* arrayPtr, const int newSize) {
    std::vector<ItemClass>& a = *static_cast<std::vector<ItemClass>*>(arrayPtr);
    int oldSize = static_cast<int>(a.size());
    a.resize(newSize);

    if (newSize > oldSize) {
        // in-place construct since OVR::Array doesn't
        for (int i = oldSize; i < newSize; ++i) {
            ItemClass* item = static_cast<ItemClass*>(&a[i]);
            new (item) ItemClass();
        }
    }
}

template <class ArrayClass, class ElementClass>
void SetArrayElementPtr(void* objPtr, const int index, ElementClass elementPtr) {
    ArrayClass& a = *static_cast<ArrayClass*>(objPtr);
    a[index] = elementPtr;
}

template <class ArrayClass, class ElementClass>
void SetArrayElement(void* objPtr, const int index, ElementClass* elementPtr) {
    ArrayClass& a = *static_cast<ArrayClass*>(objPtr);
    a[index] = *elementPtr;
}

void Resize_std_vector_VRMenuComponent_Ptr(void* objPtr, const int newSize) {
    ResizePtrArray<VRMenuComponent*>(objPtr, newSize);
}

void SetArrayElementFn_std_vector_VRMenuComponent_Ptr(
    void* objPtr,
    const int index,
    void* elementPtr) {
    SetArrayElementPtr<std::vector<VRMenuComponent*>, VRMenuComponent*>(
        objPtr, index, static_cast<VRMenuComponent*>(elementPtr));
}

void Resize_std_vector_VRMenuSurfaceParms(void* objPtr, const int newSize) {
    ResizeArray<VRMenuSurfaceParms>(objPtr, newSize);
}

void Resize_std_vector_VRMenuObjectParms_Ptr(void* objPtr, const int newSize) {
    ResizePtrArray<VRMenuObjectParms*>(objPtr, newSize);
}

void SetArrayElementFn_std_vector_VRMenuObjectParms_Ptr(
    void* objPtr,
    const int index,
    void* elementPtr) {
    SetArrayElementPtr<std::vector<VRMenuObjectParms*>, VRMenuObjectParms*>(
        objPtr, index, static_cast<VRMenuObjectParms*>(elementPtr));
}

void SetArrayElementFn_std_vector_VRMenuSurfaceParms(
    void* objPtr,
    const int index,
    void* elementPtr) {
    SetArrayElement<std::vector<VRMenuSurfaceParms>, VRMenuSurfaceParms>(
        objPtr, index, static_cast<VRMenuSurfaceParms*>(elementPtr));
}

void SetArrayElementFn_string(void* objPtr, const int index, void* elementPtr) {
    static_cast<std::string*>(objPtr)[index] = *static_cast<std::string*>(elementPtr);
}

void SetArrayElementFn_int(void* objPtr, const int index, void* elementPtr) {
    static_cast<int*>(objPtr)[index] = *static_cast<int*>(elementPtr);
}

void SetArrayElementFn_uint32_t(void* objPtr, const int index, void* elementPtr) {
    static_cast<uint32_t*>(objPtr)[index] = *static_cast<uint32_t*>(elementPtr);
}

void SetArrayElementFn_eSurfaceTextureType(void* objPtr, const int index, void* elementPtr) {
    static_cast<eSurfaceTextureType*>(objPtr)[index] =
        *static_cast<eSurfaceTextureType*>(elementPtr);
}

//=============================================================================================
// Reflection Data
//=============================================================================================

ovrMemberInfo Vector2i_Reflection[] = {
    {"x", "int", nullptr, ovrTypeOperator::NONE, offsetof(Vector2i, x), 0},
    {"y", "int", nullptr, ovrTypeOperator::NONE, offsetof(Vector2i, y), 0},
    {}};

ovrMemberInfo Vector2f_Reflection[] = {
    {"x", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector2f, x), 0},
    {"y", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector2f, y), 0},
    {}};

ovrMemberInfo Vector3f_Reflection[] = {
    {"x", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector3f, x), 0},
    {"y", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector3f, y), 0},
    {"z", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector3f, z), 0},
    {}};

ovrMemberInfo Vector4f_Reflection[] = {
    {"x", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector4f, x), 0},
    {"y", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector4f, y), 0},
    {"z", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector4f, z), 0},
    {"w", "float", nullptr, ovrTypeOperator::NONE, offsetof(Vector4f, w), 0},
    {}};

ovrMemberInfo Quatf_Reflection[] = {
    {"x", "float", nullptr, ovrTypeOperator::NONE, offsetof(Quatf, x), 0},
    {"y", "float", nullptr, ovrTypeOperator::NONE, offsetof(Quatf, y), 0},
    {"z", "float", nullptr, ovrTypeOperator::NONE, offsetof(Quatf, z), 0},
    {"w", "float", nullptr, ovrTypeOperator::NONE, offsetof(Quatf, w), 0},
    {}};

ovrMemberInfo TypesafeNumberT_longlong_eVRMenuId_INVALID_MENU_ID[] = {
    {"Value", "long long", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuId_t, Value), 0},
    {}};

ovrMemberInfo Posef_Reflection[] = {
    {"Position", "Vector3f", nullptr, ovrTypeOperator::NONE, offsetof(Posef, Translation), 0},
    {"Orientation", "Quatf", nullptr, ovrTypeOperator::NONE, offsetof(Posef, Rotation), 0},
    {}};

ovrMemberInfo VRMenuSurfaceParms_Reflection[] = {
    {"SurfaceName",
     "string",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuSurfaceParms, SurfaceName),
     0},
    {"ImageNames",
     "string[]",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuSurfaceParms, ImageNames),
     VRMENUSURFACE_IMAGE_MAX},
    {"ImageTexId",
     "uint32_t[]",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuSurfaceParms, ImageTexId),
     VRMENUSURFACE_IMAGE_MAX},
    {"ImageWidth",
     "int[]",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuSurfaceParms, ImageWidth),
     VRMENUSURFACE_IMAGE_MAX},
    {"ImageHeight",
     "int[]",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuSurfaceParms, ImageHeight),
     VRMENUSURFACE_IMAGE_MAX},
    {"TextureTypes",
     "eSurfaceTextureType[]",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuSurfaceParms, TextureTypes),
     VRMENUSURFACE_IMAGE_MAX},
    {"Contents",
     "BitFlagsT< eContentFlags >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuSurfaceParms, Contents),
     0},
    {"Color", "Vector4f", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuSurfaceParms, Color), 0},
    {"Anchors",
     "Vector2f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuSurfaceParms, Anchors),
     0},
    {"Border", "Vector4f", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuSurfaceParms, Border), 0},
    {"Dims", "Vector2f", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuSurfaceParms, Dims), 0},
    {"CropUV", "Vector4f", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuSurfaceParms, CropUV), 0},
    {"OffsetUVs",
     "Vector2f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuSurfaceParms, OffsetUVs),
     0},
    {}};

ovrMemberInfo VRMenuFontParms_Reflection[] = {
    {"AlignHoriz",
     "HorizontalJustification",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuFontParms, AlignHoriz),
     0},
    {"AlignVert",
     "VerticalJustification",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuFontParms, AlignVert),
     0},
    {"Billboard", "bool", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, Billboard), 0},
    {"TrackRoll", "bool", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, TrackRoll), 0},
    {"Outline", "bool", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, Outline), 0},
    {"ColorCenter",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuFontParms, ColorCenter)},
    {"AlphaCenter",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuFontParms, AlphaCenter)},
    {"Scale", "float", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, Scale), 0},
    {"WrapWidth", "float", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, WrapWidth), 0},
    {"MaxLines", "int", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, MaxLines), 0},
    {"MultiLine", "bool", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuFontParms, MultiLine), 0},
    {}};

ovrMemberInfo VRMenuObjectParms_Reflection[] = {
    {"Type",
     "eVRMenuObjectType",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, Type),
     0},
    {"Flags",
     "BitFlagsT< eVRMenuObjectFlags >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, Flags),
     0},
    {"InitFlags",
     "BitFlagsT< eVRMenuObjectInitFlags >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, InitFlags),
     0},
    {"Components",
     "std::vector< VRMenuComponent* >",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuObjectParms, Components),
     0},
    {"SurfaceParms",
     "std::vector< VRMenuSurfaceParms >",
     nullptr,
     ovrTypeOperator::ARRAY,
     offsetof(VRMenuObjectParms, SurfaceParms),
     0},
    {"Text", "string", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuObjectParms, Text), 0},
    {"LocalPose",
     "Posef",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, LocalPose),
     0},
    {"LocalScale",
     "Vector3f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, LocalScale),
     0},
    {"TextLocalPose",
     "Posef",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, TextLocalPose),
     0},
    {"TextLocalScale",
     "Vector3f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, TextLocalScale),
     0},
    {"FontParms",
     "VRMenuFontParms",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, FontParms),
     0},
    {"Color", "Vector4f", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuObjectParms, Color), 0},
    {"TextColor",
     "Vector4f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, TextColor),
     0},
    {"Id",
     "TypesafeNumberT< long long, eVRMenuId, INVALID_MENU_ID >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, Id),
     0},
    {"ParentId",
     "TypesafeNumberT< long long, eVRMenuId, INVALID_MENU_ID >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, ParentId),
     0},
    {"Contents",
     "BitFlagsT< eContentFlags >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, Contents),
     0},
    {"Name", "string", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuObjectParms, Name), 0},
    {"ParentName",
     "string",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, ParentName),
     0},
    {"Tag", "string", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuObjectParms, Tag), 0},
    {"TexelCoords",
     "bool",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuObjectParms, TexelCoords),
     0},
    {"Selected", "bool", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuObjectParms, Selected), 0},
    {}};

ovrMemberInfo ovrSoundLimiter_Reflection[] = {
    {"LastPlayTime",
     "double",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(ovrSoundLimiter, LastPlayTime)},
    {}};

ovrMemberInfo Fader_Reflection[] = {
    {"FadeState", "eFadeState", nullptr, ovrTypeOperator::NONE, offsetof(Fader, FadeState)},
    {"PrePauseState", "eFadeState", nullptr, ovrTypeOperator::NONE, offsetof(Fader, PrePauseState)},
    {"StartAlpha", "float", nullptr, ovrTypeOperator::NONE, offsetof(Fader, StartAlpha)},
    {"FadeAlpha", "float", nullptr, ovrTypeOperator::NONE, offsetof(Fader, FadeAlpha)},
    {}};

ovrMemberInfo SineFader_Reflection[] = {{}};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

ovrMemberInfo VRMenuComponent_Reflection[] = {
    {"EventFlags",
     "BitFlagsT< eVRMenuEventType, uint64_t >",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(VRMenuComponent, EventFlags)},
    {"Name", "string", nullptr, ovrTypeOperator::NONE, offsetof(VRMenuComponent, Name)},
    {}};

ovrMemberInfo OvrDefaultComponent_Reflection[] = {
    {"GazeOverSoundLimiter",
     "ovrSoundLimiter",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, GazeOverSoundLimiter)},
    {"DownSoundLimiter",
     "ovrSoundLimiter",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, DownSoundLimiter)},
    {"UpSoundLimiter",
     "ovrSoundLimiter",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, UpSoundLimiter)},
    {"HilightFader",
     "SineFader",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, HilightFader)},
    {"StartFadeInTime",
     "double",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, StartFadeInTime)},
    {"StartFadeOutTime",
     "double",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, StartFadeOutTime)},
    {"HilightOffset",
     "Vector3f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, HilightOffset)},
    {"HilightScale",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, HilightScale)},
    {"FadeDuration",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, FadeDuration)},
    {"FadeDelay",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, FadeDelay)},
    {"TextNormalColor",
     "Vector4f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, TextNormalColor)},
    {"TextHilightColor",
     "Vector4f",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, TextHilightColor)},
    {"SuppressText",
     "bool",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, SuppressText)},
    {"UseSurfaceAnimator",
     "bool",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrDefaultComponent, UseSurfaceAnimator)},
    {"NoHilight", "bool", nullptr, ovrTypeOperator::NONE, offsetof(OvrDefaultComponent, NoHilight)},
    {}};

ovrMemberInfo OvrAnimComponent_Reflection[] = {
    {"BaseTime", "double", nullptr, ovrTypeOperator::NONE, offsetof(OvrAnimComponent, BaseTime)},
    {"BaseFrame", "int", nullptr, ovrTypeOperator::NONE, offsetof(OvrAnimComponent, BaseFrame)},
    {"CurFrame", "int", nullptr, ovrTypeOperator::NONE, offsetof(OvrAnimComponent, CurFrame)},
    {"FramesPerSecond",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrAnimComponent, FramesPerSecond)},
    {"AnimState",
     "OvrAnimComponent::eAnimState",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrAnimComponent, AnimState)},
    {"Looping", "bool", nullptr, ovrTypeOperator::NONE, offsetof(OvrAnimComponent, Looping)},
    {"ForceVisibilityUpdate",
     "bool",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrAnimComponent, ForceVisibilityUpdate)},
    {"FractionalFrame",
     "float",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrAnimComponent, FractionalFrame)},
    {"FloatFrame",
     "double",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrAnimComponent, FloatFrame)},
    {}};

ovrMemberInfo OvrSurfaceAnimComponent_Reflection[] = {
    {"SurfacesPerFrame",
     "int",
     nullptr,
     ovrTypeOperator::NONE,
     offsetof(OvrSurfaceAnimComponent, SurfacesPerFrame)},
    {}};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

ovrTypeInfo TypeInfoList[] = {
    {"bool",
     nullptr,
     sizeof(bool),
     nullptr,
     ParseBool,
     Create_bool,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"float",
     nullptr,
     sizeof(float),
     nullptr,
     ParseFloat,
     Create_float,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"double",
     nullptr,
     sizeof(double),
     nullptr,
     ParseDouble,
     Create_double,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"int",
     nullptr,
     sizeof(int),
     nullptr,
     ParseInt,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"uint32_t",
     nullptr,
     sizeof(uint32_t),
     nullptr,
     ParseInt,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},

    {"eVRMenuObjectType",
     nullptr,
     sizeof(eVRMenuObjectType),
     VRMenuObjectType_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"eVRMenuObjectFlags",
     nullptr,
     sizeof(eVRMenuObjectFlags),
     VRMenuObjectFlag_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"eVRMenuObjectInitFlags",
     nullptr,
     sizeof(eVRMenuObjectInitFlags),
     VRMenuObjectInitFlag_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"eVRMenuId",
     nullptr,
     sizeof(eVRMenuId),
     VRMenuId_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"eSurfaceTextureType",
     nullptr,
     sizeof(eSurfaceTextureType),
     SurfaceTextureType_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"HorizontalJustification",
     nullptr,
     sizeof(HorizontalJustification),
     HorizontalJustification_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"VerticalJustification",
     nullptr,
     sizeof(VerticalJustification),
     VerticalJustification_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"eContentFlags",
     nullptr,
     sizeof(eContentFlags),
     eContentFlags_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"eVRMenuEventType",
     nullptr,
     sizeof(eVRMenuEventType),
     VRMenuEventType_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"OvrAnimComponent::eAnimState",
     nullptr,
     sizeof(OvrAnimComponent::eAnimState),
     AnimState_Enums,
     ParseEnum,
     Create_int,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},

    {"BitFlagsT< eVRMenuObjectFlags >",
     nullptr,
     sizeof(BitFlagsT<eVRMenuObjectFlags>),
     VRMenuObjectFlag_Enums,
     ParseBitFlags,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"BitFlagsT< eVRMenuObjectInitFlags >",
     nullptr,
     sizeof(BitFlagsT<eVRMenuObjectInitFlags>),
     VRMenuObjectInitFlag_Enums,
     ParseBitFlags,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"BitFlagsT< eContentFlags >",
     nullptr,
     sizeof(BitFlagsT<eContentFlags>),
     eContentFlags_Enums,
     ParseBitFlags,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"BitFlagsT< eVRMenuEventType, uint64_t >",
     nullptr,
     sizeof(BitFlagsT<eVRMenuEventType, uint64_t>),
     VRMenuEventType_Enums,
     ParseBitFlags,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},

    {"string",
     nullptr,
     sizeof(std::string),
     nullptr,
     ParseString,
     Create_string,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     nullptr},
    {"Vector2i",
     nullptr,
     sizeof(Vector2i),
     nullptr,
     ParseIntVector,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Vector2i_Reflection},
    {"Vector2f",
     nullptr,
     sizeof(Vector2f),
     nullptr,
     ParseFloatVector,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Vector2f_Reflection},
    {"Vector3f",
     nullptr,
     sizeof(Vector3f),
     nullptr,
     ParseFloatVector,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Vector3f_Reflection},
    {"Vector4f",
     nullptr,
     sizeof(Vector4f),
     nullptr,
     ParseFloatVector,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Vector4f_Reflection},
    {"Quatf",
     nullptr,
     sizeof(Quatf),
     nullptr,
     ParseFloatVector,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Quatf_Reflection},
    {"Posef",
     nullptr,
     sizeof(Posef),
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Posef_Reflection},

    {"TypesafeNumberT< long long, eVRMenuId, INVALID_MENU_ID >",
     nullptr,
     sizeof(TypesafeNumberT<long long, eVRMenuId, INVALID_MENU_ID>),
     nullptr,
     ParseTypesafeNumber_long_long,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     TypesafeNumberT_longlong_eVRMenuId_INVALID_MENU_ID},

    {"std::vector< VRMenuComponent* >",
     nullptr,
     sizeof(std::vector<VRMenuComponent*>),
     nullptr,
     ParseArray,
     nullptr,
     Resize_std_vector_VRMenuComponent_Ptr,
     SetArrayElementFn_std_vector_VRMenuComponent_Ptr,
     ovrArrayType::OVR_POINTER,
     false,
     nullptr},
    {"std::vector< VRMenuSurfaceParms >",
     nullptr,
     sizeof(std::vector<VRMenuSurfaceParms>),
     nullptr,
     ParseArray,
     nullptr,
     Resize_std_vector_VRMenuSurfaceParms,
     SetArrayElementFn_std_vector_VRMenuSurfaceParms,
     ovrArrayType::OVR_OBJECT,
     false,
     nullptr},
    {"std::vector< VRMenuObjectParms* >",
     nullptr,
     sizeof(std::vector<VRMenuObjectParms*>),
     nullptr,
     ParseArray,
     nullptr,
     Resize_std_vector_VRMenuObjectParms_Ptr,
     SetArrayElementFn_std_vector_VRMenuObjectParms_Ptr,
     ovrArrayType::OVR_POINTER,
     false,
     nullptr},

    {"string[]",
     nullptr,
     sizeof(std::string),
     nullptr,
     ParseArray,
     nullptr,
     nullptr,
     SetArrayElementFn_string,
     ovrArrayType::C_OBJECT,
     false,
     nullptr},
    {"int[]",
     nullptr,
     sizeof(int),
     nullptr,
     ParseArray,
     nullptr,
     nullptr,
     SetArrayElementFn_int,
     ovrArrayType::C_OBJECT,
     false,
     nullptr},
    {"uint32_t[]",
     nullptr,
     sizeof(uint32_t),
     nullptr,
     ParseArray,
     nullptr,
     nullptr,
     SetArrayElementFn_uint32_t,
     ovrArrayType::C_OBJECT,
     false,
     nullptr},
    {"eSurfaceTextureType[]",
     nullptr,
     sizeof(eSurfaceTextureType),
     nullptr,
     ParseArray,
     nullptr,
     nullptr,
     SetArrayElementFn_eSurfaceTextureType,
     ovrArrayType::C_OBJECT,
     false,
     nullptr},

    {"VRMenuSurfaceParms",
     nullptr,
     sizeof(VRMenuSurfaceParms),
     nullptr,
     nullptr,
     Create_VRMenuSurfaceParms,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     VRMenuSurfaceParms_Reflection},
    {"VRMenuFontParms",
     nullptr,
     sizeof(VRMenuFontParms),
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     VRMenuFontParms_Reflection},
    {"VRMenuObjectParms",
     nullptr,
     sizeof(VRMenuObjectParms),
     nullptr,
     nullptr,
     Create_VRMenuObjectParms,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     VRMenuObjectParms_Reflection},

    {"Fader",
     nullptr,
     sizeof(Fader),
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     Fader_Reflection},
    {"SineFader",
     "Fader",
     sizeof(SineFader),
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     SineFader_Reflection},
    {"ovrSoundLimiter",
     nullptr,
     sizeof(ovrSoundLimiter),
     nullptr,
     nullptr,
     Create_ovrSoundLimiter,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     ovrSoundLimiter_Reflection},
    {"VRMenuComponent",
     nullptr,
     sizeof(VRMenuComponent),
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     true,
     VRMenuComponent_Reflection},
    {"OvrDefaultComponent",
     "VRMenuComponent",
     sizeof(OvrDefaultComponent),
     nullptr,
     nullptr,
     Create_OvrDefaultComponent,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     OvrDefaultComponent_Reflection},
    {"OvrAnimComponent",
     "VRMenuComponent",
     sizeof(OvrAnimComponent),
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     true,
     OvrAnimComponent_Reflection},
    {"OvrSurfaceAnimComponent",
     "OvrAnimComponent",
     sizeof(OvrSurfaceAnimComponent),
     nullptr,
     nullptr,
     Create_OvrSurfaceAnimComponent,
     nullptr,
     nullptr,
     ovrArrayType::NONE,
     false,
     OvrSurfaceAnimComponent_Reflection},

    {}};

} // namespace OVRFW
