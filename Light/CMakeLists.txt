set(targetname TextureGen)
set(SOURCE ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/light.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lightvs.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/lightfs.frag PARENT_SCOPE)
set(HEADER ${HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/light.h PARENT_SCOPE)
