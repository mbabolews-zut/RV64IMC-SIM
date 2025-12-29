
cmake_minimum_required(VERSION 3.21.1)

# Set CMake policy to suppress VERSION warning in qt6_add_qml_module
cmake_policy(SET CMP0174 OLD)

# Set Qt path BEFORE project() to ensure proper detection
set(Qt6_DIR "$ENV{HOME}/Qt/6.10.1/gcc_64/lib/cmake/Qt6")
list(PREPEND CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.10.1/gcc_64")

option(LINK_INSIGHT "Link Qt Insight Tracker library" ON)
option(BUILD_QDS_COMPONENTS "Build design studio components" ON)

project(RISC_VISIM_QT LANGUAGES CXX)

# Ensure debug symbols for Debug builds
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-g -O0)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
    CACHE STRING "Import paths for Qt Creator's code model"
    FORCE
)
find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickTimeline ShaderTools Concurrent PATHS "$ENV{HOME}/Qt/6.10.1/gcc_64" NO_DEFAULT_PATH)
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

if (BUILD_QDS_COMPONENTS)
    include(qmlcomponents OPTIONAL)
endif()

if (LINK_INSIGHT)
    include(insight OPTIONAL)
endif ()

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
