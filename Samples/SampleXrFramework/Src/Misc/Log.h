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
/*******************************************************************************

Filename	:   Log.h
Content		:	Macros for debug logging.
Created		:   February 21, 2018
Authors		:   Jonathan Wright
Language	:   C++

*******************************************************************************/

#pragma once

#if defined(ANDROID)
#include <android/log.h>
#else
#include <stdarg.h>
#endif // defined(ANDROID)

#include <stdlib.h> // abort

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(ANDROID)
typedef enum SamplesLogPriority {
    SAMPLES_LOG_ERROR = ANDROID_LOG_ERROR,
    SAMPLES_LOG_WARN = ANDROID_LOG_WARN,
    SAMPLES_LOG_INFO = ANDROID_LOG_INFO,
    SAMPLES_LOG_VERBOSE = ANDROID_LOG_VERBOSE,
} SamplesLogPriority;
#else
typedef enum SamplesLogPriority {
    SAMPLES_LOG_ERROR = 5,
    SAMPLES_LOG_WARN = 4,
    SAMPLES_LOG_INFO = 3,
    SAMPLES_LOG_VERBOSE = 1,
} SamplesLogPriority;
#endif

void LogWithFilenameTag(const int priority, const char* filename, const char* fmt, ...);
void VLogWithFilenameTag(const int priority, const char* filename, const char* fmt, va_list args);

#define ALOGE(...) \
    { LogWithFilenameTag(SAMPLES_LOG_ERROR, __FILE__, __VA_ARGS__); }

#define ALOGE_FAIL(...)                                               \
    {                                                                 \
        LogWithFilenameTag(SAMPLES_LOG_ERROR, __FILE__, __VA_ARGS__); \
        abort();                                                      \
    }

#if 1 // DEBUG

#define ALOG(...) \
    { LogWithFilenameTag(SAMPLES_LOG_INFO, __FILE__, __VA_ARGS__); }

#define ALOGV(...) \
    { LogWithFilenameTag(SAMPLES_LOG_VERBOSE, __FILE__, __VA_ARGS__); }

#define ALOGW(...) \
    { LogWithFilenameTag(SAMPLES_LOG_WARN, __FILE__, __VA_ARGS__); }

#else
#define ALOGV(...)
#endif

#if defined(__cplusplus)
} // extern "C"
#endif
