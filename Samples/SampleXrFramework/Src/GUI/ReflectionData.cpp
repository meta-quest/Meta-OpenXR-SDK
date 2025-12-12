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
    {.Name = "HORIZONTAL_LEFT", .Value = 0},
    {.Name = "HORIZONTAL_CENTER", .Value = 1},
    {.Name = "HORIZONTAL_RIGHT", .Value = 2}};
OVR_VERIFY_ARRAY_SIZE(HorizontalJustification_Enums, 3);

ovrEnumInfo VerticalJustification_Enums[] = {
    {.Name = "VERTICAL_BASELINE", .Value = 0},
    {.Name = "VERTICAL_CENTER", .Value = 1},
    {.Name = "VERTICAL_CENTER_FIXEDHEIGHT", .Value = 2},
    {.Name = "VERTICAL_TOP", .Value = 3}};
OVR_VERIFY_ARRAY_SIZE(VerticalJustification_Enums, 4);

ovrEnumInfo eContentFlags_Enums[] = {
    {.Name = "CONTENT_NONE", .Value = 0},
    {.Name = "CONTENT_SOLID", .Value = 1},
    {.Name = "CONTENT_ALL", .Value = 0x7fffffff},
};
OVR_VERIFY_ARRAY_SIZE(eContentFlags_Enums, 3);

ovrEnumInfo VRMenuObjectType_Enums[] = {
    {.Name = "VRMENU_CONTAINER", .Value = 0},
    {.Name = "VRMENU_STATIC", .Value = 1},
    {.Name = "VRMENU_BUTTON", .Value = 2},
    {.Name = "VRMENU_MAX", .Value = 3}};
OVR_VERIFY_ARRAY_SIZE(VRMenuObjectType_Enums, VRMENU_MAX + 1);

ovrEnumInfo VRMenuObjectFlag_Enums[] = {
    {.Name = "VRMENUOBJECT_FLAG_NO_FOCUS_GAINED", .Value = 0},
    {.Name = "VRMENUOBJECT_DONT_HIT_ALL", .Value = 1},
    {.Name = "VRMENUOBJECT_DONT_HIT_TEXT", .Value = 2},
    {.Name = "VRMENUOBJECT_HIT_ONLY_BOUNDS", .Value = 3},
    {.Name = "VRMENUOBJECT_BOUND_ALL", .Value = 4},
    {.Name = "VRMENUOBJECT_FLAG_POLYGON_OFFSET", .Value = 5},
    {.Name = "VRMENUOBJECT_FLAG_NO_DEPTH", .Value = 6},
    {.Name = "VRMENUOBJECT_FLAG_NO_DEPTH_MASK", .Value = 7},
    {.Name = "VRMENUOBJECT_DONT_RENDER", .Value = 8},
    {.Name = "VRMENUOBJECT_DONT_RENDER_SURFACE", .Value = 9},
    {.Name = "VRMENUOBJECT_DONT_RENDER_TEXT", .Value = 10},
    {.Name = "VRMENUOBJECT_NO_GAZE_HILIGHT", .Value = 11},
    {.Name = "VRMENUOBJECT_RENDER_HIERARCHY_ORDER", .Value = 12},
    {.Name = "VRMENUOBJECT_FLAG_BILLBOARD", .Value = 13},
    {.Name = "VRMENUOBJECT_DONT_MOD_PARENT_COLOR", .Value = 14},
    {.Name = "VRMENUOBJECT_INSTANCE_TEXT", .Value = 15}};
OVR_VERIFY_ARRAY_SIZE(VRMenuObjectFlag_Enums, VRMENUOBJECT_MAX);

ovrEnumInfo VRMenuObjectInitFlag_Enums[] = {
    {.Name = "VRMENUOBJECT_INIT_ALIGN_TO_VIEW", .Value = 0},
    {.Name = "VRMENUOBJECT_INIT_FORCE_POSITION", .Value = 1}};
