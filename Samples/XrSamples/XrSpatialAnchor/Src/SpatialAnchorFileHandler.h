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
#pragma once

#include "SpatialAnchorExternalDataHandler.h"
#include <string>
#include <vector>
#include <openxr/openxr.h>

class SpatialAnchorFileHandler : public SpatialAnchorExternalDataHandler {
   public:
    SpatialAnchorFileHandler();

    // LoadShareUserList loads the list of FBIDs of users with whom to share Spatial Anchors.
    bool LoadShareUserList(std::vector<XrSpaceUserIdFB>& userIdList) override;
    bool LoadInboundSpatialAnchorList(std::vector<XrUuidEXT>& spatialAnchorList) override;
    bool WriteSharedSpatialAnchorList(
        const std::vector<XrUuidEXT>& spatialAnchorList,
        const std::vector<XrSpaceUserIdFB>& userIdList) override;

   private:
    std::string dataDir;
    const char* kShareUserListFilename = "shareUserList.txt";
    const char* kInboundSpatialAnchorListFilename = "inboundSpatialAnchorList.txt";
    const char* kSharedSpatialAnchorListFilename = "sharedSpatialAnchorList.txt";

// Replace this value with the path you want the named files above to be.
// Make sure to include the trailing slash (backslash for Windows).
#ifdef WIN32
    const char* kDefaultDataPath = "C:\\temp_SpatialAnchorXr\\";
#else
    const char* kDefaultDataPath = "/sdcard/Android/data/com.oculus.sdk.spatialanchor/files/";
#endif
};
