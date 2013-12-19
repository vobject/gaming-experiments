CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(raycasting_1)

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default to Debug")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

OPTION(WITH_OPENCL "Enable OpenCL option for rendering." OFF)

IF (WIN32)
    ADD_DEFINITIONS(-W4)
    SET(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} ../../lib/SDL2-2.0.1/include)
    SET(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIRS} ../../lib/SDL2_image-2.0.0/include)
    SET(SDL2_GFX_INCLUDE_DIRS ${SDL2_GFX_INCLUDE_DIRS} ../../lib/SDL2_gfx-1.0.0/include)

    IF (CMAKE_CL_64)
        LINK_DIRECTORIES(../../lib/SDL2-2.0.1/lib/x64)
        LINK_DIRECTORIES(../../lib/SDL2_image-2.0.0/lib/x64)
        LINK_DIRECTORIES(../../lib/SDL2_gfx-1.0.0/lib/x64)
        
        FILE(COPY ../../lib/SDL2-2.0.1/lib/x64/SDL2.dll DESTINATION "${CMAKE_BINARY_DIR}")
        FILE(COPY ../../lib/SDL2_image-2.0.0/lib/x64/SDL2_image.dll DESTINATION "${CMAKE_BINARY_DIR}")
        FILE(COPY ../../lib/SDL2_gfx-1.0.0/lib/x64/SDL2_gfx.dll DESTINATION "${CMAKE_BINARY_DIR}")
    ELSE()
        LINK_DIRECTORIES(../../lib/SDL2-2.0.1/lib/x86)
        LINK_DIRECTORIES(../../lib/SDL2_image-2.0.0/lib/x86)
        LINK_DIRECTORIES(../../lib/SDL2_gfx-1.0.0/lib/x86)
        
        FILE(COPY ../../lib/SDL2-2.0.1/lib/x86/SDL2.dll DESTINATION "${CMAKE_BINARY_DIR}")
        FILE(COPY ../../lib/SDL2_image-2.0.0/lib/x86/SDL2_image.dll DESTINATION "${CMAKE_BINARY_DIR}")
        FILE(COPY ../../lib/SDL2_gfx-1.0.0/lib/x86/SDL2_gfx.dll DESTINATION "${CMAKE_BINARY_DIR}")
    ENDIF()
    
    SET(SDL2_LIBRARIES ${SDL2_LIBRARIES} SDL2.lib)
    SET(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARIES} SDL2_image)
    SET(SDL2_GFX_LIBRARIES ${SDL2_GFX_LIBRARIES} SDL2_gfx)
ELSE()
    ADD_DEFINITIONS(-Wall)

    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

    SET(CMAKE_CXX_FLAGS "-std=c++11")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
ENDIF()

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_GFX_INCLUDE_DIRS}
)

FILE(GLOB PRJ_HDR src/*.hpp
                  src/*/*.hpp
                  src/*/*/*.hpp)

FILE(GLOB PRJ_SRC src/*.cpp
                  src/*/*.cpp
                  src/*/*/*.cpp)

FILE(GLOB PRJ_OCL src/render/cl/cl_renderer.cl)

IF(WITH_OPENCL)
    MESSAGE(STATUS "Using OpenCL.")

    FILE(COPY "${PRJ_OCL}" DESTINATION "${CMAKE_BINARY_DIR}")

    ADD_DEFINITIONS(-DWITH_OPENCL)
    SET(EXTRA_LIBS ${EXTRA_LIBS} OpenCL)
ELSE()
    MESSAGE(STATUS "Not using OpenCL.")
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${PRJ_HDR} ${PRJ_SRC} ${PRJ_OCL} ${EXTRA_SRC})

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_GFX_LIBRARIES}
    ${EXTRA_LIBS}
)
