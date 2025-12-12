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

Filename    :   GlGeometry.cpp
Content     :   OpenGL geometry setup.
Created     :   October 8, 2013
Authors     :   John Carmack, J.M.P. van Waveren

*************************************************************************************/

#include "GlGeometry.h"
#include "GlProgram.h"
#include "Misc/Log.h"
#include "Egl.h"

using OVR::Bounds3f;
using OVR::Vector2f;
using OVR::Vector3f;
using OVR::Vector4f;

/*
 * These are all built inside VertexArrayObjects, so no GL state other
 * than the VAO binding should be disturbed.
 *
 */
namespace OVRFW {

unsigned GlGeometry::IndexType = (sizeof(TriangleIndex) == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

template <typename _attrib_type_>
void PackVertexAttribute(
    std::vector<uint8_t>& packed,
    const std::vector<_attrib_type_>& attrib,
    const int glLocation,
    const int glType,
    const int glComponents) {
    if (attrib.size() > 0) {
        const size_t offset = packed.size();
        const size_t size = attrib.size() * sizeof(attrib[0]);

        packed.resize(offset + size);
        memcpy(&packed[offset], attrib.data(), size);

        glEnableVertexAttribArray(glLocation);
        glVertexAttribPointer(
            glLocation, glComponents, glType, false, sizeof(attrib[0]), (void*)(offset));
    } else {
        glDisableVertexAttribArray(glLocation);
    }
}

void GlGeometry::Create(const VertexAttribs& attribs, const std::vector<TriangleIndex>& indices) {
    vertexCount = attribs.position.size();
    indexCount = indices.size();

    const bool t = enableGeometryTransfom;
    std::vector<OVR::Vector3f> position;
    std::vector<OVR::Vector3f> normal;
    std::vector<OVR::Vector3f> tangent;
    std::vector<OVR::Vector3f> binormal;

    /// we asked for incoming transfom
    if (t) {
        position.resize(attribs.position.size());
        normal.resize(attribs.normal.size());
        tangent.resize(attribs.position.size());
        binormal.resize(attribs.binormal.size());

        /// Positions use 4x4
        for (size_t i = 0; i < attribs.position.size(); ++i) {
            position[i] = geometryTransfom.Transform(attribs.position[i]);
        }

        /// TBN use 3x3
        const OVR::Matrix3f nt = OVR::Matrix3f(geometryTransfom).Inverse().Transposed();
        for (size_t i = 0; i < attribs.normal.size(); ++i) {
            normal[i] = nt.Transform(attribs.normal[i]).Normalized();
        }
        for (size_t i = 0; i < attribs.tangent.size(); ++i) {
            tangent[i] = nt.Transform(attribs.tangent[i]).Normalized();
        }
        for (size_t i = 0; i < attribs.binormal.size(); ++i) {
            binormal[i] = nt.Transform(attribs.binormal[i]).Normalized();
        }
    }

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    std::vector<uint8_t> packed;
    PackVertexAttribute(
        packed, t ? position : attribs.position, VERTEX_ATTRIBUTE_LOCATION_POSITION, GL_FLOAT, 3);
    PackVertexAttribute(
        packed, t ? normal : attribs.normal, VERTEX_ATTRIBUTE_LOCATION_NORMAL, GL_FLOAT, 3);
    PackVertexAttribute(
        packed, t ? tangent : attribs.tangent, VERTEX_ATTRIBUTE_LOCATION_TANGENT, GL_FLOAT, 3);
    PackVertexAttribute(
        packed, t ? binormal : attribs.binormal, VERTEX_ATTRIBUTE_LOCATION_BINORMAL, GL_FLOAT, 3);
    PackVertexAttribute(packed, attribs.color, VERTEX_ATTRIBUTE_LOCATION_COLOR, GL_FLOAT, 4);
    PackVertexAttribute(packed, attribs.uv0, VERTEX_ATTRIBUTE_LOCATION_UV0, GL_FLOAT, 2);
    PackVertexAttribute(packed, attribs.uv1, VERTEX_ATTRIBUTE_LOCATION_UV1, GL_FLOAT, 2);
    PackVertexAttribute(
        packed, attribs.jointIndices, VERTEX_ATTRIBUTE_LOCATION_JOINT_INDICES, GL_INT, 4);
    PackVertexAttribute(
        packed, attribs.jointWeights, VERTEX_ATTRIBUTE_LOCATION_JOINT_WEIGHTS, GL_FLOAT, 4);
    // clang-format off

    glBufferData(GL_ARRAY_BUFFER, packed.size() * sizeof(packed[0]), packed.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(indices[0]),
        indices.data(),
        GL_STATIC_DRAW);

    glBindVertexArray(0);

    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_NORMAL);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_TANGENT);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_BINORMAL);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_COLOR);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV0);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV1);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_JOINT_INDICES);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_JOINT_WEIGHTS);

    localBounds.Clear();
    for (int i = 0; i < vertexCount; i++) {
        localBounds.AddPoint(attribs.position[i]);
    }
}

