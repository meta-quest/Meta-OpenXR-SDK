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
#ifndef METAX1_DYNAMIC_OBJECT_TRACKER_H_
#define METAX1_DYNAMIC_OBJECT_TRACKER_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_METAX1_dynamic_object_tracker

// XR_METAX1_dynamic_object_tracker is a preprocessor guard. Do not pass it to API calls.
#define XR_METAX1_dynamic_object_tracker 1
XR_DEFINE_HANDLE(XrDynamicObjectTrackerMETAX1)
#define XR_METAX1_dynamic_object_tracker_SPEC_VERSION 1
#define XR_METAX1_DYNAMIC_OBJECT_TRACKER_EXTENSION_NAME "XR_METAX1_dynamic_object_tracker"
// XrDynamicObjectTrackerMETAX1
static const XrObjectType XR_OBJECT_TYPE_DYNAMIC_OBJECT_TRACKER_METAX1 = (XrObjectType) 1000288000;
static const XrStructureType XR_TYPE_SYSTEM_DYNAMIC_OBJECT_TRACKER_PROPERTIES_METAX1 = (XrStructureType) 1000288000;
static const XrStructureType XR_TYPE_DYNAMIC_OBJECT_TRACKER_CREATE_INFO_METAX1 = (XrStructureType) 1000288001;
static const XrStructureType XR_TYPE_DYNAMIC_OBJECT_TRACKED_CLASSES_SET_INFO_METAX1 = (XrStructureType) 1000288002;
static const XrStructureType XR_TYPE_DYNAMIC_OBJECT_DATA_METAX1 = (XrStructureType) 1000288003;
static const XrStructureType XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_TRACKER_CREATE_RESULT_METAX1 = (XrStructureType) 1000288004;
static const XrStructureType XR_TYPE_EVENT_DATA_DYNAMIC_OBJECT_SET_TRACKED_CLASSES_RESULT_METAX1 = (XrStructureType) 1000288005;
static const XrSpaceComponentTypeFB XR_SPACE_COMPONENT_TYPE_DYNAMIC_OBJECT_DATA_METAX1 = (XrSpaceComponentTypeFB) 1000288006;

typedef enum XrDynamicObjectClassMETAX1 {
    XR_DYNAMIC_OBJECT_CLASS_KEYBOARD_METAX1 = 0,
    XR_DYNAMIC_OBJECT_CLASS_METAX1_MAX_ENUM = 0x7FFFFFFF
} XrDynamicObjectClassMETAX1;
typedef struct XrSystemDynamicObjectTrackerPropertiesMETAX1 {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsDynamicObjectTracker;
} XrSystemDynamicObjectTrackerPropertiesMETAX1;

typedef struct XrDynamicObjectTrackerCreateInfoMETAX1 {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
} XrDynamicObjectTrackerCreateInfoMETAX1;

typedef struct XrDynamicObjectTrackedClassesSetInfoMETAX1 {
    XrStructureType                      type;
    const void* XR_MAY_ALIAS             next;
    uint32_t                             classCount;
    const XrDynamicObjectClassMETAX1*    classes;
} XrDynamicObjectTrackedClassesSetInfoMETAX1;

typedef struct XrDynamicObjectDataMETAX1 {
    XrStructureType               type;
    void* XR_MAY_ALIAS            next;
    XrDynamicObjectClassMETAX1    classType;
} XrDynamicObjectDataMETAX1;

typedef struct XrEventDataDynamicObjectTrackerCreateResultMETAX1 {
    XrStructureType                 type;
    const void* XR_MAY_ALIAS        next;
    XrDynamicObjectTrackerMETAX1    handle;
    XrResult                        result;
} XrEventDataDynamicObjectTrackerCreateResultMETAX1;

typedef struct XrEventDataDynamicObjectSetTrackedClassesResultMETAX1 {
    XrStructureType                 type;
    const void* XR_MAY_ALIAS        next;
    XrDynamicObjectTrackerMETAX1    handle;
    XrResult                        result;
} XrEventDataDynamicObjectSetTrackedClassesResultMETAX1;

typedef XrResult (XRAPI_PTR *PFN_xrCreateDynamicObjectTrackerMETAX1)(XrSession session, const XrDynamicObjectTrackerCreateInfoMETAX1* createInfo, XrDynamicObjectTrackerMETAX1* dynamicObjectTracker);
typedef XrResult (XRAPI_PTR *PFN_xrDestroyDynamicObjectTrackerMETAX1)(XrDynamicObjectTrackerMETAX1 dynamicObjectTracker);
typedef XrResult (XRAPI_PTR *PFN_xrSetDynamicObjectTrackedClassesMETAX1)(XrDynamicObjectTrackerMETAX1 dynamicObjectTracker, const XrDynamicObjectTrackedClassesSetInfoMETAX1* setInfo);
typedef XrResult (XRAPI_PTR *PFN_xrGetSpaceDynamicObjectDataMETAX1)(XrSession session, XrSpace space, XrDynamicObjectDataMETAX1* dynamicObjectDataOutput);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrCreateDynamicObjectTrackerMETAX1(
    XrSession                                   session,
    const XrDynamicObjectTrackerCreateInfoMETAX1* createInfo,
    XrDynamicObjectTrackerMETAX1*               dynamicObjectTracker);

XRAPI_ATTR XrResult XRAPI_CALL xrDestroyDynamicObjectTrackerMETAX1(
    XrDynamicObjectTrackerMETAX1                dynamicObjectTracker);

XRAPI_ATTR XrResult XRAPI_CALL xrSetDynamicObjectTrackedClassesMETAX1(
    XrDynamicObjectTrackerMETAX1                dynamicObjectTracker,
    const XrDynamicObjectTrackedClassesSetInfoMETAX1* setInfo);

XRAPI_ATTR XrResult XRAPI_CALL xrGetSpaceDynamicObjectDataMETAX1(
    XrSession                                   session,
    XrSpace                                     space,
    XrDynamicObjectDataMETAX1*                  dynamicObjectDataOutput);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_METAX1_dynamic_object_tracker */

#ifdef __cplusplus
}
#endif

#endif
