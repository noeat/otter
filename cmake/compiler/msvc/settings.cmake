# set up output paths for executable binaries (.exe-files, and .dll-files on DLL-capable platforms)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MSVC_EXPECTED_VERSION 19.10)
set(MSVC_EXPECTED_VERSION_STRING "MSVC 2017")

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS MSVC_EXPECTED_VERSION)
  message(FATAL_ERROR "MSVC: requires version ${MSVC_EXPECTED_VERSION} (${MSVC_EXPECTED_VERSION_STRING}) to build but found ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# CMake sets warning flags by default, however we manage it manually
# for different core and dependency targets
string(REGEX REPLACE "/W[0-4] " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# Search twice, once for space after /W argument,
# once for end of line as CMake regex has no \b
string(REGEX REPLACE "/W[0-4]$" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "/W[0-4] " "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "/W[0-4]$" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

target_compile_options(warning-interface
  INTERFACE
    /W3 /std:c++latest)

if(PLATFORM EQUAL 64)
  target_compile_definitions(compile-option-interface
    INTERFACE
      -D_WIN64)
  message(STATUS "MSVC: 64-bit platform, enforced -D_WIN64 parameter")

else()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  message(STATUS "MSVC: Enabled large address awareness")

  target_compile_options(compile-option-interface
    INTERFACE
      /arch:SSE2)
  message(STATUS "MSVC: Enabled SSE2 support")

  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
  message(STATUS "MSVC: Disabled Safe Exception Handlers for debug builds")
endif()

if("${CMAKE_MAKE_PROGRAM}" MATCHES "MSBuild")
  target_compile_definitions(compile-option-interface
    INTERFACE
      -D_BUILD_DIRECTIVE="$(ConfigurationName)")
else()
  target_compile_definitions(compile-option-interface
    INTERFACE
      -D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")
endif()

target_compile_options(compile-option-interface
  INTERFACE
    /MP)

if((PLATFORM EQUAL 64) OR (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0.23026.0) OR BUILD_SHARED_LIBS)
  target_compile_options(compile-option-interface
    INTERFACE
      /bigobj)

  message(STATUS "MSVC: Enabled increased number of sections in object files")
endif()

if(NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0.23026.0))
  target_compile_options(compile-option-interface
    INTERFACE
      /Zc:throwingNew)
endif()

target_compile_definitions(compile-option-interface
  INTERFACE
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
message(STATUS "MSVC: Overloaded standard names")

target_compile_definitions(compile-option-interface
  INTERFACE
    -D_CRT_SECURE_NO_WARNINGS)
message(STATUS "MSVC: Disabled NON-SECURE warnings")

target_compile_definitions(compile-option-interface
  INTERFACE
    -D_CRT_NONSTDC_NO_WARNINGS)

message(STATUS "MSVC: Disabled POSIX warnings")

target_compile_options(compile-option-interface
  INTERFACE
    /wd4351
    /wd4091)

if(NOT WITH_WARNINGS)
  target_compile_options(compile-option-interface
    INTERFACE
      /wd4996
      /wd4355
      /wd4244
      /wd4985
      /wd4267
      /wd4619
      /wd4512)

  message(STATUS "MSVC: Disabled generic compiletime warnings")
endif()

if (BUILD_SHARED_LIBS)
  target_compile_options(compile-option-interface
    INTERFACE
      /wd4251
      /wd4275)

  message(STATUS "MSVC: Enabled shared linking")
endif()

target_compile_options(compile-option-interface
  INTERFACE
    /we4263
    /we4264)

macro(DisableIncrementalLinking variable)
  string(REGEX REPLACE "/INCREMENTAL *" "" ${variable} "${${variable}}")
  set(${variable} "${${variable}} /INCREMENTAL:NO")
endmacro()

set(CompilerFlags
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_MINSIZEREL
			CMAKE_CXX_FLAGS_RELWITHDEBINFO
			CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_MINSIZEREL
			CMAKE_C_FLAGS_RELWITHDEBINFO
			)
foreach(CompilerFlag ${CompilerFlags})
	string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
	message("MSVC flags: ${CompilerFlag}:${${CompilerFlag}}")
endforeach()

DisableIncrementalLinking(CMAKE_EXE_LINKER_FLAGS_DEBUG)
DisableIncrementalLinking(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)
DisableIncrementalLinking(CMAKE_SHARED_LINKER_FLAGS_DEBUG)
DisableIncrementalLinking(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)
