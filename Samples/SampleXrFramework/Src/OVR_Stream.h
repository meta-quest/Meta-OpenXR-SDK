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

Filename    :   OVR_Stream.h
Content     :   Abstraction for file streams.
Created     :   July 1, 2015
Authors     :   Jonathan E. Wright

*************************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace OVRFW {

enum ovrStreamMode { OVR_STREAM_MODE_READ, OVR_STREAM_MODE_WRITE, OVR_STREAM_MODE_MAX };

class ovrUriScheme;

//==============================================================
// ovrStream
// base class for streams
// Uses non-public virtual interface idiom so that the base class
// can preform pre- and post- operations when calling virtualized
// methods.
class ovrStream {
   public:
    ovrStream(ovrUriScheme const& scheme);
    virtual ~ovrStream();

    // Opens a stream for the specified Uri.
    bool Open(char const* Uri, ovrStreamMode const mode);

    // Closes the currently open stream.
    void Close();

    // Flushes the stream
    void Flush();

    bool GetLocalPathFromUri(const char* uri, std::string& outputPath);

    // Reads the specified number of bytes from the stream into outBuffer.
    // outBytesRead will contain the number of bytes read into outBuffer.
    // - If the number of bytes specified is read into the buffer successfully, true is returned.
    // - If the number of bytes read is too large for the buffer, the buffer is filled and false is
    // returned.
    // - If the number of bytes in the file is less than the number requested, the buffer is filled
    // with the
    //   remaining bytes and false is returned.
    bool Read(std::vector<uint8_t>& outBuffer, size_t const bytesToRead, size_t& outBytesRead);

    // Allocates a buffer large enough to fit the stream resource and reads the stream into it.
    bool ReadFile(char const* uri, std::vector<uint8_t>& outBuffer);

    // Writes the specified number of bytes to the stream.
    // - If writing fails, false is returned.
    bool Write(void const* inBuffer, size_t const bytesToWrite);

    // Returns the current offset in the file
    size_t Tell() const;

    // Returns the length of the file.
    size_t Length() const;

    // returns true if at the end of the stream
    bool AtEnd() const;

    char const* GetUri() const;

    bool IsOpen() const;

   protected:
    ovrUriScheme const& GetScheme() const {
        return Scheme;
    }

   private:
    ovrUriScheme const& Scheme;
    std::string Uri;
    ovrStreamMode Mode;

   private:
    virtual bool GetLocalPathFromUri_Internal(const char* uri, std::string& outputPath) = 0;
    virtual bool Open_Internal(char const* Uri, ovrStreamMode const mode) = 0;
    virtual void Close_Internal() = 0;
    virtual void Flush_Internal() = 0;
    virtual bool Read_Internal(
        std::vector<uint8_t>& outBuffer,
        size_t const bytesToRead,
        size_t& outBytesRead) = 0;
    virtual bool ReadFile_Internal(std::vector<uint8_t>& outBuffer) = 0;
    virtual bool Write_Internal(void const* inBuffer, size_t const bytesToWrite) = 0;
    virtual size_t Tell_Internal() const = 0;
    virtual size_t Length_Internal() const = 0;
    virtual bool AtEnd_Internal() const = 0;

    // Private assignment operator to prevent copying.
    ovrStream& operator=(ovrStream& rhs);
};

} // namespace OVRFW
