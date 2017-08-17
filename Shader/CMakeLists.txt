set(targetname TextureGen)
set(SOURCE ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/GLSLSources/fragmentshader.fsh
        ${CMAKE_CURRENT_SOURCE_DIR}/GLSLSources/vertexshader.fsh
        ${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp PARENT_SCOPE)
set(HEADER ${HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/shader.h PARENT_SCOPE)

add_subdirectory(GLSLSources)
include_directories(GLSLSources)
