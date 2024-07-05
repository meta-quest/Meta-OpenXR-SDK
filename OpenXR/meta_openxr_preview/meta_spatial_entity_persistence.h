#ifndef META_SPATIAL_ENTITY_PERSISTENCE_H_
#define META_SPATIAL_ENTITY_PERSISTENCE_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_spatial_entity_persistence

// XR_META_spatial_entity_persistence is a preprocessor guard. Do not pass it to API calls.
#define XR_META_spatial_entity_persistence 1
#define XR_META_spatial_entity_persistence_SPEC_VERSION 1
#define XR_META_SPATIAL_ENTITY_PERSISTENCE_EXTENSION_NAME "XR_META_spatial_entity_persistence"
// Error: Resource limitation prevented this operation from executing. Recommend retrying, perhaps after a short delay and/or reducing memory consumption.
static const XrResult XR_ERROR_SPACE_INSUFFICIENT_RESOURCES_META = (XrResult) -1000259000;
// Error: Operation could not be completed until resources used are reduced or storage expanded.
static const XrResult XR_ERROR_SPACE_STORAGE_AT_CAPACITY_META = (XrResult) -1000259001;
// Error: Look around the environment more for space tracking to function.
static const XrResult XR_ERROR_SPACE_INSUFFICIENT_VIEW_META = (XrResult) -1000259002;
// Error: Space operation permission insufficient. Recommend confirming the status of the required permissions needed for using Space APIs.
static const XrResult XR_ERROR_SPACE_PERMISSION_INSUFFICIENT_META = (XrResult) -1000259003;
// Error: Operation cancelled due to rate limiting. Recommend retrying after a short delay.
static const XrResult XR_ERROR_SPACE_RATE_LIMITED_META = (XrResult) -1000259004;
// Error: Environment too dark for tracking to complete operation.
static const XrResult XR_ERROR_SPACE_TOO_DARK_META = (XrResult) -1000259005;
// Error: Environment too bright for tracking to complete operation.
static const XrResult XR_ERROR_SPACE_TOO_BRIGHT_META = (XrResult) -1000259006;
static const XrStructureType XR_TYPE_SYSTEM_SPACE_PERSISTENCE_PROPERTIES_META = (XrStructureType) 1000259000;
static const XrStructureType XR_TYPE_SPACES_SAVE_INFO_META = (XrStructureType) 1000259001;
static const XrStructureType XR_TYPE_EVENT_DATA_SPACES_SAVE_RESULT_META = (XrStructureType) 1000259002;
static const XrStructureType XR_TYPE_SPACES_ERASE_INFO_META = (XrStructureType) 1000259003;
static const XrStructureType XR_TYPE_EVENT_DATA_SPACES_ERASE_RESULT_META = (XrStructureType) 1000259004;
typedef struct XrSystemSpacePersistencePropertiesMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrBool32                    supportsSpacePersistence;
} XrSystemSpacePersistencePropertiesMETA;

typedef struct XrSpacesSaveInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    spaceCount;
    XrSpace*                    spaces;
} XrSpacesSaveInfoMETA;

typedef struct XrEventDataSpacesSaveResultMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
    XrResult                    result;
} XrEventDataSpacesSaveResultMETA;

typedef struct XrSpacesEraseInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    spaceCount;
    XrSpace*                    spaces;
    uint32_t                    uuidCount;
    XrUuidEXT*                  uuids;
} XrSpacesEraseInfoMETA;

typedef struct XrEventDataSpacesEraseResultMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
    XrResult                    result;
} XrEventDataSpacesEraseResultMETA;

typedef XrResult (XRAPI_PTR *PFN_xrSaveSpacesMETA)(XrSession session, const XrSpacesSaveInfoMETA* info, XrAsyncRequestIdFB* requestId);
typedef XrResult (XRAPI_PTR *PFN_xrEraseSpacesMETA)(XrSession session, const XrSpacesEraseInfoMETA* info, XrAsyncRequestIdFB* requestId);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrSaveSpacesMETA(
    XrSession                                   session,
    const XrSpacesSaveInfoMETA*                 info,
    XrAsyncRequestIdFB*                         requestId);

XRAPI_ATTR XrResult XRAPI_CALL xrEraseSpacesMETA(
    XrSession                                   session,
    const XrSpacesEraseInfoMETA*                info,
    XrAsyncRequestIdFB*                         requestId);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_spatial_entity_persistence */

#ifdef __cplusplus
}
#endif

#endif
