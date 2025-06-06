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

#include <cstdint>

namespace OVRFW {

struct ovrGpuState {
    enum ovrBlendEnable { BLEND_DISABLE, BLEND_ENABLE, BLEND_ENABLE_SEPARATE };

    /// Aliasing some GL constant for defaults and commonly used behavior
    /// without needing the explicit GL include

    static constexpr uint32_t kGL_ONE = 0x0001;
    static constexpr uint32_t kGL_ZERO = 0x0000;
    static constexpr uint32_t kGL_SRC_ALPHA = 0x0302;
    static constexpr uint32_t kGL_ONE_MINUS_SRC_ALPHA = 0x0303;
    static constexpr uint32_t kGL_DST_ALPHA = 0x0304;
    static constexpr uint32_t kGL_ONE_MINUS_DST_ALPHA = 0x0305;

    static constexpr uint32_t kGL_FUNC_ADD = 0x8006;
    static constexpr uint32_t kGL_FUNC_SUBTRACT = 0x800A;
    static constexpr uint32_t kGL_FUNC_REVERSE_SUBTRACT = 0x800B;
    static constexpr uint32_t kGL_MIN = 0x8007;
    static constexpr uint32_t kGL_MAX = 0x8008;

    static constexpr uint32_t kGL_LEQUAL = 0x0203;
    static constexpr uint32_t kGL_GREATER = 0x0204;

    static constexpr uint32_t kGL_CW = 0x0900;
    static constexpr uint32_t kGL_CCW = 0x0901;

    static constexpr uint32_t kGL_FILL = 0x1B02;

    ovrGpuState()
        : blendMode(kGL_FUNC_ADD),
          blendSrc(kGL_ONE),
          blendDst(kGL_ZERO),
          blendSrcAlpha(kGL_ONE),
          blendDstAlpha(kGL_ZERO),
          blendModeAlpha(kGL_FUNC_ADD),
          depthFunc(kGL_LEQUAL),
          frontFace(kGL_CCW),
          polygonMode(kGL_FILL),
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

    uint32_t blendMode; // GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX
    uint32_t blendSrc;
    uint32_t blendDst;
    uint32_t blendSrcAlpha;
    uint32_t blendDstAlpha;
    uint32_t blendModeAlpha;
    uint32_t depthFunc;
    uint32_t frontFace; // GL_CW, GL_CCW
    uint32_t polygonMode;

    ovrBlendEnable blendEnable; // off, normal, separate

    bool depthEnable;
    bool depthMaskEnable;
    bool colorMaskEnable[4];
    bool polygonOffsetEnable;
    bool cullEnable;
    float lineWidth;
    float depthRange[2]; // nearVal, farVal
};

} // namespace OVRFW
