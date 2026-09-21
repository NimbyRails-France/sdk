// Candidate VA 14055d2c0; RVA 0x55d2c0
// Ghidra inferred prototype: undefined FUN_14055d2c0()

void FUN_14055d2c0(longlong param_1)

{
  undefined1 local_88 [72];
  void *local_40;
  undefined1 local_28 [32];

  FUN_14055b7e0(local_88,param_1 + 0x18);
  *(longlong *)(param_1 + 0x208) = *(longlong *)(param_1 + 0x208) + 1;
  FUN_140567d60(param_1 + 0x90,local_88);
  FUN_140567d00(local_28);
  if (local_40 != (void *)0x0) {
    free(local_40);
  }
  return;
}


// Incoming references
// 0xc2d704 DATA caller none
// 0xa83478 DATA caller none
