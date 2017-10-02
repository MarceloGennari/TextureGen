set(targetname TextureGen)
set(HEADER ${HEADER}
         ${CMAKE_CURRENT_SOURCE_DIR}/objects.h
         ${CMAKE_CURRENT_SOURCE_DIR}/image.h
         ${CMAKE_CURRENT_SOURCE_DIR}/image.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/imagefilters.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/imageprocessing.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/GLSL/imfragsh.fsh
         ${CMAKE_CURRENT_SOURCE_DIR}/GLSL/imvertsh.vsh
         ${CMAKE_CURRENT_SOURCE_DIR}/parse_stl.h
         ${CMAKE_CURRENT_SOURCE_DIR}/parse_stl.cpp
               PARENT_SCOPE
    )

INCLUDE_DIRECTORIES(GLSL)
add_subdirectory(GLSL)
