project(stlReader)
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
include_directories(Shader)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SOURCE} ${HEADER})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLUT_LIBRARY})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OPENGL_LIBRARY})
