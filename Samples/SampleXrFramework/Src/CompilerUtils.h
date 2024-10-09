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

Filename    :   CompilerUtils.h
Content     :   Macros and utilities for compile-time validation.
Created     :   March 20, 2018
Authors     :   Jonathan Wright
Language    :   C++

*******************************************************************************/

#pragma once

// included here with the intention of implementing a custom assert macro later.
#include <assert.h>

#ifndef OVR_UNUSED
#define OVR_UNUSED(a) (void)(a)
#endif
