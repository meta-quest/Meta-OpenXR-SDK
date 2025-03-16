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

static const char VERTEX_SHADER[] = R"(
  #define NUM_VIEWS 2
  #define VIEW_ID gl_ViewID_OVR
  #extension GL_OVR_multiview2 : require
  layout(num_views=NUM_VIEWS) in;
  in vec3 vertexPosition;
  in vec4 vertexColor;
  uniform mat4 ModelMatrix;
  uniform SceneMatrices {
  	uniform mat4 ViewMatrix[NUM_VIEWS];
  	uniform mat4 ProjectionMatrix[NUM_VIEWS];
  } sm;
  out vec4 fragmentColor;
  void main() {
  	gl_Position = sm.ProjectionMatrix[VIEW_ID] * (sm.ViewMatrix[VIEW_ID] * (ModelMatrix * vec4(vertexPosition, 1.0)));
  	fragmentColor = vertexColor;
  }
)";

static const char FRAGMENT_SHADER[] = R"(
  in lowp vec4 fragmentColor;
  out lowp vec4 outColor;
  void main() {
  	outColor = fragmentColor;
  }
)";

static const char STAGE_VERTEX_SHADER[] = R"(
  #define NUM_VIEWS 2
  #define VIEW_ID gl_ViewID_OVR
  #extension GL_OVR_multiview2 : require
  layout(num_views=NUM_VIEWS) in;
  in vec3 vertexPosition;
  uniform mat4 ModelMatrix;
  uniform SceneMatrices {
  	uniform mat4 ViewMatrix[NUM_VIEWS];
  	uniform mat4 ProjectionMatrix[NUM_VIEWS];
  } sm;
  void main() {
  	gl_Position = sm.ProjectionMatrix[VIEW_ID] * (sm.ViewMatrix[VIEW_ID] * (ModelMatrix * (vec4(vertexPosition, 1.0))));
  }
)";

static const char STAGE_FRAGMENT_SHADER[] = R"(
  out lowp vec4 outColor;
  void main() {
  	outColor = vec4(0.5, 0.5, 1.0, 0.5);
  }
)";

static const char MESH_FRAGMENT_SHADER[] = R"(
  out lowp vec4 outColor;
  void main() {
    outColor = vec4(1.0, 1.0, 1.0, 0.8);
  }
)";
