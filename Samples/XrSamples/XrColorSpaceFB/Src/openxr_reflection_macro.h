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
#include <openxr/openxr_reflection.h>

// Macro copied from example in openxr_reflection.h
#define XR_ENUM_CASE_STR(name, val) \
    case name:                      \
        return #name;

#define XR_ENUM_STR(enumType)                                                                \
    constexpr const char* XrEnumStr(enumType e) {                                            \
        switch (e) { XR_LIST_ENUM_##enumType(XR_ENUM_CASE_STR) default : return "Unknown"; } \
    }

// Creates overloads of XrEnumStr() function for these enum types
XR_ENUM_STR(XrColorSpaceFB);
XR_ENUM_STR(XrResult);

#undef XR_ENUM_CASE_STR
#undef XR_ENUM_STR
