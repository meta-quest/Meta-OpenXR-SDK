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
#ifndef META_PASSTHROUGH_LAYER_RESUMED_EVENT_H_
#define META_PASSTHROUGH_LAYER_RESUMED_EVENT_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_passthrough_layer_resumed_event

// XR_META_passthrough_layer_resumed_event is a preprocessor guard. Do not pass it to API calls.
#define XR_META_passthrough_layer_resumed_event 1
#define XR_META_passthrough_layer_resumed_event_SPEC_VERSION 1
#define XR_META_PASSTHROUGH_LAYER_RESUMED_EVENT_EXTENSION_NAME "XR_META_passthrough_layer_resumed_event"
static const XrStructureType XR_TYPE_EVENT_DATA_PASSTHROUGH_LAYER_RESUMED_META = (XrStructureType) 1000282000;
typedef struct XrEventDataPassthroughLayerResumedMETA {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrPassthroughLayerFB        layer;
} XrEventDataPassthroughLayerResumedMETA;

#endif /* XR_META_passthrough_layer_resumed_event */

#ifdef __cplusplus
}
#endif

#endif
