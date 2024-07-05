# Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.
# Get all directories in the current directory
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

# Loop through the list of directories
foreach(child ${children})
    # If the current item is a directory
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        # Add it as a subdirectory
        add_subdirectory(${child})
    endif()
endforeach()
