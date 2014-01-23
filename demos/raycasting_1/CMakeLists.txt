CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(raycasting_1)

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default to Debug")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

OPTION(WITH_TEXTURE "Enable textured rendering." OFF)
OPTION(WITH_OPENCL "Enable OpenCL option for rendering." OFF)
OPTION(WITH_SVG "Enable SVG loading option for rendering." OFF)

IF (WIN32)
    ADD_DEFINITIONS(-W4 -DSDL_MAIN_HANDLED -D_CRT_SECURE_NO_WARNINGS)
    SET(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} ../../lib/SDL2-2.0.1/include)
    SET(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIRS} ../../lib/SDL2_image-2.0.0/include)
    SET(SDL2_GFX_INCLUDE_DIRS ${SDL2_GFX_INCLUDE_DIRS} ../../lib/SDL2_gfx-1.0.0/include)

    IF (CMAKE_CL_64)
        LINK_DIRECTORIES(../../lib/SDL2-2.0.1/lib/x64)
        LINK_DIRECTORIES(../../lib/SDL2_image-2.0.0/lib/x64)
        LINK_DIRECTORIES(../../lib/SDL2_gfx-1.0.0/lib/x64)
    ELSE()
        LINK_DIRECTORIES(../../lib/SDL2-2.0.1/lib/x86)
        LINK_DIRECTORIES(../../lib/SDL2_image-2.0.0/lib/x86)
        LINK_DIRECTORIES(../../lib/SDL2_gfx-1.0.0/lib/x86)
    ENDIF()
    
    SET(SDL2_LIBRARIES ${SDL2_LIBRARIES} SDL2.lib)
    SET(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARIES} SDL2_image)
    SET(SDL2_GFX_LIBRARIES ${SDL2_GFX_LIBRARIES} SDL2_gfx)
ELSE()
    ADD_DEFINITIONS(-Wall)

    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    PKG_SEARCH_MODULE(SDL2 REQUIRED SDL2_image)
    SET(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARIES} SDL2_image)
    SET(SDL2_GFX_LIBRARIES ${SDL2_GFX_LIBRARIES} SDL2_gfx)

    SET(CMAKE_CXX_FLAGS "-std=c++1y")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
ENDIF()

SET(LUA_INCLUDE_DIR ../../lib/lua-5.2.3/src)
FILE(GLOB LUA_HDR ../../lib/lua-5.2.3/src/*.h
                  ../../lib/lua-5.2.3/src/*.hpp)
FILE(GLOB LUA_SRC ../../lib/lua-5.2.3/src/*.c)
SOURCE_GROUP(_lua-5.2.3 FILES ${LUA_HDR} ${LUA_SRC})

FILE(GLOB PRJ_HDR src/*.hpp)
FILE(GLOB PRJ_SRC src/*.cpp)
SOURCE_GROUP(src FILES ${PRJ_HDR} ${PRJ_SRC})

FILE(GLOB RENDER_HDR src/render/*.hpp
                     src/render/*/*.hpp)
FILE(GLOB RENDER_SRC src/render/*.cpp
                     src/render/*/*.cpp)
FILE(GLOB RENDER_OCL src/render/cl/*.cl)
SOURCE_GROUP(render FILES ${RENDER_HDR} ${RENDER_SRC} ${RENDER_OCL})

IF(WITH_TEXTURE)
    MESSAGE(STATUS "Enabling textured renders.")
    ADD_DEFINITIONS(-DWITH_TEXTURE)
ELSE()
    MESSAGE(STATUS "Not using textured renders.")
ENDIF()

IF(WITH_OPENCL)
    MESSAGE(STATUS "Using OpenCL.")
    ADD_DEFINITIONS(-DWITH_OPENCL)
    SET(EXTRA_LIBS ${EXTRA_LIBS} OpenCL)

    FILE(COPY "${PRJ_OCL}" DESTINATION "${CMAKE_BINARY_DIR}")
ELSE()
    MESSAGE(STATUS "Not using OpenCL.")
ENDIF()

IF(WITH_SVG)
    MESSAGE(STATUS "Using SVG.")
    ADD_DEFINITIONS(-DWITH_SVG)

    FILE(GLOB EXTRA_HDR src/klib/*.h
                        src/klib/*/*.h
                        src/klib/*/*/*.h)
    FILE(GLOB EXTRA_SRC src/klib/*.cpp
                        src/klib/*/*.cpp
                        src/klib/*/*/*.cpp)
ELSE()
    MESSAGE(STATUS "Not using SVG.")
ENDIF()

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_GFX_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR}
)

ADD_EXECUTABLE(${PROJECT_NAME}
               ${PRJ_HDR} ${PRJ_SRC}
               ${RENDER_HDR} ${RENDER_SRC} ${RENDER_OCL}
               ${LUA_HDR} ${LUA_SRC}
               ${EXTRA_HDR} ${EXTRA_SRC})

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_GFX_LIBRARIES}
    ${EXTRA_LIBS}
)
