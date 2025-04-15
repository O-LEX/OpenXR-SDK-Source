set(LOCAL_HEADERS
    check.h
    common.h
    geometry.h
    graphicsapi.h
    graphicsplugin.h
    logger.h
    openxr_program.h
    options.h
    pch.h
    platformplugin.h
)
set(LOCAL_SOURCE
    graphicsplugin_opengl.cpp
    logger.cpp
    main.cpp
    openxr_program.cpp
    pch.cpp
)

add_executable(garu ${LOCAL_SOURCE} ${LOCAL_HEADERS})

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
