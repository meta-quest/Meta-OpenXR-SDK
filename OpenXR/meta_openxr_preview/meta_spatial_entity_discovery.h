#ifndef META_SPATIAL_ENTITY_DISCOVERY_H_
#define META_SPATIAL_ENTITY_DISCOVERY_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_spatial_entity_discovery

// XR_META_spatial_entity_discovery is a preprocessor guard. Do not pass it to API calls.
#define XR_META_spatial_entity_discovery 1
#define XR_META_spatial_entity_discovery_SPEC_VERSION 1
#define XR_META_SPATIAL_ENTITY_DISCOVERY_EXTENSION_NAME "XR_META_spatial_entity_discovery"
static const XrStructureType XR_TYPE_SYSTEM_SPACE_DISCOVERY_PROPERTIES_META = (XrStructureType) 1000247000;
static const XrStructureType XR_TYPE_SPACE_DISCOVERY_INFO_META = (XrStructureType) 1000247001;
static const XrStructureType XR_TYPE_SPACE_FILTER_UUID_META = (XrStructureType) 1000247003;
static const XrStructureType XR_TYPE_SPACE_FILTER_COMPONENT_META = (XrStructureType) 1000247004;
static const XrStructureType XR_TYPE_SPACE_DISCOVERY_RESULT_META = (XrStructureType) 1000247005;
static const XrStructureType XR_TYPE_SPACE_DISCOVERY_RESULTS_META = (XrStructureType) 1000247006;
static const XrStructureType XR_TYPE_EVENT_DATA_SPACE_DISCOVERY_RESULTS_AVAILABLE_META = (XrStructureType) 1000247007;
static const XrStructureType XR_TYPE_EVENT_DATA_SPACE_DISCOVERY_COMPLETE_META = (XrStructureType) 1000247008;
typedef struct XrSystemSpaceDiscoveryPropertiesMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrBool32                    supportsSpaceDiscovery;
} XrSystemSpaceDiscoveryPropertiesMETA;

typedef struct XR_MAY_ALIAS XrSpaceFilterBaseHeaderMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrSpaceFilterBaseHeaderMETA;

typedef struct XrSpaceDiscoveryInfoMETA {
    XrStructureType                               type;
    const void* XR_MAY_ALIAS                      next;
    uint32_t                                      filterCount;
    const XrSpaceFilterBaseHeaderMETA* const *    filters;
} XrSpaceDiscoveryInfoMETA;

typedef struct XrSpaceFilterUuidMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    uuidCount;
    const XrUuidEXT*            uuids;
} XrSpaceFilterUuidMETA;

typedef struct XrSpaceFilterComponentMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrSpaceComponentTypeFB      componentType;
} XrSpaceFilterComponentMETA;

typedef struct XrSpaceDiscoveryResultMETA {
    XrSpace      space;
    XrUuidEXT    uuid;
} XrSpaceDiscoveryResultMETA;

typedef struct XrSpaceDiscoveryResultsMETA {
    XrStructureType                type;
    const void* XR_MAY_ALIAS       next;
    uint32_t                       resultCapacityInput;
    uint32_t                       resultCountOutput;
    XrSpaceDiscoveryResultMETA*    results;
} XrSpaceDiscoveryResultsMETA;

typedef struct XrEventDataSpaceDiscoveryResultsAvailableMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
} XrEventDataSpaceDiscoveryResultsAvailableMETA;

typedef struct XrEventDataSpaceDiscoveryCompleteMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAsyncRequestIdFB          requestId;
    XrResult                    result;
} XrEventDataSpaceDiscoveryCompleteMETA;

typedef XrResult (XRAPI_PTR *PFN_xrDiscoverSpacesMETA)(XrSession session, const XrSpaceDiscoveryInfoMETA* info, XrAsyncRequestIdFB* requestId);
typedef XrResult (XRAPI_PTR *PFN_xrRetrieveSpaceDiscoveryResultsMETA)(XrSession session, XrAsyncRequestIdFB requestId, XrSpaceDiscoveryResultsMETA* results);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrDiscoverSpacesMETA(
    XrSession                                   session,
    const XrSpaceDiscoveryInfoMETA*             info,
    XrAsyncRequestIdFB*                         requestId);

XRAPI_ATTR XrResult XRAPI_CALL xrRetrieveSpaceDiscoveryResultsMETA(
    XrSession                                   session,
    XrAsyncRequestIdFB                          requestId,
    XrSpaceDiscoveryResultsMETA*                results);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_spatial_entity_discovery */

#ifdef __cplusplus
}
#endif

#endif
