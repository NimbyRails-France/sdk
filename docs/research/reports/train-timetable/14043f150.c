// Candidate VA 14043f150; RVA 0x43f150
// Ghidra inferred prototype: undefined FUN_14043f150()

longlong * FUN_14043f150(longlong *param_1,longlong *param_2,longlong param_3)

{
  longlong lVar1;
  longlong lVar2;

  if ((*(char *)(param_3 + 0x5f0) != '\0') && (*(longlong *)(param_3 + 0x5d8) != 0)) {
    lVar1 = FUN_14033f710(*param_2 + 0x280);
    if (lVar1 != 0) {
      lVar2 = FUN_1403e44d0(lVar1,*(undefined8 *)(param_3 + 0x5e0));
      if (lVar2 != 0) {
        param_1[1] = lVar2;
        *param_1 = lVar1;
        return param_1;
      }
    }
  }
  *param_1 = 0;
  param_1[1] = 0;
  return param_1;
}


// Incoming references
// 0xc22fe8 DATA caller none
// 0x44cd63 UNCONDITIONAL_CALL caller 14044cc70
// 0x44c560 UNCONDITIONAL_CALL caller 14044c390
// 0x44a71e UNCONDITIONAL_CALL caller 14044a330
// 0x44aebc UNCONDITIONAL_CALL caller 14044a330
// 0x44b04f UNCONDITIONAL_CALL caller 14044a330
// 0x44407a UNCONDITIONAL_CALL caller 140444030
// 0x447fcd UNCONDITIONAL_CALL caller 140447c50
// 0x44cec9 UNCONDITIONAL_CALL caller 14044ce00