OVR_VERIFY_ARRAY_SIZE(VRMenuObjectInitFlag_Enums, 2);

ovrEnumInfo SurfaceTextureType_Enums[] = {
    {.Name = "SURFACE_TEXTURE_DIFFUSE", .Value = 0},
    {.Name = "SURFACE_TEXTURE_DIFFUSE_ALPHA_DISCARD", .Value = 1},
    {.Name = "SURFACE_TEXTURE_ADDITIVE", .Value = 2},
    {.Name = "SURFACE_TEXTURE_COLOR_RAMP", .Value = 3},
    {.Name = "SURFACE_TEXTURE_COLOR_RAMP_TARGET", .Value = 4},
    {.Name = "SURFACE_TEXTURE_ALPHA_MASK", .Value = 5},
    {.Name = "SURFACE_TEXTURE_MAX", .Value = 6}};
OVR_VERIFY_ARRAY_SIZE(SurfaceTextureType_Enums, SURFACE_TEXTURE_MAX + 1);

ovrEnumInfo VRMenuId_Enums[] = {{.Name = "INVALID_MENU_ID", .Value = INT_MIN}};

ovrEnumInfo VRMenuEventType_Enums[] = {
    {.Name = "VRMENU_EVENT_FOCUS_GAINED", .Value = 0},
    {.Name = "VRMENU_EVENT_FOCUS_LOST", .Value = 1},
    {.Name = "VRMENU_EVENT_TOUCH_DOWN", .Value = 2},
    {.Name = "VRMENU_EVENT_TOUCH_UP", .Value = 3},
    {.Name = "VRMENU_EVENT_TOUCH_RELATIVE", .Value = 4},
    {.Name = "VRMENU_EVENT_TOUCH_ABSOLUTE", .Value = 5},
    {.Name = "VRMENU_EVENT_SWIPE_FORWARD", .Value = 6},
    {.Name = "VRMENU_EVENT_SWIPE_BACK", .Value = 7},
    {.Name = "VRMENU_EVENT_SWIPE_UP", .Value = 8},
    {.Name = "VRMENU_EVENT_SWIPE_DOWN", .Value = 9},
    {.Name = "VRMENU_EVENT_FRAME_UPDATE", .Value = 10},
    {.Name = "VRMENU_EVENT_SUBMIT_FOR_RENDERING", .Value = 11},
    {.Name = "VRMENU_EVENT_RENDER", .Value = 12},
    {.Name = "VRMENU_EVENT_OPENING", .Value = 13},
    {.Name = "VRMENU_EVENT_OPENED", .Value = 14},
    {.Name = "VRMENU_EVENT_CLOSING", .Value = 15},
    {.Name = "VRMENU_EVENT_CLOSED", .Value = 16},
    {.Name = "VRMENU_EVENT_INIT", .Value = 17},
    {.Name = "VRMENU_EVENT_SELECTED", .Value = 18},
    {.Name = "VRMENU_EVENT_DESELECTED", .Value = 19},
    {.Name = "VRMENU_EVENT_UPDATE_OBJECT", .Value = 20},
    {.Name = "VRMENU_EVENT_SWIPE_COMPLETE", .Value = 21},
    {.Name = "VRMENU_EVENT_ITEM_ACTION_COMPLETE", .Value = 22},
    {.Name = "VRMENU_EVENT_MAX", .Value = 23}};
OVR_VERIFY_ARRAY_SIZE(VRMenuEventType_Enums, VRMENU_EVENT_MAX + 1);

ovrEnumInfo AnimState_Enums[] = {
    {.Name = "ANIMSTATE_PAUSED", .Value = 0},
    {.Name = "ANIMSTATE_PLAYING", .Value = 1}};
OVR_VERIFY_ARRAY_SIZE(AnimState_Enums, OvrAnimComponent::ANIMSTATE_MAX);

