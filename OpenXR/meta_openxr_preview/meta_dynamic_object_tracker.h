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
#ifndef META_DYNAMIC_OBJECT_TRACKER_H_
#define META_DYNAMIC_OBJECT_TRACKER_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_dynamic_object_tracker

// XR_META_dynamic_object_tracker is a preprocessor guard. Do not pass it to API calls.
#define XR_META_dynamic_object_tracker 1
XR_DEFINE_HANDLE(XrDynamicObjectTrackerMETA)
#define XR_META_dynamic_object_tracker_SPEC_VERSION 1
#define XR_META_DYNAMIC_OBJECT_TRACKER_EXTENSION_NAME "XR_META_dynamic_object_tracker"
// XrDynamicObjectTrackerMETA
static const XrObjectType XR_OBJECT_TYPE_DYNAMIC_OBJECT_TRACKER_META = (XrObjectType) 1000288000;
static const XrStructureType XR_TYPE_DYNAMIC_OBJECT_TRACKER_CREATE_INFO_META = (XrStructureType) 1000288001;
static const XrStructureType XR_TYPE_DYNAMIC_OBJECT_TRACKED_CLASSES_SET_INFO_META = (XrStructureType) 1000288002;
static const XrStructureType XR_TYPE_DYNAMIC_OBJECT_DATA_META = (XrStructureType) 1000288003;
static const XrStructureType XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_TRACKER_CREATE_RESULT_META = (XrStructureType) 1000288004;
static const XrStructureType XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_SET_TRACKED_CLASSES_RESULT_META = (XrStructureType) 1000288005;
static const XrStructureType XR_TYPE_SYSTEM_DYNAMIC_OBJECT_TRACKER_PROPERTIES_META = (XrStructureType) 1000288006;
static const XrSpaceComponentTypeFB XR_SPACE_COMPONENT_TYPE_DYNAMIC_OBJECT_DATA_META = (XrSpaceComponentTypeFB) 1000288007;

typedef enum XrDynamicObjectClassMETA {
    XR_DYNAMIC_OBJECT_CLASS_MAX_ENUM_META = 0x7FFFFFFF
} XrDynamicObjectClassMETA;
typedef struct XrSystemDynamicObjectTrackerPropertiesMETA {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsDynamicObjectTracker;
} XrSystemDynamicObjectTrackerPropertiesMETA;

typedef struct XrDynamicObjectTrackerCreateInfoMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrDynamicObjectTrackerCreateInfoMETA;

typedef struct XrDynamicObjectTrackedClassesSetInfoMETA {
    XrStructureType                    type;
    const void* XR_MAY_ALIAS           next;
    uint32_t                           classCount;
    const XrDynamicObjectClassMETA*    classes;
} XrDynamicObjectTrackedClassesSetInfoMETA;

typedef struct XrDynamicObjectDataMETA {
    XrStructureType             type;
    void* XR_MAY_ALIAS          next;
    XrDynamicObjectClassMETA    classType;
} XrDynamicObjectDataMETA;

typedef struct XrEventDataDynamicObjectTrackerCreateResultMETA {
    XrStructureType               type;
    const void* XR_MAY_ALIAS      next;
    XrDynamicObjectTrackerMETA    handle;
    XrResult                      result;
} XrEventDataDynamicObjectTrackerCreateResultMETA;

typedef struct XrEventDataDynamicObjectSetTrackedClassesResultMETA {
    XrStructureType               type;
    const void* XR_MAY_ALIAS      next;
    XrDynamicObjectTrackerMETA    handle;
    XrResult                      result;
} XrEventDataDynamicObjectSetTrackedClassesResultMETA;

typedef XrResult (XRAPI_PTR *PFN_xrCreateDynamicObjectTrackerMETA)(XrSession session, const XrDynamicObjectTrackerCreateInfoMETA* createInfo, XrDynamicObjectTrackerMETA* dynamicObjectTracker);
typedef XrResult (XRAPI_PTR *PFN_xrDestroyDynamicObjectTrackerMETA)(XrDynamicObjectTrackerMETA dynamicObjectTracker);
typedef XrResult (XRAPI_PTR *PFN_xrSetDynamicObjectTrackedClassesMETA)(XrDynamicObjectTrackerMETA dynamicObjectTracker, const XrDynamicObjectTrackedClassesSetInfoMETA* setInfo);
typedef XrResult (XRAPI_PTR *PFN_xrGetSpaceDynamicObjectDataMETA)(XrSpace space, XrDynamicObjectDataMETA* dynamicObjectDataOutput);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrCreateDynamicObjectTrackerMETA(
    XrSession                                   session,
    const XrDynamicObjectTrackerCreateInfoMETA* createInfo,
    XrDynamicObjectTrackerMETA*                 dynamicObjectTracker);

XRAPI_ATTR XrResult XRAPI_CALL xrDestroyDynamicObjectTrackerMETA(
    XrDynamicObjectTrackerMETA                  dynamicObjectTracker);

XRAPI_ATTR XrResult XRAPI_CALL xrSetDynamicObjectTrackedClassesMETA(
    XrDynamicObjectTrackerMETA                  dynamicObjectTracker,
    const XrDynamicObjectTrackedClassesSetInfoMETA* setInfo);

XRAPI_ATTR XrResult XRAPI_CALL xrGetSpaceDynamicObjectDataMETA(
    XrSpace                                     space,
    XrDynamicObjectDataMETA*                    dynamicObjectDataOutput);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_dynamic_object_tracker */

#ifdef __cplusplus
}
#endif

#endif
