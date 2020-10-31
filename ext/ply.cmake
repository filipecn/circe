add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/ply" "ext_build/ply")
set_target_properties(ply PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
set(PLY_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/ext/ply")
set(PLY_LIBS ply)
