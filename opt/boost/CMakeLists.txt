
if(POLICY CMP0074)
  # TODO:
  # 1. Find*.cmake modules need to be individually verified.
  # 2. PCLConfig.cmake needs to be changed.
  cmake_policy(SET CMP0074 OLD)
endif()

if(WIN32)
 set(BOOST_DEBUG ON)
  if(DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT $ENV{BOOST_ROOT})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0)
      set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-12.0)
    elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.10)
      set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.0)
    else()
      list(APPEND BOOST_LIBRARYDIR
        ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.1
        ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.0)
    endif()
  else()
    message(FATAL_ERROR "No BOOST_ROOT environment variable could be found! Please make sure it is set and the points to your Boost installation.")
  endif()

  set(Boost_USE_STATIC_LIBS        ON)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME     OFF)
endif()

include (CheckCXXSourceCompiles)

check_cxx_source_compiles("
  #include <regex>
  int main() { std::wregex r(L\".*\"); }"
  STD_HAS_WORKING_WREGEX)

if (STD_HAS_WORKING_WREGEX)
  find_package(Boost 1.58 REQUIRED system filesystem thread program_options)
else()
  find_package(Boost 1.58 REQUIRED system filesystem thread program_options regex)
endif()

# Find if Boost was compiled in C++03 mode because it requires -DBOOST_NO_CXX11_SCOPED_ENUMS

set(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
set(CMAKE_REQUIRED_FLAGS "-std=c++11")
check_cxx_source_compiles("
  #include <boost/filesystem/path.hpp>
  #include <boost/filesystem/operations.hpp>
  int main() { boost::filesystem::copy_file(boost::filesystem::path(), boost::filesystem::path()); }"
boost_filesystem_copy_links_without_NO_SCOPED_ENUM)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)
unset(CMAKE_REQUIRED_FLAGS)

add_library(boost INTERFACE)

target_link_libraries(boost
  INTERFACE
    ${Boost_LIBRARIES})

target_include_directories(boost
  INTERFACE
    ${Boost_INCLUDE_DIRS})

target_compile_definitions(boost
  INTERFACE
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_REGEX_NO_LIB
    -DBOOST_CHRONO_NO_LIB
    -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    -DBOOST_ASIO_NO_DEPRECATED)

if (NOT boost_filesystem_copy_links_without_NO_SCOPED_ENUM)
  target_compile_definitions(boost
    INTERFACE
      -DBOOST_NO_CXX11_SCOPED_ENUMS)
endif()

if (NOT STD_HAS_WORKING_WREGEX)
  target_compile_definitions(boost
    INTERFACE
      -DTC_HAS_BROKEN_WSTRING_REGEX)
endif()
