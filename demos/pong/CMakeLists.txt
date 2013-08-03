CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(pong)

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default to Debug")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

ADD_DEFINITIONS(-Wall)
SET(CMAKE_CXX_FLAGS "-std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

IF(MINGW)
    # We need to know where MinGW is installed to find the headers and
    # libraries that are not out intside the repository.
    # Try to reading MINGW_HOME environment variable. Set it to C:/MinGW if that fails.
    IF(DEFINED ENV{MINGW_HOME})
        MESSAGE("Found MinGW installation folder under: " "$ENV{MINGW_HOME}")
    ELSE()
       SET(ENV{MINGW_HOME} "C:/MinGW")
       MESSAGE("Unable to find MinGW installation folder. Set default to: " "$ENV{MINGW_HOME}")
    ENDIF()

    # SDLDIR is used by the FIND_PACKAGE script.
    SET(ENV{SDLDIR} "$ENV{MINGW_HOME}") # must be an environment variable
    MESSAGE("Set SDLDIR to: " "$ENV{MINGW_HOME}")

    FILE(COPY "$ENV{MINGW_HOME}/bin/SDL.dll" DESTINATION "${CMAKE_BINARY_DIR}")

    # let the exe not have dependencies on external compiler DLLs.
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
ENDIF()

SET(SDL_CONSOLE_DIR ../../lib/sdlconsole-2.1)

FIND_PACKAGE(SDL REQUIRED)

INCLUDE_DIRECTORIES(
    ${SDL_INCLUDE_DIR}
    ${SDL_CONSOLE_DIR}/include
)

FILE(GLOB SDL_CONSOLE_HDR ${SDL_CONSOLE_DIR}/include/*.h)
FILE(GLOB SDL_CONSOLE_SRC ${SDL_CONSOLE_DIR}/src/*.c)

FILE(GLOB PRJ_HDR src/*.hpp)
FILE(GLOB PRJ_SRC src/*.cpp)

ADD_EXECUTABLE(
    ${PROJECT_NAME}
    ${PRJ_HDR} ${PRJ_SRC}
    ${SDL_CONSOLE_HDR} ${SDL_CONSOLE_SRC}
)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ${SDL_LIBRARY}
)

FILE(COPY "res/font.bmp" DESTINATION "${CMAKE_BINARY_DIR}")
