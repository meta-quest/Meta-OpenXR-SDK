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
Filename    :   xr_helper.h
Content     :   Base interface to wrap openxr extension functionality
Created     :   April 2021
Authors     :   Federico Schliemann
Language    :   C++
Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.
************************************************************************************************/
#pragma once

#if defined(ANDROID)
#define XR_USE_GRAPHICS_API_OPENGL_ES 1
#define XR_USE_PLATFORM_ANDROID 1
#else
#include "unknwn.h"
#define XR_USE_GRAPHICS_API_OPENGL 1
#define XR_USE_PLATFORM_WIN32 1
#endif

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#if !defined(XRLOG)
#define XRLOG(...)
#endif

class XrHelper {
   public:
    XrHelper(XrInstance instance) : instance_(instance), lastError_(XR_SUCCESS) {}
    virtual ~XrHelper() = default;
    virtual bool SessionInit(XrSession session) = 0;
    virtual bool SessionEnd() = 0;
    virtual bool Update(XrSpace currentSpace, XrTime predictedDisplayTime) = 0;
    XrResult GetLastError() const {
        return lastError_;
    }

   protected:
    bool _oxr(XrResult xr, const char* func) {
        if (XR_FAILED(xr)) {
            char errorBuffer[XR_MAX_RESULT_STRING_SIZE];
            xrResultToString(instance_, xr, errorBuffer);
            XRLOG("XR FAIL: `%s` -> `%s` 0x%08X", func, errorBuffer, (uint64_t)xr);
            lastError_ = xr;
            return false;
        }
        return true;
    }
    XrInstance GetInstance() const {
        return instance_;
    }

   private:
    XrInstance instance_;
    XrResult lastError_;
};

#if !defined(oxr)
#define oxr(func) _oxr(func, #func)
#endif
