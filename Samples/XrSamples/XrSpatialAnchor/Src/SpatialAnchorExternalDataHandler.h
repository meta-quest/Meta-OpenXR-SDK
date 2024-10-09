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

#include <vector>
#include <openxr/openxr.h>

class SpatialAnchorExternalDataHandler {
   public:
    virtual ~SpatialAnchorExternalDataHandler() {}

    // LoadShareUserList loads the list of FBIDs of users with whom to share Spatial Anchors.
    virtual bool LoadShareUserList(std::vector<XrSpaceUserIdFB>& userIdList) = 0;

    // LoadInboundSpatialAnchorList loads the list of Spatial Anchors that have
    // been shared with the current user.
    virtual bool LoadInboundSpatialAnchorList(std::vector<XrUuidEXT>& spatialAnchorList) = 0;

    // WriteSharedSpatialAnchorList emits the list of Spatial Anchors shared by the current user
    // to the specified list of users.
    virtual bool WriteSharedSpatialAnchorList(
        const std::vector<XrUuidEXT>& spatialAnchorList,
        const std::vector<XrSpaceUserIdFB>& userIdList) = 0;
};
