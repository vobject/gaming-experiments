PROJECT(rc_demo_1)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF (NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default to Debug")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(SDL REQUIRED)

INCLUDE_DIRECTORIES(
   ${X11_INCLUDE_DIR}
   ${SDL_INCLUDE_DIR}
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
                   ./Renderer.hpp
                   ./SwRenderer.hpp
                   ./ClRenderer.hpp
                   ./kernel_params.h)

FILE(GLOB SRC_LIST ./main.cpp
                   ./RcDemo.cpp
                   ./MainFrame.cpp
                   ./Level.cpp
                   ./Player.cpp
                   ./Input.cpp
                   ./SwRenderer.cpp
                   ./ClRenderer.cpp)

FILE(GLOB KRNL_LIST ./kernel.cl)

ADD_EXECUTABLE(${PROJECT_NAME} ${HDR_LIST} ${SRC_LIST} ${KRNL_LIST})

TARGET_LINK_LIBRARIES(
   ${PROJECT_NAME}
   ${X11_LIBRARIES}
   ${SDL_LIBRARY}
   OpenCL
)
