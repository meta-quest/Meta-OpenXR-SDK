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
#ifndef EXTX1_HAPTIC_PARAMETRIC_H_
#define EXTX1_HAPTIC_PARAMETRIC_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_EXTX1_haptic_parametric

// XR_EXTX1_haptic_parametric is a preprocessor guard. Do not pass it to API calls.
#define XR_EXTX1_haptic_parametric 1
#define XR_HAPTIC_PARAMETRIC_FREQUENCY_MAX_HZ_EXTX1 1000
#define XR_HAPTIC_PARAMETRIC_FREQUENCY_MIN_HZ_EXTX1 1
#define XR_HAPTIC_PARAMETRIC_MAX_FRAME_DELAY_NS_EXTX1 50000000
#define XR_HAPTIC_PARAMETRIC_MAX_POINTS_TRANSIENTS_EXTX1 500
#define XR_EXTX1_haptic_parametric_SPEC_VERSION 1
#define XR_EXTX1_HAPTIC_PARAMETRIC_EXTENSION_NAME "XR_EXTX1_haptic_parametric"
static const XrStructureType XR_TYPE_HAPTIC_PARAMETRIC_VIBRATION_EXTX1 = (XrStructureType) 1000775000;
static const XrStructureType XR_TYPE_HAPTIC_PARAMETRIC_PROPERTIES_EXTX1 = (XrStructureType) 1000775001;
static const XrStructureType XR_TYPE_SYSTEM_HAPTIC_PARAMETRIC_PROPERTIES_EXTX1 = (XrStructureType) 1000775002;

typedef enum XrHapticParametricStreamFrameTypeEXTX1 {
    XR_HAPTIC_PARAMETRIC_STREAM_FRAME_TYPE_NONE_EXTX1 = 0,
    XR_HAPTIC_PARAMETRIC_STREAM_FRAME_TYPE_FIRST_FRAME_EXTX1 = 1,
    XR_HAPTIC_PARAMETRIC_STREAM_FRAME_TYPE_INTERMEDIATE_FRAME_EXTX1 = 2,
    XR_HAPTIC_PARAMETRIC_STREAM_FRAME_TYPE_LAST_FRAME_EXTX1 = 3,
    XR_HAPTIC_PARAMETRIC_STREAM_FRAME_TYPE_EXTX1_MAX_ENUM = 0x7FFFFFFF
} XrHapticParametricStreamFrameTypeEXTX1;
typedef struct XrHapticParametricPointEXTX1 {
    XrDuration    time;
    float         value;
} XrHapticParametricPointEXTX1;

typedef struct XrHapticParametricPropertiesEXTX1 {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrDuration                  idealFrameSubmissionRate;
    XrDuration                  minimumFirstFrameDuration;
    float                       minFrequencyHz;
    float                       maxFrequencyHz;
} XrHapticParametricPropertiesEXTX1;

typedef struct XrHapticParametricTransientEXTX1 {
    XrDuration    time;
    float         amplitude;
    float         frequency;
} XrHapticParametricTransientEXTX1;

typedef struct XrHapticParametricVibrationEXTX1 {
    XrStructureType                            type;
    const void* XR_MAY_ALIAS                   next;
    uint32_t                                   amplitudePointCount;
    const XrHapticParametricPointEXTX1*        amplitudePoints;
    uint32_t                                   frequencyPointCount;
    const XrHapticParametricPointEXTX1*        frequencyPoints;
    uint32_t                                   transientCount;
    const XrHapticParametricTransientEXTX1*    transients;
    float                                      minFrequencyHz;
    float                                      maxFrequencyHz;
    XrHapticParametricStreamFrameTypeEXTX1     streamFrameType;
} XrHapticParametricVibrationEXTX1;

typedef struct XrSystemHapticParametricPropertiesEXTX1 {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsParametricHaptics;
} XrSystemHapticParametricPropertiesEXTX1;

typedef XrResult (XRAPI_PTR *PFN_xrHapticParametricGetPropertiesEXTX1)(XrSession session, const XrHapticActionInfo* hapticActionInfo, XrHapticParametricPropertiesEXTX1* parametricProperties);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrHapticParametricGetPropertiesEXTX1(
    XrSession                                   session,
    const XrHapticActionInfo*                   hapticActionInfo,
    XrHapticParametricPropertiesEXTX1*          parametricProperties);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_EXTX1_haptic_parametric */

#ifdef __cplusplus
}
#endif

#endif
