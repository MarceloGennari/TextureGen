project(TextureGen)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

###################
# UseGLUT.cmake #
###################

IF(MSVC_IDE)
  FIND_PATH(GLUT_ROOT Readme.txt HINTS "${PROJECT_SOURCE_DIR}/freeglut")
  FIND_LIBRARY(GLUT_LIBRARY freeglut HINTS "${GLUT_ROOT}/lib/x64")
  FIND_PATH(GLUT_INCLUDE_DIR GL/glut.h HINTS "${GLUT_ROOT}/include")
ELSEIF(APPLE)
  FIND_PACKAGE(GLUT REQUIRED)
ELSEIF("${CMAKE_SYSTEM}" MATCHES "Linux")
  FIND_LIBRARY(GLUT_LIBRARY glut HINTS "/usr/lib/x86_64-linux-gnu")
  FIND_PATH(GLUT_INCLUDE_DIR glut.h HINTS "/usr/include/GL")
ENDIF()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})

add_subdirectory(Shader)
add_subdirectory(Camera)
add_subdirectory(Texture)
add_subdirectory(Mesh)
add_subdirectory(Light)
add_subdirectory(Engines)
add_subdirectory(Utils)
add_subdirectory(Teddy)
add_subdirectory(Teddy/Frames)
add_subdirectory(Teddy/Poses)
file(GLOB frames_teddy "Teddy/Frames/*.ppm")
file(GLOB poses_teddy "Teddy/Poses/*.txt")
file(GLOB frames_teddy_pgm "Teddy/Frames/*.pgm")
include_directories(Shader)
include_directories(Camera)
include_directories(Texture)
include_directories(Mesh)
include_directories(Light)
include_directories(Engines)
include_directories(Utils)
#######################
# INCLUDE_DIRECTORIES(your/header/dir)
# LINK_DIRECTORIES(your/library/dir)
# rosbuild_add_executable(kinectueye src/kinect_ueye.cpp)
# TARGET_LINK_LIBRARIES(kinectueye lib1 lib2 lib2 ...)
#
# This is how to include a .SO library
# When installing in other PC, keep that in mind and change accordingly
#########################


include_directories(/home/marcelo/Downloads/assimp-4.0.1/include)
include_directories(/home/marcelo/Downloads/assimp-4.0.1/build/include)
LINK_DIRECTORIES(/home/marcelo/Downloads/assimp-4.0.1/build/code)

set(SOURCE ${SOURCE} model.cpp model.h)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${SOURCE} ${HEADER} ${frames_teddy} ${poses_teddy} ${frames_teddy_pgm})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLUT_LIBRARY})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OPENGL_LIBRARY})

# The X11 and the pthread are included here because of the CImg library
target_link_libraries(${PROJECT_NAME} libassimp.so libassimp.so.4 X11 pthread)
