
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

project(uzi C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CheckCmakeCompat)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckStructHasMember)
include(FindPkgConfig)
include(TestBigEndian)
include(FindFeature)
include(ConfigOptions)
include(ComplexLibrary)
include(FeatureSummary)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(GNUInstallDirsWrapper)
include(CMakePackageConfigHelpers)
include(CheckCLinkerFlag)

file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" VERSION_STRINGS)
list(GET VERSION_STRINGS 0 VERSION_STRING)

string(REGEX REPLACE "([0-9]+).[0-9]+.[0-9]+" "\\1" UZI_VERSION_MAJOR ${VERSION_STRING})
string(REGEX REPLACE "[0-9]+.([0-9]+).[0-9]+" "\\1" UZI_VERSION_MINOR ${VERSION_STRING})
string(REGEX REPLACE "[0-9]+.[0-9]+.([0-9]+)" "\\1" UZI_VERSION_PATCH ${VERSION_STRING})
set(UZI_VERSION "${UZI_VERSION_MAJOR}.${UZI_VERSION_MINOR}.${UZI_VERSION_PATCH}")

set(UZI_NAME "UZI")
set(UZI_VENDOR "Devolutions Inc.")
set(UZI_COPYRIGHT "Copyright 2018 ${UZI_VENDOR} All Rights Reserved.")

message(STATUS "${UZI_NAME} Version ${UZI_VERSION}")

if(CMAKE_TOOLCHAIN_FILE)
	message(STATUS "Using toolchain file ${CMAKE_TOOLCHAIN_FILE}")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if(NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(ANDROID)
	list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH})
	list(REMOVE_DUPLICATES CMAKE_FIND_ROOT_PATH)
elseif(IOS)
	list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH})
	list(REMOVE_DUPLICATES CMAKE_FIND_ROOT_PATH)
endif()

