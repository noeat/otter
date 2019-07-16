project(otter CXX  C)
CollectSourceFiles(
${CMAKE_CURRENT_SOURCE_DIR}
OTTER_PRIVATE_SRC
#Exclude
)

if( WIN32 )
  list(APPEND OTTER_PRIVATE_SRC ${sources_windows})
endif()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(otter  ${OTTER_PRIVATE_SRC})
target_link_libraries(otter
  PRIVATE
    core-interface
  PUBLIC
    common
	lua_interface
	boost)
	