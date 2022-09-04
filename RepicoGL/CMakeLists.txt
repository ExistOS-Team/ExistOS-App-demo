
include_directories(.)

AUX_SOURCE_DIRECTORY(. DIR_PICOGL_SRCS)

ADD_LIBRARY(picogl ${DIR_PICOGL_SRCS})



