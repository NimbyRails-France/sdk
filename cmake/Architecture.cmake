cmake_minimum_required(VERSION 3.24)
# Source boundary check, independent of the host OS and installed toolchains.
# Selectors and the target-layout registry are the only permitted cross-platform
# includes. Platform implementation must stay in an explicit windows/linux folder.
if(NOT DEFINED SDK_ROOT)
    get_filename_component(SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
set(selectors
    include/nimby/detail/diagnostics.hpp
    include/nimby/detail/native_library.hpp
    include/nimby/detail/platform/export.h
    include/nimby/detail/platform/host.hpp
    include/nimby/detail/platform/control_pipe.hpp
    include/nimby/detail/platform/paths.hpp
    include/nimby/detail/platform/mod_abi.hpp
    include/nimby/detail/platform/mod_entry.hpp
    include/engine/game_layout.h)
# A stale public client would silently reintroduce a second consumer API.
if(EXISTS "${SDK_ROOT}/include/nimby/client.hpp")
    message(FATAL_ERROR "The retired public C++ client must not be restored; use the Kotlin API.")
endif()
file(GLOB_RECURSE sources RELATIVE "${SDK_ROOT}"
    "${SDK_ROOT}/src/*.cpp" "${SDK_ROOT}/include/*.h" "${SDK_ROOT}/include/*.hpp"
    "${SDK_ROOT}/tests/*.cpp" "${SDK_ROOT}/tests/*.hpp" "${SDK_ROOT}/tests/*.c"
    "${SDK_ROOT}/tools/*.cpp" "${SDK_ROOT}/tools/*.hpp" "${SDK_ROOT}/kotlin/native/*.cpp")
foreach(source IN LISTS sources)
    file(READ "${SDK_ROOT}/${source}" contents)
    if(contents MATCHES "nimby::Client|#[ \t]*include[^\n]*nimby/client\\.hpp")
        message(FATAL_ERROR "Retired public C++ client reference in ${source}")
    endif()
    if(source MATCHES "/(windows|linux)/" OR source IN_LIST selectors)
        continue()
    endif()
    if(contents MATCHES "#[ \t]*include[ \t]*[<\"](windows\\.h|tlhelp32\\.h|unistd\\.h|dlfcn\\.h|sys/|platform/(windows|linux)/)"
        OR contents MATCHES "#[ \t]*(if|ifdef|ifndef|elif)[^\n]*(_WIN32|__linux__)"
        OR contents MATCHES "(ReadProcessMemory|WriteProcessMemory|GetTickCount64|GetCurrentProcessId|CreateFileMappingW|dlopen|process_vm_readv)[ \t]*\\(")
        message(FATAL_ERROR "Platform dependency in shared source: ${source}. Extract the OS operation into windows/ or linux/.")
    endif()
endforeach()
message(STATUS "SDK platform boundaries verified")
