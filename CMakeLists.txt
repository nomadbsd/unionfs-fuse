project(unionfs-fuse C)

cmake_minimum_required(VERSION 2.0)
INCLUDE (CheckIncludeFiles)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

# Set a default build type for single-configuration
# CMake generators if no build type is set.
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Select flags.
SET(CMAKE_C_FLAGS "-pipe -Wall -O2 -static")
#SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -DDEBUG")

add_definitions(-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)
add_definitions(-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -D__BSD_VISIBLE=1)
option(WITH_XATTR "Enable support for extended attributes" OFF)

# .h include files
IF (WITH_XATTR)
	CHECK_INCLUDE_FILES("sys/xattr.h" HAVE_LIBC_XATTR)
	CHECK_INCLUDE_FILES("attr/xattr.h" HAVE_LIBATTR_XATTR)

	IF (HAVE_LIBC_XATTR)
		add_definitions(-DLIBC_XATTR)
	ELSEIF(HAVE_LIBATTR_XATTR)
		add_definitions(-DLIBATTR_XATTR)
	ENDIF()

	IF (NOT HAVE_LIBC_XATTR AND NOT HAVE_LIBATTR_XATTR)
		add_definitions(-DDISABLE_XATTR)
	ENDIF()
ELSE (WITH_XATTR)
	add_definitions(-DDISABLE_XATTR)
ENDIF (WITH_XATTR)

add_subdirectory(src)
add_subdirectory(man)
