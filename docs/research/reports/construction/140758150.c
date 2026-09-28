
void FUN_140758150(longlong param_1)

{
  if (*(char *)(param_1 + 600) != '\0') {
    FUN_140002d30();
  }
  FUN_1407580f0(param_1 + 0x1e8);
  FUN_1402531c0(param_1 + 0x1a0);
  if (1 < *(ulonglong *)(param_1 + 0x1b0)) {
    free(*(void **)(param_1 + 0x1a8));
  }
  if (*(void **)(param_1 + 0x188) != (void *)0x0) {
    free(*(void **)(param_1 + 0x188));
  }
  FUN_140002d30(param_1 + 0x160);
  FUN_140582130(param_1 + 0x30);
  return;
}

