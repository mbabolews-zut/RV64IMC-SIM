cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0174 OLD)
project(RV64_SIM LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug symbols for Debug build
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
endif()

############################################
# MAIN APPLICATION
############################################
add_subdirectory(source/qt_app)

# Create an alias target so CLion can find 'RV64_SIM' as a run target
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
