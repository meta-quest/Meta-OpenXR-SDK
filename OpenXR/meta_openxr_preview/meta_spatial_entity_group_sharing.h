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
#ifndef META_SPATIAL_ENTITY_GROUP_SHARING_H_
#define META_SPATIAL_ENTITY_GROUP_SHARING_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_spatial_entity_group_sharing

// XR_META_spatial_entity_group_sharing is a preprocessor guard. Do not pass it to API calls.
#define XR_META_spatial_entity_group_sharing 1
#define XR_META_spatial_entity_group_sharing_SPEC_VERSION 1
#define XR_META_SPATIAL_ENTITY_GROUP_SHARING_EXTENSION_NAME "XR_META_spatial_entity_group_sharing"
// The group uuid could not be found within the runtime
static const XrResult XR_ERROR_SPACE_GROUP_NOT_FOUND_META = (XrResult) -1000572002;
static const XrStructureType XR_TYPE_SHARE_SPACES_RECIPIENT_GROUPS_META = (XrStructureType) 1000572000;
static const XrStructureType XR_TYPE_SPACE_GROUP_UUID_FILTER_INFO_META = (XrStructureType) 1000572001;
static const XrStructureType XR_TYPE_SYSTEM_SPATIAL_ENTITY_GROUP_SHARING_PROPERTIES_META = (XrStructureType) 1000572100;
typedef struct XrSystemSpatialEntityGroupSharingPropertiesMETA {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsSpatialEntityGroupSharing;
} XrSystemSpatialEntityGroupSharingPropertiesMETA;

typedef struct XrShareSpacesRecipientGroupsMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    groupCount;
    XrUuid*                     groups;
} XrShareSpacesRecipientGroupsMETA;

typedef struct XrSpaceGroupUuidFilterInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrUuid                      groupUuid;
} XrSpaceGroupUuidFilterInfoMETA;

#endif /* XR_META_spatial_entity_group_sharing */

#ifdef __cplusplus
}
#endif

#endif
