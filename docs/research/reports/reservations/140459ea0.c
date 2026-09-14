// Candidate VA 140459ea0; RVA 0x459ea0
// Ghidra inferred prototype: undefined FUN_140459ea0()

longlong * FUN_140459ea0(longlong *param_1,longlong *param_2,ulonglong param_3)

{
  ulonglong uVar1;
  ulonglong *puVar2;
  longlong lVar3;
  
  param_2[3] = 0;
  param_2[4] = 0;
  param_2[5] = 0;
  *param_2 = (longlong)param_1;
  param_2[1] = param_3;
  uVar1 = (param_3 >> 0x21 ^ param_3) * -0xae502812aa7333;
  uVar1 = (uVar1 >> 0x21 ^ uVar1) * -0x3b314601e57a13ad;
  uVar1 = uVar1 >> 0x21 ^ uVar1;
  param_2[2] = uVar1;
  uVar1 = (param_1[1] - *param_1 >> 3) - 1U & uVar1;
  param_2[2] = uVar1;
  lVar3 = *param_1;
  if (param_1[1] - lVar3 >> 3 != 0) {
    do {
      uVar1 = (param_1[1] - lVar3 >> 3) - 1U & uVar1;
      param_2[2] = uVar1;
      uVar1 = *(ulonglong *)(*param_1 + uVar1 * 8);
      param_3 = param_2[1];
      if ((uVar1 == param_3) || (uVar1 == 0)) break;
      param_2[2] = param_2[2] + 1;
      param_1 = (longlong *)*param_2;
      uVar1 = param_2[2];
      lVar3 = *param_1;
    } while (param_1[1] != lVar3);
  }
  lVar3 = *param_2;
  puVar2 = *(ulonglong **)
            (*(longlong *)(lVar3 + 0x38) + (param_3 % (ulonglong)*(uint *)(lVar3 + 0x40)) * 8);
  if (puVar2 != (ulonglong *)0x0) {
    while (param_3 != *puVar2) {
      puVar2 = (ulonglong *)puVar2[4];
      if (puVar2 == (ulonglong *)0x0) {
        return param_2;
      }
    }
    if (puVar2 != *(ulonglong **)(*(longlong *)(lVar3 + 0x38) + *(longlong *)(lVar3 + 0x40) * 8)) {
      param_2[3] = puVar2[1];
      param_2[4] = puVar2[2];
    }
  }
  return param_2;
}


// Incoming references
// 0x43ffdc UNCONDITIONAL_CALL caller 14043fde0
// 0x44307b UNCONDITIONAL_CALL caller 140442f40
// 0x4464bf UNCONDITIONAL_CALL caller 140446390
// 0x4516c8 UNCONDITIONAL_CALL caller 1404515c0
// 0x449799 UNCONDITIONAL_CALL caller 140449700
// 0x4582f8 UNCONDITIONAL_CALL caller 1404582d0
// 0x47ac93 UNCONDITIONAL_CALL caller 14047a5f0
// 0x457d71 UNCONDITIONAL_CALL caller 140457d40
// 0x4525f2 UNCONDITIONAL_CALL caller 140452480
// 0x4527c5 UNCONDITIONAL_CALL caller 1404526b0
// 0x452bb4 UNCONDITIONAL_CALL caller 140452aa0
// 0x4697dc UNCONDITIONAL_CALL caller 140469720
// 0x46a8f4 UNCONDITIONAL_CALL caller 14046a8a0
// 0x6ebed3 UNCONDITIONAL_CALL caller 1406eb6d0
