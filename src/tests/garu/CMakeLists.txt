set(LOCAL_HEADERS
    check.h
    common.h
    geometry.h
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

set(OPENGL_SHADERS
)

add_executable(garu ${LOCAL_SOURCE} ${LOCAL_HEADERS} ${OPENGL_SHADERS})

set_target_properties(garu PROPERTIES FOLDER ${SAMPLES_FOLDER})
source_group("Headers" FILES ${LOCAL_HEADERS})
source_group("Shaders" FILES ${OPENGL_SHADERS})

target_link_libraries(garu PRIVATE OpenXR::openxr_loader)

target_include_directories(
    garu
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src/common"
        # for helper headers
        "${PROJECT_SOURCE_DIR}/external/include"
)

if(TARGET openxr-gfxwrapper)
    target_link_libraries(garu PRIVATE openxr-gfxwrapper)
endif()
