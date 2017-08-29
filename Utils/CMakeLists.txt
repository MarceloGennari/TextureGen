set(targetname TextureGen)
set(HEADER ${HEADER}
         ${CMAKE_CURRENT_SOURCE_DIR}/objects.h
         ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
         ${CMAKE_CURRENT_SOURCE_DIR}/image.h
         ${CMAKE_CURRENT_SOURCE_DIR}/image.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/imagefilters.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/imageprocessing.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/GLSL/imfragsh.fsh
         ${CMAKE_CURRENT_SOURCE_DIR}/GLSL/imvertsh.vsh
                 PARENT_SCOPE
    )

INCLUDE_DIRECTORIES(GLSL)
add_subdirectory(GLSL)
