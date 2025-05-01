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

Filename    :   openxr_extension_helpers.h
Content     :   Helpers for private and experimental extension definition headers.
Language    :   C99

*************************************************************************************/

#pragma once

#include <openxr/openxr.h>

#define XR_ENUM(type, enm, constant) static const type enm = (type)constant
#define XR_STRUCT_ENUM(enm, constant) XR_ENUM(XrStructureType, enm, constant)
#define XR_REFSPACE_ENUM(enm, constant) XR_ENUM(XrReferenceSpaceType, enm, constant)
#define XR_RESULT_ENUM(enm, constant) XR_ENUM(XrResult, enm, constant)
#define XR_COMPONENT_ENUM(enm, constant) XR_ENUM(XrComponentTypeFB, enm, constant)
#define XR_OBJECT_TYPE_ENUM(enm, constant) XR_ENUM(XrObjectType, enm, constant)
