// Candidate VA 14055c420; RVA 0x55c420
// Ghidra inferred prototype: undefined FUN_14055c420()

void FUN_14055c420(longlong param_1)

{
  longlong lVar1;
  uint uVar2;
  int iVar3;
  float fVar4;
  undefined1 in_XMM1 [16];
  undefined1 auVar5 [16];
  undefined1 auVar6 [16];
  undefined1 auVar7 [16];

  if (*(char *)(param_1 + 0x4c) == '\0') {
    iVar3 = 0;
  }
  else {
    iVar3 = *(int *)(param_1 + 0x50);
  }
  lVar1 = *(longlong *)(*(longlong *)(*(longlong *)(param_1 + 8) + 0x5ec0) + (longlong)iVar3 * 8);
  auVar5._4_12_ = in_XMM1._4_12_;
  auVar5._0_4_ = in_XMM1._0_4_ * *(float *)(lVar1 + 8);
  iVar3 = (int)auVar5._0_4_;
  if ((iVar3 != -0x80000000) && ((float)iVar3 != auVar5._0_4_)) {
    auVar7._0_8_ = auVar5._0_8_;
    auVar7._8_4_ = in_XMM1._4_4_;
    auVar7._12_4_ = in_XMM1._4_4_;
    auVar6._8_8_ = auVar7._8_8_;
    auVar6._4_4_ = auVar5._0_4_;
    auVar6._0_4_ = auVar5._0_4_;
    uVar2 = movmskps((int)lVar1,auVar6);
    auVar5 = ZEXT416((uint)(float)(int)(iVar3 - (uVar2 & 1)));
  }
  if ((*(char *)(param_1 + 0x30) == '\0') || (*(float *)(param_1 + 0x34) <= 0.0)) {
    fVar4 = auVar5._0_4_ / *(float *)(param_1 + 0xb0);
    *(float *)(param_1 + 0x34) = fVar4;
    if (fVar4 != 0.0) {
      *(undefined1 *)(param_1 + 0x30) = 1;
      return;
    }
    *(undefined1 *)(param_1 + 0x30) = 0;
  }
  return;
}


// Incoming references
// 0x55c99f UNCONDITIONAL_CALL caller 14055c990
// 0x55c9e4 UNCONDITIONAL_CALL caller 14055c9c0
// 0x55caa0 UNCONDITIONAL_CALL caller 14055ca70
// 0x55cc70 UNCONDITIONAL_CALL caller 14055cc40
// 0x55cd2f UNCONDITIONAL_CALL caller 14055cd20
// 0x55cdaa UNCONDITIONAL_CALL caller 14055cd80
// 0x55cdfb UNCONDITIONAL_CALL caller 14055cdd0
// 0x55ce34 UNCONDITIONAL_CALL caller 14055ce20
// 0x55ce7b UNCONDITIONAL_CALL caller 14055ce50
