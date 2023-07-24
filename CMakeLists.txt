cmake_minimum_required(VERSION 3.10)

project(MPEG_TS C)


list(APPEND GNU_CC_STRICT_MODE_OPTIONS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic")

list(APPEND MSVC_CC_STRICT_MODE_OPTIONS
	# "/Wall"
	# "/WX"
    )

set(MPEGTS_CC_IS_KNOWN FALSE)
set(CC_STRICT_MODE_OPTIONS "")

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    set(CC_STRICT_MODE_OPTIONS ${GNU_CC_STRICT_MODE_OPTIONS})
    
    # see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
    list(APPEND CC_STRICT_MODE_OPTIONS "-Wno-missing-braces")

    set(MPEGTS_CC_IS_KNOWN TRUE)
endif()


if(DEFINED CMAKE_C_COMPILER_FRONTEND_VARIANT AND NOT ${CMAKE_C_COMPILER_FRONTEND_VARIANT} STREQUAL "")
    if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang" AND ${CMAKE_C_COMPILER_FRONTEND_VARIANT} STREQUAL "GNU")
        set(CC_STRICT_MODE_OPTIONS ${GNU_CC_STRICT_MODE_OPTIONS})
        set(MPEGTS_CC_IS_KNOWN TRUE)
    endif()
    if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang" AND ${CMAKE_C_COMPILER_FRONTEND_VARIANT} STREQUAL "MSVC")
        set(CC_STRICT_MODE_OPTIONS ${MSVC_CC_STRICT_MODE_OPTIONS})
	set(MPEGTS_CC_IS_KNOWN TRUE)
    endif()
endif()

if(MPEGTS_CC_IS_KNOWN)
    add_compile_options(${CC_STRICT_MODE_OPTIONS}) 
else()
    message(WARNING "[MpegTs]: Unknown compiler, CC_STRICT_MODE_OPTIONS set to empty")
endif()

add_subdirectory(mpegts)

option(MPEGTS_ENABLE_TESTING "Enable compiling tests for mpegts library" ON)
if (MPEGTS_ENABLE_TESTING)
    enable_testing()
    message(NOTICE "[MpegTs]: building tests")
    add_subdirectory(tests)
endif()

option(MPEGTS_BUILD_TOOLS "Enable compiling mpegts tools" ON)
if (MPEGTS_BUILD_TOOLS)
    message(NOTICE "[MpegTs]: building tools")
    add_subdirectory(examples)
endif()

