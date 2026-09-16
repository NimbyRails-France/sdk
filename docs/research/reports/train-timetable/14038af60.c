// Candidate VA 14038af60; RVA 0x38af60
// Ghidra inferred prototype: undefined FUN_14038af60()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 * FUN_14038af60(longlong param_1,undefined8 *param_2,longlong param_3,longlong param_4)

{
  double dVar1;
  double *pdVar2;
  longlong lVar3;
  undefined1 *puVar4;
  undefined1 *puVar5;
  longlong lVar6;
  int iVar7;
  bool bVar8;
  uint uVar9;
  uint uVar10;
  double dVar11;
  double dVar12;
  undefined *local_2a8;
  undefined8 uStack_2a0;
  undefined8 local_298;
  undefined **ppuStack_290;
  undefined8 local_280;
  undefined8 uStack_278;
  undefined8 local_270;
  undefined8 uStack_268;
  undefined *local_258 [2];
  undefined **local_248;
  undefined1 *local_240;
  undefined8 local_238;
  ulonglong local_230;
  undefined1 local_228 [512];

  if (*(longlong *)(param_1 + 0xd0) == 0) {
    *param_2 = 0;
    param_2[1] = 0;
    param_2[2] = 0;
    param_2[3] = 0xf;
    *(undefined1 *)param_2 = 0;
    return param_2;
  }
  if (*(char *)(param_1 + 200) == '\0') {
    FUN_140019ca0(param_2,param_1 + 0xa8);
    return param_2;
  }
  local_2a8 = (undefined *)CONCAT44(local_2a8._4_4_,*(int *)(param_1 + 0xa0) + 1);
  local_258[0] = local_2a8;
  iVar7 = 1;
  ppuStack_290 = local_258;
  local_238 = 0;
  local_248 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_240 = local_228;
  local_230 = 500;
  local_298 = 1;
  local_2a8 = &DAT_140a49948;
  uStack_2a0 = 2;
  FUN_140022830(&local_248,&local_2a8,&local_298);
  local_280 = 0;
  uStack_278 = 0;
  local_270 = 0;
  uStack_268 = 0;
  FUN_140002c00(&local_280,local_240);
  local_248 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_240 != local_228) {
    puVar5 = local_240;
    if ((0xfff < local_230) &&
       (puVar5 = *(undefined1 **)(local_240 + -8),
       (undefined1 *)0x1f < local_240 + (-8 - (longlong)puVar5))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar5);
  }
  if ((param_3 == 0) || (param_4 == 0)) goto LAB_14038b1c7;
  lVar6 = -1;
  if (*(longlong *)(param_3 + 0xd0) == 0) {
    if (*(longlong *)(param_4 + 0xd0) == 0) goto LAB_14038b1c7;
    iVar7 = -1;
  }
  else if (*(longlong *)(param_4 + 0xd0) != 0) goto LAB_14038b1c7;
  pdVar2 = *(double **)(param_1 + 0x1b0);
  lVar3 = *(longlong *)(param_1 + 0x1b8);
  bVar8 = iVar7 == -1;
  if ((ulonglong)(lVar3 - (longlong)pdVar2 >> 4) < 2) {
LAB_14038b19f:
    puVar4 = &DAT_140a4e3f0;
    puVar5 = &DAT_140a6efa0;
  }
  else {
    dVar1 = pdVar2[1];
    dVar11 = *pdVar2 - *(double *)(lVar3 + -0x10);
    dVar12 = (double)CONCAT44((uint)((ulonglong)dVar11 >> 0x20) & _UNK_140aad5e4,
                              SUB84(dVar11,0) & _DAT_140aad5e0);
    dVar11 = dVar1 - *(double *)(lVar3 + -8);
    uVar9 = SUB84(dVar11,0) & _DAT_140aad5e0;
    uVar10 = (uint)((ulonglong)dVar11 >> 0x20) & _UNK_140aad5e4;
    if (dVar12 < (double)CONCAT44(uVar10,uVar9) || dVar12 == (double)CONCAT44(uVar10,uVar9)) {
      if (*(double *)(lVar3 + -8) <= dVar1 && dVar1 != *(double *)(lVar3 + -8)) {
        bVar8 = iVar7 != -1;
      }
      goto LAB_14038b19f;
    }
    if (*(double *)(lVar3 + -0x10) < *pdVar2) {
      bVar8 = iVar7 != -1;
    }
    puVar4 = &DAT_140a6efa8;
    puVar5 = &DAT_140a6efa4;
  }
  if (bVar8) {
    puVar5 = puVar4;
  }
  do {
    lVar6 = lVar6 + 1;
  } while (puVar5[lVar6] != '\0');
  FUN_1400254f0(&local_280);
LAB_14038b1c7:
  *param_2 = local_280;
  param_2[1] = uStack_278;
  param_2[2] = local_270;
  param_2[3] = uStack_268;
  return param_2;
}


// Incoming references
// 0xc1e2cc DATA caller none
// 0x36d194 UNCONDITIONAL_CALL caller 14036d0a0
// 0x4a309b UNCONDITIONAL_CALL caller 1404a2e70
// 0x5a3ddf UNCONDITIONAL_CALL caller 1405a39b0
// 0x5a446f UNCONDITIONAL_CALL caller 1405a42d0
// 0x733337 UNCONDITIONAL_CALL caller 140732390
// 0x624393 UNCONDITIONAL_CALL caller 1406236d0
// 0x8060cf UNCONDITIONAL_CALL caller 140805710
// 0x6ba4e5 UNCONDITIONAL_CALL caller 1406b9c40
// 0x6bd7ee UNCONDITIONAL_CALL caller 1406bd5f0
// 0x6bde35 UNCONDITIONAL_CALL caller 1406bd5f0
