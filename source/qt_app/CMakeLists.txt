cmake_minimum_required(VERSION 3.21.1)

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
    if(MSVC)
        add_compile_options(/Zi)
    else()
        add_compile_options(-g -O0)
    endif()
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
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets Qml Quick Concurrent)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(${CMAKE_PROJECT_NAME})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "RISC_VISIM")
qt_add_resources(${CMAKE_PROJECT_NAME} "configuration"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf)


# Deploy Qt dependencies on Windows
if(WIN32)
    # Copy qtquickcontrols2.conf to output directory
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/qtquickcontrols2.conf"
            "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/qtquickcontrols2.conf"
            COMMENT "Copying qtquickcontrols2.conf to output directory..."
    )

    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --qmldir "${CMAKE_CURRENT_SOURCE_DIR}"
                --no-compiler-runtime
                --no-translations
                "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
                COMMENT "Running windeployqt to deploy Qt dependencies..."
        )

        set(QT_QML_DIR "${Qt6_DIR}/../../../qml")
        set(OUTPUT_QML_DIR "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/qml")
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${QT_QML_DIR}/QtQuick/NativeStyle/controls"
                "${OUTPUT_QML_DIR}/QtQuick/NativeStyle/controls"
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${QT_QML_DIR}/QtQuick/NativeStyle/util"
                "${OUTPUT_QML_DIR}/QtQuick/NativeStyle/util"
                COMMENT "Copying NativeStyle QML files (Qt 6.10+ windeployqt workaround)..."
        )
    else()
        message(WARNING "windeployqt not found. Qt DLLs will not be automatically deployed.")
    endif()
endif()

add_subdirectory(../core ${CMAKE_BINARY_DIR}/core)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE RV64_SIM_CORE)

include(qds)

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