ovrEnumInfo EventDispatchType_Enums[] = {
    {.Name = "EVENT_DISPATCH_TARGET", .Value = 0},
    {.Name = "EVENT_DISPATCH_FOCUS", .Value = 1},
    {.Name = "EVENT_DISPATCH_BROADCAST", .Value = 2}};
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
    {.MemberName = "x",
     .TypeName = "int",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector2i, x),
     .ArraySize = 0},
    {.MemberName = "y",
     .TypeName = "int",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector2i, y),
     .ArraySize = 0},
    {}};

ovrMemberInfo Vector2f_Reflection[] = {
    {.MemberName = "x",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector2f, x),
     .ArraySize = 0},
    {.MemberName = "y",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector2f, y),
     .ArraySize = 0},
    {}};

ovrMemberInfo Vector3f_Reflection[] = {
    {.MemberName = "x",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector3f, x),
     .ArraySize = 0},
    {.MemberName = "y",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector3f, y),
     .ArraySize = 0},
    {.MemberName = "z",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector3f, z),
     .ArraySize = 0},
    {}};

ovrMemberInfo Vector4f_Reflection[] = {
    {.MemberName = "x",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector4f, x),
     .ArraySize = 0},
    {.MemberName = "y",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector4f, y),
     .ArraySize = 0},
    {.MemberName = "z",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector4f, z),
     .ArraySize = 0},
    {.MemberName = "w",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Vector4f, w),
     .ArraySize = 0},
    {}};

ovrMemberInfo Quatf_Reflection[] = {
    {.MemberName = "x",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Quatf, x),
     .ArraySize = 0},
    {.MemberName = "y",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Quatf, y),
     .ArraySize = 0},
    {.MemberName = "z",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Quatf, z),
     .ArraySize = 0},
    {.MemberName = "w",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Quatf, w),
     .ArraySize = 0},
    {}};

ovrMemberInfo TypesafeNumberT_longlong_eVRMenuId_INVALID_MENU_ID[] = {
    {.MemberName = "Value",
     .TypeName = "long long",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuId_t, Value),
     .ArraySize = 0},
    {}};

ovrMemberInfo Posef_Reflection[] = {
    {.MemberName = "Position",
     .TypeName = "Vector3f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Posef, Translation),
     .ArraySize = 0},
    {.MemberName = "Orientation",
     .TypeName = "Quatf",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Posef, Rotation),
     .ArraySize = 0},
    {}};

ovrMemberInfo VRMenuSurfaceParms_Reflection[] = {
    {.MemberName = "SurfaceName",
     .TypeName = "string",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, SurfaceName),
     .ArraySize = 0},
    {.MemberName = "ImageNames",
     .TypeName = "string[]",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuSurfaceParms, ImageNames),
     .ArraySize = VRMENUSURFACE_IMAGE_MAX},
    {.MemberName = "ImageTexId",
     .TypeName = "uint32_t[]",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuSurfaceParms, ImageTexId),
     .ArraySize = VRMENUSURFACE_IMAGE_MAX},
    {.MemberName = "ImageWidth",
     .TypeName = "int[]",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuSurfaceParms, ImageWidth),
     .ArraySize = VRMENUSURFACE_IMAGE_MAX},
    {.MemberName = "ImageHeight",
     .TypeName = "int[]",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuSurfaceParms, ImageHeight),
     .ArraySize = VRMENUSURFACE_IMAGE_MAX},
    {.MemberName = "TextureTypes",
     .TypeName = "eSurfaceTextureType[]",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuSurfaceParms, TextureTypes),
     .ArraySize = VRMENUSURFACE_IMAGE_MAX},
    {.MemberName = "Contents",
     .TypeName = "BitFlagsT< eContentFlags >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, Contents),
     .ArraySize = 0},
    {.MemberName = "Color",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, Color),
     .ArraySize = 0},
    {.MemberName = "Anchors",
     .TypeName = "Vector2f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, Anchors),
     .ArraySize = 0},
    {.MemberName = "Border",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, Border),
     .ArraySize = 0},
    {.MemberName = "Dims",
     .TypeName = "Vector2f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, Dims),
     .ArraySize = 0},
    {.MemberName = "CropUV",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, CropUV),
     .ArraySize = 0},
    {.MemberName = "OffsetUVs",
     .TypeName = "Vector2f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuSurfaceParms, OffsetUVs),
     .ArraySize = 0},
    {}};

