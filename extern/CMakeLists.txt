# ==============================================================================
# Third-party header-only targets bundled for dependency-free (no vcpkg) builds.
# Target-First: reuse parent targets if already defined (e.g. MQEngine).
# ==============================================================================

function(_uipc_define_header_only_target TARGET_NAME)
    cmake_parse_arguments(ARG "" "" "INCLUDES;LINK_LIBS" ${ARGN})
    if(TARGET "${TARGET_NAME}")
        return()
    endif()
    add_library("${TARGET_NAME}" INTERFACE IMPORTED GLOBAL)
    if(ARG_INCLUDES)
        set_property(TARGET "${TARGET_NAME}" PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ARG_INCLUDES})
    endif()
    if(ARG_LINK_LIBS)
        set_property(TARGET "${TARGET_NAME}" PROPERTY INTERFACE_LINK_LIBRARIES ${ARG_LINK_LIBS})
    endif()
endfunction()

_uipc_define_header_only_target(Eigen3::Eigen
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/eigen"
)

_uipc_define_header_only_target(nlohmann_json::nlohmann_json
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json/include"
)

_uipc_define_header_only_target(cppitertools::cppitertools
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/cppitertools"
)

_uipc_define_header_only_target(magic_enum::magic_enum
    INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include/magic_enum"
)

_uipc_define_header_only_target(igl::igl_core
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/libigl/include"
    LINK_LIBS Eigen3::Eigen
)

_uipc_define_header_only_target(Octree::Octree
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/octree"
)

set(UIPC_DYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dylib/include"
    CACHE PATH "Bundled dylib include directory" FORCE)
set(UIPC_TINYGLTF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinygltf"
    CACHE PATH "Bundled tinygltf include directory" FORCE)
