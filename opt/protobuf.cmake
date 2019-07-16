set(protobuf_WITH_ZLIB_DEFAULT OFF)
option(protobuf_WITH_ZLIB "Build with zlib support" ${protobuf_WITH_ZLIB_DEFAULT})

if (BUILD_SHARED_LIBS)
  set(protobuf_BUILD_SHARED_LIBS_DEFAULT ON)
else (BUILD_SHARED_LIBS)
  set(protobuf_BUILD_SHARED_LIBS_DEFAULT OFF)
endif (BUILD_SHARED_LIBS)
option(protobuf_BUILD_SHARED_LIBS "Build Shared Libraries" ${protobuf_BUILD_SHARED_LIBS_DEFAULT})
include(CMakeDependentOption)
cmake_dependent_option(protobuf_MSVC_STATIC_RUNTIME "Link static runtime libraries" ON
  "NOT protobuf_BUILD_SHARED_LIBS" OFF)
  
if (protobuf_BUILD_SHARED_LIBS)
  set(protobuf_SHARED_OR_STATIC "SHARED")
else (protobuf_BUILD_SHARED_LIBS)
  set(protobuf_SHARED_OR_STATIC "STATIC")
  # In case we are building static libraries, link also the runtime library statically
  # so that MSVCR*.DLL is not required at runtime.
  # https://msdn.microsoft.com/en-us/library/2kzt1wy3.aspx
  # This is achieved by replacing msvc option /MD with /MT and /MDd with /MTd
  # http://www.cmake.org/Wiki/CMake_FAQ#How_can_I_build_my_MSVC_application_with_a_static_runtime.3F
  if (MSVC AND protobuf_MSVC_STATIC_RUNTIME)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
  endif (MSVC AND protobuf_MSVC_STATIC_RUNTIME)
endif (protobuf_BUILD_SHARED_LIBS)

