target_compile_definitions(compile-option-interface
  INTERFACE
    -D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")

if(WITH_WARNINGS)
  target_compile_options(warning-interface
    INTERFACE
      -W
      -Wall
      -Wextra
      -Winit-self
      -Wfatal-errors
      -Wno-mismatched-tags
      -Woverloaded-virtual)

  message(STATUS "Clang: All warnings enabled")
endif()

if(WITH_COREDEBUG)
  target_compile_options(compile-option-interface
    INTERFACE
      -g3)

  message(STATUS "Clang: Debug-flags set (-g3)")
endif()

target_compile_options(compile-option-interface
  INTERFACE
    -Wno-narrowing
    -Wno-deprecated-register)

if (BUILD_SHARED_LIBS)
  target_compile_options(compile-option-interface
    INTERFACE
      -fPIC)

  target_compile_options(hidden-symbols-interface
    INTERFACE
      -fvisibility=hidden)

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --no-undefined")
  message(STATUS "Clang: Disallow undefined symbols")
endif()
