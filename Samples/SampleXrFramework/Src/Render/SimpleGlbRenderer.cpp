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
/************************************************************************************************
Filename    :   SimpleGlbRenderer.cpp
Content     :   A one stop for glb models from the render model OpenXR extension
Created     :   April 2021
Authors     :   Federico Schliemann
Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.
************************************************************************************************/
#include "SimpleGlbRenderer.h"
#include "Model/ModelFile.h"
#include "Model/ModelFileLoading.h"
#include "Model/ModelDef.h"
#include <string.h>

using OVR::Matrix4f;
using OVR::Posef;
using OVR::Quatf;
using OVR::Vector3f;
using OVR::Vector4f;

namespace OVRFW {
namespace SimpleGlbShaders {

/// clang-format off
static const char* VertexShaderSrc = R"glsl(
attribute highp vec4 Position;
attribute highp vec3 Normal;
attribute highp vec2 TexCoord;
attribute highp vec4 JointIndices;

varying lowp vec3 oEye;
varying lowp vec3 oNormal;
varying lowp vec2 oTexCoord;

uniform JointMatrices
{
    highp mat4 Joints[16];
} jb;

vec3 multiply( mat4 m, vec3 v )
{
  return vec3(
  m[0].x * v.x + m[1].x * v.y + m[2].x * v.z,
  m[0].y * v.x + m[1].y * v.y + m[2].y * v.z,
  m[0].z * v.x + m[1].z * v.y + m[2].z * v.z );
}

vec3 transposeMultiply( mat4 m, vec3 v )
{
  return vec3(
  m[0].x * v.x + m[0].y * v.y + m[0].z * v.z,
  m[1].x * v.x + m[1].y * v.y + m[1].z * v.z,
  m[2].x * v.x + m[2].y * v.y + m[2].z * v.z );
}

void main()
{
    highp vec4 localPos = jb.Joints[int(JointIndices.x)] * Position;
    gl_Position = TransformVertex( localPos );
    highp vec3 eye = transposeMultiply(sm.ViewMatrix[VIEW_ID],
                                        -vec3(sm.ViewMatrix[VIEW_ID][3]));
    oEye = normalize(eye - vec3( ModelMatrix * localPos ));
    highp vec3 iNormal = multiply(jb.Joints[int(JointIndices.x)], Normal);
    oNormal = normalize(multiply(ModelMatrix,  iNormal));

    oTexCoord = TexCoord;
}
)glsl";

static const char* FragmentShaderSrc = R"glsl(
precision lowp float;

uniform sampler2D Texture0;
uniform lowp vec3 SpecularLightDirection;
uniform lowp vec3 SpecularLightColor;
uniform lowp vec3 AmbientLightColor;
uniform float Opacity;
uniform float AlphaBlend;

varying lowp vec3 oEye;
varying lowp vec3 oNormal;
varying lowp vec2 oTexCoord;

lowp vec3 multiply( lowp mat3 m, lowp vec3 v )
{
  return vec3(
  m[0].x * v.x + m[1].x * v.y + m[2].x * v.z,
  m[0].y * v.x + m[1].y * v.y + m[2].y * v.z,
  m[0].z * v.x + m[1].z * v.y + m[2].z * v.z );
}

void main()
{
  lowp vec3 eyeDir = normalize( oEye.xyz );
  lowp vec3 Normal = normalize( oNormal );

  lowp vec3 reflectionDir = dot( eyeDir, Normal ) * 2.0 * Normal - eyeDir;
  lowp vec4 diffuse = texture2D( Texture0, oTexCoord );
  lowp vec3 ambientValue = diffuse.xyz * AmbientLightColor;

  lowp float nDotL = max( dot( Normal , SpecularLightDirection ), 0.0 );
  lowp vec3 diffuseValue = diffuse.xyz * SpecularLightColor * nDotL;

  lowp float specularPower = 1.0f - diffuse.a;
  specularPower = specularPower * specularPower;

  lowp vec3 H = normalize( SpecularLightDirection + eyeDir );
  lowp float nDotH = max( dot( Normal, H ), 0.0 );
  lowp float specularIntensity = pow( nDotH, 64.0f * ( specularPower ) ) * specularPower;
  lowp vec3 specularValue = specularIntensity * SpecularLightColor;

  lowp vec3 controllerColor = diffuseValue + ambientValue + specularValue;

  float alphaBlendFactor = max(diffuse.w, AlphaBlend) * Opacity;

  // apply alpha
  gl_FragColor.w = alphaBlendFactor;
  // premult
  gl_FragColor.xyz = controllerColor * gl_FragColor.w;
}
)glsl";

/// clang-format on

} // namespace SimpleGlbShaders

SimpleGlbRenderer::~SimpleGlbRenderer() {
    if (jointsBuffer) {
        jointsBuffer->Destroy();
    }
}

std::vector<Matrix4f> SimpleGlbRenderer::GetDefaultPoseTransforms() const {
    std::vector<Matrix4f> outTransforms;

    if (!RenderModel->Skins.empty()) {
        outTransforms.resize(RenderModel->Skins[0].jointIndexes.size());
        for (size_t joint = 0; joint < RenderModel->Skins[0].jointIndexes.size(); ++joint) {
            outTransforms[joint] = (RenderModel->Nodes[RenderModel->Skins[0].jointIndexes[joint]]
                                        .GetGlobalTransform() *
                                    RenderModel->Skins[0].inverseBindMatrices[joint])
                                       .Transposed();
        }
    } else {
        Matrix4f transform; // identity
        for (size_t nodeIndex = 0; nodeIndex < RenderModel->Nodes.size(); ++nodeIndex) {
            const ModelNode& node = RenderModel->Nodes[nodeIndex];

            if (node.parentIndex < 0) {
                transform = node.GetLocalTransform();
                break;
            }
        }

        outTransforms.resize(kMaxJoints);
        for (size_t joint = 0; joint < kMaxJoints; ++joint) {
            outTransforms[joint] = transform;
        }
    }

    return outTransforms;
}

