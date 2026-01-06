cmake_minimum_required(VERSION 3.21)
project(RV64_SIM LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Enable folder organization in Visual Studio/IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Windows setup
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
    add_compile_options(/MP) # Parallel compilation
endif()

include(CheckIncludeFileCXX)
check_include_file_cxx(unistd.h HAVE_UNISTD_H)

if(NOT HAVE_UNISTD_H)
    add_definitions(-DYY_NO_UNISTD_H)
endif()

############################################
# MAIN APPLICATION
############################################
add_subdirectory(source/qt_app)

# Visual studio project setup
if(TARGET RV64_SIM)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RV64_SIM)
endif()


# Create an alias target
if(TARGET RISC_VISIM_QT)
    add_custom_target(RV64_SIM_RUN
        COMMAND $<TARGET_FILE:RISC_VISIM_QT>
        DEPENDS RISC_VISIM_QT
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running RISC_VISIM application"
    )
endif()

############################################
# TESTS (excluded from default build)
############################################
enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
