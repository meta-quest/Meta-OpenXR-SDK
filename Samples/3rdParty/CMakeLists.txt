function(find_ktx)
    # use prebuild library for Android to avoid runtime crash, further
    # investigation needed on building from ktx source
    if (ANDROID)
        set(KTX_DIR ${CMAKE_CURRENT_LIST_DIR}/khronos/ktx)

        add_library(ktx SHARED IMPORTED GLOBAL)
        set(KTXLIB ${KTX_DIR}/lib/android/${ANDROID_ABI}/libktx.so)
        set_target_properties(ktx PROPERTIES
            IMPORTED_LOCATION ${KTXLIB}
            INTERFACE_INCLUDE_DIRECTORIES ${KTX_DIR}/include)
    elseif (WIN32)
        set(KTX_DIR ${CMAKE_CURRENT_LIST_DIR}/khronos/ktx_src)
        include(FetchContent)

        FetchContent_Declare(
            libktx
            GIT_REPOSITORY  https://github.com/KhronosGroup/KTX-Software.git
            GIT_TAG         v4.2.1
            SOURCE_DIR      ${KTX_DIR}
        )

        FetchContent_GetProperties(libktx)
        if(NOT libktx_POPULATED)
            FetchContent_Populate(libktx)
        endif()

        if (WIN32)
            set(KTX_FEATURE_STATIC_LIBRARY ON CACHE BOOL "Build static ktx target to avoid DLL hell on Windows")
        endif()


        # Common compile definitions
        add_subdirectory(${libktx_SOURCE_DIR} ${libktx_BINARY_DIR})
    endif()
endfunction()

function(find_zlib)
    set(Z_DIR ${CMAKE_CURRENT_LIST_DIR}/zlib)

    include(FetchContent)

    FetchContent_Declare(
        zlib
        GIT_REPOSITORY  https://github.com/madler/zlib.git
        GIT_TAG         v1.2.13
        SOURCE_DIR      ${Z_DIR}
    )

    FetchContent_GetProperties(zlib)
    if(NOT zlib_POPULATED)
        FetchContent_Populate(zlib)
    endif()

    # Common compile definitions
    add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
endfunction()

function(find_minizip)
    # Define the source files
    set(MINIZIP_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/minizip/src/ioapi.c
        ${CMAKE_CURRENT_LIST_DIR}/minizip/src/miniunz.c
        ${CMAKE_CURRENT_LIST_DIR}/minizip/src/mztools.c
        ${CMAKE_CURRENT_LIST_DIR}/minizip/src/unzip.c
        ${CMAKE_CURRENT_LIST_DIR}/minizip/src/zip.c
    )

    # Add the library
    add_library(minizip STATIC ${MINIZIP_SRCS})

    # Add the include directories
    target_include_directories(minizip PUBLIC ${CMAKE_CURRENT_LIST_DIR}/minizip/src)

    # no-shadow doesn't exist on MSVC
    if(NOT MSVC)
        target_compile_options(minizip PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wno-shadow>
        )
    endif()

    if(ANDROID)
        target_compile_options(minizip PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wno-unused-command-line-argument;-marm;-mfpu=neon>
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-command-line-argument;-marm>
        )
        target_link_libraries(minizip z)
    elseif (WIN32)
        target_include_directories(minizip
            PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/zlib
                ${zlib_BINARY_DIR})
        target_link_libraries(minizip zlibstatic)
    endif()
endfunction()

function(find_stb)
    set(STB_SRC
        ${CMAKE_CURRENT_LIST_DIR}/stb/src/stb_image.c
        ${CMAKE_CURRENT_LIST_DIR}/stb/src/stb_image_write.c
        ${CMAKE_CURRENT_LIST_DIR}/stb/src/stb_vorbis.c
    )

    add_library(stb STATIC ${STB_SRC})

    target_include_directories(stb PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stb/src)
endfunction()

function(find_openxr_loader)
    # Pull openxr loader for windows build as there is no aar method on windows
    if(WIN32)
        include(FetchContent)

        FetchContent_Declare(
            openxr
            GIT_REPOSITORY  https://github.com/KhronosGroup/OpenXR-SDK.git
            GIT_TAG         release-1.1.51
            SOURCE_DIR      ${CMAKE_CURRENT_LIST_DIR}/openxr
        )

        FetchContent_GetProperties(openxr)
        if(NOT openxr_POPULATED)
            FetchContent_Populate(openxr)
        endif()

        # Common compile definitions
        add_subdirectory(${openxr_SOURCE_DIR} ${openxr_BINARY_DIR})
    endif()
endfunction()

find_ktx()
find_stb()

# this need to be defined before minizip since minizip depends on zlib
if (WIN32)
    find_zlib()
endif()
find_minizip()
find_openxr_loader()