void GlGeometry::Update(const VertexAttribs& attribs, const bool updateBounds) {
    vertexCount = attribs.position.size();

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    std::vector<uint8_t> packed;
    PackVertexAttribute(packed, attribs.position, VERTEX_ATTRIBUTE_LOCATION_POSITION, GL_FLOAT, 3);
    PackVertexAttribute(packed, attribs.normal, VERTEX_ATTRIBUTE_LOCATION_NORMAL, GL_FLOAT, 3);
    PackVertexAttribute(packed, attribs.tangent, VERTEX_ATTRIBUTE_LOCATION_TANGENT, GL_FLOAT, 3);
    PackVertexAttribute(packed, attribs.binormal, VERTEX_ATTRIBUTE_LOCATION_BINORMAL, GL_FLOAT, 3);
    PackVertexAttribute(packed, attribs.color, VERTEX_ATTRIBUTE_LOCATION_COLOR, GL_FLOAT, 4);
    PackVertexAttribute(packed, attribs.uv0, VERTEX_ATTRIBUTE_LOCATION_UV0, GL_FLOAT, 2);
    PackVertexAttribute(packed, attribs.uv1, VERTEX_ATTRIBUTE_LOCATION_UV1, GL_FLOAT, 2);
    PackVertexAttribute(
        packed, attribs.jointIndices, VERTEX_ATTRIBUTE_LOCATION_JOINT_INDICES, GL_INT, 4);
    PackVertexAttribute(
        packed, attribs.jointWeights, VERTEX_ATTRIBUTE_LOCATION_JOINT_WEIGHTS, GL_FLOAT, 4);

    glBufferData(GL_ARRAY_BUFFER, packed.size() * sizeof(packed[0]), packed.data(), GL_STATIC_DRAW);

    if (updateBounds) {
        localBounds.Clear();
        for (int i = 0; i < vertexCount; i++) {
            localBounds.AddPoint(attribs.position[i]);
        }
    }
}

void GlGeometry::Free() {
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &vertexBuffer);

    indexBuffer = 0;
    vertexBuffer = 0;
    vertexArrayObject = 0;
    vertexCount = 0;
    indexCount = 0;

    localBounds.Clear();
}

// Sets up VB and VAO for font drawing
GlGeometry FontGeometryCreate(fontVertex_t* verts, int numVerts, OVR::Bounds3f& localBounds) {
    GlGeometry Geo;

    const int maxQuads = numVerts / 4;
    Geo.indexCount = maxQuads * 6;
    Geo.vertexCount = maxQuads * 4;

    Geo.localBounds = localBounds;

    // font VAO
    glGenVertexArrays(1, &Geo.vertexArrayObject);
    glBindVertexArray(Geo.vertexArrayObject);

    // vertex buffer
    const int vertexByteCount = Geo.vertexCount * sizeof(fontVertex_t);
    glGenBuffers(1, &Geo.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Geo.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexByteCount, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION); // x, y and z
    glVertexAttribPointer(
        VERTEX_ATTRIBUTE_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(fontVertex_t), nullptr);

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV0); // s and t
    glVertexAttribPointer(
        VERTEX_ATTRIBUTE_LOCATION_UV0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(fontVertex_t),
        (void*)offsetof(fontVertex_t, s));

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_COLOR); // color
    glVertexAttribPointer(
        VERTEX_ATTRIBUTE_LOCATION_COLOR,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(fontVertex_t),
        (void*)offsetof(fontVertex_t, rgba));

    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV1);

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_FONT_PARMS); // outline parms
    glVertexAttribPointer(
        VERTEX_ATTRIBUTE_LOCATION_FONT_PARMS,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(fontVertex_t),
        (void*)offsetof(fontVertex_t, fontParms));

    fontIndex_t* indices = new fontIndex_t[Geo.indexCount];
    const int indexByteCount = Geo.indexCount * sizeof(fontIndex_t);

    // indices never change
    fontIndex_t v = 0;
    for (int i = 0; i < maxQuads; i++) {
        indices[i * 6 + 0] = v + 2;
        indices[i * 6 + 1] = v + 1;
        indices[i * 6 + 2] = v + 0;
        indices[i * 6 + 3] = v + 3;
        indices[i * 6 + 4] = v + 2;
        indices[i * 6 + 5] = v + 0;
        v += 4;
    }

    glGenBuffers(1, &Geo.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Geo.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexByteCount, (void*)indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    delete[] indices;

    glBindVertexArray(Geo.vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Geo.vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVerts * sizeof(fontVertex_t), (void*)verts);
    glBindVertexArray(0);

    return Geo;
}

void FontGeometryUpdate(GlGeometry& geo, fontVertex_t* verts, int numVerts, int numIndices) {
    // Update the font VB/IB
    glBindVertexArray(geo.vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, geo.vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVerts * sizeof(fontVertex_t), (void*)verts);
    glBindVertexArray(0);
    geo.indexCount = numIndices;
}

} // namespace OVRFW
