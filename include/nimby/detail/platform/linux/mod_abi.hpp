#pragma once
#define NRF_CALL
#define NRF_MOD_EXPORT extern "C" __attribute__((visibility("default"))) uint32_t NRF_CALL