#set(${protobuf_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/protobuf" CACHE STRING "protobuf code path")
message(STATUS "protobuf path ${CMAKE_CURRENT_SOURCE_DIR}")
#get_filename_component(protobuf_source_dir ${protobuf_SOURCE_DIR} PATH)
set(protobuf_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/protobuf" CACHE PATH "x")
message(STATUS "protobuf path ${protobuf_source_dir}")
set(libprotobuf_files
  ${protobuf_source_dir}/src/google/protobuf/any.cc
  ${protobuf_source_dir}/src/google/protobuf/any.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/api.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/importer.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/parser.cc
  ${protobuf_source_dir}/src/google/protobuf/descriptor.cc
  ${protobuf_source_dir}/src/google/protobuf/descriptor.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/descriptor_database.cc
  ${protobuf_source_dir}/src/google/protobuf/duration.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/dynamic_message.cc
  ${protobuf_source_dir}/src/google/protobuf/empty.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/extension_set_heavy.cc
  ${protobuf_source_dir}/src/google/protobuf/field_mask.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/generated_message_reflection.cc
  ${protobuf_source_dir}/src/google/protobuf/generated_message_table_driven.cc
  ${protobuf_source_dir}/src/google/protobuf/io/gzip_stream.cc
  ${protobuf_source_dir}/src/google/protobuf/io/printer.cc
  ${protobuf_source_dir}/src/google/protobuf/io/tokenizer.cc
  ${protobuf_source_dir}/src/google/protobuf/map_field.cc
  ${protobuf_source_dir}/src/google/protobuf/message.cc
  ${protobuf_source_dir}/src/google/protobuf/reflection_ops.cc
  ${protobuf_source_dir}/src/google/protobuf/service.cc
  ${protobuf_source_dir}/src/google/protobuf/source_context.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/struct.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/mathlimits.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/substitute.cc
  ${protobuf_source_dir}/src/google/protobuf/text_format.cc
  ${protobuf_source_dir}/src/google/protobuf/timestamp.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/type.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/unknown_field_set.cc
  ${protobuf_source_dir}/src/google/protobuf/util/delimited_message_util.cc
  ${protobuf_source_dir}/src/google/protobuf/util/field_comparator.cc
  ${protobuf_source_dir}/src/google/protobuf/util/field_mask_util.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/datapiece.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/default_value_objectwriter.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/error_listener.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/field_mask_utility.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/json_escaping.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/json_objectwriter.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/json_stream_parser.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/object_writer.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/proto_writer.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/protostream_objectsource.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/protostream_objectwriter.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/type_info.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/type_info_test_helper.cc
  ${protobuf_source_dir}/src/google/protobuf/util/internal/utility.cc
  ${protobuf_source_dir}/src/google/protobuf/util/json_util.cc
  ${protobuf_source_dir}/src/google/protobuf/util/message_differencer.cc
  ${protobuf_source_dir}/src/google/protobuf/util/time_util.cc
  ${protobuf_source_dir}/src/google/protobuf/util/type_resolver_util.cc
  ${protobuf_source_dir}/src/google/protobuf/wire_format.cc
  ${protobuf_source_dir}/src/google/protobuf/wrappers.pb.cc
)

set(libprotobuf_includes
  ${protobuf_source_dir}/src/google/protobuf/any.h
  ${protobuf_source_dir}/src/google/protobuf/any.pb.h
  ${protobuf_source_dir}/src/google/protobuf/api.pb.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/importer.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/parser.h
  ${protobuf_source_dir}/src/google/protobuf/descriptor.h
  ${protobuf_source_dir}/src/google/protobuf/descriptor.pb.h
  ${protobuf_source_dir}/src/google/protobuf/descriptor_database.h
  ${protobuf_source_dir}/src/google/protobuf/duration.pb.h
  ${protobuf_source_dir}/src/google/protobuf/dynamic_message.h
  ${protobuf_source_dir}/src/google/protobuf/empty.pb.h
  ${protobuf_source_dir}/src/google/protobuf/field_mask.pb.h
  ${protobuf_source_dir}/src/google/protobuf/generated_message_reflection.h
  ${protobuf_source_dir}/src/google/protobuf/io/gzip_stream.h
  ${protobuf_source_dir}/src/google/protobuf/io/printer.h
  ${protobuf_source_dir}/src/google/protobuf/io/tokenizer.h
  ${protobuf_source_dir}/src/google/protobuf/map_field.h
  ${protobuf_source_dir}/src/google/protobuf/message.h
  ${protobuf_source_dir}/src/google/protobuf/reflection_ops.h
  ${protobuf_source_dir}/src/google/protobuf/service.h
  ${protobuf_source_dir}/src/google/protobuf/source_context.pb.h
  ${protobuf_source_dir}/src/google/protobuf/struct.pb.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/mathlimits.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/substitute.h
  ${protobuf_source_dir}/src/google/protobuf/text_format.h
  ${protobuf_source_dir}/src/google/protobuf/timestamp.pb.h
  ${protobuf_source_dir}/src/google/protobuf/type.pb.h
  ${protobuf_source_dir}/src/google/protobuf/unknown_field_set.h
  ${protobuf_source_dir}/src/google/protobuf/util/delimited_message_util.h
  ${protobuf_source_dir}/src/google/protobuf/util/field_comparator.h
  ${protobuf_source_dir}/src/google/protobuf/util/field_mask_util.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/datapiece.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/default_value_objectwriter.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/error_listener.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/field_mask_utility.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/json_escaping.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/json_objectwriter.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/json_stream_parser.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/object_writer.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/proto_writer.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/protostream_objectsource.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/protostream_objectwriter.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/type_info.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/type_info_test_helper.h
  ${protobuf_source_dir}/src/google/protobuf/util/internal/utility.h
  ${protobuf_source_dir}/src/google/protobuf/util/json_util.h
  ${protobuf_source_dir}/src/google/protobuf/util/message_differencer.h
  ${protobuf_source_dir}/src/google/protobuf/util/time_util.h
  ${protobuf_source_dir}/src/google/protobuf/util/type_resolver_util.h
  ${protobuf_source_dir}/src/google/protobuf/wire_format.h
  ${protobuf_source_dir}/src/google/protobuf/wrappers.pb.h
)

set(libprotobuf_lite_files
  ${protobuf_source_dir}/src/google/protobuf/any_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/arena.cc
  ${protobuf_source_dir}/src/google/protobuf/extension_set.cc
  ${protobuf_source_dir}/src/google/protobuf/generated_enum_util.cc
  ${protobuf_source_dir}/src/google/protobuf/generated_message_table_driven_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/generated_message_util.cc
  ${protobuf_source_dir}/src/google/protobuf/implicit_weak_message.cc
  ${protobuf_source_dir}/src/google/protobuf/io/coded_stream.cc
  ${protobuf_source_dir}/src/google/protobuf/io/io_win32.cc
  ${protobuf_source_dir}/src/google/protobuf/io/strtod.cc
  ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream.cc
   ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl.cc
  ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/message_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/parse_context.cc
  ${protobuf_source_dir}/src/google/protobuf/repeated_field.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/bytestream.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/common.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/int128.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/status.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/statusor.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/stringpiece.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/stringprintf.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/structurally_valid.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/strutil.cc
  ${protobuf_source_dir}/src/google/protobuf/stubs/time.cc
  ${protobuf_source_dir}/src/google/protobuf/wire_format_lite.cc
)

set(libprotobuf_lite_includes
  ${protobuf_source_dir}/src/google/protobuf/arena.h
  ${protobuf_source_dir}/src/google/protobuf/arenastring.h
  ${protobuf_source_dir}/src/google/protobuf/extension_set.h
  ${protobuf_source_dir}/src/google/protobuf/generated_message_util.h
  ${protobuf_source_dir}/src/google/protobuf/implicit_weak_message.h
  ${protobuf_source_dir}/src/google/protobuf/parse_context.h
  ${protobuf_source_dir}/src/google/protobuf/io/coded_stream.h
  ${protobuf_source_dir}/src/google/protobuf/io/strtod.h
  ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream.h
  ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl.h
  ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl_lite.h
  ${protobuf_source_dir}/src/google/protobuf/message_lite.h
  ${protobuf_source_dir}/src/google/protobuf/repeated_field.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/bytestream.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/common.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/int128.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/once.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/status.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/statusor.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/stringpiece.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/stringprintf.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/strutil.h
  ${protobuf_source_dir}/src/google/protobuf/stubs/time.h
  ${protobuf_source_dir}/src/google/protobuf/wire_format_lite.h
)

#message(STATUS "protobuf path ${libprotobuf_lite_files}")
add_library(libprotobuf-lite ${protobuf_SHARED_OR_STATIC}
  ${libprotobuf_lite_files} ${libprotobuf_lite_includes} )
target_link_libraries(libprotobuf-lite ${CMAKE_THREAD_LIBS_INIT} PRIVATE core-interface)
target_include_directories(libprotobuf-lite PUBLIC ${protobuf_source_dir}/src)
if(MSVC AND protobuf_BUILD_SHARED_LIBS)
  target_compile_definitions(libprotobuf-lite
    PUBLIC  PROTOBUF_USE_DLLS
    PRIVATE LIBPROTOBUF_EXPORTS)
endif()
add_library(protobuf::libprotobuf-lite ALIAS libprotobuf-lite)

add_library(libprotobuf ${protobuf_SHARED_OR_STATIC}
  ${libprotobuf_lite_files} ${libprotobuf_files} ${libprotobuf_includes})
target_link_libraries(libprotobuf ${CMAKE_THREAD_LIBS_INIT} PRIVATE core-interface)
if(protobuf_WITH_ZLIB)
    target_link_libraries(libprotobuf ${ZLIB_LIBRARIES})
endif()
target_include_directories(libprotobuf PUBLIC ${protobuf_source_dir}/src)
if(MSVC AND protobuf_BUILD_SHARED_LIBS)
  target_compile_definitions(libprotobuf
    PUBLIC  PROTOBUF_USE_DLLS
    PRIVATE LIBPROTOBUF_EXPORTS)
endif()

add_library(protobuf::libprotobuf ALIAS libprotobuf)
