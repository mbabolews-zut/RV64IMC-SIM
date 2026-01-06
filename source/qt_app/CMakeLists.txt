cmake_minimum_required(VERSION 3.21.1)

# Set CMake policy to suppress VERSION warning in qt6_add_qml_module
cmake_policy(SET CMP0174 OLD)

############################################
# Qt6 PATH CONFIGURATION
############################################
# Qt6 can be found via:
#   1. CMAKE_PREFIX_PATH (set externally or via -DCMAKE_PREFIX_PATH=...)
#   2. Qt6_DIR environment variable
#   3. QT_ROOT environment variable (common with Qt installer)
#   4. System-installed Qt (Linux package managers)
#
# Example usage:
#   cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..
#   cmake -DQt6_DIR=/path/to/Qt/6.x.x/gcc_64/lib/cmake/Qt6 ..

# Check for QT_ROOT environment variable (Qt installer convention)
if(DEFINED ENV{QT_ROOT} AND NOT Qt6_DIR)
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{QT_ROOT}")
    message(STATUS "Using Qt from QT_ROOT: $ENV{QT_ROOT}")
endif()

# Check for Qt6_DIR environment variable
if(DEFINED ENV{Qt6_DIR} AND NOT Qt6_DIR)
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
    message(STATUS "Using Qt6_DIR from environment: $ENV{Qt6_DIR}")
endif()


project(RISC_VISIM_QT LANGUAGES CXX)

# Ensure debug symbols for Debug builds
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-g -O0)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
    CACHE STRING "Import paths for Qt Creator's code model"
    FORCE
)

# Find Qt6 - let CMake search standard paths
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets Qml Quick Concurrent)

qt_standard_project_setup()

qt_add_executable(${CMAKE_PROJECT_NAME})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "RISC_VISIM")
qt_add_resources(${CMAKE_PROJECT_NAME} "configuration"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf)

add_subdirectory(../core ${CMAKE_BINARY_DIR}/core)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE RV64_SIM_CORE)

include(qds)

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
