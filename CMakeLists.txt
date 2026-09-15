cmake_minimum_required(VERSION 3.24)
project(NimbyRailsSDK VERSION 0.6.0 LANGUAGES C CXX)
if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "NimbyRailsSDK requires Windows x64")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(nimby_build INTERFACE)
target_compile_definitions(nimby_build INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
if(MSVC)
    target_compile_options(nimby_build INTERFACE /W4 /permissive- /utf-8)
else()
    target_compile_options(nimby_build INTERFACE -Wall -Wextra -Wpedantic)
endif()
add_library(nimby_minhook STATIC
    third_party/minhook/buffer.c
    third_party/minhook/hook.c
    third_party/minhook/trampoline.c
    third_party/minhook/hde/hde64.c)
target_include_directories(nimby_minhook SYSTEM PUBLIC third_party/minhook)
add_library(nimby_hook_backend STATIC src/hooks/backend.cpp)
target_include_directories(nimby_hook_backend PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(nimby_hook_backend PUBLIC nimby_minhook PRIVATE nimby_build)
add_library(NimbyRailsSDK SHARED
    src/main.cpp
    src/runtime/bootstrap.cpp
    src/runtime/runtime.cpp
    src/runtime/observation.cpp
    src/engine/live_state.cpp
    src/engine/network.cpp
    src/engine/signal_textures.cpp
    src/engine/track_usage.cpp
    src/engine/binary_identity.cpp)
target_compile_definitions(NimbyRailsSDK PRIVATE NIMBY_SDK_BUILD)
target_include_directories(NimbyRailsSDK
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<INSTALL_INTERFACE:include>")
target_link_libraries(NimbyRailsSDK PRIVATE nimby_build bcrypt nimby_hook_backend)
set_target_properties(NimbyRailsSDK PROPERTIES PREFIX "" EXPORT_NAME SDK)
add_library(NimbyRailsSDK::SDK ALIAS NimbyRailsSDK)
if(MINGW)
    # Exceptions stay inside the C ABI. GCC/C++ runtimes are embedded;
    # this MinGW toolchain still requires the shipped libwinpthread-1.dll.
    target_link_options(NimbyRailsSDK PRIVATE -static-libgcc -static-libstdc++)
    target_compile_options(nimby_hook_backend PRIVATE -fno-exceptions -fno-rtti)
    # Keep the runtime matching the selected compiler next to the SDK.
    get_filename_component(NIMBY_COMPILER_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
    if(EXISTS "${NIMBY_COMPILER_BIN}/../licenses")
        install(DIRECTORY "${NIMBY_COMPILER_BIN}/../licenses/" DESTINATION share/licenses/MinGW)
    endif()
    foreach(runtime libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
        if(EXISTS "${NIMBY_COMPILER_BIN}/${runtime}")
            add_custom_command(TARGET NimbyRailsSDK POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${NIMBY_COMPILER_BIN}/${runtime}" "$<TARGET_FILE_DIR:NimbyRailsSDK>")
            install(FILES "${NIMBY_COMPILER_BIN}/${runtime}" DESTINATION bin)
        endif()
    endforeach()
endif()
add_library(nimby_loader_core STATIC src/loader/loader.cpp src/engine/binary_identity.cpp)
target_include_directories(nimby_loader_core PUBLIC include)
target_compile_definitions(nimby_loader_core PUBLIC _WIN32_WINNT=0x0A00)
target_link_libraries(nimby_loader_core PUBLIC nimby_build bcrypt)
add_executable(NimbyRailsLoader src/loader/main.cpp)
target_link_libraries(NimbyRailsLoader PRIVATE nimby_loader_core)
if(MINGW)
    target_link_options(NimbyRailsLoader PRIVATE -municode)
endif()
add_dependencies(NimbyRailsLoader NimbyRailsSDK)
add_library(NimbyRailsProxy SHARED src/proxy/main.cpp src/proxy/SDL3.def src/engine/binary_identity.cpp)
target_include_directories(NimbyRailsProxy PRIVATE include)
target_link_libraries(NimbyRailsProxy PRIVATE nimby_build bcrypt)
set_target_properties(NimbyRailsProxy PROPERTIES PREFIX "" OUTPUT_NAME "SDL3"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/drop-in")
if(MINGW)
    target_compile_options(NimbyRailsProxy PRIVATE -fno-exceptions -fno-rtti)
endif()
add_dependencies(NimbyRailsProxy NimbyRailsSDK)
add_custom_command(TARGET NimbyRailsProxy POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:NimbyRailsSDK>" "$<TARGET_FILE_DIR:NimbyRailsProxy>")
if(MINGW)
    add_custom_command(TARGET NimbyRailsProxy POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${NIMBY_COMPILER_BIN}/libwinpthread-1.dll" "$<TARGET_FILE_DIR:NimbyRailsProxy>")
endif()
option(BUILD_TESTING "Build standalone SDK tests" ON)
add_executable(NimbyTrainMonitor WIN32 src/train_monitor.cpp src/engine/binary_identity.cpp src/engine/live_state.cpp src/engine/network.cpp)
target_include_directories(NimbyTrainMonitor PRIVATE include)
target_compile_definitions(NimbyTrainMonitor PRIVATE UNICODE _UNICODE)
target_link_libraries(NimbyTrainMonitor PRIVATE nimby_build bcrypt comctl32 gdi32)
if(MINGW)
    foreach(runtime libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
        if(EXISTS "${NIMBY_COMPILER_BIN}/${runtime}")
            add_custom_command(TARGET NimbyTrainMonitor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${NIMBY_COMPILER_BIN}/${runtime}" "$<TARGET_FILE_DIR:NimbyTrainMonitor>")
        endif()
    endforeach()
endif()
if(BUILD_TESTING)
    enable_testing()
    add_executable(nimby_signal_texture_tests tests/signal_textures.cpp src/engine/signal_textures.cpp src/engine/live_state.cpp)
    target_include_directories(nimby_signal_texture_tests PRIVATE include)
    target_link_libraries(nimby_signal_texture_tests PRIVATE nimby_build)
    add_test(NAME signal_texture_catalog COMMAND nimby_signal_texture_tests)
    add_executable(nimby_signal_state_tests tests/signal_states.cpp
        src/engine/binary_identity.cpp src/engine/live_state.cpp src/engine/network.cpp src/engine/track_usage.cpp src/engine/signal_textures.cpp)
    target_include_directories(nimby_signal_state_tests PRIVATE include)
    target_compile_definitions(nimby_signal_state_tests PRIVATE NIMBY_SDK_BUILD)
    target_link_libraries(nimby_signal_state_tests PRIVATE nimby_build bcrypt)
    add_test(NAME signal_state_contract COMMAND nimby_signal_state_tests)
    add_executable(nimby_track_usage_tests tests/track_usage.cpp src/engine/track_usage.cpp src/engine/live_state.cpp)
    target_include_directories(nimby_track_usage_tests PRIVATE include)
    target_link_libraries(nimby_track_usage_tests PRIVATE nimby_build)
    add_test(NAME track_usage_guards COMMAND nimby_track_usage_tests)
    add_executable(nimby_observation_tests tests/observation.cpp)
    target_link_libraries(nimby_observation_tests PRIVATE NimbyRailsSDK nimby_build)
    add_test(NAME public_observation_abi COMMAND nimby_observation_tests)
    add_executable(nimby_public_c_test tests/public_c.c)
    target_compile_features(nimby_public_c_test PRIVATE c_std_11)
    target_link_libraries(nimby_public_c_test PRIVATE NimbyRailsSDK)
    add_test(NAME public_c_header COMMAND nimby_public_c_test)
    add_executable(nimby_network_tests tests/network.cpp src/engine/network.cpp src/engine/live_state.cpp)
    target_include_directories(nimby_network_tests PRIVATE include)
    target_link_libraries(nimby_network_tests PRIVATE nimby_build)
    add_test(NAME network_read_guards COMMAND nimby_network_tests)
    add_executable(nimby_live_state_tests tests/live_state.cpp src/engine/live_state.cpp)
    target_include_directories(nimby_live_state_tests PRIVATE include)
    target_link_libraries(nimby_live_state_tests PRIVATE nimby_build)
    add_test(NAME live_state_resolution COMMAND nimby_live_state_tests)
    add_test(NAME train_probe_guards COMMAND NimbyTrainMonitor --self-test)
    add_executable(nimby_sdk_host tests/host.cpp)
    target_include_directories(nimby_sdk_host PRIVATE include)
    target_link_libraries(nimby_sdk_host PRIVATE nimby_build)
    add_dependencies(nimby_sdk_host NimbyRailsSDK)
    add_test(NAME dll_lifecycle COMMAND nimby_sdk_host "$<TARGET_FILE:NimbyRailsSDK>")
    add_executable(nimby_component_tests tests/components.cpp src/engine/binary_identity.cpp)
    target_include_directories(nimby_component_tests PRIVATE include)
    target_link_libraries(nimby_component_tests PRIVATE nimby_build bcrypt)
    add_test(NAME identity_and_policy COMMAND nimby_component_tests)
    add_executable(nimby_minhook_tests tests/minhook.cpp tests/hook_target.cpp)
    target_link_libraries(nimby_minhook_tests PRIVATE nimby_hook_backend nimby_build)
    add_dependencies(nimby_minhook_tests NimbyRailsSDK)
    add_test(NAME minhook_observation COMMAND nimby_minhook_tests)
    add_executable(nimby_loader_target tests/loader_target.cpp)
    target_include_directories(nimby_loader_target PRIVATE include)
    target_link_libraries(nimby_loader_target PRIVATE nimby_build nimby_minhook)
    add_executable(nimby_loader_tests tests/loader.cpp)
    target_link_libraries(nimby_loader_tests PRIVATE nimby_loader_core)
    if(MINGW)
        target_link_options(nimby_loader_target PRIVATE -municode)
        target_link_options(nimby_loader_tests PRIVATE -municode)
    endif()
    add_dependencies(nimby_loader_tests NimbyRailsSDK nimby_loader_target)
    add_test(NAME loader_restart COMMAND nimby_loader_tests "$<TARGET_FILE:nimby_loader_target>" "$<TARGET_FILE:NimbyRailsSDK>")
    set_tests_properties(loader_restart PROPERTIES TIMEOUT 60)
    add_executable(nimby_proxy_tests tests/proxy.cpp)
    target_link_libraries(nimby_proxy_tests PRIVATE nimby_build)
    if(MINGW)
        target_link_options(nimby_proxy_tests PRIVATE -municode)
    endif()
    add_dependencies(nimby_proxy_tests NimbyRailsProxy)
    # Set this to the unmodified SDL shipped with the game for integration tests.
    set(NIMBY_SDL_ORIGINAL "" CACHE FILEPATH "Original SDL3.dll for optional proxy integration tests")
    if(NIMBY_SDL_ORIGINAL AND EXISTS "${NIMBY_SDL_ORIGINAL}")
        add_custom_command(TARGET NimbyRailsProxy POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/proxy-test"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${NIMBY_SDL_ORIGINAL}" "${CMAKE_BINARY_DIR}/proxy-test/NimbyRailsSDL3Original.dll"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:NimbyRailsProxy>" "${CMAKE_BINARY_DIR}/proxy-test/SDL3.dll"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:NimbyRailsSDK>" "${CMAKE_BINARY_DIR}/proxy-test/NimbyRailsSDK.dll")
        add_test(NAME proxy_forwarding COMMAND nimby_proxy_tests "${CMAKE_BINARY_DIR}/proxy-test/SDL3.dll")
    endif()
endif()
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/NimbyRailsSDKConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsSDKConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NimbyRailsSDK)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsSDKConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(TARGETS NimbyRailsSDK EXPORT NimbyRailsSDKTargets RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
install(TARGETS NimbyRailsLoader RUNTIME DESTINATION bin)
install(EXPORT NimbyRailsSDKTargets NAMESPACE NimbyRailsSDK:: DESTINATION lib/cmake/NimbyRailsSDK)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsSDKConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsSDKConfigVersion.cmake" DESTINATION lib/cmake/NimbyRailsSDK)
install(DIRECTORY include/nimby DESTINATION include)
install(FILES third_party/minhook/LICENSE.txt DESTINATION share/licenses/MinHook)
install(FILES docs/developing.md DESTINATION share/doc/NimbyRailsSDK)
install(DIRECTORY examples/observer examples/first-observer DESTINATION share/NimbyRailsSDK/examples
    FILES_MATCHING PATTERN "*.cpp" PATTERN "CMakeLists.txt"
    PATTERN ".idea" EXCLUDE PATTERN "cmake-build-*" EXCLUDE)
install(DIRECTORY docs/ DESTINATION share/doc/NimbyRailsSDK
    PATTERN "reports" EXCLUDE PATTERN "source-reports" EXCLUDE)
install(FILES README.md DESTINATION .)
add_custom_target(sdk-dev-install
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --prefix "${CMAKE_INSTALL_PREFIX}"
    DEPENDS NimbyRailsSDK NimbyRailsLoader
    COMMENT "Install the SDK package for independent consumers (TCO)"
    VERBATIM)
