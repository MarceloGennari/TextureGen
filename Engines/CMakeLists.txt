set(targetname TextureGen)
set(SOURCE ${SOURCE}
         ${CMAKE_CURRENT_SOURCE_DIR}/surfacesimplificationengine.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/spatiotemporalengine.cpp
         PARENT_SCOPE
    )
set(HEADER ${HEADER}
         ${CMAKE_CURRENT_SOURCE_DIR}/textureengine.h
         PARENT_SCOPE
    )
