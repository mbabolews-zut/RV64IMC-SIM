add_subdirectory(RISC_VISIM_APP)
add_subdirectory(RISC_VISIM_APPContent)
add_subdirectory(App)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    RISC_VISIM_APPplugin
    RISC_VISIM_APPContentplugin)