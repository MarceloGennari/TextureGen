set(targetname TextureGen)
set(HEADER ${HEADER}
         ${CMAKE_CURRENT_SOURCE_DIR}/objects.h
         ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/image.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imfragsh.fsh
        ${CMAKE_CURRENT_SOURCE_DIR}/imvertsh.vsh
                 PARENT_SCOPE
    )
