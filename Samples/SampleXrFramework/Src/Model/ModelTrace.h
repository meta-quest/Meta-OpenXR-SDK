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
/************************************************************************************

Filename    :   ModelTrace.h
Content     :   Ray tracer using a KD-Tree.
Created     :   May, 2014
Authors     :   J.M.P. van Waveren

*************************************************************************************/

#pragma once

#include "OVR_Math.h"

#include <vector>

namespace OVRFW {

const int RT_KDTREE_MAX_LEAF_TRIANGLES = 4;

struct kdtree_header_t {
    int numVertices;
    int numUvs;
    int numIndices;
    int numNodes;
    int numLeafs;
    int numOverflow;
    OVR::Bounds3f bounds;
};

struct kdtree_node_t {
    // bits [ 0,0] = leaf flag
    // bits [ 2,1] = split plane (0 = x, 1 = y, 2 = z, 3 = invalid)
    // bits [31,3] = index of left child (+1 = right child index), or index of leaf data
    unsigned int data;
    float dist;
};

struct kdtree_leaf_t {
    int triangles[RT_KDTREE_MAX_LEAF_TRIANGLES];
    int ropes[6];
    OVR::Bounds3f bounds;
};

struct traceResult_t {
    int triangleIndex;
    float fraction;
    OVR::Vector2f uv;
    OVR::Vector3f normal;
};

class ModelTrace {
   public:
    ModelTrace() {}
    ~ModelTrace() {}

    bool Validate(const bool fullVerify) const;

    traceResult_t Trace(const OVR::Vector3f& start, const OVR::Vector3f& end) const;
    traceResult_t Trace_Exhaustive(const OVR::Vector3f& start, const OVR::Vector3f& end) const;

    void PrintStatsToLog() const;

   public:
    kdtree_header_t header;
    std::vector<OVR::Vector3f> vertices;
    std::vector<OVR::Vector2f> uvs;
    std::vector<int> indices;
    std::vector<kdtree_node_t> nodes;
    std::vector<kdtree_leaf_t> leafs;
    std::vector<int> overflow; // this is a flat array that stores extra triangle indices for leaves
                               // with > RT_KDTREE_MAX_LEAF_TRIANGLES
};

} // namespace OVRFW
