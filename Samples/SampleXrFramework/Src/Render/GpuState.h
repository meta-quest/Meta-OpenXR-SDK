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

Filename    :   GpuState.h
Content     :   Gpu state management.
Created     :   August 9. 2013
Authors     :   John Carmack

*************************************************************************************/

#pragma once

#include "Egl.h"

#if !defined(GL_FILL)
#define GL_FILL 0x00 // just a placeholder -- this define doesn't exist in GL ES
#endif

namespace OVRFW {

struct ovrGpuState {
    enum ovrBlendEnable { BLEND_DISABLE, BLEND_ENABLE, BLEND_ENABLE_SEPARATE };

    ovrGpuState()
        : blendMode(GL_FUNC_ADD),
          blendSrc(GL_ONE),
          blendDst(GL_ZERO),
          blendSrcAlpha(GL_ONE),
          blendDstAlpha(GL_ZERO),
          blendModeAlpha(GL_FUNC_ADD),
          depthFunc(GL_LEQUAL),
          frontFace(GL_CCW),
          polygonMode(GL_FILL),
          blendEnable(BLEND_DISABLE),
          depthEnable(true),
          depthMaskEnable(true),
          polygonOffsetEnable(false),
          cullEnable(true),
          lineWidth(1.0f) {
        colorMaskEnable[0] = colorMaskEnable[1] = colorMaskEnable[2] = colorMaskEnable[3] = true;
        depthRange[0] = 0.0f;
        depthRange[1] = 1.0f;
    }

    GLenum blendMode; // GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX
    GLenum blendSrc;
    GLenum blendDst;
    GLenum blendSrcAlpha;
    GLenum blendDstAlpha;
    GLenum blendModeAlpha;
    GLenum depthFunc;
    GLenum frontFace; // GL_CW, GL_CCW
    GLenum polygonMode;

    ovrBlendEnable blendEnable; // off, normal, separate

    bool depthEnable;
    bool depthMaskEnable;
    bool colorMaskEnable[4];
    bool polygonOffsetEnable;
    bool cullEnable;
    GLfloat lineWidth;
    GLfloat depthRange[2]; // nearVal, farVal
};

} // namespace OVRFW
