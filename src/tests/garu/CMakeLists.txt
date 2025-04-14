# Copyright (c) 2017-2025 The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(LOCAL_HEADERS
    check.h
    common.h
    d3d_common.h
    geometry.h
    graphicsapi.h
    graphicsplugin.h
    logger.h
    openxr_program.h
    options.h
    pch.h
    platformdata.h
    platformplugin.h
)
set(LOCAL_SOURCE
    d3d_common.cpp
    graphicsplugin_d3d11.cpp
    graphicsplugin_d3d12.cpp
    graphicsplugin_factory.cpp
    graphicsplugin_opengl.cpp
    graphicsplugin_opengles.cpp
    graphicsplugin_vulkan.cpp
    graphicsplugin_metal.cpp
    logger.cpp
    main.cpp
    openxr_program.cpp
    pch.cpp
    platformplugin_android.cpp
    platformplugin_factory.cpp
    platformplugin_posix.cpp
    platformplugin_win32.cpp
)
set(VULKAN_SHADERS vulkan_shaders/frag.glsl vulkan_shaders/vert.glsl)

if(ANDROID)
    add_library(
        garu MODULE
        ${LOCAL_SOURCE}
        ${LOCAL_HEADERS}
        ${VULKAN_SHADERS}
        $<TARGET_OBJECTS:android_native_app_glue>
    )
    target_link_libraries(
        garu PRIVATE ${ANDROID_LIBRARY} ${ANDROID_LOG_LIBRARY}
    )

    # Only for Android because it lacks a command line
    set(HELLOXR_DEFAULT_GRAPHICS_PLUGIN
        "OpenGLES"
        CACHE
            STRING
            "Which graphics plugin should be used by default for Hello XR artifacts?"
    )
    # These are the recognized options: provide them as a helper in the CMake GUI
    set_property(
        CACHE HELLOXR_DEFAULT_GRAPHICS_PLUGIN PROPERTY STRINGS "OpenGLES"
                                                       "Vulkan"
    )

    # Now handle the value, normally provided by Gradle
    if(HELLOXR_DEFAULT_GRAPHICS_PLUGIN STREQUAL "OpenGLES")
        message(STATUS "garu will default to OpenGL ES graphics plugin")
        target_compile_definitions(
            garu PRIVATE DEFAULT_GRAPHICS_PLUGIN_OPENGLES
        )
    elseif(HELLOXR_DEFAULT_GRAPHICS_PLUGIN STREQUAL "Vulkan")
        message(STATUS "garu will default to Vulkan graphics plugin")
        target_compile_definitions(
            garu PRIVATE DEFAULT_GRAPHICS_PLUGIN_VULKAN
        )
    endif()

else()
    add_executable(garu ${LOCAL_SOURCE} ${LOCAL_HEADERS} ${VULKAN_SHADERS})
endif()

if(APPLE)
    # use C++17 since there is a dependency on metal-cpp
    target_compile_features(garu PUBLIC cxx_std_17)
endif()

set_target_properties(garu PROPERTIES FOLDER ${SAMPLES_FOLDER})
source_group("Headers" FILES ${LOCAL_HEADERS})
source_group("Shaders" FILES ${VULKAN_SHADERS})

target_link_libraries(garu PRIVATE OpenXR::openxr_loader)

compile_glsl(run_garu_glsl_compiles ${VULKAN_SHADERS})

add_dependencies(garu run_garu_glsl_compiles)

target_include_directories(
    garu
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src/common"
        "${PROJECT_SOURCE_DIR}/src/external/metal-cpp"
        "${PROJECT_SOURCE_DIR}/src/tests/garu/vulkan_shaders"
        # for helper headers
        "${PROJECT_SOURCE_DIR}/external/include"
        # For including compiled shaders
        "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(garu PRIVATE OpenXR::openxr_loader)

if(GLSLANG_VALIDATOR AND NOT GLSLC_COMMAND)
    target_compile_definitions(garu PRIVATE USE_GLSLANGVALIDATOR)
endif()

if(XR_USE_GRAPHICS_API_VULKAN)
    target_include_directories(garu PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(garu PRIVATE ${Vulkan_LIBRARY})
endif()

if(TARGET openxr-gfxwrapper)
    target_link_libraries(garu PRIVATE openxr-gfxwrapper)
endif()
if(WIN32)
    target_link_libraries(garu PRIVATE ole32)
    if(MSVC)
        target_compile_definitions(garu PRIVATE _CRT_SECURE_NO_WARNINGS)
        target_compile_options(garu PRIVATE /Zc:wchar_t /Zc:forScope /W4)
        if(NOT
           CMAKE_CXX_COMPILER_ID
           STREQUAL
           "Clang"
        )
            # If actually msvc and not clang-cl
            target_compile_options(openxr_c_compile_test PRIVATE /WX)
        endif()

        # Right now can't build d3d features on MinGW because of directxcolors, directxmath, etc.
        target_link_libraries(
            garu
            PRIVATE
                d3d11
                d3d12
                d3dcompiler
                dxgi
        )
    endif()
endif()

if(APPLE)
    target_link_libraries(
        garu PRIVATE "-framework Foundation" "-framework CoreGraphics"
                         "-framework Metal"
    )
endif()

if(NOT ANDROID)
    install(
        TARGETS garu RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                                 COMPONENT garu
    )
    if(NOT WIN32)
        install(
            FILES garu.1
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1/
            COMPONENT ManPages
        )
    endif()
endif()
