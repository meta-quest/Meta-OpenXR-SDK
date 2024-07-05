#ifndef META_BOUNDARY_VISIBILITY_H_
#define META_BOUNDARY_VISIBILITY_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_boundary_visibility

// XR_META_boundary_visibility is a preprocessor guard. Do not pass it to API calls.
#define XR_META_boundary_visibility 1
#define XR_META_boundary_visibility_SPEC_VERSION 1
#define XR_META_BOUNDARY_VISIBILITY_EXTENSION_NAME "XR_META_boundary_visibility"
static const XrResult XR_BOUNDARY_VISIBILITY_SUPPRESSION_NOT_ALLOWED_META = (XrResult) 1000528000;
static const XrStructureType XR_TYPE_SYSTEM_BOUNDARY_VISIBILITY_PROPERTIES_META = (XrStructureType) 1000528000;
static const XrStructureType XR_TYPE_EVENT_DATA_BOUNDARY_VISIBILITY_CHANGED_META = (XrStructureType) 1000528001;

typedef enum XrBoundaryVisibilityMETA {
    // Boundary is not suppressed.
    XR_BOUNDARY_VISIBILITY_NOT_SUPPRESSED_META = 1,
    // Boundary is suppressed.
    XR_BOUNDARY_VISIBILITY_SUPPRESSED_META = 2,
    XR_BOUNDARY_VISIBILITY_MAX_ENUM_META = 0x7FFFFFFF
} XrBoundaryVisibilityMETA;
typedef struct XrSystemBoundaryVisibilityPropertiesMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrBool32                    supportsBoundaryVisibility;
} XrSystemBoundaryVisibilityPropertiesMETA;

typedef struct XrEventDataBoundaryVisibilityChangedMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrBoundaryVisibilityMETA    boundaryVisibility;
} XrEventDataBoundaryVisibilityChangedMETA;

typedef XrResult (XRAPI_PTR *PFN_xrRequestBoundaryVisibilityMETA)(XrSession session, XrBoundaryVisibilityMETA boundaryVisibility);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrRequestBoundaryVisibilityMETA(
    XrSession                                   session,
    XrBoundaryVisibilityMETA                    boundaryVisibility);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_boundary_visibility */

#ifdef __cplusplus
}
#endif

#endif
