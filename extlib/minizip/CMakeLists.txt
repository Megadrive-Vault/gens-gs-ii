PROJECT(minizip)
cmake_minimum_required(VERSION 2.6.0)

# Sources.
SET(minizip_SRCS
	unzip.c
	zip.c
	ioapi.c
	)

# Platform-specific sources.
IF(WIN32)
	SET(minizip_PLATFORM_SRCS iowin32.c iowin32u.c)
ENDIF(WIN32)

# ZLIB include directory.
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

# Build the library.
ADD_LIBRARY(minizip STATIC ${minizip_SRCS} ${minizip_PLATFORM_SRCS})
TARGET_LINK_LIBRARIES(minizip ${ZLIB_LIBRARY})