// Candidate VA 140422720; RVA 0x422720
// Ghidra inferred prototype: undefined FUN_140422720()

void FUN_140422720(longlong *param_1,undefined8 param_2)

{
  ulonglong uVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;

  FUN_140422860();
  uVar1 = param_1[1];
  if (uVar1 < (ulonglong)param_1[2]) {
    FUN_14030daa0(uVar1,param_2);
    param_1[1] = param_1[1] + 0x50;
    FUN_140422630(param_1);
    return;
  }
  lVar3 = (longlong)(uVar1 - *param_1) >> 4;
  if (lVar3 * -0x3333333333333333 == 0) {
    lVar8 = 1;
  }
  else {
    lVar8 = lVar3 * -0x6666666666666666;
    if (lVar8 == 0) {
      lVar4 = 0;
      lVar3 = lVar3 << 5;
      goto LAB_1404227c0;
    }
  }
  lVar3 = lVar8 * 0x50;
  lVar4 = thunk_FUN_140983da8(lVar3);
LAB_1404227c0:
  lVar5 = FUN_14033aef0(*param_1,param_1[1],lVar4);
  FUN_14030daa0(lVar5,param_2);
  lVar2 = param_1[1];
  lVar7 = *param_1;
  lVar6 = lVar3;
  if (lVar7 != lVar2) {
    do {
      FUN_14030dbd0(lVar7);
      lVar7 = lVar7 + 0x50;
    } while (lVar7 != lVar2);
    lVar6 = lVar8 * 0x50;
  }
  if ((void *)*param_1 != (void *)0x0) {
    free((void *)*param_1);
    lVar6 = lVar3;
  }
  *param_1 = lVar4;
  param_1[1] = lVar5 + 0x50;
  param_1[2] = lVar6 + lVar4;
  FUN_140422630(param_1);
  return;
}


// Incoming references
// 0xc227e4 DATA caller none
// 0xb061b4 DATA caller none
// 0xb061cc DATA caller none
// 0x42792d UNCONDITIONAL_CALL caller 1404277a0
// 0x492705 UNCONDITIONAL_CALL caller 1404925d0
// 0x30da6b UNCONDITIONAL_CALL caller 14030d940
