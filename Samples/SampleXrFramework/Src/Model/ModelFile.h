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

Filename    :   ModelFile.h
Content     :   Model file loading.
Created     :   December 2013
Authors     :   John Carmack, J.M.P. van Waveren

*************************************************************************************/

#pragma once

#include "ModelDef.h"
#include "OVR_FileSys.h"

namespace OVRFW {

// A ModelFile is the in-memory representation of a digested model file.
// It should be imutable in normal circumstances, but it is ok to load
// and modify a model for a particular task, such as changing materials.
class ModelFile {
   public:
    ModelFile();
    ModelFile(const char* name) : FileName(name) {}
    ~ModelFile(); // Frees all textures and geometry

    ovrSurfaceDef* FindNamedSurface(const char* name) const;
    const ModelTexture* FindNamedTexture(const char* name) const;
    const ModelJoint* FindNamedJoint(const char* name) const;

    // #TODO: deprecate, we should be doing things off Nodes instead of Tags now.
    const ModelTag* FindNamedTag(const char* name) const;

    OVR::Bounds3f GetBounds() const;

   public:
    std::string FileName;
    bool UsingSrgbTextures;

    float animationStartTime;
    float animationEndTime;

    std::vector<ModelTag> Tags;

    // This is used by the movement code
    ModelCollision Collisions;
    ModelCollision GroundCollisions;

    // This is typically used for gaze selection.
    ModelTrace TraceModel;

    std::vector<ModelBuffer> Buffers;
    std::vector<ModelBufferView> BufferViews;
    std::vector<ModelAccessor> Accessors;
    std::vector<ModelTexture> Textures;
    std::vector<ModelSampler> Samplers;
    std::vector<ModelTextureWrapper> TextureWrappers;
    std::vector<ModelMaterial> Materials;
    std::vector<Model> Models;
    std::vector<ModelCamera> Cameras;
    std::vector<ModelNode> Nodes;
    std::vector<ModelAnimation> Animations;
    std::vector<ModelAnimationTimeLine> AnimationTimeLines;
    std::vector<ModelSkin> Skins;
    std::vector<ModelSubScene> SubScenes;
};

// Pass in the programs that will be used for the model materials.
// Obviously not very general purpose.
// Returns nullptr if there is an error loading the file
ModelFile* LoadModelFileFromMemory(
    const char* fileName,
    const void* buffer,
    int bufferLength,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms,
    ModelGeo* outModelGeo = nullptr);

// Returns nullptr if there is an error loading the file
ModelFile* LoadModelFile(
    const char* fileName,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms);

// Returns nullptr if the file is not found.
ModelFile* LoadModelFileFromOtherApplicationPackage(
    void* zipFile,
    const char* nameInZip,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms);

// Returns nullptr if there is an error loading the file
ModelFile* LoadModelFileFromApplicationPackage(
    const char* nameInZip,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms);

// Returns nullptr if there is an error loading the file
ModelFile* LoadModelFile(
    class ovrFileSys& fileSys,
    const char* uri,
    const ModelGlPrograms& programs,
    const MaterialParms& materialParms);

} // namespace OVRFW
