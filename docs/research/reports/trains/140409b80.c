// Candidate VA 140409b80; RVA 0x409b80
// Ghidra inferred prototype: undefined FUN_140409b80()

/* WARNING: Type propagation algorithm not settling */

void FUN_140409b80(longlong param_1)

{
  undefined8 *puVar1;
  undefined4 *puVar2;
  bool bVar3;
  longlong lVar4;
  void *pvVar5;
  undefined8 *******pppppppuVar6;
  undefined8 *******pppppppuVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  undefined8 *puVar10;
  int *piVar11;
  int *piVar12;
  int *piVar13;
  int *piVar14;
  longlong lVar15;
  int iVar16;
  undefined8 uVar17;
  longlong lVar18;
  undefined4 local_res8;
  undefined4 local_resc;
  undefined8 ****local_238;
  undefined1 *puStack_230;
  undefined4 ****local_228;
  undefined8 uStack_220;
  undefined4 local_218 [2];
  undefined8 local_210;
  undefined8 uStack_208;
  undefined8 local_200;
  ulonglong local_1f8;
  undefined4 local_1f0;
  undefined1 local_1e8 [48];
  int local_1b8 [4];
  undefined4 local_1a8 [2];
  undefined8 local_1a0;
  undefined8 uStack_198;
  undefined8 local_190;
  undefined8 uStack_188;
  undefined4 local_180;
  undefined8 *******local_178;
  undefined8 *******pppppppuStack_170;
  undefined8 *******local_168;
  undefined8 uStack_160;
  longlong local_158;
  undefined4 local_148 [2];
  undefined8 local_140;
  undefined8 uStack_138;
  undefined8 local_130;
  undefined8 uStack_128;
  undefined4 local_120;
  undefined8 *******local_118;
  undefined8 *******pppppppuStack_110;
  undefined8 *******local_108;
  undefined8 uStack_100;
  longlong local_f8;
  undefined4 local_e8 [2];
  undefined8 local_e0;
  undefined8 uStack_d8;
  undefined8 local_d0;
  undefined8 uStack_c8;
  undefined4 local_c0;
  undefined8 *******local_b8;
  undefined8 *******pppppppuStack_b0;
  undefined8 *******local_a8;
  undefined8 uStack_a0;
  longlong local_98;
  undefined4 local_88 [2];
  undefined8 local_80;
  undefined8 uStack_78;
  undefined8 local_70;
  ulonglong local_68;
  undefined4 local_60;
  undefined8 *******local_58;
  undefined8 *******pppppppuStack_50;
  undefined8 *******local_48;
  undefined8 uStack_40;
  longlong local_38;
  
  puVar1 = (undefined8 *)(param_1 + 0x510);
  puVar10 = (undefined8 *)(param_1 + 0x4f8);
  FUN_140418cd0(puVar10);
  FUN_140418d30(puVar1);
  local_1a8[0] = 0;
  lVar4 = FUN_1402d82e0("rtstat_pax_total","Contains total pax");
  local_1a0 = (void *)0x0;
  uStack_198 = 0;
  local_190 = 0;
  uStack_188 = 0;
  lVar18 = -1;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_1a0,lVar4);
  local_180 = 1;
  local_1b8[0] = 1;
  local_1b8[1] = 2;
  local_1b8[2] = 3;
  piVar13 = local_1b8;
  local_178 = &local_178;
  pppppppuStack_170 = &local_178;
  local_168 = (undefined8 *******)0x0;
  uStack_160 = 0;
  local_158 = 0;
  do {
    pvVar5 = (void *)thunk_FUN_140983da8(0x28);
    iVar16 = *piVar13;
    *(int *)((longlong)pvVar5 + 0x20) = iVar16;
    bVar3 = true;
    pppppppuVar7 = &local_178;
    pppppppuVar6 = local_168;
    while (pppppppuVar6 != (undefined8 *******)0x0) {
      bVar3 = iVar16 < *(int *)(pppppppuVar6 + 4);
      pppppppuVar7 = pppppppuVar6;
      if (iVar16 < *(int *)(pppppppuVar6 + 4)) {
        pppppppuVar6 = (undefined8 *******)pppppppuVar6[1];
      }
      else {
        pppppppuVar6 = (undefined8 *******)*pppppppuVar6;
      }
    }
    pppppppuVar6 = pppppppuVar7;
    if (bVar3) {
      if (pppppppuVar7 != pppppppuStack_170) {
        pppppppuVar6 = (undefined8 *******)FUN_14001dcf0(pppppppuVar7);
        goto LAB_140409cc4;
      }
LAB_140409cca:
      FUN_14001de10(pvVar5,pppppppuVar7);
      local_158 = local_158 + 1;
    }
    else {
LAB_140409cc4:
      if (*(int *)(pppppppuVar6 + 4) < iVar16) goto LAB_140409cca;
      free(pvVar5);
    }
    piVar13 = piVar13 + 1;
  } while (piVar13 != local_1b8 + 3);
  puVar2 = *(undefined4 **)(param_1 + 0x500);
  if (puVar2 < *(undefined4 **)(param_1 + 0x508)) {
    *(undefined4 **)(param_1 + 0x500) = puVar2 + 0x18;
    *puVar2 = local_1a8[0];
    *(undefined8 *)(puVar2 + 2) = 0;
    *(undefined8 *)(puVar2 + 4) = 0;
    *(undefined8 *)(puVar2 + 6) = 0;
    *(undefined8 *)(puVar2 + 8) = 0;
    puVar2[2] = (undefined4)local_1a0;
    puVar2[3] = local_1a0._4_4_;
    puVar2[4] = (undefined4)uStack_198;
    puVar2[5] = uStack_198._4_4_;
    puVar2[6] = (undefined4)local_190;
    puVar2[7] = local_190._4_4_;
    puVar2[8] = (undefined4)uStack_188;
    puVar2[9] = uStack_188._4_4_;
    local_190 = 0;
    uStack_188 = 0xf;
    local_1a0 = (void *)((ulonglong)local_1a0 & 0xffffffffffffff00);
    puVar2[10] = local_180;
    FUN_14031fe30(puVar2 + 0xc,&local_178);
    pppppppuVar7 = local_168;
  }
  else {
    FUN_14041a5d0(puVar10,local_1a8);
    pppppppuVar7 = local_168;
  }
  while (pppppppuVar7 != (undefined8 *******)0x0) {
    FUN_1402450d0(&local_178,*pppppppuVar7);
    pppppppuVar6 = (undefined8 *******)pppppppuVar7[1];
    free(pppppppuVar7);
    pppppppuVar7 = pppppppuVar6;
  }
  if (0xf < uStack_188) {
    pvVar5 = local_1a0;
    if ((0xfff < uStack_188 + 1) &&
       (pvVar5 = *(void **)((longlong)local_1a0 + -8),
       0x1f < (ulonglong)((longlong)local_1a0 + (-8 - (longlong)pvVar5)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar5);
  }
  local_148[0] = 1;
  lVar4 = FUN_1402d82e0("rtstat_pax_new","Contains new pax");
  local_140 = (void *)0x0;
  uStack_138 = 0;
  local_130 = 0;
  uStack_128 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_140,lVar4);
  local_120 = 1;
  local_118 = &local_118;
  pppppppuStack_110 = &local_118;
  local_108 = (undefined8 *******)0x0;
  uStack_100 = 0;
  local_f8 = 0;
  pvVar5 = (void *)thunk_FUN_140983da8(0x28);
  *(undefined4 *)((longlong)pvVar5 + 0x20) = 1;
  bVar3 = true;
  pppppppuVar7 = &local_118;
  pppppppuVar6 = local_108;
  while (pppppppuVar6 != (undefined8 *******)0x0) {
    bVar3 = 1 < *(int *)(pppppppuVar6 + 4);
    pppppppuVar7 = pppppppuVar6;
    if (*(int *)(pppppppuVar6 + 4) < 2) {
      pppppppuVar6 = (undefined8 *******)*pppppppuVar6;
    }
    else {
      pppppppuVar6 = (undefined8 *******)pppppppuVar6[1];
    }
  }
  pppppppuVar6 = pppppppuVar7;
  if (bVar3) {
    if (pppppppuVar7 != pppppppuStack_110) {
      pppppppuVar7 = (undefined8 *******)FUN_14001dcf0(pppppppuVar7);
      goto LAB_140409eb8;
    }
LAB_140409ebe:
    FUN_14001de10(pvVar5,pppppppuVar6);
    local_f8 = local_f8 + 1;
  }
  else {
LAB_140409eb8:
    if (*(int *)(pppppppuVar7 + 4) < 1) goto LAB_140409ebe;
    free(pvVar5);
  }
  puVar2 = *(undefined4 **)(param_1 + 0x500);
  if (puVar2 < *(undefined4 **)(param_1 + 0x508)) {
    *(undefined4 **)(param_1 + 0x500) = puVar2 + 0x18;
    *puVar2 = local_148[0];
    *(undefined8 *)(puVar2 + 2) = 0;
    *(undefined8 *)(puVar2 + 4) = 0;
    *(undefined8 *)(puVar2 + 6) = 0;
    *(undefined8 *)(puVar2 + 8) = 0;
    puVar2[2] = (undefined4)local_140;
    puVar2[3] = local_140._4_4_;
    puVar2[4] = (undefined4)uStack_138;
    puVar2[5] = uStack_138._4_4_;
    puVar2[6] = (undefined4)local_130;
    puVar2[7] = local_130._4_4_;
    puVar2[8] = (undefined4)uStack_128;
    puVar2[9] = uStack_128._4_4_;
    local_130 = 0;
    uStack_128 = 0xf;
    local_140 = (void *)((ulonglong)local_140 & 0xffffffffffffff00);
    puVar2[10] = local_120;
    FUN_14031fe30(puVar2 + 0xc,&local_118);
    pppppppuVar7 = local_108;
  }
  else {
    FUN_14041a5d0(puVar10,local_148);
    pppppppuVar7 = local_108;
  }
  while (pppppppuVar7 != (undefined8 *******)0x0) {
    FUN_1402450d0(&local_118,*pppppppuVar7);
    pppppppuVar6 = (undefined8 *******)pppppppuVar7[1];
    free(pppppppuVar7);
    pppppppuVar7 = pppppppuVar6;
  }
  if (0xf < uStack_128) {
    pvVar5 = local_140;
    if ((0xfff < uStack_128 + 1) &&
       (pvVar5 = *(void **)((longlong)local_140 + -8),
       0x1f < (ulonglong)((longlong)local_140 + (-8 - (longlong)pvVar5)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar5);
  }
  local_e8[0] = 2;
  lVar4 = FUN_1402d82e0("rtstat_pax_transfer","Contains transfer pax");
  local_e0 = (void *)0x0;
  uStack_d8 = 0;
  local_d0 = 0;
  uStack_c8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_e0,lVar4);
  local_c0 = 1;
  local_b8 = &local_b8;
  pppppppuStack_b0 = &local_b8;
  local_a8 = (undefined8 *******)0x0;
  uStack_a0 = 0;
  local_98 = 0;
  pvVar5 = (void *)thunk_FUN_140983da8(0x28);
  *(undefined4 *)((longlong)pvVar5 + 0x20) = 1;
  bVar3 = true;
  pppppppuVar7 = &local_b8;
  pppppppuVar6 = local_a8;
  while (pppppppuVar6 != (undefined8 *******)0x0) {
    bVar3 = 1 < *(int *)(pppppppuVar6 + 4);
    pppppppuVar7 = pppppppuVar6;
    if (*(int *)(pppppppuVar6 + 4) < 2) {
      pppppppuVar6 = (undefined8 *******)*pppppppuVar6;
    }
    else {
      pppppppuVar6 = (undefined8 *******)pppppppuVar6[1];
    }
  }
  pppppppuVar6 = pppppppuVar7;
  if (bVar3) {
    if (pppppppuVar7 != pppppppuStack_b0) {
      pppppppuVar7 = (undefined8 *******)FUN_14001dcf0(pppppppuVar7);
      goto LAB_14040a0db;
    }
LAB_14040a0e1:
    FUN_14001de10(pvVar5,pppppppuVar6);
    local_98 = local_98 + 1;
  }
  else {
LAB_14040a0db:
    if (*(int *)(pppppppuVar7 + 4) < 1) goto LAB_14040a0e1;
    free(pvVar5);
  }
  puVar2 = *(undefined4 **)(param_1 + 0x500);
  if (puVar2 < *(undefined4 **)(param_1 + 0x508)) {
    *(undefined4 **)(param_1 + 0x500) = puVar2 + 0x18;
    *puVar2 = local_e8[0];
    *(undefined8 *)(puVar2 + 2) = 0;
    *(undefined8 *)(puVar2 + 4) = 0;
    *(undefined8 *)(puVar2 + 6) = 0;
    *(undefined8 *)(puVar2 + 8) = 0;
    puVar2[2] = (undefined4)local_e0;
    puVar2[3] = local_e0._4_4_;
    puVar2[4] = (undefined4)uStack_d8;
    puVar2[5] = uStack_d8._4_4_;
    puVar2[6] = (undefined4)local_d0;
    puVar2[7] = local_d0._4_4_;
    puVar2[8] = (undefined4)uStack_c8;
    puVar2[9] = uStack_c8._4_4_;
    local_d0 = 0;
    uStack_c8 = 0xf;
    local_e0 = (void *)((ulonglong)local_e0 & 0xffffffffffffff00);
    puVar2[10] = local_c0;
    FUN_14031fe30(puVar2 + 0xc,&local_b8);
    pppppppuVar7 = local_a8;
  }
  else {
    FUN_14041a5d0(puVar10,local_e8);
    pppppppuVar7 = local_a8;
  }
  while (pppppppuVar7 != (undefined8 *******)0x0) {
    FUN_1402450d0(&local_b8,*pppppppuVar7);
    pppppppuVar6 = (undefined8 *******)pppppppuVar7[1];
    free(pppppppuVar7);
    pppppppuVar7 = pppppppuVar6;
  }
  if (0xf < uStack_c8) {
    pvVar5 = local_e0;
    if ((0xfff < uStack_c8 + 1) &&
       (pvVar5 = *(void **)((longlong)local_e0 + -8),
       0x1f < (ulonglong)((longlong)local_e0 + (-8 - (longlong)pvVar5)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar5);
  }
  local_88[0] = 3;
  lVar4 = FUN_1402d82e0("rtstat_pax_with_origin","Pax with origin at");
  local_80 = 0;
  uStack_78 = 0;
  local_70 = 0;
  local_68 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_80,lVar4);
  local_60 = 1;
  local_58 = &local_58;
  pppppppuStack_50 = &local_58;
  local_48 = (undefined8 *******)0x0;
  uStack_40 = 0;
  local_38 = 0;
  pvVar5 = (void *)thunk_FUN_140983da8(0x28);
  *(undefined4 *)((longlong)pvVar5 + 0x20) = 1;
  bVar3 = true;
  pppppppuVar7 = &local_58;
  pppppppuVar6 = local_48;
  while (pppppppuVar6 != (undefined8 *******)0x0) {
    bVar3 = 1 < *(int *)(pppppppuVar6 + 4);
    pppppppuVar7 = pppppppuVar6;
    if (*(int *)(pppppppuVar6 + 4) < 2) {
      pppppppuVar6 = (undefined8 *******)*pppppppuVar6;
    }
    else {
      pppppppuVar6 = (undefined8 *******)pppppppuVar6[1];
    }
  }
  pppppppuVar6 = pppppppuVar7;
  if (bVar3) {
    if (pppppppuVar7 != pppppppuStack_50) {
      pppppppuVar7 = (undefined8 *******)FUN_14001dcf0(pppppppuVar7);
      goto LAB_14040a32b;
    }
  }
  else {
LAB_14040a32b:
    if (0 < *(int *)(pppppppuVar7 + 4)) {
      free(pvVar5);
      goto LAB_14040a36d;
    }
  }
  if (((undefined8 ********)pppppppuVar6 == &local_58) ||
     (uVar17 = 1, 1 < *(int *)(pppppppuVar6 + 4))) {
    uVar17 = 0;
  }
  FUN_14001de10(pvVar5,pppppppuVar6,&local_58,uVar17);
  local_38 = local_38 + 1;
LAB_14040a36d:
  if (*(ulonglong *)(param_1 + 0x500) < *(ulonglong *)(param_1 + 0x508)) {
    *(ulonglong *)(param_1 + 0x500) = *(ulonglong *)(param_1 + 0x500) + 0x60;
    FUN_1404199c0();
    pppppppuVar7 = local_48;
  }
  else {
    FUN_14041a5d0(puVar10,local_88);
    pppppppuVar7 = local_48;
  }
  while (pppppppuVar7 != (undefined8 *******)0x0) {
    FUN_1402450d0(&local_58,*pppppppuVar7);
    pppppppuVar6 = (undefined8 *******)pppppppuVar7[1];
    free(pppppppuVar7);
    pppppppuVar7 = pppppppuVar6;
  }
  if (0xf < local_68) {
    FUN_140003040(&local_80,local_80);
  }
  local_218[0] = 4;
  lVar4 = FUN_1402d82e0("rtstat_pax_with_destination","Pax with destination at");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 1;
  local_res8 = 1;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  local_228 = (undefined4 ****)local_238;
  uStack_220 = (undefined8 *)puStack_230;
  FUN_140417ef0(local_1e8,&local_238);
  if (*(ulonglong *)(param_1 + 0x500) < *(ulonglong *)(param_1 + 0x508)) {
    *(ulonglong *)(param_1 + 0x500) = *(ulonglong *)(param_1 + 0x500) + 0x60;
    FUN_1404199c0();
  }
  else {
    FUN_14041a5d0(puVar10,local_218);
  }
  FUN_1402f2390(local_1e8);
  if (0xf < local_1f8) {
    FUN_140003040(&local_210,local_210);
  }
  local_218[0] = 5;
  lVar4 = FUN_1402d82e0("rtstat_pax_with_next","Pax with next stop at");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 1;
  local_res8 = 1;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  local_228 = (undefined4 ****)local_238;
  uStack_220 = (undefined8 *)puStack_230;
  FUN_140417ef0(local_1e8,&local_238);
  if (*(ulonglong *)(param_1 + 0x500) < *(ulonglong *)(param_1 + 0x508)) {
    *(ulonglong *)(param_1 + 0x500) = *(ulonglong *)(param_1 + 0x500) + 0x60;
    FUN_1404199c0();
  }
  else {
    FUN_14041a5d0(puVar10,local_218);
  }
  FUN_1402f2390(local_1e8);
  if (0xf < local_1f8) {
    FUN_140003040(&local_210,local_210);
  }
  local_218[0] = 6;
  lVar4 = FUN_1402d82e0("rtstat_pax_avg_time","Avg. total time of contained pax");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 3;
  local_res8 = 1;
  local_resc = 2;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = &stack0x00000010;
  local_228 = (undefined4 ****)local_238;
  uStack_220 = (undefined8 *)puStack_230;
  FUN_140417ef0(local_1e8,&local_238);
  if (*(ulonglong *)(param_1 + 0x500) < *(ulonglong *)(param_1 + 0x508)) {
    *(ulonglong *)(param_1 + 0x500) = *(ulonglong *)(param_1 + 0x500) + 0x60;
    FUN_1404199c0();
  }
  else {
    FUN_14041a5d0(puVar10,local_218);
  }
  FUN_1402f2390(local_1e8);
  if (0xf < local_1f8) {
    FUN_140003040(&local_210,local_210);
  }
  local_218[0] = 7;
  lVar4 = FUN_1402d82e0("rtstat_pax_avg_leg_time","Avg. leg time of contained pax");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 3;
  local_res8 = 1;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  local_228 = (undefined4 ****)local_238;
  uStack_220 = (undefined8 *)puStack_230;
  FUN_140417ef0(local_1e8,&local_238);
  if (*(ulonglong *)(param_1 + 0x500) < *(ulonglong *)(param_1 + 0x508)) {
    *(ulonglong *)(param_1 + 0x500) = *(ulonglong *)(param_1 + 0x500) + 0x60;
    FUN_1404199c0();
  }
  else {
    FUN_14041a5d0(puVar10,local_218);
  }
  FUN_1402f2390(local_1e8);
  if (0xf < local_1f8) {
    FUN_140003040(&local_210,local_210);
  }
  local_218[0] = 8;
  lVar4 = FUN_1402d82e0("rtstat_pax_capacity","Capacity");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 1;
  local_228 = (undefined4 ****)0x200000001;
  uStack_220 = (undefined8 *)CONCAT44(uStack_220._4_4_,3);
  local_238 = &local_228;
  puStack_230 = (undefined1 *)((longlong)&uStack_220 + 4);
  FUN_140417ef0(local_1e8,&local_238);
  if (*(ulonglong *)(param_1 + 0x500) < *(ulonglong *)(param_1 + 0x508)) {
    *(ulonglong *)(param_1 + 0x500) = *(ulonglong *)(param_1 + 0x500) + 0x60;
    FUN_1404199c0();
  }
  else {
    FUN_14041a5d0(puVar10,local_218);
  }
  FUN_1402f2390(local_1e8);
  if (0xf < local_1f8) {
    FUN_140003040(&local_210,local_210);
  }
  local_218[0] = 9;
  lVar4 = FUN_1402d82e0("rtstat_pax_capactity_usage","Capacity used %");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 9;
  local_228 = (undefined4 ****)0x200000001;
  uStack_220._0_4_ = 3;
  local_238 = &local_228;
  puStack_230 = (undefined1 *)((longlong)&uStack_220 + 4);
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  local_218[0] = 10;
  lVar4 = FUN_1402d82e0("rtstat_leg_m","Leg remaining distance");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 2;
  local_res8 = 2;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  local_218[0] = 0xb;
  lVar4 = FUN_1402d82e0("rtstat_leg_s","Time to scheduled arrival");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 3;
  local_res8 = 2;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  local_218[0] = 0xc;
  lVar4 = FUN_1402d82e0("rtstat_leg_late","Lateness estimation");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 3;
  local_res8 = 2;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  local_218[0] = 0xd;
  lVar4 = FUN_1402d82e0("rtstat_leg_s","Remaining wait");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 3;
  local_res8 = 2;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  local_218[0] = 0xe;
  lVar4 = FUN_1402d82e0("rtstat_speed","Speed");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  lVar15 = -1;
  do {
    lVar15 = lVar15 + 1;
  } while (*(char *)(lVar4 + lVar15) != '\0');
  FUN_140002c00(&local_210,lVar4);
  local_1f0 = 4;
  local_res8 = 2;
  local_238 = (undefined8 ****)&local_res8;
  puStack_230 = (undefined1 *)&local_resc;
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  local_218[0] = 0xf;
  lVar4 = FUN_1402d82e0("rtstat_creation","Creation date");
  local_210 = 0;
  uStack_208 = 0;
  local_200 = 0;
  local_1f8 = 0;
  do {
    lVar18 = lVar18 + 1;
  } while (*(char *)(lVar4 + lVar18) != '\0');
  FUN_140002c00(&local_210,lVar4,lVar18);
  local_1f0 = 0xb;
  local_228 = (undefined4 ****)0x300000001;
  uStack_220 = (undefined8 *)CONCAT44(uStack_220._4_4_,2);
  local_238 = &local_228;
  puStack_230 = (undefined1 *)((longlong)&uStack_220 + 4);
  FUN_140417ef0(local_1e8,&local_238);
  FUN_140418c60(puVar10,local_218);
  FUN_140353c70(local_218);
  piVar13 = *(int **)(param_1 + 0x500);
  piVar12 = (int *)*puVar10;
  if (piVar12 != piVar13) {
    piVar14 = piVar12 + 2;
    do {
      puVar10 = puVar1;
      if (*(undefined8 **)(param_1 + 0x520) != (undefined8 *)0x0) {
        puVar8 = *(undefined8 **)(param_1 + 0x520);
        do {
          if (*(int *)(puVar8 + 4) < *piVar12) {
            puVar9 = (undefined8 *)*puVar8;
          }
          else {
            puVar9 = (undefined8 *)puVar8[1];
            puVar10 = puVar8;
          }
          puVar8 = puVar9;
        } while (puVar9 != (undefined8 *)0x0);
      }
      if ((puVar10 == puVar1) || (*piVar12 < *(int *)(puVar10 + 4))) {
        puVar10 = (undefined8 *)FUN_14041b430(puVar1,&local_res8);
        puVar10 = (undefined8 *)*puVar10;
      }
      *(int *)(puVar10 + 5) = *piVar12;
      if ((int *)(puVar10 + 6) != piVar14) {
        piVar11 = piVar14;
        if (0xf < *(ulonglong *)(piVar14 + 6)) {
          piVar11 = *(int **)piVar14;
        }
        FUN_140030630(puVar10 + 6,piVar11,*(undefined8 *)(piVar14 + 4));
      }
      *(int *)(puVar10 + 10) = piVar14[8];
      FUN_140418fc0(puVar10 + 0xb,piVar14 + 10);
      piVar12 = piVar12 + 0x18;
      piVar14 = piVar14 + 0x18;
    } while (piVar12 != piVar13);
  }
  return;
}


// Incoming references
// 0xc217dc DATA caller none
// 0x489c0f UNCONDITIONAL_CALL caller 140489ad0
