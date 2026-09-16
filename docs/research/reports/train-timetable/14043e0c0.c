// Candidate VA 14043e0c0; RVA 0x43e0c0
// Ghidra inferred prototype: undefined FUN_14043e0c0()

longlong FUN_14043e0c0(longlong param_1,longlong param_2,longlong param_3)

{
  float fVar1;

  if ((((9 < *(int *)(param_1 + 0x1e0)) && (DAT_140aac19c <= *(float *)(param_1 + 0x1dc))) &&
      (DAT_140aabae4 <= *(float *)(param_1 + 0x1e8))) &&
     ((param_2 == 0 ||
      ((param_2 != *(longlong *)(param_1 + 0x30) && (param_2 != *(longlong *)(param_1 + 0x48)))))))
  {
    fVar1 = (*(float *)(param_1 + 0x1dc) - *(float *)(param_1 + 0x1e4)) /
            *(float *)(param_1 + 0x1e8);
    if (fVar1 <= 0.0) {
      fVar1 = 0.0;
    }
    return ((longlong)fVar1 * 1000000 - *(longlong *)(param_1 + 0x1f8)) + param_3;
  }
  return 0;
}


// Incoming references
// 0x448a4d UNCONDITIONAL_CALL caller 140448710
// 0x480b6a UNCONDITIONAL_CALL caller 140480970
// 0x58c918 UNCONDITIONAL_CALL caller 14058c400
// 0x6b8486 UNCONDITIONAL_CALL caller 1406b7c00