if(APPLE)
	if(NOT DEFINED CMAKE_OSX_ARCHITECTURES OR "${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
		set(CMAKE_OSX_ARCHITECTURES "x86_64")
		if(IOS)
			list(APPEND CMAKE_OSX_ARCHITECTURES "armv7;armv7s;arm64")
		endif()
	endif()
endif()

if(MSVC)
	include(MSVCRuntime)
	if(NOT DEFINED MSVC_RUNTIME AND NOT WAYK_UWP)
		set(MSVC_RUNTIME "static")
	endif()
	configure_msvc_runtime()

	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

	if(NOT DEFINED CMAKE_SUPPRESS_REGENERATION)
		set(CMAKE_SUPPRESS_REGENERATION ON)
	endif()
endif()

# Compiler-specific flags
if(CMAKE_COMPILER_IS_GNUCC)
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
		CHECK_SYMBOL_EXISTS(__x86_64__ "" IS_X86_64)
		if(IS_X86_64)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
		else()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=i686")
		endif()
	else()
		if(CMAKE_POSITION_INDEPENDENT_CODE)
			if(${CMAKE_VERSION} VERSION_LESS 2.8.9)
				set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
			endif()
		endif()
	endif()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

	CHECK_C_COMPILER_FLAG(-Wno-unused-result Wno-unused-result)
	if(Wno-unused-result)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")
	endif()
	CHECK_C_COMPILER_FLAG(-Wno-unused-but-set-variable Wno-unused-but-set-variable)
	if(Wno-unused-but-set-variable)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")
	endif()
	CHECK_C_COMPILER_FLAG(-Wno-deprecated-declarations Wno-deprecated-declarations)
	if(Wno-deprecated-declarations)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
	endif()
	CHECK_CXX_COMPILER_FLAG(-Wno-deprecated-declarations Wno-deprecated-declarationsCXX)
	if(Wno-deprecated-declarationsCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
	endif()
	CHECK_CXX_COMPILER_FLAG(-Wno-reorder Wno-reorderCXX)
	if(Wno-reorderCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
	endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

	if(BUILD_TESTING)
		CHECK_C_COMPILER_FLAG(-Wno-format Wno-format)
		if(Wno-format)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format")
		endif()
	endif()
	CHECK_C_COMPILER_FLAG (-Werror-implicit-function-declaration Werror-implicit-function-declaration)
	if(Wimplicit-function-declaration)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration")
	endif()

	CHECK_C_COMPILER_FLAG (-Wredundant-decls Wredundant-decls)
	if(Wredundant-decls)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wredundant-decls")
	endif()

	if(WITH_SSE2)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
	endif()

	set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
	set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-macros -Wno-padded")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-c11-extensions -Wno-gnu")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")
	if(WITH_SSE2)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mssse3")
	endif()
	CHECK_C_COMPILER_FLAG(-Wno-deprecated-declarations Wno-deprecated-declarations)
	if(Wno-deprecated-declarations)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
	endif()
	CHECK_CXX_COMPILER_FLAG(-Wno-deprecated-declarations Wno-deprecated-declarationsCXX)
	if(Wno-deprecated-declarationsCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
	endif()
	CHECK_CXX_COMPILER_FLAG(-Wno-reorder Wno-reorderCXX)
	if(Wno-reorderCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
	endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if(ANDROID)
	# workaround for https://github.com/android-ndk/ndk/issues/243
	string(REPLACE "-g " "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
	string(REPLACE "-g " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

if(MSVC)
	# Remove previous warning definitions,
	# NMake is otherwise complaining.
	foreach (flags_var_to_scrub
		CMAKE_C_FLAGS
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_C_FLAGS_MINSIZEREL)
		string (REGEX REPLACE "(^| )[/-]W[ ]*[1-9]" " "
		"${flags_var_to_scrub}" "${${flags_var_to_scrub}}")
	endforeach()

	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
	set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

	set(C_FLAGS "/W3 /wd4996 /Gd")

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(C_FLAGS "${C_FLAGS} -D_AMD64_")
	else()
		set(C_FLAGS "${C_FLAGS} -D_X86_")
	endif()

	# https://cloudblogs.microsoft.com/microsoftsecure/2012/06/06/warnings-sdl-and-improving-uninitialized-variable-detection/
	if(WITH_SDL_CHECKS)
		set(C_FLAGS "${C_FLAGS} /sdl")
	endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

	set(C_FLAGS_RELEASE "/Zi /GF")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${C_FLAGS_RELEASE}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${C_FLAGS_RELEASE}")

	# http://devcenter.wintellect.com/jrobbins/correctly-creating-native-c-release-build-pdbs
	set(LINKER_FLAGS_RELEASE "/DEBUG")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /MAP /MAPINFO:EXPORTS")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} ${LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)
endif()

if(WIN32)
	set(C_FLAGS "")
	set(C_FLAGS "${C_FLAGS} -D_UNICODE")
	set(C_FLAGS "${C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
	set(C_FLAGS "${C_FLAGS} -DWIN32_LEAN_AND_MEAN")
	set(C_FLAGS "${C_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")

	if(NOT DEFINED CMAKE_WINDOWS_VERSION)
		set(CMAKE_WINDOWS_VERSION "WINXP")
	endif()

	if(CMAKE_WINDOWS_VERSION STREQUAL "WINXP")
		set(C_FLAGS "${C_FLAGS} -DWINVER=0x0501 -D_WIN32_WINNT=0x0501")
	elseif(CMAKE_WINDOWS_VERSION STREQUAL "WIN7")
		set(C_FLAGS "${C_FLAGS} -DWINVER=0x0601 -D_WIN32_WINNT=0x0601")
	elseif(CMAKE_WINDOWS_VERSION STREQUAL "WIN8")
		set(C_FLAGS "${C_FLAGS} -DWINVER=0x0602 -D_WIN32_WINNT=0x0602")
	elseif(CMAKE_WINDOWS_VERSION STREQUAL "WIN10")
		set(C_FLAGS "${C_FLAGS} -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00")
	endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")

	set(HAVE_FCNTL_H 1 CACHE INTERNAL "")
	set(HAVE_STDINT_H 1 CACHE INTERNAL "")
	set(HAVE_INTTYPES_H 1 CACHE INTERNAL "")
	set(HAVE_SYS_SELECT_H "" CACHE INTERNAL "")
	set(HAVE_SYS_STRTIO_H "" CACHE INTERNAL "")
	set(HAVE_MATH_C99_LONG_DOUBLE "" CACHE INTERNAL "")
	set(HAVE_TM_GMTOFF "" CACHE INTERNAL "")
	set(UZI_HAVE_STDBOOL_H 1 CACHE INTERNAL "")
	set(UZI_HAVE_STDINT_H 1 CACHE INTERNAL "")
	set(UZI_HAVE_INTTYPES_H 1 CACHE INTERNAL "")
endif()

if(IOS)
	set(HAVE_FCNTL_H 1)
	set(HAVE_STDINT_H 1)
	set(HAVE_INTTYPES_H 1)
	set(HAVE_SYS_SELECT_H 1)
	set(HAVE_POLL_H 1)
	set(HAVE_UNISTD_H 1)
	set(HAVE_EXECINFO_H 1)
	set(HAVE_SYS_FILIO_H 1)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/out/library)
else()
	check_include_files(fcntl.h HAVE_FCNTL_H)
	check_include_files(stdint.h HAVE_STDINT_H)
	check_include_files(inttypes.h HAVE_INTTYPES_H)
	check_include_files(sys/select.h HAVE_SYS_SELECT_H)
	check_include_files(sys/strtio.h HAVE_SYS_STRTIO_H)

	if(NOT WIN32)
		check_include_files(poll.h HAVE_POLL_H)
		check_include_files(unistd.h HAVE_UNISTD_H)
		check_include_files(execinfo.h HAVE_EXECINFO_H)
		check_include_files(sys/modem.h HAVE_SYS_MODEM_H)
		check_include_files(sys/filio.h HAVE_SYS_FILIO_H)
		if(NOT APPLE)
			check_include_files(sys/timerfd.h HAVE_TIMERFD_H)
			check_include_files(sys/eventfd.h HAVE_AIO_H)
			check_include_files(sys/eventfd.h HAVE_EVENTFD_H)
		endif()
	endif()
endif()

if(HAVE_EVENTFD_H)
	check_symbol_exists(eventfd_read sys/eventfd.h WITH_EVENTFD_READ_WRITE)
endif()

list(APPEND CMAKE_REQUIRED_LIBRARIES m)
check_symbol_exists(ceill math.h HAVE_MATH_C99_LONG_DOUBLE)
list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES m)

if(ANDROID)
	set(HAVE_MATH_C99_LONG_DOUBLE 0)
endif()

check_struct_has_member("struct tm" tm_gmtoff time.h HAVE_TM_GMTOFF)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

if(NOT IOS)
	find_package(Threads REQUIRED)
endif()

if(NOT WIN32)
	list(APPEND CMAKE_REQUIRED_LIBRARIES pthread)
	check_symbol_exists(pthread_mutex_timedlock pthread.h HAVE_PTHREAD_MUTEX_TIMEDLOCK)
	list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES pthread)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions("-DHAVE_CONFIG_H")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if(CMAKE_SKIP_RPATH)
       set(CMAKE_SKIP_RPATH FALSE)
       set(CMAKE_SKIP_INSTALL_RPATH TRUE)
endif()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}:\$ORIGIN/..")

include(CTest)

if(BUILD_TESTING)
	enable_testing()

	if(MSVC)
		set(TESTING_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
	else()
		set(TESTING_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testing")
	endif()
endif()

# libuzi

set(UZI_VERSION_MAJOR "2")
set(UZI_VERSION_MINOR "0")
set(UZI_VERSION_REVISION "0")
set(UZI_VERSION "${UZI_VERSION_MAJOR}.${UZI_VERSION_MINOR}")
set(UZI_VERSION_FULL "${UZI_VERSION}.${UZI_VERSION_REVISION}")

if(NOT IOS)
	check_include_files(stdbool.h UZI_HAVE_STDBOOL_H)
	check_include_files(stdint.h UZI_HAVE_STDINT_H)
	check_include_files(inttypes.h UZI_HAVE_INTTYPES_H)
else()
	set(UZI_HAVE_STDBOOL_H 1)
	set(UZI_HAVE_STDINT_H 1)
	set(UZI_HAVE_INTTYPES_H 1)
endif()

set(UZI_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(UZI_BINARY_DIR "${CMAKE_SOURCE_DIR}")

set(UZI_INCLUDE_SOURCE_DIR "${UZI_SOURCE_DIR}/include")
set(UZI_INCLUDE_BINARY_DIR "${UZI_BINARY_DIR}/include")

include_directories(${UZI_INCLUDE_SOURCE_DIR})
include_directories(${UZI_INCLUDE_BINARY_DIR})

file(GLOB UZI_HDRS "${UZI_INCLUDE_SOURCE_DIR}/uzi/*.h")

set(UZI_REL_DIR "${CMAKE_SOURCE_DIR}")
set(UZI_SRCS "")
set(UZI_LIBS "")
set(UZI_INCLUDES "")
set(UZI_DEFINITIONS "")

macro(uzi_module_add)
	file(RELATIVE_PATH _relPath "${UZI_REL_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_src ${ARGN})
		if(_relPath)
			list(APPEND UZI_SRCS "${_relPath}/${_src}")
		else()
			list(APPEND UZI_SRCS "${_src}")
		endif()
	endforeach()
	if(_relPath)
		set(UZI_SRCS ${UZI_SRCS} PARENT_SCOPE)
	endif()
endmacro()

macro(uzi_include_directory_add)
	file(RELATIVE_PATH _relPath "${UZI_REL_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_inc ${ARGN})
		if(IS_ABSOLUTE ${_inc})
			list(APPEND UZI_INCLUDES "${_inc}")
		else()
			if (_relPath)
				list(APPEND UZI_INCLUDES "${_relPath}/${_inc}")
			else()
				list(APPEND UZI_INCLUDES "${_inc}")
			endif()
		endif()
	endforeach()
	if(_relPath)
		set(UZI_INCLUDES ${UZI_INCLUDES} PARENT_SCOPE)
	endif()
endmacro()

macro(uzi_library_add)
	foreach(_lib ${ARGN})
		list(APPEND UZI_LIBS "${_lib}")
	endforeach()
	set(UZI_LIBS ${UZI_LIBS} PARENT_SCOPE)
endmacro()

macro(uzi_definition_add)
	foreach(_define ${ARGN})
		list(APPEND UZI_DEFINITIONS "${_define}")
	endforeach()
	set(UZI_DEFINITIONS ${UZI_DEFINITIONS} PARENT_SCOPE)
endmacro()

set(UZI_MODULES api core handle error file interlocked library synch sysinfo thread utils winsock)

foreach(UZI_MODULE ${UZI_MODULES})
	add_subdirectory(${UZI_SOURCE_DIR}/libuzi/${UZI_MODULE} ${UZI_BINARY_DIR}/libuzi/${UZI_MODULE})
endforeach()

list(REMOVE_DUPLICATES UZI_DEFINITIONS)
list(REMOVE_DUPLICATES UZI_LIBS)
list(REMOVE_DUPLICATES UZI_INCLUDES)
include_directories(${UZI_INCLUDES})

add_library(uzi ${UZI_SRCS} ${UZI_HDRS})

add_definitions(-DUZI_EXPORTS)

add_definitions(${UZI_DEFINITIONS})
target_link_libraries(uzi ${UZI_LIBS})

include(CTest)

if(IOS)
	set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

