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
#ifndef META_COLOCATION_DISCOVERY_H_
#define META_COLOCATION_DISCOVERY_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_colocation_discovery

// XR_META_colocation_discovery is a preprocessor guard. Do not pass it to API calls.
#define XR_META_colocation_discovery 1
#define XR_MAX_COLOCATION_DISCOVERY_BUFFER_SIZE_META 1024
#define XR_META_colocation_discovery_SPEC_VERSION 1
#define XR_META_COLOCATION_DISCOVERY_EXTENSION_NAME "XR_META_colocation_discovery"
// The network request failed.
static const XrResult XR_ERROR_COLOCATION_DISCOVERY_NETWORK_FAILED_META = (XrResult) -1000571001;
// The runtime does not have any methods available to perform discovery.
static const XrResult XR_ERROR_COLOCATION_DISCOVERY_NO_DISCOVERY_METHOD_META = (XrResult) -1000571002;
// Colocation advertisement has already been enabled
static const XrResult XR_COLOCATION_DISCOVERY_ALREADY_ADVERTISING_META = (XrResult) 1000571003;
// Colocation discovery has already been enabled
static const XrResult XR_COLOCATION_DISCOVERY_ALREADY_DISCOVERING_META = (XrResult) 1000571004;
static const XrStructureType XR_TYPE_COLOCATION_DISCOVERY_START_INFO_META = (XrStructureType) 1000571010;
static const XrStructureType XR_TYPE_COLOCATION_DISCOVERY_STOP_INFO_META = (XrStructureType) 1000571011;
static const XrStructureType XR_TYPE_COLOCATION_ADVERTISEMENT_START_INFO_META = (XrStructureType) 1000571012;
static const XrStructureType XR_TYPE_COLOCATION_ADVERTISEMENT_STOP_INFO_META = (XrStructureType) 1000571013;
static const XrStructureType XR_TYPE_EVENT_DATA_START_COLOCATION_ADVERTISEMENT_COMPLETE_META = (XrStructureType) 1000571020;
static const XrStructureType XR_TYPE_EVENT_DATA_STOP_COLOCATION_ADVERTISEMENT_COMPLETE_META = (XrStructureType) 1000571021;
static const XrStructureType XR_TYPE_EVENT_DATA_COLOCATION_ADVERTISEMENT_COMPLETE_META = (XrStructureType) 1000571022;
static const XrStructureType XR_TYPE_EVENT_DATA_START_COLOCATION_DISCOVERY_COMPLETE_META = (XrStructureType) 1000571023;
static const XrStructureType XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_RESULT_META = (XrStructureType) 1000571024;
static const XrStructureType XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_COMPLETE_META = (XrStructureType) 1000571025;
static const XrStructureType XR_TYPE_EVENT_DATA_STOP_COLOCATION_DISCOVERY_COMPLETE_META = (XrStructureType) 1000571026;
static const XrStructureType XR_TYPE_SYSTEM_COLOCATION_DISCOVERY_PROPERTIES_META = (XrStructureType) 1000571030;
typedef struct XrColocationDiscoveryStartInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrColocationDiscoveryStartInfoMETA;

typedef struct XrColocationDiscoveryStopInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrColocationDiscoveryStopInfoMETA;

typedef struct XrColocationAdvertisementStartInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    bufferSize;
    uint8_t*                    buffer;
} XrColocationAdvertisementStartInfoMETA;

typedef struct XrColocationAdvertisementStopInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrColocationAdvertisementStopInfoMETA;

typedef struct XrEventDataStartColocationAdvertisementCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          advertisementRequestId;
    XrResult                    result;
    XrUuid                      advertisementUuid;
} XrEventDataStartColocationAdvertisementCompleteMETA;

typedef struct XrEventDataStopColocationAdvertisementCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
    XrResult                    result;
} XrEventDataStopColocationAdvertisementCompleteMETA;

typedef struct XrEventDataColocationAdvertisementCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          advertisementRequestId;
    XrResult                    result;
} XrEventDataColocationAdvertisementCompleteMETA;

typedef struct XrEventDataStartColocationDiscoveryCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          discoveryRequestId;
    XrResult                    result;
} XrEventDataStartColocationDiscoveryCompleteMETA;

typedef struct XrEventDataColocationDiscoveryResultMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          discoveryRequestId;
    XrUuid                      advertisementUuid;
    uint32_t                    bufferSize;
    uint8_t                     buffer[XR_MAX_COLOCATION_DISCOVERY_BUFFER_SIZE_META];
} XrEventDataColocationDiscoveryResultMETA;

typedef struct XrEventDataColocationDiscoveryCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          discoveryRequestId;
    XrResult                    result;
} XrEventDataColocationDiscoveryCompleteMETA;

typedef struct XrEventDataStopColocationDiscoveryCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
    XrResult                    result;
} XrEventDataStopColocationDiscoveryCompleteMETA;

typedef struct XrSystemColocationDiscoveryPropertiesMETA {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsColocationDiscovery;
} XrSystemColocationDiscoveryPropertiesMETA;

typedef XrResult (XRAPI_PTR *PFN_xrStartColocationDiscoveryMETA)(XrSession session, const XrColocationDiscoveryStartInfoMETA* info, XrAsyncRequestIdFB* discoveryRequestId);
typedef XrResult (XRAPI_PTR *PFN_xrStopColocationDiscoveryMETA)(XrSession session, const XrColocationDiscoveryStopInfoMETA* info, XrAsyncRequestIdFB* requestId);
typedef XrResult (XRAPI_PTR *PFN_xrStartColocationAdvertisementMETA)(XrSession session, const XrColocationAdvertisementStartInfoMETA* info, XrAsyncRequestIdFB* advertisementRequestId);
typedef XrResult (XRAPI_PTR *PFN_xrStopColocationAdvertisementMETA)(XrSession session, const XrColocationAdvertisementStopInfoMETA* info, XrAsyncRequestIdFB* requestId);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrStartColocationDiscoveryMETA(
    XrSession                                   session,
    const XrColocationDiscoveryStartInfoMETA*   info,
    XrAsyncRequestIdFB*                         discoveryRequestId);

XRAPI_ATTR XrResult XRAPI_CALL xrStopColocationDiscoveryMETA(
    XrSession                                   session,
    const XrColocationDiscoveryStopInfoMETA*    info,
    XrAsyncRequestIdFB*                         requestId);

XRAPI_ATTR XrResult XRAPI_CALL xrStartColocationAdvertisementMETA(
    XrSession                                   session,
    const XrColocationAdvertisementStartInfoMETA* info,
    XrAsyncRequestIdFB*                         advertisementRequestId);

XRAPI_ATTR XrResult XRAPI_CALL xrStopColocationAdvertisementMETA(
    XrSession                                   session,
    const XrColocationAdvertisementStopInfoMETA* info,
    XrAsyncRequestIdFB*                         requestId);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_colocation_discovery */

#ifdef __cplusplus
}
#endif

#endif
