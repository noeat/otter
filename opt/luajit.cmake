find_library(LUAJIT_LIBRARY lua51 libluajit PATH ${CMAKE_CURRENT_SOURCE_DIR}/luajit/lib)
add_library(lua_interface INTERFACE)
target_include_directories(lua_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/luajit/include)
target_link_libraries(lua_interface INTERFACE ${LUAJIT_LIBRARY})