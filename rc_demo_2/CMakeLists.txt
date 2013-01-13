CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(rc_demo_2)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default is Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin-dbg)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
ELSE()
    MESSAGE(FATAL_ERROR "Invalid build type. Aborting.")
ENDIF()

FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(SDL_gfx REQUIRED)

INCLUDE_DIRECTORIES(
   ${X11_INCLUDE_DIR}
   ${SDL_INCLUDE_DIR}
   ${SDLIMAGE_INCLUDE_DIR}
   ${SDLGFX_INCLUDE_DIR}
)

ADD_DEFINITIONS(
   -Wall
   -std=c++11
)

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

FILE(GLOB HDR_LIST ./RcDemo.hpp
                   ./MainFrame.hpp
                   ./Level.hpp
                   ./Player.hpp
                   ./Input.hpp
                   ./Render.hpp
                   ./ResourceCache.hpp
                   ./Ray.hpp
                   ./Slice.hpp
                   ./Utils.hpp)

FILE(GLOB SRC_LIST ./main.cpp
                   ./RcDemo.cpp
                   ./MainFrame.cpp
                   ./Level.cpp
                   ./Player.cpp
                   ./Input.cpp
                   ./Render.cpp
                   ./ResourceCache.cpp
                   ./Ray.cpp
                   ./Slice.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${HDR_LIST} ${SRC_LIST})

TARGET_LINK_LIBRARIES(
   ${PROJECT_NAME}
   ${X11_LIBRARIES}
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLGFX_LIBRARY}
)
