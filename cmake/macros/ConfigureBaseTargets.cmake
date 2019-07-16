add_library(compile-option-interface INTERFACE)
set(CXX_EXTENSIONS OFF)
add_library(feature-interface INTERFACE)
target_compile_features(feature-interface
  INTERFACE
    cxx_alias_templates
    cxx_auto_type
    cxx_constexpr
    cxx_decltype
    cxx_decltype_auto
    cxx_final
    cxx_lambdas
    cxx_generic_lambdas
    cxx_variadic_templates
    cxx_defaulted_functions
    cxx_nullptr
    cxx_trailing_return_types
    cxx_return_type_deduction)
add_library(warning-interface INTERFACE)
add_library(default-interface INTERFACE)
target_link_libraries(default-interface
  INTERFACE
    compile-option-interface
    feature-interface)
add_library(no-warning-interface INTERFACE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(no-warning-interface
    INTERFACE
      /W0)
else()
  target_compile_options(no-warning-interface
    INTERFACE
      -w)
endif()

add_library(hidden-symbols-interface INTERFACE)
add_library(dependency-interface INTERFACE)
target_link_libraries(dependency-interface
  INTERFACE
    default-interface
    no-warning-interface
    hidden-symbols-interface)
add_library(core-interface INTERFACE)
target_link_libraries(core-interface
  INTERFACE
    default-interface
    warning-interface)
