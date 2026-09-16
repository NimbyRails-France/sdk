cmake_minimum_required(VERSION 3.24)
project(MyFirstNimbyTool LANGUAGES CXX)

find_package(NimbyRailsFranceSDK 0.7.0 CONFIG REQUIRED)
add_executable(MyFirstNimbyTool main.cpp)
target_compile_features(MyFirstNimbyTool PRIVATE cxx_std_20)
target_link_libraries(MyFirstNimbyTool PRIVATE NimbyRailsFranceSDK::SDK)
if(MINGW)
    target_link_options(MyFirstNimbyTool PRIVATE -static-libgcc -static-libstdc++)
endif()

# Deploy the DLLs from the same package as the headers and import library.
add_custom_command(TARGET MyFirstNimbyTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${NimbyRailsFranceSDK_RUNTIME_FILES} "$<TARGET_FILE_DIR:MyFirstNimbyTool>"
    COMMAND_EXPAND_LISTS)
