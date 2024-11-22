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
#include "CubesRenderer.h"
#include <cstddef>

void CubesRenderer::Add(const OVR::Vector4f& colorOptions) {
    // Add cube at the pose to render
    OVRFW::GeometryBuilder templateCubeGeometry;
    templateCubeGeometry.Add(
        OVRFW::BuildUnitCubeDescriptor(), OVRFW::GeometryBuilder::kInvalidIndex, colorOptions);

    OVRFW::GeometryRenderer cubeRenderer;
    CubesRenderer.push_back(cubeRenderer);

    CubesRenderer.back().ChannelControl = OVR::Vector4f(1, 1, 1, 1);
    CubesRenderer.back().Init(templateCubeGeometry.ToGeometryDescriptor());

    const float cubeLength = 0.05;
    CubesRenderer.back().SetScale({cubeLength, cubeLength, cubeLength});
}

void CubesRenderer::Shutdown() {
    for (auto& cube : CubesRenderer) {
        cube.Shutdown();
    }
}

void CubesRenderer::Render(std::vector<OVRFW::ovrDrawSurface>& surfaces) {
    for (auto& cube : CubesRenderer) {
        cube.Render(surfaces);
    }
}

void CubesRenderer::UpdatePoses(const std::vector<AnchorPose>& poses) {
    while (!CubesRenderer.empty()) {
        auto cube = CubesRenderer.back();
        CubesRenderer.pop_back();
        cube.Shutdown();
    }

    for (size_t i = 0; i < poses.size(); i++) {
        this->Add(poses[i].Color);
        CubesRenderer[i].SetPose(poses[i].Pose);
        CubesRenderer[i].Update();
    }
}
