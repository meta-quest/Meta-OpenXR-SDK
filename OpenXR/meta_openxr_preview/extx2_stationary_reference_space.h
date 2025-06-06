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
#ifndef EXTX2_STATIONARY_REFERENCE_SPACE_H_
#define EXTX2_STATIONARY_REFERENCE_SPACE_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_EXTX2_stationary_reference_space

// XR_EXTX2_stationary_reference_space is a preprocessor guard. Do not pass it to API calls.
#define XR_EXTX2_stationary_reference_space 1
#define XR_EXTX2_stationary_reference_space_SPEC_VERSION 1
#define XR_EXTX2_STATIONARY_REFERENCE_SPACE_EXTENSION_NAME "XR_EXTX2_stationary_reference_space"
static const XrReferenceSpaceType XR_REFERENCE_SPACE_TYPE_STATIONARY_EXTX2 = (XrReferenceSpaceType) 1000742000;
static const XrStructureType XR_TYPE_STATIONARY_REFERENCE_SPACE_ID_GET_INFO_EXTX2 = (XrStructureType) 1000742001;
static const XrStructureType XR_TYPE_STATIONARY_REFERENCE_SPACE_ID_RESULT_EXTX2 = (XrStructureType) 1000742002;
typedef struct XrStationaryReferenceSpaceIdGetInfoEXTX2 {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrStationaryReferenceSpaceIdGetInfoEXTX2;

typedef struct XrStationaryReferenceSpaceIdResultEXTX2 {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrUuid                generationId;
} XrStationaryReferenceSpaceIdResultEXTX2;

typedef XrResult (XRAPI_PTR *PFN_xrGetStationaryReferenceSpaceIdEXTX2)(XrSession  session, const XrStationaryReferenceSpaceIdGetInfoEXTX2* getInfo, XrStationaryReferenceSpaceIdResultEXTX2* result);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetStationaryReferenceSpaceIdEXTX2(
    XrSession                                   session,
    const XrStationaryReferenceSpaceIdGetInfoEXTX2* getInfo,
    XrStationaryReferenceSpaceIdResultEXTX2*    result);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_EXTX2_stationary_reference_space */

#ifdef __cplusplus
}
#endif

#endif