ovrMemberInfo VRMenuFontParms_Reflection[] = {
    {.MemberName = "AlignHoriz",
     .TypeName = "HorizontalJustification",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, AlignHoriz),
     .ArraySize = 0},
    {.MemberName = "AlignVert",
     .TypeName = "VerticalJustification",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, AlignVert),
     .ArraySize = 0},
    {.MemberName = "Billboard",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, Billboard),
     .ArraySize = 0},
    {.MemberName = "TrackRoll",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, TrackRoll),
     .ArraySize = 0},
    {.MemberName = "Outline",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, Outline),
     .ArraySize = 0},
    {.MemberName = "ColorCenter",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, ColorCenter)},
    {.MemberName = "AlphaCenter",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, AlphaCenter)},
    {.MemberName = "Scale",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, Scale),
     .ArraySize = 0},
    {.MemberName = "WrapWidth",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, WrapWidth),
     .ArraySize = 0},
    {.MemberName = "MaxLines",
     .TypeName = "int",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, MaxLines),
     .ArraySize = 0},
    {.MemberName = "MultiLine",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuFontParms, MultiLine),
     .ArraySize = 0},
    {}};

ovrMemberInfo VRMenuObjectParms_Reflection[] = {
    {.MemberName = "Type",
     .TypeName = "eVRMenuObjectType",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Type),
     .ArraySize = 0},
    {.MemberName = "Flags",
     .TypeName = "BitFlagsT< eVRMenuObjectFlags >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Flags),
     .ArraySize = 0},
    {.MemberName = "InitFlags",
     .TypeName = "BitFlagsT< eVRMenuObjectInitFlags >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, InitFlags),
     .ArraySize = 0},
    {.MemberName = "Components",
     .TypeName = "std::vector< VRMenuComponent* >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuObjectParms, Components),
     .ArraySize = 0},
    {.MemberName = "SurfaceParms",
     .TypeName = "std::vector< VRMenuSurfaceParms >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::ARRAY,
     .Offset = offsetof(VRMenuObjectParms, SurfaceParms),
     .ArraySize = 0},
    {.MemberName = "Text",
     .TypeName = "string",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Text),
     .ArraySize = 0},
    {.MemberName = "LocalPose",
     .TypeName = "Posef",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, LocalPose),
     .ArraySize = 0},
    {.MemberName = "LocalScale",
     .TypeName = "Vector3f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, LocalScale),
     .ArraySize = 0},
    {.MemberName = "TextLocalPose",
     .TypeName = "Posef",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, TextLocalPose),
     .ArraySize = 0},
    {.MemberName = "TextLocalScale",
     .TypeName = "Vector3f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, TextLocalScale),
     .ArraySize = 0},
    {.MemberName = "FontParms",
     .TypeName = "VRMenuFontParms",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, FontParms),
     .ArraySize = 0},
    {.MemberName = "Color",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Color),
     .ArraySize = 0},
    {.MemberName = "TextColor",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, TextColor),
     .ArraySize = 0},
    {.MemberName = "Id",
     .TypeName = "TypesafeNumberT< long long, eVRMenuId, INVALID_MENU_ID >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Id),
     .ArraySize = 0},
    {.MemberName = "ParentId",
     .TypeName = "TypesafeNumberT< long long, eVRMenuId, INVALID_MENU_ID >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, ParentId),
     .ArraySize = 0},
    {.MemberName = "Contents",
     .TypeName = "BitFlagsT< eContentFlags >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Contents),
     .ArraySize = 0},
    {.MemberName = "Name",
     .TypeName = "string",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Name),
     .ArraySize = 0},
    {.MemberName = "ParentName",
     .TypeName = "string",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, ParentName),
     .ArraySize = 0},
    {.MemberName = "Tag",
     .TypeName = "string",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Tag),
     .ArraySize = 0},
    {.MemberName = "TexelCoords",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, TexelCoords),
     .ArraySize = 0},
    {.MemberName = "Selected",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuObjectParms, Selected),
     .ArraySize = 0},
    {}};

