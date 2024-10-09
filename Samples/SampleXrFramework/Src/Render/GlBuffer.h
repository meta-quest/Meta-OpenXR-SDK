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

Filename    :   GlBuffer.h
Content     :   OpenGL Buffer Management.
Created     :
Authors     :

*************************************************************************************/

#pragma once

#include "Egl.h"

namespace OVRFW {

enum GlBufferType_t {
    GLBUFFER_TYPE_UNIFORM,
};

class GlBuffer {
   public:
    GlBuffer();

    bool Create(const GlBufferType_t type, const size_t dataSize, const void* data);
    void Destroy();

    void Update(const size_t updateDataSize, const void* data) const;

    void* MapBuffer() const;
    void UnmapBuffer() const;

    unsigned int GetBuffer() const {
        return buffer;
    }
    size_t GetSize() const {
        return size;
    }

   private:
    unsigned int target;
    unsigned int buffer;
    size_t size;
};

} // namespace OVRFW
