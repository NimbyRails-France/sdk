# Contracts that run unchanged on either host. No bridge or game process needed.
if(BUILD_TESTING)
    foreach(contract automatic_controller driving_command mod_manifest train_monitor_model train_metadata control_lease construction signal_actions signal_ui_isolation signal_ui_retirement signal_preview_diagnostics translations mod_shortcuts mod_options mod_options_codec mod_options_client)
        add_executable(nimby_${contract}_tests tests/${contract}.cpp)
        target_include_directories(nimby_${contract}_tests PRIVATE include)
        target_link_libraries(nimby_${contract}_tests PRIVATE nimby_build)
        add_test(NAME ${contract} COMMAND nimby_${contract}_tests)
        set_tests_properties(${contract} PROPERTIES TIMEOUT 30)
    endforeach()
    target_sources(nimby_train_monitor_model_tests PRIVATE src/engine/trains.cpp src/engine/live_state.cpp)
    target_sources(nimby_train_metadata_tests PRIVATE src/engine/trains.cpp src/engine/live_state.cpp)
    # Replace only the platform process connection, preserving the production
    # observation ABI and engine for latency, fault and lifetime regressions.
    add_executable(nimby_observation_isolation_tests tests/observation_isolation.cpp
        src/runtime/observation.cpp src/engine/live_state.cpp src/engine/driving.cpp
        src/engine/network.cpp src/engine/trains.cpp src/engine/signal_texture_states.cpp
        src/engine/track_usage.cpp src/engine/signal_textures.cpp)
    target_include_directories(nimby_observation_isolation_tests PRIVATE include)
    target_compile_definitions(nimby_observation_isolation_tests PRIVATE NIMBY_SDK_BUILD)
    target_link_libraries(nimby_observation_isolation_tests PRIVATE nimby_build)
    add_test(NAME observation_mod_isolation COMMAND nimby_observation_isolation_tests)
    set_tests_properties(observation_mod_isolation PROPERTIES TIMEOUT 20)
    add_executable(nimby_texture_publications_tests tests/texture_publications.cpp)
    target_include_directories(nimby_texture_publications_tests PRIVATE include)
    target_link_libraries(nimby_texture_publications_tests PRIVATE nimby_build)
    add_test(NAME texture_owner_publications COMMAND nimby_texture_publications_tests)
    add_executable(nimby_texture_batch_client_tests tests/texture_batch_client.cpp src/runtime/texture_client.cpp)
    target_include_directories(nimby_texture_batch_client_tests PRIVATE include)
    target_compile_definitions(nimby_texture_batch_client_tests PRIVATE NIMBY_SDK_BUILD)
    target_link_libraries(nimby_texture_batch_client_tests PRIVATE nimby_build)
    add_test(NAME texture_batch_client COMMAND nimby_texture_batch_client_tests)
endif()
