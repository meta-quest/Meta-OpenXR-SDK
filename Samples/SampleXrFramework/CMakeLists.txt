# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# Licensed under the Oculus SDK License Agreement (the "License");
# you may not use the Oculus SDK except in compliance with the License,
# which is provided at the time of installation or download, or which
# otherwise accompanies this software in either electronic or hard copy form.
#
# You may obtain a copy of the License at
# https://developer.oculus.com/licenses/oculussdk/
#
# Unless required by applicable law or agreed to in writing, the Oculus SDK
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
if(NOT TARGET OpenXR::openxr_loader)
    find_package(OpenXR REQUIRED)
endif()

if(ANDROID)
    add_library(app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
endif()

set(OPENXR_PREVIEW_HEADER ../../OpenXR)

set(1STPARTY_PATH ../1stParty)
set(3RDPARTY_PATH ../3rdParty)

file(GLOB_RECURSE CPP_SOURCES "Src/*.cpp")
file(GLOB_RECURSE C_SOURCES "Src/*.c")
add_library(samplexrframework STATIC ${CPP_SOURCES} ${C_SOURCES})


# Add include directories
target_include_directories(
    samplexrframework
    PUBLIC
        Src
        ${1STPARTY_PATH}/OVR/Include
        ${1STPARTY_PATH}/utilities/include
        #### 3rd Party Headers from the OpenXR repo ####
        ${3RDPARTY_PATH}/khronos/openxr/OpenXR-SDK/src/common
        # Extra vendor-provided headers
        ${OPENXR_PREVIEW_HEADER}
)

target_link_libraries(
    samplexrframework
    PUBLIC
        minizip
        stb
        ktx
        OpenXR::openxr_loader
)

target_compile_definitions(
    samplexrframework PUBLIC $<IF:$<CONFIG:Debug>,OVR_BUILD_DEBUG=1,>
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        samplexrframework
        PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-invalid-offsetof>
    )
endif()

if(ANDROID)
    target_include_directories(samplexrframework PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
    target_compile_definitions(
        samplexrframework
        PUBLIC
            XR_OS_ANDROID=1
            XR_USE_PLATFORM_ANDROID=1
            XR_USE_GRAPHICS_API_OPENGL_ES=1
            ANDROID_NDK
            $<IF:$<CONFIG:Debug>,OVR_BUILD_DEBUG=1,>
    )
    target_compile_options(
        samplexrframework
        PUBLIC
            -Werror
            -Wall
            -Wextra
            -Wno-unused-parameter
            -Wno-missing-field-initializers
    )
    target_link_libraries(
        samplexrframework
        PRIVATE
            android
            EGL
            GLESv3
            log
            app_glue
    )
    if(USE_ASAN)
        target_compile_options(
            samplexrframework
            PUBLIC $<IF:$<CONFIG:Debug>,-fsanitize=address,>
                    $<IF:$<CONFIG:Debug>,-fno-omit-frame-pointer,>
        )
        target_link_options(
            samplexrframework PUBLIC
            $<IF:$<CONFIG:Debug>,-fsanitize=address,>
        )
    endif()
elseif(WIN32)
    add_definitions(-DNOMINMAX)
    if(MSVC)
        target_compile_definitions(samplexrframework PUBLIC /wd4267)
    endif()

    add_library(
        samplecommon_win32gl STATIC
        Src/Render/GlWrapperWin32.c
    )

    target_include_directories(
        samplecommon_win32gl
        PUBLIC
            Src
            ${3RDPARTY_PATH}/glext
            ${1STPARTY_PATH}/utilities/include
            ${1STPARTY_PATH}/utilities/include/vulkan
            ${1STPARTY_PATH}/utilities/include/utils
    )

    target_link_libraries(samplecommon_win32gl PUBLIC opengl32 gdi32 user32)
    target_link_libraries(samplexrframework PUBLIC samplecommon_win32gl)
    target_compile_definitions(
        samplexrframework PUBLIC XR_OS_WINDOWS=1 XR_USE_PLATFORM_WIN32=1
                                    XR_USE_GRAPHICS_API_OPENGL=1
    )
endif()

# ================= OpenGL ES Based Framework ====================
# Create an interface target
add_library(samplecommon_gl INTERFACE)
# Add include directories
target_include_directories(
    samplecommon_gl
    INTERFACE
        ${1STPARTY_PATH}/OVR/Include
        ${3RDPARTY_PATH}/khronos/openxr/OpenXR-SDK/src/common
        # Extra vendor-provided headers
        ${OPENXR_PREVIEW_HEADER}
)
target_link_libraries(samplecommon_gl INTERFACE OpenXR::openxr_loader)
if(ANDROID)
    target_include_directories(samplecommon_gl INTERFACE ${ANDROID_NDK}/sources/android/native_app_glue)
    # Link the interface target to the dependency targets
    target_link_libraries(samplecommon_gl INTERFACE
        android
        EGL
        GLESv3
        log
        app_glue
    )
elseif(WIN32)
    target_compile_definitions(samplecommon_gl INTERFACE _USE_MATH_DEFINES)
    target_link_libraries(samplecommon_gl INTERFACE samplecommon_win32gl)
endif()

# ================= Android NativeActivity Based Framework ====================
if(ANDROID)
    # Create an interface target
    add_library(native_activity_framework INTERFACE)
    # Add include directories
    target_include_directories(
        native_activity_framework
        INTERFACE
            ${1STPARTY_PATH}/OVR/Include
            ${3RDPARTY_PATH}/khronos/openxr/OpenXR-SDK/src/common
            # Extra vendor-provided headers
            ${OPENXR_PREVIEW_HEADER}
            ${ANDROID_NDK}/sources/android/native_app_glue
    )
    # Link the interface target to the dependency targets
    target_link_libraries(native_activity_framework INTERFACE
        android
        EGL
        GLESv3
        log
        app_glue
        OpenXR::openxr_loader
    )
endif()
