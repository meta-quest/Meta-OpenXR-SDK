// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
