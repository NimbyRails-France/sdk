cmake_minimum_required(VERSION 3.24)
project(MyNimbyObserver LANGUAGES CXX)
find_package(NimbyRailsSDK 0.6.6 CONFIG REQUIRED)
add_executable(MyNimbyObserver main.cpp)
target_compile_features(MyNimbyObserver PRIVATE cxx_std_20)
target_link_libraries(MyNimbyObserver PRIVATE NimbyRailsSDK::SDK)
if(MINGW)
    target_link_options(MyNimbyObserver PRIVATE -static-libgcc -static-libstdc++)
endif()
add_custom_command(TARGET MyNimbyObserver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${NimbyRailsSDK_RUNTIME_FILES} "$<TARGET_FILE_DIR:MyNimbyObserver>"
    COMMAND_EXPAND_LISTS)
