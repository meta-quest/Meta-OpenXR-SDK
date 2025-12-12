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
#ifndef META_SPATIAL_ENTITY_ROOM_MESH_H_
#define META_SPATIAL_ENTITY_ROOM_MESH_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_META_spatial_entity_room_mesh

// XR_META_spatial_entity_room_mesh is a preprocessor guard. Do not pass it to API calls.
#define XR_META_spatial_entity_room_mesh 1
#define XR_META_spatial_entity_room_mesh_SPEC_VERSION 1
#define XR_META_SPATIAL_ENTITY_ROOM_MESH_EXTENSION_NAME "XR_META_spatial_entity_room_mesh"
static const XrStructureType XR_TYPE_ROOM_MESH_FACE_INDICES_META = (XrStructureType) 1000553000;
static const XrStructureType XR_TYPE_SPACE_ROOM_MESH_GET_INFO_META = (XrStructureType) 1000553001;
static const XrStructureType XR_TYPE_ROOM_MESH_META = (XrStructureType) 1000553002;
static const XrSpaceComponentTypeFB XR_SPACE_COMPONENT_TYPE_ROOM_MESH_META = (XrSpaceComponentTypeFB) 1000553000;
typedef struct XrRoomMeshFaceIndicesMETA {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    uint32_t              indexCapacityInput;
    uint32_t              indexCountOutput;
    uint32_t*             indices;
} XrRoomMeshFaceIndicesMETA;

typedef struct XrRoomMeshFaceMETA {
    XrUuid                 uuid;
    XrUuid                 parentUuid;
    XrSemanticLabelMETA    semanticLabel;
} XrRoomMeshFaceMETA;

typedef struct XrRoomMeshMETA {
    XrStructureType        type;
    void* XR_MAY_ALIAS     next;
    uint32_t               vertexCapacityInput;
    uint32_t               vertexCountOutput;
    XrVector3f*            vertices;
    uint32_t               faceCapacityInput;
    uint32_t               faceCountOutput;
    XrRoomMeshFaceMETA*    faces;
} XrRoomMeshMETA;

typedef struct XrSpaceRoomMeshGetInfoMETA {
    XrStructureType               type;
    const void* XR_MAY_ALIAS      next;
    uint32_t                      recognizedSemanticLabelCount;
    const XrSemanticLabelMETA*    recognizedSemanticLabels;
} XrSpaceRoomMeshGetInfoMETA;

typedef XrResult (XRAPI_PTR *PFN_xrGetSpaceRoomMeshFaceIndicesMETA)(XrSpace space, const XrUuid* faceUuid, XrRoomMeshFaceIndicesMETA* roomMeshFaceIndicesOutput);
typedef XrResult (XRAPI_PTR *PFN_xrGetSpaceRoomMeshMETA)(XrSpace space, const XrSpaceRoomMeshGetInfoMETA* getInfo, XrRoomMeshMETA* roomMeshOutput);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetSpaceRoomMeshFaceIndicesMETA(
    XrSpace                                     space,
    const XrUuid*                               faceUuid,
    XrRoomMeshFaceIndicesMETA*                  roomMeshFaceIndicesOutput);

XRAPI_ATTR XrResult XRAPI_CALL xrGetSpaceRoomMeshMETA(
    XrSpace                                     space,
    const XrSpaceRoomMeshGetInfoMETA*           getInfo,
    XrRoomMeshMETA*                             roomMeshOutput);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */
#endif /* XR_META_spatial_entity_room_mesh */

#ifdef __cplusplus
}
#endif

#endif
