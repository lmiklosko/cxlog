# Extract the directory where *this* file has been installed (determined at cmake run-time)
# Get the absolute path with no ../.. relative marks, to eliminate implicit linker warnings
set(TARGET_NAME cxlog)

get_filename_component(${TARGET_NAME}_CONFIG_PATH "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(${TARGET_NAME}_INSTALL_PATH "${${TARGET_NAME}_CONFIG_PATH}/../../../" REALPATH)

add_library(${TARGET_NAME} SHARED IMPORTED)
set_target_properties(${TARGET_NAME} PROPERTIES
        IMPORTED_LOCATION "${${TARGET_NAME}_INSTALL_PATH}/lib/lib${TARGET_NAME}.so"
        INTERFACE_INCLUDE_DIRECTORIES "${${TARGET_NAME}_INSTALL_PATH}/include/${TARGET_NAME}1"
)