// Candidate VA 1402a9590; RVA 0x2a9590
// Ghidra inferred prototype: undefined FUN_1402a9590()

undefined8 ****
FUN_1402a9590(longlong param_1,undefined8 *param_2,undefined8 *param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  longlong lVar5;
  longlong *plVar6;
  undefined8 ****ppppuVar7;
  undefined8 ****ppppuVar8;
  undefined1 local_48 [16];
  undefined8 ***local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  ulonglong uStack_20;
  undefined8 local_18 [2];
  
  local_38 = (undefined8 ****)0x0;
  uStack_30 = 0;
  local_28 = 0;
  uStack_20 = 0;
  FUN_140002c00(&local_38,*param_2);
  local_18[0] = FUN_1402a9cb0(*param_2,param_2[1]);
  lVar5 = FUN_1402a9e80(param_1 + 0x38,local_18);
  if (lVar5 == 0) {
    plVar6 = (longlong *)FUN_1402aaf10(param_1 + 0x38,local_48);
    lVar5 = *plVar6;
    ppppuVar8 = (undefined8 ****)(lVar5 + 0x18);
    if (ppppuVar8 != &local_38) {
      ppppuVar7 = &local_38;
      if (0xf < uStack_20) {
        ppppuVar7 = (undefined8 ****)local_38;
      }
      FUN_140030630(ppppuVar8,ppppuVar7);
    }
    *(undefined8 *)(lVar5 + 0x38) = local_18[0];
    uVar4 = param_3[1];
    *(undefined8 *)(lVar5 + 0x40) = *param_3;
    *(undefined8 *)(lVar5 + 0x48) = uVar4;
    uVar1 = *(undefined4 *)((longlong)param_3 + 0x14);
    uVar2 = *(undefined4 *)(param_3 + 3);
    uVar3 = *(undefined4 *)((longlong)param_3 + 0x1c);
    *(undefined4 *)(lVar5 + 0x50) = *(undefined4 *)(param_3 + 2);
    *(undefined4 *)(lVar5 + 0x54) = uVar1;
    *(undefined4 *)(lVar5 + 0x58) = uVar2;
    *(undefined4 *)(lVar5 + 0x5c) = uVar3;
    *(undefined4 *)(lVar5 + 0x60) = param_4;
    FUN_140016040(param_1 + 0x78,local_18);
  }
  else {
    ppppuVar8 = (undefined8 ****)0x0;
  }
  if (0xf < uStack_20) {
    ppppuVar7 = (undefined8 ****)local_38;
    if ((0xfff < uStack_20 + 1) &&
       (ppppuVar7 = (undefined8 ****)local_38[-1],
       0x1f < (ulonglong)((longlong)local_38 + (-8 - (longlong)ppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppuVar7);
  }
  return ppppuVar8;
}


// Incoming references
// 0xc142f4 DATA caller none
// 0x429dc2 UNCONDITIONAL_CALL caller 140428790
// 0x429e4d UNCONDITIONAL_CALL caller 140428790
// 0x42a05a UNCONDITIONAL_CALL caller 140428790
// 0x42a0f0 UNCONDITIONAL_CALL caller 140428790
// 0x42a1c7 UNCONDITIONAL_CALL caller 140428790
// 0x42a299 UNCONDITIONAL_CALL caller 140428790
// 0x42a328 UNCONDITIONAL_CALL caller 140428790
// 0x42a613 UNCONDITIONAL_CALL caller 140428790
// 0x42a6a9 UNCONDITIONAL_CALL caller 140428790
// 0x42a73b UNCONDITIONAL_CALL caller 140428790
// 0x42a80d UNCONDITIONAL_CALL caller 140428790
// 0x42a89c UNCONDITIONAL_CALL caller 140428790
// 0x42aaba UNCONDITIONAL_CALL caller 140428790
// 0x42ab42 UNCONDITIONAL_CALL caller 140428790
// 0x42abca UNCONDITIONAL_CALL caller 140428790
// 0x42b053 UNCONDITIONAL_CALL caller 140428790
// 0x42b131 UNCONDITIONAL_CALL caller 140428790
// 0x42b1c7 UNCONDITIONAL_CALL caller 140428790
// 0x42ca9f UNCONDITIONAL_CALL caller 140428790
// 0x42cb27 UNCONDITIONAL_CALL caller 140428790
// 0x42cbb9 UNCONDITIONAL_CALL caller 140428790
// 0x42cc4b UNCONDITIONAL_CALL caller 140428790
// 0x42ccdd UNCONDITIONAL_CALL caller 140428790
// 0x42cd6f UNCONDITIONAL_CALL caller 140428790
// 0x42ce04 UNCONDITIONAL_CALL caller 140428790
// 0x42ce93 UNCONDITIONAL_CALL caller 140428790
// 0x42cf22 UNCONDITIONAL_CALL caller 140428790
// 0x42cfb1 UNCONDITIONAL_CALL caller 140428790
// 0x42d040 UNCONDITIONAL_CALL caller 140428790
// 0x42d0cf UNCONDITIONAL_CALL caller 140428790
// 0x42d15e UNCONDITIONAL_CALL caller 140428790
// 0x42d239 UNCONDITIONAL_CALL caller 140428790
// 0x42d30c UNCONDITIONAL_CALL caller 140428790
// 0x42d394 UNCONDITIONAL_CALL caller 140428790
// 0x42d41c UNCONDITIONAL_CALL caller 140428790
// 0x42d4a4 UNCONDITIONAL_CALL caller 140428790
// 0x42d52c UNCONDITIONAL_CALL caller 140428790
// 0x42d5b4 UNCONDITIONAL_CALL caller 140428790
// 0x42d63c UNCONDITIONAL_CALL caller 140428790
// 0x42d6c4 UNCONDITIONAL_CALL caller 140428790
// 0x42d74c UNCONDITIONAL_CALL caller 140428790
// 0x42dac6 UNCONDITIONAL_CALL caller 140428790
// 0x42db4e UNCONDITIONAL_CALL caller 140428790
// 0x42dbd6 UNCONDITIONAL_CALL caller 140428790
// 0x42dc62 UNCONDITIONAL_CALL caller 140428790
// 0x42dcf1 UNCONDITIONAL_CALL caller 140428790
// 0x42ddd4 UNCONDITIONAL_CALL caller 140428790
// 0x42de5c UNCONDITIONAL_CALL caller 140428790
// 0x42df3f UNCONDITIONAL_CALL caller 140428790
// 0x42dfc7 UNCONDITIONAL_CALL caller 140428790
// 0x42e04f UNCONDITIONAL_CALL caller 140428790
// 0x42e12b UNCONDITIONAL_CALL caller 140428790
// 0x42e20f UNCONDITIONAL_CALL caller 140428790
// 0x42e29e UNCONDITIONAL_CALL caller 140428790
// 0x42e382 UNCONDITIONAL_CALL caller 140428790
// 0x42e40b UNCONDITIONAL_CALL caller 140428790
// 0x42e543 UNCONDITIONAL_CALL caller 140428790
// 0x42e5d9 UNCONDITIONAL_CALL caller 140428790
// 0x42e6c3 UNCONDITIONAL_CALL caller 140428790
// 0x42e74b UNCONDITIONAL_CALL caller 140428790
// 0x42f67d UNCONDITIONAL_CALL caller 140428790
// 0x42f74d UNCONDITIONAL_CALL caller 140428790
// 0x42f7dc UNCONDITIONAL_CALL caller 140428790
// 0x42f8c6 UNCONDITIONAL_CALL caller 140428790
// 0x42facc UNCONDITIONAL_CALL caller 140428790
// 0x42fb57 UNCONDITIONAL_CALL caller 140428790
// 0x4301f4 UNCONDITIONAL_CALL caller 140428790
// 0x43027c UNCONDITIONAL_CALL caller 140428790
// 0x430304 UNCONDITIONAL_CALL caller 140428790
// 0x430390 UNCONDITIONAL_CALL caller 140428790
// 0x43041f UNCONDITIONAL_CALL caller 140428790
// 0x4304aa UNCONDITIONAL_CALL caller 140428790
// 0x430535 UNCONDITIONAL_CALL caller 140428790
// 0x430619 UNCONDITIONAL_CALL caller 140428790
// 0x4306ff UNCONDITIONAL_CALL caller 140428790
// 0x43078e UNCONDITIONAL_CALL caller 140428790
// 0x43086e UNCONDITIONAL_CALL caller 140428790
// 0x430900 UNCONDITIONAL_CALL caller 140428790
// 0x43098f UNCONDITIONAL_CALL caller 140428790
// 0x431e45 UNCONDITIONAL_CALL caller 140428790
// 0x431fbe UNCONDITIONAL_CALL caller 140428790
// 0x432046 UNCONDITIONAL_CALL caller 140428790
// 0x4320ce UNCONDITIONAL_CALL caller 140428790
// 0x432244 UNCONDITIONAL_CALL caller 140428790
// 0x4322cc UNCONDITIONAL_CALL caller 140428790
// 0x432354 UNCONDITIONAL_CALL caller 140428790
// 0x4323df UNCONDITIONAL_CALL caller 140428790
// 0x432467 UNCONDITIONAL_CALL caller 140428790
// 0x454b6d UNCONDITIONAL_CALL caller 140452f40
// 0x454bd1 UNCONDITIONAL_CALL caller 140452f40
// 0x2b1efe UNCONDITIONAL_CALL caller 1402b0c40
