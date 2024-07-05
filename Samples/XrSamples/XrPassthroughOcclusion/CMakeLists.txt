# Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

project(xrpassthroughocclusion)

if(NOT TARGET OpenXR::openxr_loader)
    find_package(OpenXR REQUIRED)
endif()

file(GLOB_RECURSE SRC_FILES
    Src/*.c
    Src/*.cpp
)

if(ANDROID)
    add_library(${PROJECT_NAME} MODULE ${SRC_FILES})
    target_link_libraries(${PROJECT_NAME} PRIVATE samplecommon_gl)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")
elseif(WIN32)
    add_executable(${PROJECT_NAME} ${SRC_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        VERBATIM)

    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_SOURCE_DIR}/SampleXrFramework/res/raw"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/font/res/raw"
        VERBATIM)
    target_link_libraries(${PROJECT_NAME} PRIVATE samplecommon_gl)
endif()

# Common across platforms
target_include_directories(${PROJECT_NAME} PRIVATE Src)
