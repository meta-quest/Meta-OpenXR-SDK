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
#ifndef META_FACE_TRACKING_VISEMES_H_
#define META_FACE_TRACKING_VISEMES_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_face_tracking_visemes

// XR_META_face_tracking_visemes is a preprocessor guard. Do not pass it to API calls.
#define XR_META_face_tracking_visemes 1
#define XR_FACE_TRACKING_VISEME_COUNT_META 15
#define XR_META_face_tracking_visemes_SPEC_VERSION 1
#define XR_META_FACE_TRACKING_VISEMES_EXTENSION_NAME "XR_META_face_tracking_visemes"
static const XrStructureType XR_TYPE_FACE_TRACKING_VISEMES_META = (XrStructureType) 1000541000;
static const XrStructureType XR_TYPE_SYSTEM_FACE_TRACKING_VISEMES_PROPERTIES_META = (XrStructureType) 1000541001;

typedef enum XrFaceTrackingVisemeMETA {
    XR_FACE_TRACKING_VISEME_SIL_META = 0,
    XR_FACE_TRACKING_VISEME_PP_META = 1,
    XR_FACE_TRACKING_VISEME_FF_META = 2,
    XR_FACE_TRACKING_VISEME_TH_META = 3,
    XR_FACE_TRACKING_VISEME_DD_META = 4,
    XR_FACE_TRACKING_VISEME_KK_META = 5,
    XR_FACE_TRACKING_VISEME_CH_META = 6,
    XR_FACE_TRACKING_VISEME_SS_META = 7,
    XR_FACE_TRACKING_VISEME_NN_META = 8,
    XR_FACE_TRACKING_VISEME_RR_META = 9,
    XR_FACE_TRACKING_VISEME_AA_META = 10,
    XR_FACE_TRACKING_VISEME_E_META = 11,
    XR_FACE_TRACKING_VISEME_IH_META = 12,
    XR_FACE_TRACKING_VISEME_OH_META = 13,
    XR_FACE_TRACKING_VISEME_OU_META = 14,
    XR_FACE_TRACKING_VISEME_MAX_ENUM_META = 0x7FFFFFFF
} XrFaceTrackingVisemeMETA;
// XrFaceTrackingVisemesMETA extends XrFaceExpressionWeights2FB
typedef struct XrFaceTrackingVisemesMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrBool32                    isValid;
    float                       visemes[XR_FACE_TRACKING_VISEME_COUNT_META];
} XrFaceTrackingVisemesMETA;

typedef struct XrSystemFaceTrackingVisemesPropertiesMETA {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsVisemes;
} XrSystemFaceTrackingVisemesPropertiesMETA;

#endif /* XR_META_face_tracking_visemes */

#ifdef __cplusplus
}
#endif

#endif
