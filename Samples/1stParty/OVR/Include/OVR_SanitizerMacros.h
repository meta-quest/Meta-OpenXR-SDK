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

#if defined(__has_feature)

// ASAN
#if __has_feature(address_sanitizer)
#define OVR_USING_ADDRESS_SANITIZER
#endif // __has_feature(address_sanitizer)

// TSAN
#if __has_feature(thread_sanitizer)
#define OVR_USING_THREAD_SANITIZER
#endif // __has_feature(thread_sanitizer)

#endif // defined(__has_feature)

// ASAN supression macro
#if defined(OVR_USING_ADDRESS_SANITIZER)

// Refer to https://clang.llvm.org/docs/AddressSanitizer.html#issue-suppression
#define OVR_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))

#else // defined(OVR_USING_ADDRESS_SANITIZER)

#define OVR_NO_SANITIZE_ADDRESS

#endif // defined(OVR_USING_ADDRESS_SANITIZER)

// TSAN supression macro
#if defined(OVR_USING_THREAD_SANITIZER)

// Refer to https://clang.llvm.org/docs/ThreadSanitizer.html
#define OVR_NO_SANITIZE_THREAD __attribute__((no_sanitize("thread")))

#else // defined(OVR_USING_THREAD_SANITIZER)

#define OVR_NO_SANITIZE_THREAD

#endif // defined(OVR_USING_THREAD_SANITIZER)
