set(STATIC_LINKAGE OFF CACHE BOOL "Static linkage for all binaries")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
set(CMAKE_USE_RELATIVE_PATHS TRUE)
set(CMAKE_AR ${CROSS_PREFIX}gcc-ar)
set(CMAKE_NM ${CROSS_PREFIX}gcc-nm)
set(CMAKE_RANLIB ${CROSS_PREFIX}gcc-ranlib)

include(cmake/DetectArch.cmake)
detect_arch()
message("ARCH: ${ARCH}")

set(COMMON_FLAGS "-g -Wall -Wextra -pedantic")
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-strict-overflow")
set(COMMON_FLAGS "${COMMON_FLAGS} -D_DEFAULT_SOURCE=1")
if(MINGW)
set(COMMON_FLAGS "${COMMON_FLAGS} -mno-ms-bitfields -D__USE_MINGW_ANSI_STDIO=1")
endif()
if(STATIC_LINKAGE)
	message("static linkage enabled")
	set(COMMON_FLAGS "${COMMON_FLAGS} -static -Wl,-static")
	set(CMAKE_LINK_SEARCH_END_STATIC ON)
elseif(NOT MINGW)
	set(COMMON_FLAGS "${COMMON_FLAGS} -fPIC")
endif()

set(RELEASE_FLAGS "-DNDEBUG")
set(RELEASE_FLAGS "${RELEASE_FLAGS} -D_FORTIFY_SOURCE=1")

if(MINGW)
	list(APPEND CMAKE_FIND_LIBRARY_PREFIXES "lib")
	list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	add_definitions(-D__USE_MINGW_ANSI_STDIO)
else()
set(RELEASE_FLAGS "${RELEASE_FLAGS} -Werror -pedantic-errors")
if(MIPS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -minterlink-compressed")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -minterlink-compressed")
endif()
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
set(CLANG_UNWARN "${CLANG_UNWARN} -Wno-conversion -Wno-double-promotion")
set(CLANG_UNWARN "${CLANG_UNWARN} -Wno-padded -Wno-unused-macros -Wno-float-equal")
set(CLANG_UNWARN "${CLANG_UNWARN} -Wno-disabled-macro-expansion")
set(CLANG_UNWARN "${CLANG_UNWARN} -Wno-cast-align -Wno-covered-switch-default -Wno-used-but-marked-unused")
set(CLANG_UNWARN "${CLANG_UNWARN} -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-packed -Wno-switch-enum")
set(CLANG_UNWARN "${CLANG_UNWARN} -Wno-exit-time-destructors -Wno-undefined-reinterpret-cast")
set(DEBUG_FLAGS "${DEBUG_FLAGS} -Weverything ${CLANG_UNWARN}")
set(COMMON_FLAGS "${COMMON_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-old-style-cast")
set(COMMON_FLAGS "${COMMON_FLAGS} -Wno-error=unused-private-field -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-error=unused-function")
set(COMMON_FLAGS "${COMMON_FLAGS} -Wno-vla-extension")
else() # not Clang
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-tree-vrp")
set(COMMON_FLAGS "${COMMON_FLAGS} -Wno-format-truncation")
set(COMMON_FLAGS "${COMMON_FLAGS} -D__STDC_FORMAT_MACROS=1")
set(COMMON_FLAGS "${COMMON_FLAGS} -D_GLIBCXX_USE_C99=1")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
if(STATIC_LINKAGE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -Wl,-static")
endif()
if(MINGW)
link_directories(C:/work/boost-build/lib)
else()
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
