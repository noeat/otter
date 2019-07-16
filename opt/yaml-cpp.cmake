
#yaml-cpp project
CollectSourceFiles(
${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp
YAML_PRIVATE_SRC
#Exclude
${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/test
${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/util
)
GroupSources(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)
add_library(yaml-cpp  ${YAML_PRIVATE_SRC})
CollectIncludeDirectories( 
	${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp
	YAML_PUBLIC_INCLUDES
	# Exclude
	${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/test
	${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/util
	${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/src
	${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include/yaml-cpp)
	
target_include_directories(yaml-cpp PUBLIC ${YAML_PUBLIC_INCLUDES})

target_link_libraries(yaml-cpp
  PRIVATE    
    core-interface)
	