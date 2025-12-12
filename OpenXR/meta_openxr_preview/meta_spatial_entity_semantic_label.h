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
#ifndef META_SPATIAL_ENTITY_SEMANTIC_LABEL_H_
#define META_SPATIAL_ENTITY_SEMANTIC_LABEL_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_spatial_entity_semantic_label

// XR_META_spatial_entity_semantic_label is a preprocessor guard. Do not pass it to API calls.
#define XR_META_spatial_entity_semantic_label 1
#define XR_META_spatial_entity_semantic_label_SPEC_VERSION 1
#define XR_META_SPATIAL_ENTITY_SEMANTIC_LABEL_EXTENSION_NAME "XR_META_spatial_entity_semantic_label"

typedef enum XrSemanticLabelMETA {
    // Unknown. This is a valid label that is used when the runtime is not able to classify the entity using another label, or when the application does not recognize the label.
    XR_SEMANTIC_LABEL_UNKNOWN_META = 0,
    // Floor of a room or space.
    XR_SEMANTIC_LABEL_FLOOR_META = 1,
    // Ceiling of a room or space.
    XR_SEMANTIC_LABEL_CEILING_META = 2,
    // Wall face of a room or space. Wall faces, along with invisible wall faces, are used to define the outer boundary of a room.
    XR_SEMANTIC_LABEL_WALL_FACE_META = 3,
    // Inner wall face, which is a wall face that exists inside a room and is not connected to the outer boundary of the room. For example, a pillar that exists at the center of a room may: be represented by using four inner wall faces.
    XR_SEMANTIC_LABEL_INNER_WALL_FACE_META = 4,
    // Invisible wall face, which is used to conceptually separate a space (e.g., separate a living space from a kitchen space in an open floor plan house even though there is no real wall between the two spaces).
    XR_SEMANTIC_LABEL_INVISIBLE_WALL_FACE_META = 5,
    // Door frame, which usually exists on a wall face.
    XR_SEMANTIC_LABEL_DOOR_FRAME_META = 6,
    // Window frame, which usually exists on a wall face.
    XR_SEMANTIC_LABEL_WINDOW_FRAME_META = 7,
    XR_SEMANTIC_LABEL_MAX_ENUM_META = 0x7FFFFFFF
} XrSemanticLabelMETA;
#endif /* XR_META_spatial_entity_semantic_label */

#ifdef __cplusplus
}
#endif

#endif
