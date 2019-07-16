set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_compile_definitions(compile-option-interface
  INTERFACE
    -D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")

if(PLATFORM EQUAL 32)
  target_compile_options(compile-option-interface
    INTERFACE
      -msse2
      -mfpmath=sse)
endif()
target_compile_definitions(compile-option-interface
  INTERFACE
    -DHAVE_SSE2
    -D__SSE2__)
message(STATUS "GCC: SFMT enabled, SSE2 flags forced")

if( WITH_WARNINGS )
  target_compile_options(warning-interface
    INTERFACE
      -W
      -Wall
      -Wextra
      -Winit-self
      -Winvalid-pch
      -Wfatal-errors
      -Woverloaded-virtual)

  message(STATUS "GCC: All warnings enabled")
endif()

if( WITH_COREDEBUG )
  target_compile_options(compile-option-interface
    INTERFACE
      -g3)

  message(STATUS "GCC: Debug-flags set (-g3)")
endif()
