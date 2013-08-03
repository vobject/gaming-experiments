# Locate SDL_gfx library
# This module defines
# SDLGFX_LIBRARY, the name of the library to link against
# SDLGFX_FOUND, if false, do not try to link to SDL
# SDLGFX_INCLUDE_DIR, where to find SDL/SDL.h
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Based on FindSDL_image.cmake.

FIND_PATH(SDLGFX_INCLUDE_DIR SDL_gfxPrimitives.h
  HINTS
    ENV SDLGFXDIR
    ENV SDLDIR
  PATH_SUFFIXES include/SDL include/SDL12 include/SDL11 include
)

FIND_LIBRARY(SDLGFX_LIBRARY
  NAMES SDL_gfx
  HINTS
    ENV SDLGFXDIR
    ENV SDLDIR
  PATH_SUFFIXES lib
)

SET(SDLGFX_FOUND "NO")
IF(SDLGFX_LIBRARY AND SDLGFX_INCLUDE_DIR)
  SET(SDLGFX_FOUND "YES")
ENDIF(SDLGFX_LIBRARY AND SDLGFX_INCLUDE_DIR)
