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
#ifndef META_HAND_TRACKING_WIDE_MOTION_MODE_H_
#define META_HAND_TRACKING_WIDE_MOTION_MODE_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_hand_tracking_wide_motion_mode

// XR_META_hand_tracking_wide_motion_mode is a preprocessor guard. Do not pass it to API calls.
#define XR_META_hand_tracking_wide_motion_mode 1
#define XR_META_hand_tracking_wide_motion_mode_SPEC_VERSION 1
#define XR_META_HAND_TRACKING_WIDE_MOTION_MODE_EXTENSION_NAME "XR_META_hand_tracking_wide_motion_mode"
static const XrStructureType XR_TYPE_HAND_TRACKING_WIDE_MOTION_MODE_INFO_META = (XrStructureType) 1000539000;

typedef enum XrHandTrackingWideMotionModeMETA {
    // The runtime will attempt to leverage high fidelity body tracking algorithms to calculate hand pose infomation outside of the range of usual tracking area.
    XR_HAND_TRACKING_WIDE_MOTION_MODE_HIGH_FIDELITY_BODY_TRACKING_META = 1,
    XR_HAND_TRACKING_WIDE_MOTION_MODE_MAX_ENUM_META = 0x7FFFFFFF
} XrHandTrackingWideMotionModeMETA;
// XrHandTrackingWideMotionModeInfoMETA extends XrHandTrackerCreateInfoEXT
typedef struct XrHandTrackingWideMotionModeInfoMETA {
    XrStructureType                     type;
    const void* XR_MAY_ALIAS            next;
    XrHandTrackingWideMotionModeMETA    requestedWideMotionMode;
} XrHandTrackingWideMotionModeInfoMETA;

#endif /* XR_META_hand_tracking_wide_motion_mode */

#ifdef __cplusplus
}
#endif

#endif
