#file(GLOB DoomSources *.c)
#file(GLOB PlatformSources platform/*.c)


include_directories(.)
include_directories(./include)
include_directories(./platform)

AUX_SOURCE_DIRECTORY(. DIR_DOOM_SRCS)
AUX_SOURCE_DIRECTORY(./platform DIR_DOOM_SRCS)

ADD_LIBRARY(doom ${DIR_DOOM_SRCS})

