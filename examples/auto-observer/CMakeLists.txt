cmake_minimum_required(VERSION 3.24)
project(MyNimbyClient LANGUAGES CXX)
find_package(NimbyRailsFranceSDK 0.7.0 CONFIG REQUIRED)
add_executable(MyNimbyClient main.cpp)
target_compile_features(MyNimbyClient PRIVATE cxx_std_20)
target_link_libraries(MyNimbyClient PRIVATE NimbyRailsFranceSDK::SDK)
if(MINGW)
    target_link_options(MyNimbyClient PRIVATE -static-libgcc -static-libstdc++)
endif()
add_custom_command(TARGET MyNimbyClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${NimbyRailsFranceSDK_RUNTIME_FILES} "$<TARGET_FILE_DIR:MyNimbyClient>"
    COMMAND_EXPAND_LISTS)
