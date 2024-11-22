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
#ifndef META_SPATIAL_ENTITY_SHARING_H_
#define META_SPATIAL_ENTITY_SHARING_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_spatial_entity_sharing

// XR_META_spatial_entity_sharing is a preprocessor guard. Do not pass it to API calls.
#define XR_META_spatial_entity_sharing 1
#define XR_META_spatial_entity_sharing_SPEC_VERSION 1
#define XR_META_SPATIAL_ENTITY_SHARING_EXTENSION_NAME "XR_META_spatial_entity_sharing"
static const XrStructureType XR_TYPE_SYSTEM_SPATIAL_ENTITY_SHARING_PROPERTIES_META = (XrStructureType) 1000290000;
static const XrStructureType XR_TYPE_SHARE_SPACES_INFO_META = (XrStructureType) 1000290001;
static const XrStructureType XR_TYPE_EVENT_DATA_SHARE_SPACES_COMPLETE_META = (XrStructureType) 1000290002;
typedef struct XrSystemSpatialEntitySharingPropertiesMETA {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsSpatialEntitySharing;
} XrSystemSpatialEntitySharingPropertiesMETA;

typedef struct XR_MAY_ALIAS XrShareSpacesRecipientBaseHeaderMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrShareSpacesRecipientBaseHeaderMETA;

typedef struct XrShareSpacesInfoMETA {
    XrStructureType                                type;
    const void* XR_MAY_ALIAS                       next;
    uint32_t                                       spaceCount;
    XrSpace*                                       spaces;
    const XrShareSpacesRecipientBaseHeaderMETA*    recipientInfo;
} XrShareSpacesInfoMETA;

typedef struct XrEventDataShareSpacesCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
    XrResult                    result;
} XrEventDataShareSpacesCompleteMETA;

typedef XrResult (XRAPI_PTR *PFN_xrShareSpacesMETA)(XrSession session, const XrShareSpacesInfoMETA* info, XrAsyncRequestIdFB* requestId);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrShareSpacesMETA(
    XrSession                                   session,
    const XrShareSpacesInfoMETA*                info,
    XrAsyncRequestIdFB*                         requestId);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_spatial_entity_sharing */

#ifdef __cplusplus
}
#endif

#endif