ovrMemberInfo ovrSoundLimiter_Reflection[] = {
    {.MemberName = "LastPlayTime",
     .TypeName = "double",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(ovrSoundLimiter, LastPlayTime)},
    {}};

ovrMemberInfo Fader_Reflection[] = {
    {.MemberName = "FadeState",
     .TypeName = "eFadeState",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Fader, FadeState)},
    {.MemberName = "PrePauseState",
     .TypeName = "eFadeState",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Fader, PrePauseState)},
    {.MemberName = "StartAlpha",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Fader, StartAlpha)},
    {.MemberName = "FadeAlpha",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(Fader, FadeAlpha)},
    {}};

ovrMemberInfo SineFader_Reflection[] = {{}};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

ovrMemberInfo VRMenuComponent_Reflection[] = {
    {.MemberName = "EventFlags",
     .TypeName = "BitFlagsT< eVRMenuEventType, uint64_t >",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuComponent, EventFlags)},
    {.MemberName = "Name",
     .TypeName = "string",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(VRMenuComponent, Name)},
    {}};

ovrMemberInfo OvrDefaultComponent_Reflection[] = {
    {.MemberName = "GazeOverSoundLimiter",
     .TypeName = "ovrSoundLimiter",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, GazeOverSoundLimiter)},
    {.MemberName = "DownSoundLimiter",
     .TypeName = "ovrSoundLimiter",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, DownSoundLimiter)},
    {.MemberName = "UpSoundLimiter",
     .TypeName = "ovrSoundLimiter",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, UpSoundLimiter)},
    {.MemberName = "HilightFader",
     .TypeName = "SineFader",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, HilightFader)},
    {.MemberName = "StartFadeInTime",
     .TypeName = "double",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, StartFadeInTime)},
    {.MemberName = "StartFadeOutTime",
     .TypeName = "double",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, StartFadeOutTime)},
    {.MemberName = "HilightOffset",
     .TypeName = "Vector3f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, HilightOffset)},
    {.MemberName = "HilightScale",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, HilightScale)},
    {.MemberName = "FadeDuration",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, FadeDuration)},
    {.MemberName = "FadeDelay",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, FadeDelay)},
    {.MemberName = "TextNormalColor",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, TextNormalColor)},
    {.MemberName = "TextHilightColor",
     .TypeName = "Vector4f",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, TextHilightColor)},
    {.MemberName = "SuppressText",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, SuppressText)},
    {.MemberName = "UseSurfaceAnimator",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, UseSurfaceAnimator)},
    {.MemberName = "NoHilight",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrDefaultComponent, NoHilight)},
    {}};

ovrMemberInfo OvrAnimComponent_Reflection[] = {
    {.MemberName = "BaseTime",
     .TypeName = "double",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, BaseTime)},
    {.MemberName = "BaseFrame",
     .TypeName = "int",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, BaseFrame)},
    {.MemberName = "CurFrame",
     .TypeName = "int",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, CurFrame)},
    {.MemberName = "FramesPerSecond",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, FramesPerSecond)},
    {.MemberName = "AnimState",
     .TypeName = "OvrAnimComponent::eAnimState",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, AnimState)},
    {.MemberName = "Looping",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, Looping)},
    {.MemberName = "ForceVisibilityUpdate",
     .TypeName = "bool",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, ForceVisibilityUpdate)},
    {.MemberName = "FractionalFrame",
     .TypeName = "float",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, FractionalFrame)},
    {.MemberName = "FloatFrame",
     .TypeName = "double",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrAnimComponent, FloatFrame)},
    {}};

