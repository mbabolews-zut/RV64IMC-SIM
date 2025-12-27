cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0174 OLD)
project(RV64_SIM LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(source/qt_app)