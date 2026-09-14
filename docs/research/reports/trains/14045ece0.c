// Candidate VA 14045ece0; RVA 0x45ece0
// Ghidra inferred prototype: undefined FUN_14045ece0()

void FUN_14045ece0(longlong param_1,longlong *param_2)

{
  uint uVar1;
  longlong *plVar2;
  longlong lVar3;
  ulonglong uVar4;
  longlong lVar5;
  longlong lVar6;
  longlong *plVar7;
  longlong lVar8;
  longlong lVar9;
  longlong lVar10;
  longlong lVar11;
  uint uVar12;
  ulonglong uVar13;
  longlong lVar14;
  
  plVar7 = *(longlong **)(param_1 + 0x18);
  plVar2 = *(longlong **)(param_1 + 0x20);
  do {
    if (plVar7 == plVar2) {
      return;
    }
    lVar3 = *plVar7;
    if (lVar3 != 0) {
      uVar1 = *(uint *)(param_1 + 8);
      uVar13 = 0;
      lVar9 = lVar3;
      if (uVar1 != 0) {
        do {
          if (*(short *)(lVar9 + 6) == 5) {
            lVar14 = uVar13 * 0x638 + lVar3;
            uVar4 = param_2[1];
            if (uVar4 < (ulonglong)param_2[2]) {
              param_2[1] = uVar4 + 0x638;
              FUN_14045ac60(uVar4,lVar14);
            }
            else {
              lVar8 = (longlong)(uVar4 - *param_2) >> 3;
              if (lVar8 * 0x2697cc8aef46c0f7 == 0) {
                lVar8 = 1;
LAB_14045edde:
                lVar6 = thunk_FUN_140983da8(lVar8 * 0x638);
              }
              else {
                lVar8 = lVar8 * 0x4d2f9915de8d81ee;
                if (lVar8 != 0) goto LAB_14045edde;
                lVar6 = 0;
              }
              lVar5 = param_2[1];
              lVar11 = lVar6;
              for (lVar10 = *param_2; lVar10 != lVar5; lVar10 = lVar10 + 0x638) {
                FUN_1404642f0(lVar11,lVar10);
                lVar11 = lVar11 + 0x638;
              }
              FUN_14045ac60(lVar11,lVar14);
              lVar5 = param_2[1];
              for (lVar14 = *param_2; lVar14 != lVar5; lVar14 = lVar14 + 0x638) {
                FUN_140351080(lVar14);
              }
              if ((void *)*param_2 != (void *)0x0) {
                free((void *)*param_2);
              }
              *param_2 = lVar6;
              param_2[1] = lVar11 + 0x638;
              param_2[2] = lVar8 * 0x638 + lVar6;
            }
          }
          uVar12 = (int)uVar13 + 1;
          uVar13 = (ulonglong)uVar12;
          lVar9 = lVar9 + 0x638;
        } while (uVar12 < uVar1);
      }
    }
    plVar7 = plVar7 + 1;
  } while( true );
}


// Incoming references
// 0xc23fe4 DATA caller none
// 0xb093ec DATA caller none
// 0xb093fc DATA caller none
// 0x479c4b UNCONDITIONAL_CALL caller 140479bd0
// 0x4ab122 UNCONDITIONAL_CALL caller 1404aafd0
// 0x4a21bb UNCONDITIONAL_CALL caller 1404a1710