ovrMemberInfo OvrSurfaceAnimComponent_Reflection[] = {
    {.MemberName = "SurfacesPerFrame",
     .TypeName = "int",
     .VarType = nullptr,
     .Operator = ovrTypeOperator::NONE,
     .Offset = offsetof(OvrSurfaceAnimComponent, SurfacesPerFrame)},
    {}};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

ovrTypeInfo TypeInfoList[] = {
    {.TypeName = "bool",
     .ParentTypeName = nullptr,
     .Size = sizeof(bool),
     .EnumInfos = nullptr,
     .ParseFn = ParseBool,
     .CreateFn = Create_bool,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "float",
     .ParentTypeName = nullptr,
     .Size = sizeof(float),
     .EnumInfos = nullptr,
     .ParseFn = ParseFloat,
     .CreateFn = Create_float,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "double",
     .ParentTypeName = nullptr,
     .Size = sizeof(double),
     .EnumInfos = nullptr,
     .ParseFn = ParseDouble,
     .CreateFn = Create_double,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "int",
     .ParentTypeName = nullptr,
     .Size = sizeof(int),
     .EnumInfos = nullptr,
     .ParseFn = ParseInt,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "uint32_t",
     .ParentTypeName = nullptr,
     .Size = sizeof(uint32_t),
     .EnumInfos = nullptr,
     .ParseFn = ParseInt,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},

    {.TypeName = "eVRMenuObjectType",
     .ParentTypeName = nullptr,
     .Size = sizeof(eVRMenuObjectType),
     .EnumInfos = VRMenuObjectType_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eVRMenuObjectFlags",
     .ParentTypeName = nullptr,
     .Size = sizeof(eVRMenuObjectFlags),
     .EnumInfos = VRMenuObjectFlag_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eVRMenuObjectInitFlags",
     .ParentTypeName = nullptr,
     .Size = sizeof(eVRMenuObjectInitFlags),
     .EnumInfos = VRMenuObjectInitFlag_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eVRMenuId",
     .ParentTypeName = nullptr,
     .Size = sizeof(eVRMenuId),
     .EnumInfos = VRMenuId_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eSurfaceTextureType",
     .ParentTypeName = nullptr,
     .Size = sizeof(eSurfaceTextureType),
     .EnumInfos = SurfaceTextureType_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "HorizontalJustification",
     .ParentTypeName = nullptr,
     .Size = sizeof(HorizontalJustification),
     .EnumInfos = HorizontalJustification_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "VerticalJustification",
     .ParentTypeName = nullptr,
     .Size = sizeof(VerticalJustification),
     .EnumInfos = VerticalJustification_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eContentFlags",
     .ParentTypeName = nullptr,
     .Size = sizeof(eContentFlags),
     .EnumInfos = eContentFlags_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eVRMenuEventType",
     .ParentTypeName = nullptr,
     .Size = sizeof(eVRMenuEventType),
     .EnumInfos = VRMenuEventType_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "OvrAnimComponent::eAnimState",
     .ParentTypeName = nullptr,
     .Size = sizeof(OvrAnimComponent::eAnimState),
     .EnumInfos = AnimState_Enums,
     .ParseFn = ParseEnum,
     .CreateFn = Create_int,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},

    {.TypeName = "BitFlagsT< eVRMenuObjectFlags >",
     .ParentTypeName = nullptr,
     .Size = sizeof(BitFlagsT<eVRMenuObjectFlags>),
     .EnumInfos = VRMenuObjectFlag_Enums,
     .ParseFn = ParseBitFlags,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "BitFlagsT< eVRMenuObjectInitFlags >",
     .ParentTypeName = nullptr,
     .Size = sizeof(BitFlagsT<eVRMenuObjectInitFlags>),
     .EnumInfos = VRMenuObjectInitFlag_Enums,
     .ParseFn = ParseBitFlags,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "BitFlagsT< eContentFlags >",
     .ParentTypeName = nullptr,
     .Size = sizeof(BitFlagsT<eContentFlags>),
     .EnumInfos = eContentFlags_Enums,
     .ParseFn = ParseBitFlags,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "BitFlagsT< eVRMenuEventType, uint64_t >",
     .ParentTypeName = nullptr,
     .Size = sizeof(BitFlagsT<eVRMenuEventType, uint64_t>),
     .EnumInfos = VRMenuEventType_Enums,
     .ParseFn = ParseBitFlags,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},

    {.TypeName = "string",
     .ParentTypeName = nullptr,
     .Size = sizeof(std::string),
     .EnumInfos = nullptr,
     .ParseFn = ParseString,
     .CreateFn = Create_string,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "Vector2i",
     .ParentTypeName = nullptr,
     .Size = sizeof(Vector2i),
     .EnumInfos = nullptr,
     .ParseFn = ParseIntVector,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Vector2i_Reflection},
    {.TypeName = "Vector2f",
     .ParentTypeName = nullptr,
     .Size = sizeof(Vector2f),
     .EnumInfos = nullptr,
     .ParseFn = ParseFloatVector,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Vector2f_Reflection},
    {.TypeName = "Vector3f",
     .ParentTypeName = nullptr,
     .Size = sizeof(Vector3f),
     .EnumInfos = nullptr,
     .ParseFn = ParseFloatVector,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Vector3f_Reflection},
    {.TypeName = "Vector4f",
     .ParentTypeName = nullptr,
     .Size = sizeof(Vector4f),
     .EnumInfos = nullptr,
     .ParseFn = ParseFloatVector,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Vector4f_Reflection},
    {.TypeName = "Quatf",
     .ParentTypeName = nullptr,
     .Size = sizeof(Quatf),
     .EnumInfos = nullptr,
     .ParseFn = ParseFloatVector,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Quatf_Reflection},
    {.TypeName = "Posef",
     .ParentTypeName = nullptr,
     .Size = sizeof(Posef),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Posef_Reflection},

    {.TypeName = "TypesafeNumberT< long long, eVRMenuId, INVALID_MENU_ID >",
     .ParentTypeName = nullptr,
     .Size = sizeof(TypesafeNumberT<long long, eVRMenuId, INVALID_MENU_ID>),
     .EnumInfos = nullptr,
     .ParseFn = ParseTypesafeNumber_long_long,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = TypesafeNumberT_longlong_eVRMenuId_INVALID_MENU_ID},

    {.TypeName = "std::vector< VRMenuComponent* >",
     .ParentTypeName = nullptr,
     .Size = sizeof(std::vector<VRMenuComponent*>),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = Resize_std_vector_VRMenuComponent_Ptr,
     .SetArrayElementFn = SetArrayElementFn_std_vector_VRMenuComponent_Ptr,
     .ArrayType = ovrArrayType::OVR_POINTER,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "std::vector< VRMenuSurfaceParms >",
     .ParentTypeName = nullptr,
     .Size = sizeof(std::vector<VRMenuSurfaceParms>),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = Resize_std_vector_VRMenuSurfaceParms,
     .SetArrayElementFn = SetArrayElementFn_std_vector_VRMenuSurfaceParms,
     .ArrayType = ovrArrayType::OVR_OBJECT,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "std::vector< VRMenuObjectParms* >",
     .ParentTypeName = nullptr,
     .Size = sizeof(std::vector<VRMenuObjectParms*>),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = Resize_std_vector_VRMenuObjectParms_Ptr,
     .SetArrayElementFn = SetArrayElementFn_std_vector_VRMenuObjectParms_Ptr,
     .ArrayType = ovrArrayType::OVR_POINTER,
     .Abstract = false,
     .MemberInfo = nullptr},

    {.TypeName = "string[]",
     .ParentTypeName = nullptr,
     .Size = sizeof(std::string),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = SetArrayElementFn_string,
     .ArrayType = ovrArrayType::C_OBJECT,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "int[]",
     .ParentTypeName = nullptr,
     .Size = sizeof(int),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = SetArrayElementFn_int,
     .ArrayType = ovrArrayType::C_OBJECT,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "uint32_t[]",
     .ParentTypeName = nullptr,
     .Size = sizeof(uint32_t),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = SetArrayElementFn_uint32_t,
     .ArrayType = ovrArrayType::C_OBJECT,
     .Abstract = false,
     .MemberInfo = nullptr},
    {.TypeName = "eSurfaceTextureType[]",
     .ParentTypeName = nullptr,
     .Size = sizeof(eSurfaceTextureType),
     .EnumInfos = nullptr,
     .ParseFn = ParseArray,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = SetArrayElementFn_eSurfaceTextureType,
     .ArrayType = ovrArrayType::C_OBJECT,
     .Abstract = false,
     .MemberInfo = nullptr},

    {.TypeName = "VRMenuSurfaceParms",
     .ParentTypeName = nullptr,
     .Size = sizeof(VRMenuSurfaceParms),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = Create_VRMenuSurfaceParms,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = VRMenuSurfaceParms_Reflection},
    {.TypeName = "VRMenuFontParms",
     .ParentTypeName = nullptr,
     .Size = sizeof(VRMenuFontParms),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = VRMenuFontParms_Reflection},
    {.TypeName = "VRMenuObjectParms",
     .ParentTypeName = nullptr,
     .Size = sizeof(VRMenuObjectParms),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = Create_VRMenuObjectParms,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = VRMenuObjectParms_Reflection},

    {.TypeName = "Fader",
     .ParentTypeName = nullptr,
     .Size = sizeof(Fader),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = Fader_Reflection},
    {.TypeName = "SineFader",
     .ParentTypeName = "Fader",
     .Size = sizeof(SineFader),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = SineFader_Reflection},
    {.TypeName = "ovrSoundLimiter",
     .ParentTypeName = nullptr,
     .Size = sizeof(ovrSoundLimiter),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = Create_ovrSoundLimiter,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = ovrSoundLimiter_Reflection},
    {.TypeName = "VRMenuComponent",
     .ParentTypeName = nullptr,
     .Size = sizeof(VRMenuComponent),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = true,
     .MemberInfo = VRMenuComponent_Reflection},
    {.TypeName = "OvrDefaultComponent",
     .ParentTypeName = "VRMenuComponent",
     .Size = sizeof(OvrDefaultComponent),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = Create_OvrDefaultComponent,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = OvrDefaultComponent_Reflection},
    {.TypeName = "OvrAnimComponent",
     .ParentTypeName = "VRMenuComponent",
     .Size = sizeof(OvrAnimComponent),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = nullptr,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = true,
     .MemberInfo = OvrAnimComponent_Reflection},
    {.TypeName = "OvrSurfaceAnimComponent",
     .ParentTypeName = "OvrAnimComponent",
     .Size = sizeof(OvrSurfaceAnimComponent),
     .EnumInfos = nullptr,
     .ParseFn = nullptr,
     .CreateFn = Create_OvrSurfaceAnimComponent,
     .ResizeArrayFn = nullptr,
     .SetArrayElementFn = nullptr,
     .ArrayType = ovrArrayType::NONE,
     .Abstract = false,
     .MemberInfo = OvrSurfaceAnimComponent_Reflection},

    {}};

} // namespace OVRFW
