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

Filename    :   OVR_MappedFile.cpp
Content     :   Cross-platform memory-mapped file wrapper.
Created     :   May 12, 2014
Authors     :   Chris Taylor

*************************************************************************************/

#include "OVR_MappedFile.h"
#include "OVR_Types.h"

#if defined(OVR_OS_ANDROID)

#if defined(OVR_OS_ANDROID)
// disable warnings on implicit type conversion where value may be changed by conversion for
// sys/stat.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#if defined(OVR_OS_ANDROID)
// restore warnings on implicit type conversion where value may be changed by conversion for
// sys/stat.h
#pragma GCC diagnostic pop
#endif

namespace OVRFW {

static uint32_t DEFAULT_ALLOCATION_GRANULARITY = 65536;

static uint32_t GetAllocationGranularity() {
    uint32_t alloc_gran = 0;

#if defined(OVR_OS_WIN32)

    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    alloc_gran = sys_info.dwAllocationGranularity;

#elif defined(OVR_OS_MAC) || defined(OVR_OS_IPHONE)

    alloc_gran = (uint32_t)getpagesize();

#elif defined(_SC_PAGE_SIZE)

    alloc_gran = (uint32_t)sysconf(_SC_PAGE_SIZE);

#endif

    return (alloc_gran > 0) ? alloc_gran : DEFAULT_ALLOCATION_GRANULARITY;
}

/*
    MappedFile
*/

MappedFile::MappedFile() {
    Length = 0;
    File = -1;
}

MappedFile::~MappedFile() {
    Close();
}

bool MappedFile::OpenRead(const char* path, bool read_ahead, bool no_cache) {
    Close();

    ReadOnly = true;

    // Don't allow private files to be read by other applications.
    File = open(path, O_RDONLY);

    if (File == -1) {
        return false;
    } else {
        Length = lseek(File, 0, SEEK_END);

        if (Length <= 0) {
            return false;
        } else {
#if defined(F_RDAHEAD)
            if (read_ahead) {
                fcntl(File, F_RDAHEAD, 1);
            }
#endif

#if defined(F_NOCACHE)
            if (no_cache) {
                fcntl(File, F_NOCACHE, 1);
            }
#endif
        }
    }

    return true;
}

bool MappedFile::OpenWrite(const char* path, size_t size) {
    Close();

    ReadOnly = false;
    Length = size;

    // Don't allow private files to be read or written by
    // other applications.
    File = open(path, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0660);

    if (File == -1) {
        return false;
    } else {
        if (-1 == lseek(File, size - 1, SEEK_SET)) {
            return false;
        } else {
            if (1 != write(File, "", 1)) {
                return false;
            }
        }
    }

    return true;
}

void MappedFile::Close() {
    if (File != -1) {
        close(File);
        File = -1;
    }

    Length = 0;
}

/*
    MappedView
*/

MappedView::MappedView() {
    Data = 0;
    Length = 0;
    Offset = 0;
    File = 0;
    Map = MAP_FAILED;
}

MappedView::~MappedView() {
    Close();
}

bool MappedView::Open(MappedFile* file) {
    Close();

    if (!file || !file->IsValid()) {
        return false;
    }

    File = file;
    return true;
}

uint8_t* MappedView::MapView(size_t offset, uint32_t length) {
    if (length == 0) {
        length = static_cast<uint32_t>(File->GetLength());
    }

    if (offset) {
        uint32_t granularity = GetAllocationGranularity();

        // Bring offset back to the previous allocation granularity
        uint32_t mask = granularity - 1;
        uint32_t masked = (uint32_t)offset & mask;
        if (masked) {
            offset -= masked;
            length += masked;
        }
    }

    int prot = PROT_READ;
    if (!File->ReadOnly) {
        prot |= PROT_WRITE;
    }

    // Use MAP_PRIVATE so that memory is not exposed to other processes.
    Map = mmap(0, length, prot, MAP_PRIVATE, File->File, offset);

    if (Map == MAP_FAILED) {
        return 0;
    }

    Data = reinterpret_cast<uint8_t*>(Map);

    Offset = offset;
    Length = length;

    return Data;
}

void MappedView::Close() {
    if (Map != MAP_FAILED) {
        munmap(Map, Length);
        Map = MAP_FAILED;
    }
    Data = 0;
    Length = 0;
    Offset = 0;
}

} // namespace OVRFW

#endif // defined(OVR_OS_ANDROID)
