add_library(uipc_io SHARED)
add_library(uipc::io ALIAS uipc_io)

uipc_target_add_include_files(uipc_io)
if(DEFINED UIPC_TINYGLTF_INCLUDE_DIR AND EXISTS "${UIPC_TINYGLTF_INCLUDE_DIR}/tiny_gltf.h")
    target_include_directories(uipc_io PRIVATE "${UIPC_TINYGLTF_INCLUDE_DIR}")
else()
    find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
    if(TINYGLTF_INCLUDE_DIRS)
        target_include_directories(uipc_io PRIVATE "${TINYGLTF_INCLUDE_DIRS}")
    endif()
endif()

target_link_libraries(uipc_io PUBLIC uipc::core uipc::geometry)

target_compile_definitions(uipc_io PRIVATE UIPC_IO_EXPORT_DLL=1) # export dll

file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h" "details/*.inl")
if(UIPC_WITH_URDF_SUPPORT)
    find_package(urdfdom CONFIG REQUIRED)
    target_link_libraries(uipc_io PRIVATE
        urdfdom::urdf_parser
        urdfdom::urdfdom_model
        urdfdom::urdfdom_world
        urdfdom::urdfdom_sensor)
else()
    list(FILTER SOURCES EXCLUDE REGEX ".*[/\\\\]urdf_io\\.cpp$")
endif()
target_sources(uipc_io PRIVATE ${SOURCES})
uipc_target_set_relative_source_file(uipc_io)

uipc_target_set_output_directory(uipc_io)
uipc_target_set_rpath(uipc_io)

# ------------------------------------------------------------------------------
# setup source group for the IDE
# ------------------------------------------------------------------------------
file(GLOB_RECURSE SOURCE_GROUP_FILES CONFIGURE_DEPENDS "*.h" "*.cpp" "*.inl")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${SOURCE_GROUP_FILES})

# ------------------------------------------------------------------------------
# Install targets
# ------------------------------------------------------------------------------
uipc_install_target(uipc_io)