# Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

project(xrspacewarp)

if(NOT TARGET OpenXR::openxr_loader)
    find_package(OpenXR REQUIRED)
endif()

if(ANDROID)
    file(GLOB_RECURSE SRC_FILES
        Src/*.c
        Src/*.cpp
    )

    add_library(${PROJECT_NAME} MODULE ${SRC_FILES})
    target_link_libraries(${PROJECT_NAME} PRIVATE native_activity_framework)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")

    # Common across platforms
    target_include_directories(${PROJECT_NAME} PRIVATE Src)
endif()
