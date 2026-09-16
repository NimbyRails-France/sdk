
void FUN_14043e300(longlong param_1)

{
  void *_Memory;

  *(undefined1 *)(param_1 + 0x5d0) = 0;
  *(undefined1 *)(param_1 + 0x5f0) = 0;
  *(undefined1 *)(param_1 + 0x4d0) = 0;
  *(undefined1 *)(param_1 + 0x510) = 0;
  *(undefined1 *)(param_1 + 0x558) = 0;
  *(undefined1 *)(param_1 + 0x4f0) = 0;
  if (*(char *)(param_1 + 0x1d0) != '\0') {
    _Memory = *(void **)(param_1 + 0xe0);
    if ((_Memory != (void *)0x0) && (_Memory != *(void **)(param_1 + 0x100))) {
      free(_Memory);
    }
    *(undefined1 *)(param_1 + 0x1d0) = 0;
  }
  *(undefined1 *)(param_1 + 0x1f0) = 0;
  FUN_14045f560(param_1 + 0x290);
  *(undefined1 *)(param_1 + 0x608) = 0;
  *(undefined1 *)(param_1 + 0x611) = 0;
  *(undefined1 *)(param_1 + 0xb0) = 0;
  *(undefined1 *)(param_1 + 0x288) = 0;
  *(undefined1 *)(param_1 + 0x218) = 0;
  FUN_140458a00(param_1 + 0x618);
  return;
}


// ref 140c22f10 caller none
// ref 14044c8ae caller 14044c810
// ref 14044c968 caller 14044c810
// ref 14047d4ff caller 14047d410
// ref 14044b408 caller 14044a330
// ref 14044b41d caller 14044a330
// ref 1404d769a caller 1404d7590
// ref 1404d78f3 caller 1404d77f0
