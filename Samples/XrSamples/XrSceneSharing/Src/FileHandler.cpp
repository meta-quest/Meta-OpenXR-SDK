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

Filename  : FileHandler.cpp
Content   : Handler for text file-based communication of Anchors and User IDs.
Created   :
Authors   :

Copyright : Copyright (c) Meta Platforms, Inc. and its affiliates. All rights reserved.

*************************************************************************************/

#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>

#include "AnchorUtilities.h"
#include "FileHandler.h"

#if defined(ANDROID)
#include <android/log.h>
#endif

#define DEBUG 1

#if defined(ANDROID)
#define OVR_LOG_TAG "SceneSharingFileHandler"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, OVR_LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, OVR_LOG_TAG, __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, OVR_LOG_TAG, __VA_ARGS__)
#else
#define ALOGE(...)       \
    printf("ERROR: ");   \
    printf(__VA_ARGS__); \
    printf("\n")
#define ALOGW(...)       \
    printf("WARN: ");    \
    printf(__VA_ARGS__); \
    printf("\n")
#define ALOGV(...)       \
    printf("VERBOSE: "); \
    printf(__VA_ARGS__); \
    printf("\n")
#endif

FileHandler::FileHandler() {
    dataDir = kDefaultDataPath;
    ALOGV("Using data path %s", dataDir.c_str());
    assert(dataDir.back() == '/' || dataDir.back() == '\\');
}

bool FileHandler::LoadSharedGroupUuid(XrUuidEXT& groupUuid) {
    ALOGV("%s", __func__);

    std::string filePath = dataDir + kSharedGroupUuidFilename;

    ::FILE* file = ::fopen(filePath.c_str(), "r");
    if (!file) {
        ALOGE("%s: Failed to open file: %s", __func__, filePath.c_str());
        return false;
    }
    if (::feof(file)) {
        ALOGE("%s: File is empty", __func__);
        return false;
    }
    const int uuidCstrLength = XR_UUID_SIZE_EXT * 2 + 1;
    char line[uuidCstrLength];
    while (!::feof(file)) {
        ::fscanf(file, "%32s\n", line);
        if (::ferror(file)) {
            ALOGE("%s: Failed to read from file: %s", __func__, filePath.c_str());
            fclose(file);
            return false;
        }
        XrUuidEXT uuid;
        if (!hexStringToUuid(line, uuid)) {
            ALOGE("%s: Failed to parse UUID string: %s", __func__, line);
        }
        groupUuid = uuid;
    }
    fclose(file);
    return true;
}

bool FileHandler::WriteSharedGroupUuid(
    const XrUuidEXT& groupUuid) {
    ALOGV("%s", __func__);

    std::string filePath = dataDir + kSharedGroupUuidFilename;
    ::FILE* file = ::fopen(filePath.c_str(), "w");
    if (!file) {
        ALOGE("%s: Failed to create file: %s", __func__, filePath.c_str());
        return false;
    }

    int res;
    // We'll use a human-readable format for easier debugging.
    res = ::fprintf(file, "%s\n", uuidToHexString(groupUuid).c_str());
    if (res <= 0) {
        ALOGE("%s: Failed to write data to file: %s", __func__, filePath.c_str());
    }
    fclose(file);
    return (res > 0);
}
