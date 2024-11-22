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
/************************************************************************************************
Filename    :   XrPassthroughHelper.h
Content     :   Helper inteface for openxr XR_FB_passthrough and related extensions
Created     :   September 2023
Authors     :   Adam Bengis, Peter Chan
Language    :   C++
Copyright   :   Copyright (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
************************************************************************************************/

#pragma once

#include <openxr/openxr.h>

#include "XrHelper.h"

class XrPassthroughHelper : public XrHelper {
   public:
    static std::vector<const char*> RequiredExtensionNames();

   public:
    explicit XrPassthroughHelper(XrInstance instance);

    void SessionInit(XrSession session) override;
    void SessionEnd() override;
    void Update(XrSpace currentSpace, XrTime predictedDisplayTime) override;

    XrPassthroughLayerFB CreateProjectedLayer() const;
    void DestroyLayer(XrPassthroughLayerFB layer) const;

    XrGeometryInstanceFB CreateGeometryInstance(
        XrPassthroughLayerFB layer,
        XrSpace baseSpace,
        const std::vector<XrVector3f>& vertices,
        const std::vector<uint32_t>& indices);
    void DestroyGeometryInstance(XrGeometryInstanceFB instance);
    void SetGeometryInstanceTransform(
        XrGeometryInstanceFB instance,
        XrTime time,
        const XrPosef& pose,
        const XrVector3f& scale);

    void Start() const;
    void Pause() const;

   private:
    XrSession Session = XR_NULL_HANDLE;

    // XR_FB_passthrough
    PFN_xrCreatePassthroughFB XrCreatePassthroughFB = nullptr;
    PFN_xrDestroyPassthroughFB XrDestroyPassthroughFB = nullptr;
    PFN_xrPassthroughStartFB XrPassthroughStartFB = nullptr;
    PFN_xrPassthroughPauseFB XrPassthroughPauseFB = nullptr;
    PFN_xrCreatePassthroughLayerFB XrCreatePassthroughLayerFB = nullptr;
    PFN_xrDestroyPassthroughLayerFB XrDestroyPassthroughLayerFB = nullptr;
    PFN_xrPassthroughLayerSetStyleFB XrPassthroughLayerSetStyleFB = nullptr;
    PFN_xrCreateGeometryInstanceFB XrCreateGeometryInstanceFB = nullptr;
    PFN_xrDestroyGeometryInstanceFB XrDestroyGeometryInstanceFB = nullptr;
    PFN_xrGeometryInstanceSetTransformFB XrGeometryInstanceSetTransformFB = nullptr;

    // XR_FB_triangle_mesh
    PFN_xrCreateTriangleMeshFB XrCreateTriangleMeshFB = nullptr;
    PFN_xrDestroyTriangleMeshFB XrDestroyTriangleMeshFB = nullptr;

    struct Geometry {
        XrGeometryInstanceFB Instance = XR_NULL_HANDLE;
        XrTriangleMeshFB Mesh = XR_NULL_HANDLE;
        XrGeometryInstanceTransformFB Transform{XR_TYPE_GEOMETRY_INSTANCE_TRANSFORM_FB};
    };

    XrPassthroughFB Passthrough = XR_NULL_HANDLE;
    std::vector<Geometry> Geometries;
};
