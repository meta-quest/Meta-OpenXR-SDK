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

Filename    :   OVR_BinaryFile2.h
Content     :   Simple helper class to read a binary file.
Created     :   Jun, 2014
Authors     :   J.M.P. van Waveren

*************************************************************************************/

#pragma once

#include "OVR_Types.h"
#include <vector>

/*
    This is a simple helper class to read binary data next to a JSON file.
*/

namespace OVRFW {

class BinaryReader {
   public:
    BinaryReader(const uint8_t* binData, const int binSize)
        : Data(binData), Size(binSize), Offset(0), Allocated(false) {}
    ~BinaryReader();

    BinaryReader(const char* path, const char** perror);

    uint32_t ReadUInt32() const {
        const int bytes = sizeof(uint32_t);
        if (Data == NULL || bytes > Size - Offset) {
            return 0;
        }
        Offset += bytes;
        return *(uint32_t*)(Data + Offset - bytes);
    }

    template <typename _type_>
    bool ReadArray(std::vector<_type_>& out, const int numElements) const {
        const int bytes = numElements * sizeof(out[0]);
        if (Data == NULL || bytes > Size - Offset) {
            out.resize(0);
            return false;
        }
        out.resize(numElements);
        memcpy(out.data(), &Data[Offset], bytes);
        Offset += bytes;
        return true;
    }

    bool IsAtEnd() const {
        return (Offset == Size);
    }

   private:
    const uint8_t* Data;
    int32_t Size;
    mutable int32_t Offset;
    bool Allocated;
};

std::vector<uint8_t> MemBufferFile(const char* fileName);

} // namespace OVRFW
