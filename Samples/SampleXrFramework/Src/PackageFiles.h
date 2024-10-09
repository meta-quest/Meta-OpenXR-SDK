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

Filename    :   PackageFiles.h
Content     :   Read files from the application package zip
Created     :   August 18, 2014
Authors     :   John Carmack

*************************************************************************************/

#pragma once

#include <vector>

// The application package is the moral equivalent of the filesystem, so
// I don't feel too bad about making it globally accessible, versus requiring
// an App pointer to be handed around to everything that might want to load
// a texture or model.

namespace OVRFW {

//==============================================================
// OvrApkFile
// RAII class for application packages
//==============================================================
class OvrApkFile {
   public:
    OvrApkFile(void* zipFile);
    ~OvrApkFile();

    operator void*() const {
        return ZipFile;
    }
    operator bool() const {
        return ZipFile != 0;
    }

   private:
    void* ZipFile;
};

//--------------------------------------------------------------
// Functions for reading assets from other application packages
//--------------------------------------------------------------

// Call this to open a specific package and use the returned handle in calls to functions for
// loading from other application packages.
void* ovr_OpenOtherApplicationPackage(const char* packageName);

// Call this to close another application package after loading resources from it.
void ovr_CloseOtherApplicationPackage(void*& zipFile);

// These are probably NOT thread safe!
bool ovr_OtherPackageFileExists(void* zipFile, const char* nameInZip);

// Returns NULL buffer if the file is not found.
bool ovr_ReadFileFromOtherApplicationPackage(
    void* zipFile,
    const char* nameInZip,
    int& length,
    void*& buffer);
bool ovr_ReadFileFromOtherApplicationPackage(
    void* zipFile,
    const char* nameInZip,
    std::vector<uint8_t>& buffer);

//--------------------------------------------------------------
// Functions for reading assets from this process's application package
//--------------------------------------------------------------

// returns the zip file for the applications own package
void* ovr_GetApplicationPackageFile();

// Returns something like "/data/data/com.oculus.vrscript/cache/"
// Applications can write files here and expect them to be cleaned up on
// application uninstall, unlike writing to /sdcard.  Also, this is on a
// proper linux filesystem, so exec permissions can be set.
const char* ovr_GetApplicationPackageCachePath();

// App.cpp calls this very shortly after startup.
// If cachePath is not NULL, compressed files that are read will be written
// out to the cachePath with the CRC as the filename so they can be read
// back in much faster.
void ovr_OpenApplicationPackage(const char* packageName, const char* cachePath);

// These are probably NOT thread safe!
bool ovr_PackageFileExists(const char* nameInZip);

// Returns NULL buffer if the file is not found.
bool ovr_ReadFileFromApplicationPackage(const char* nameInZip, int& length, void*& buffer);

// Returns an empty MemBufferFile if the file is not found.
bool ovr_ReadFileFromApplicationPackage(const char* nameInZip, std::vector<uint8_t>& buffer);

} // namespace OVRFW
