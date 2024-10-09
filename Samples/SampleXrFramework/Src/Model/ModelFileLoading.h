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

Filename    :   ModelFileLoading.h
Content     :   Model file loading.
Created     :   December 2013
Authors     :   John Carmack, J.M.P. van Waveren

*************************************************************************************/

#pragma once
#include "ModelFile.h"

#include <math.h>
#include <vector>

#include "OVR_Math.h"

#include <unzip.h>

// Verbose log, redefine this as LOG() to get lots more info dumped
// #define LOGV ALOG
#define LOGV(...)

namespace OVRFW {

void CalculateTransformFromRTS(
    OVR::Matrix4f* localTransform,
    const OVR::Quatf rotation,
    const OVR::Vector3f translation,
    const OVR::Vector3f scale);

void LoadModelFileTexture(
    ModelFile& model,
    const char* textureName,
    const char* buffer,
    const int size,
    const MaterialParms& materialParms);

bool LoadModelFile_OvrScene(
    ModelFile* modelPtr,
    unzFile zfp,
    const char* fileName,
    const char* fileData,
    const int fileDataLength,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms,
    ModelGeo* outModelGeo = NULL);

bool LoadModelFile_glTF_OvrScene(
    ModelFile* modelFilePtr,
    unzFile zfp,
    const char* fileName,
    const char* fileData,
    const int fileDataLength,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms,
    ModelGeo* outModelGeo = NULL);

ModelFile* LoadModelFile_glB(
    const char* fileName,
    const char* fileData,
    const int fileDataLength,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms,
    ModelGeo* outModelGeo = NULL);
} // namespace OVRFW
