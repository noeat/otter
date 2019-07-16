CollectSourceFiles( 
	${CMAKE_CURRENT_SOURCE_DIR}  
	PRIVATE_SOURCES  # Exclude
	${CMAKE_CURRENT_SOURCE_DIR}/debug)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})
add_library(common  ${PRIVATE_SOURCES})
CollectIncludeDirectories( 
	${CMAKE_CURRENT_SOURCE_DIR} 
	PUBLIC_INCLUDES
	# Exclude
	${CMAKE_CURRENT_SOURCE_DIR}/sol
	${CMAKE_CURRENT_SOURCE_DIR}/spdlog
	${CMAKE_CURRENT_SOURCE_DIR}/debug)
target_include_directories(common PUBLIC ${PUBLIC_INCLUDES})
target_link_libraries(common
  PRIVATE    
    core-interface
	yaml-cpp
	boost)


