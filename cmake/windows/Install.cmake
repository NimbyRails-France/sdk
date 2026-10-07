# Installed consumer layout is a compatibility contract. Internal source moves
# must not change package names, exported targets or mod entry source paths.

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/NimbyRailsFranceSDKConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsFranceSDKConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NimbyRailsFranceSDK)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsFranceSDKConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(TARGETS NimbyRailsFranceSDK EXPORT NimbyRailsFranceSDKTargets RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
install(TARGETS NimbyRailsFranceLoader RUNTIME DESTINATION bin)
install(TARGETS NimbyRailsFranceClockBridge RUNTIME DESTINATION bin)
install(TARGETS nimby_mod_metadata_bridge RUNTIME DESTINATION bin)
install(EXPORT NimbyRailsFranceSDKTargets NAMESPACE NimbyRailsFranceSDK:: DESTINATION lib/cmake/NimbyRailsFranceSDK)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsFranceSDKConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NimbyRailsFranceSDKConfigVersion.cmake" DESTINATION lib/cmake/NimbyRailsFranceSDK)
install(DIRECTORY include/nimby DESTINATION include)
# CMake installation overlays an existing developer prefix. Remove this one
# retired header explicitly so upgrading the local 0.8 kit cannot resurrect it.
install(CODE "file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/include/nimby/client.hpp\")")
install(FILES src/mod/entry.cpp DESTINATION share/NimbyRailsFranceSDK/mod)
install(DIRECTORY kotlin/ DESTINATION share/NimbyRailsFranceSDK/kotlin)
install(FILES third_party/nlohmann-json-LICENSE.MIT DESTINATION share/licenses/nlohmann-json)
install(FILES third_party/windows/minhook/LICENSE.txt DESTINATION share/licenses/MinHook)
install(FILES docs/developing.md DESTINATION share/doc/NimbyRailsFranceSDK)
# Tutorials live on the wiki; installed packages contain no starter projects.
install(DIRECTORY kotlin-client DESTINATION share/NimbyRailsFranceSDK
    PATTERN ".idea" EXCLUDE PATTERN ".gradle" EXCLUDE PATTERN ".kotlin" EXCLUDE PATTERN "build" EXCLUDE)
install(DIRECTORY docs/ DESTINATION share/doc/NimbyRailsFranceSDK
    PATTERN "reports" EXCLUDE PATTERN "source-reports" EXCLUDE)
install(FILES README.md DESTINATION .)
add_custom_target(sdk-dev-install
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --prefix "${CMAKE_INSTALL_PREFIX}"
    DEPENDS NimbyRailsFranceSDK NimbyRailsFranceLoader NimbyRailsFranceModHost
    COMMENT "Install the SDK package for independent consumers (TCO)"
    VERBATIM)