bool SimpleGlbRenderer::Init(std::vector<uint8_t>& modelBuffer) {
    /// Shader
    ovrProgramParm UniformParms[] = {
        {"Texture0", ovrProgramParmType::TEXTURE_SAMPLED},
        {"SpecularLightDirection", ovrProgramParmType::FLOAT_VECTOR3},
        {"SpecularLightColor", ovrProgramParmType::FLOAT_VECTOR3},
        {"AmbientLightColor", ovrProgramParmType::FLOAT_VECTOR3},
        {"Opacity", ovrProgramParmType::FLOAT},
        {"AlphaBlend", ovrProgramParmType::FLOAT},
        {"JointMatrices", ovrProgramParmType::BUFFER_UNIFORM},
    };
    ProgRenderModel = GlProgram::Build(
        "",
        SimpleGlbShaders::VertexShaderSrc,
        "",
        SimpleGlbShaders::FragmentShaderSrc,
        UniformParms,
        sizeof(UniformParms) / sizeof(ovrProgramParm));

    MaterialParms materials = {};
    ModelGlPrograms programs = {};
    programs.ProgSingleTexture = &ProgRenderModel;
    programs.ProgBaseColorPBR = &ProgRenderModel;
    programs.ProgSkinnedBaseColorPBR = &ProgRenderModel;
    programs.ProgLightMapped = &ProgRenderModel;
    programs.ProgBaseColorEmissivePBR = &ProgRenderModel;
    programs.ProgSkinnedBaseColorEmissivePBR = &ProgRenderModel;
    programs.ProgSimplePBR = &ProgRenderModel;
    programs.ProgSkinnedSimplePBR = &ProgRenderModel;

    RenderModel = std::unique_ptr<ModelFile>(LoadModelFile_glB(
        "modelrenderer", (const char*)modelBuffer.data(), modelBuffer.size(), programs, materials));

    if (RenderModel == nullptr || static_cast<int>(RenderModel->Models.size()) < 1) {
        ALOGE("Couldn't load modelrenderer model!");
        return false;
    }

    jointsBuffer.reset(new OVRFW::GlBuffer());
    jointsBuffer->Create(
        OVRFW::GlBufferType_t::GLBUFFER_TYPE_UNIFORM, kMaxJoints * sizeof(Matrix4f), nullptr);

    std::vector<Matrix4f> poseTransforms = GetDefaultPoseTransforms();
    Matrix4f* pose = (Matrix4f*)jointsBuffer->MapBuffer();
    for (size_t joint = 0; joint < poseTransforms.size(); ++joint) {
        pose[joint] = poseTransforms[joint];
    }
    jointsBuffer->UnmapBuffer();

    for (auto& model : RenderModel->Models) {
        auto& gc = model.surfaces[0].surfaceDef.graphicsCommand;
        gc.UniformData[0].Data = &gc.Textures[0];
        gc.UniformData[1].Data = &SpecularLightDirection;
        gc.UniformData[2].Data = &SpecularLightColor;
        gc.UniformData[3].Data = &AmbientLightColor;
        gc.UniformData[4].Data = &Opacity;
        gc.UniformData[5].Data = &AlphaBlendFactor;
        gc.UniformData[6].Data = jointsBuffer.get();
        gc.GpuState.depthEnable = gc.GpuState.depthMaskEnable = true;
        gc.GpuState.blendEnable = ovrGpuState::BLEND_ENABLE;
        gc.GpuState.blendMode = GL_FUNC_ADD;
        gc.GpuState.blendSrc = GL_ONE;
        gc.GpuState.blendDst = GL_ONE_MINUS_SRC_ALPHA;
    }

    /// Set defaults
    SpecularLightDirection = Vector3f(1.0f, 1.0f, 0.0f);
    SpecularLightColor = Vector3f(1.0f, 0.95f, 0.8f) * 0.75f;
    AmbientLightColor = Vector3f(1.0f, 1.0f, 1.0f) * 0.15f;

    /// all good
    Initialized = true;
    return true;
}

void SimpleGlbRenderer::Shutdown() {
    OVRFW::GlProgram::Free(ProgRenderModel);
}

void SimpleGlbRenderer::Update(const OVR::Posef& pose) {
    /// Compute transform for the root

    OVR::Posef offsetPose = pose;

    Transform = Matrix4f(offsetPose);
}

void SimpleGlbRenderer::Render(std::vector<ovrDrawSurface>& surfaceList) {
    /// toggle alpha override
    AlphaBlendFactor = UseSolidTexture ? 1.0f : 0.0f;
    if (RenderModel != nullptr) {
        for (int i = 0; i < static_cast<int>(RenderModel->Models.size()); i++) {
            auto& model = RenderModel->Models[i];
            ovrDrawSurface controllerSurface;
            for (int j = 0; j < static_cast<int>(model.surfaces.size()); j++) {
                controllerSurface.surface = &(model.surfaces[j].surfaceDef);
                controllerSurface.modelMatrix = Transform;
                surfaceList.push_back(controllerSurface);
            }
        }
    }
}

} // namespace OVRFW
