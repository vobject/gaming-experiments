CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(raycasting_1)

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default to Debug")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

OPTION(WITH_OPENCL "Enable OpenCL option for rendering." ON)

ADD_DEFINITIONS(-Wall)
SET(CMAKE_CXX_FLAGS "-std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

IF(MINGW)
#    # We need to know where MinGW is installed to find the headers and
#    # libraries that are not out intside the repository.
#    # Try to reading MINGW_HOME environment variable. Set it to C:/MinGW if that fails.
#    IF(DEFINED ENV{MINGW_HOME})
#        MESSAGE("Found MinGW installation folder under: " "$ENV{MINGW_HOME}")
#    ELSE()
#       SET(ENV{MINGW_HOME} "C:/MinGW")
#       MESSAGE("Unable to find MinGW installation folder. Set default to: " "$ENV{MINGW_HOME}")
#    ENDIF()

#    # SDLDIR is used by the FIND_PACKAGE script.
#    SET(ENV{SDLDIR} "$ENV{MINGW_HOME}") # must be an environment variable
#    MESSAGE("Set SDLDIR to: " "$ENV{MINGW_HOME}")

#    FILE(COPY "$ENV{MINGW_HOME}/bin/SDL.dll" DESTINATION "${CMAKE_BINARY_DIR}")

#    # let the exe not have dependencies on external compiler DLLs.
#    SET(CMAKE_EXE_LINKER_FLAGS "-static")

#    # currently no OpenCL bindings for MinGW
#    SET(WITH_OPENCL OFF)
ENDIF()

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIRS}
)

FILE(GLOB PRJ_HDR src/*.hpp)
FILE(GLOB PRJ_SRC src/*.cpp)
FILE(GLOB PRJ_OCL cl/kernel.cl)

IF(WITH_OPENCL)
    MESSAGE(STATUS "Using OpenCL.")

    FILE(COPY "${PRJ_OCL}" DESTINATION "${CMAKE_BINARY_DIR}")

    ADD_DEFINITIONS(-DWITH_OPENCL)
    SET(EXTRA_LIBS ${EXTRA_LIBS} OpenCL)
ELSE()
    MESSAGE(STATUS "Not using OpenCL.")
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${PRJ_HDR} ${PRJ_SRC} ${PRJ_OCL})

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${EXTRA_LIBS}
)
