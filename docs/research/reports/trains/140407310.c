// Candidate VA 140407310; RVA 0x407310
// Ghidra inferred prototype: undefined FUN_140407310()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140407310(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined4 *puVar3;
  bool bVar4;
  undefined8 *****pppppuVar5;
  longlong lVar6;
  void *pvVar7;
  undefined8 *******pppppppuVar8;
  undefined8 *******pppppppuVar9;
  undefined8 uVar10;
  undefined8 *puVar11;
  int *piVar12;
  undefined8 *puVar13;
  undefined8 *puVar14;
  char *_Memory;
  longlong lVar15;
  int *piVar16;
  undefined8 ******ppppppuVar17;
  undefined8 ********ppppppppuVar18;
  undefined4 uVar19;
  longlong lVar20;
  undefined8 ********ppppppppuVar21;
  size_t _Size;
  undefined8 ********ppppppppuVar22;
  longlong lVar23;
  undefined8 uVar24;
  undefined8 *******pppppppuVar25;
  undefined8 local_res8;
  undefined1 local_res10 [8];
  undefined1 local_res18 [8];
  undefined8 **********local_238;
  undefined8 uStack_230;
  undefined8 *********local_228;
  undefined1 *puStack_220;
  undefined4 local_218;
  undefined4 local_214;
  undefined8 local_210;
  undefined8 uStack_208;
  undefined8 ******local_200;
  undefined8 ******local_1f8;
  char *local_1f0;
  undefined8 uStack_1e8;
  undefined8 ******local_1e0;
  undefined8 ******local_1d8;
  undefined4 local_1d0;
  undefined1 local_1c8 [48];
  undefined1 local_198;
  undefined4 local_190 [2];
  undefined8 local_188;
  undefined8 uStack_180;
  undefined8 ******local_178;
  undefined8 ******ppppppuStack_170;
  undefined1 local_168;
  undefined8 local_160;
  void *local_158;
  undefined8 uStack_150;
  undefined8 local_148;
  ulonglong local_140;
  char *local_138;
  undefined8 uStack_130;
  undefined8 ******local_128;
  ulonglong local_120;
  undefined4 local_118;
  undefined8 ******local_110;
  undefined8 ******ppppppuStack_108;
  undefined8 ******local_100;
  undefined8 uStack_f8;
  longlong local_f0;
  undefined1 local_e0;
  undefined8 local_d8;
  undefined8 local_d0;
  undefined8 uStack_c8;
  undefined8 ******local_c0;
  undefined8 ******local_b8;
  char *local_b0;
  undefined8 uStack_a8;
  undefined8 ******local_a0;
  undefined8 ******local_98;
  undefined4 local_90;
  undefined8 ******local_88;
  undefined8 ******ppppppuStack_80;
  undefined8 ******local_78;
  undefined8 uStack_70;
  undefined8 ******local_68;
  undefined1 local_58;
  
  lVar15 = param_1 + 0x468;
  FUN_140418a80(lVar15);
  puVar11 = (undefined8 *)(param_1 + 0x480);
  FUN_140418b10(puVar11);
  puVar1 = (undefined8 *)(param_1 + 0x498);
  FUN_140418b70(puVar1);
  puVar2 = (undefined8 *)(param_1 + 0x4c8);
  FUN_140418bf0(puVar2);
  local_190[0] = 0;
  lVar6 = FUN_1402d82e0("accounting_financing","Financing");
  local_188 = (void *)0x0;
  uStack_180 = 0;
  local_178 = (undefined8 ******)0x0;
  ppppppuStack_170 = (undefined8 ******)0x0;
  lVar20 = -1;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  FUN_140002c00(&local_188,lVar6);
  local_168 = 1;
  puVar3 = *(undefined4 **)(param_1 + 0x470);
  if (puVar3 < *(undefined4 **)(param_1 + 0x478)) {
    *(undefined4 **)(param_1 + 0x470) = puVar3 + 0xc;
    *puVar3 = local_190[0];
    *(void **)(puVar3 + 2) = local_188;
    *(undefined8 *)(puVar3 + 4) = uStack_180;
    *(undefined8 *******)(puVar3 + 6) = local_178;
    *(undefined8 *******)(puVar3 + 8) = ppppppuStack_170;
    local_188 = (void *)((ulonglong)local_188 & 0xffffffffffffff00);
    *(undefined1 *)(puVar3 + 10) = 1;
    ppppppuVar17 = (undefined8 ******)0xf;
  }
  else {
    FUN_14041a2a0(lVar15,local_190);
    ppppppuVar17 = ppppppuStack_170;
  }
  if (0xf < ppppppuVar17) {
    pvVar7 = local_188;
    if ((0xfff < (longlong)ppppppuVar17 + 1U) &&
       (pvVar7 = *(void **)((longlong)local_188 + -8),
       0x1f < (ulonglong)((longlong)local_188 + (-8 - (longlong)pvVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar7);
  }
  local_160 = 1;
  lVar6 = FUN_1402d82e0("accounting_financing_credit","Credit");
  local_158 = (void *)0x0;
  uStack_150 = 0;
  local_148 = 0;
  local_140 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  FUN_140002c00(&local_158,lVar6);
  local_138 = (char *)0x0;
  uStack_130 = 0;
  local_128 = (undefined8 ******)0x0;
  local_120 = 0;
  local_138 = (char *)FUN_140003270(0x20);
  uVar24 = s_financing_credit_140a70908._8_8_;
  local_128 = (undefined8 ******)&DAT_00000010;
  local_120 = 0x1f;
  *(undefined8 *)local_138 = s_financing_credit_140a70908._0_8_;
  *(undefined8 *)(local_138 + 8) = uVar24;
  local_138[0x10] = '\0';
  local_118 = 0;
  local_110 = &local_110;
  ppppppuStack_108 = &local_110;
  local_100 = (undefined8 *******)0x0;
  uStack_f8 = 0;
  local_f0 = 0;
  pvVar7 = (void *)thunk_FUN_140983da8(0x28);
  *(undefined4 *)((longlong)pvVar7 + 0x20) = 0;
  bVar4 = true;
  pppppppuVar8 = &local_110;
  pppppppuVar9 = (undefined8 *******)local_100;
  while (pppppppuVar9 != (undefined8 *******)0x0) {
    bVar4 = 0 < *(int *)(pppppppuVar9 + 4);
    pppppppuVar8 = pppppppuVar9;
    if (*(int *)(pppppppuVar9 + 4) < 1) {
      pppppppuVar9 = (undefined8 *******)*pppppppuVar9;
    }
    else {
      pppppppuVar9 = (undefined8 *******)pppppppuVar9[1];
    }
  }
  pppppppuVar9 = pppppppuVar8;
  if (bVar4) {
    if (pppppppuVar8 != (undefined8 *******)ppppppuStack_108) {
      pppppppuVar8 = (undefined8 *******)FUN_14001dcf0(pppppppuVar8);
      goto LAB_140407557;
    }
LAB_14040755d:
    FUN_14001de10(pvVar7,pppppppuVar9);
    local_f0 = local_f0 + 1;
  }
  else {
LAB_140407557:
    if (*(int *)(pppppppuVar8 + 4) < 0) goto LAB_14040755d;
    free(pvVar7);
  }
  local_e0 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_160);
  }
  pppppppuVar8 = (undefined8 *******)local_100;
  while (pppppppuVar8 != (undefined8 *******)0x0) {
    FUN_1402450d0(&local_110,*pppppppuVar8);
    pppppppuVar9 = (undefined8 *******)pppppppuVar8[1];
    free(pppppppuVar8);
    pppppppuVar8 = pppppppuVar9;
  }
  pppppppuVar8 = (undefined8 *******)0x0;
  if (0xf < local_120) {
    _Memory = local_138;
    if ((0xfff < local_120 + 1) &&
       (_Memory = *(char **)(local_138 + -8), (char *)0x1f < local_138 + (-8 - (longlong)_Memory)))
    goto LAB_140407669;
    free(_Memory);
  }
  local_120 = 0xf;
  local_138 = (char *)((ulonglong)local_138 & 0xffffffffffffff00);
  local_128 = pppppppuVar8;
  if (0xf < local_140) {
    pvVar7 = local_158;
    if ((0xfff < local_140 + 1) &&
       (pvVar7 = *(void **)((longlong)local_158 + -8),
       0x1f < (ulonglong)((longlong)local_158 + (-8 - (longlong)pvVar7)))) {
LAB_140407669:
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar7);
  }
  local_d8 = 2;
  lVar6 = FUN_1402d82e0("accounting_financing_interest","Interest");
  local_d0 = 0;
  uStack_c8 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_c0 = pppppppuVar8;
  local_b8 = pppppppuVar8;
  FUN_140002c00(&local_d0,lVar6);
  local_b0 = (char *)0x0;
  uStack_a8 = 0;
  local_a0 = pppppppuVar8;
  local_98 = pppppppuVar8;
  local_b0 = (char *)FUN_140003270(0x20);
  uVar24 = s_financing_interest_140a70a20._8_8_;
  local_a0 = (undefined8 ******)0x12;
  local_98 = (undefined8 ******)0x1f;
  *(undefined8 *)local_b0 = s_financing_interest_140a70a20._0_8_;
  *(undefined8 *)(local_b0 + 8) = uVar24;
  *(undefined2 *)(local_b0 + 0x10) = s_financing_interest_140a70a20._16_2_;
  local_b0[0x12] = '\0';
  uVar19 = 0;
  local_88 = &local_88;
  ppppppuStack_80 = &local_88;
  uStack_70 = 0;
  local_90 = uVar19;
  local_78 = pppppppuVar8;
  local_68 = pppppppuVar8;
  pvVar7 = (void *)thunk_FUN_140983da8(0x28);
  *(undefined4 *)((longlong)pvVar7 + 0x20) = 0;
  bVar4 = true;
  pppppppuVar9 = &local_88;
  pppppppuVar25 = (undefined8 *******)local_78;
  while (pppppppuVar25 != (undefined8 *******)0x0) {
    bVar4 = 0 < *(int *)(pppppppuVar25 + 4);
    pppppppuVar9 = pppppppuVar25;
    if (*(int *)(pppppppuVar25 + 4) < 1) {
      pppppppuVar25 = (undefined8 *******)*pppppppuVar25;
    }
    else {
      pppppppuVar25 = (undefined8 *******)pppppppuVar25[1];
    }
  }
  pppppppuVar25 = pppppppuVar9;
  if (bVar4) {
    if (pppppppuVar9 != (undefined8 *******)ppppppuStack_80) {
      pppppppuVar9 = (undefined8 *******)FUN_14001dcf0(pppppppuVar9);
      goto LAB_1404077da;
    }
  }
  else {
LAB_1404077da:
    if (-1 < *(int *)(pppppppuVar9 + 4)) {
      free(pvVar7);
      goto LAB_14040781e;
    }
  }
  if ((pppppppuVar25 == &local_88) || (uVar24 = 1, 0 < *(int *)(pppppppuVar25 + 4))) {
    uVar24 = 0;
  }
  FUN_14001de10(pvVar7,pppppppuVar25,&local_88,uVar24);
  local_68 = (undefined8 ******)((longlong)local_68 + 1);
LAB_14040781e:
  local_58 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_d8);
  }
  FUN_140353c00(&local_d8);
  local_190[0] = 1;
  lVar6 = FUN_1402d82e0("accounting_construction","Construction");
  local_188 = (void *)0x0;
  uStack_180 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_178 = pppppppuVar8;
  ppppppuStack_170 = pppppppuVar8;
  FUN_140002c00(&local_188,lVar6);
  local_168 = 1;
  puVar3 = *(undefined4 **)(param_1 + 0x470);
  if (puVar3 < *(undefined4 **)(param_1 + 0x478)) {
    *(undefined4 **)(param_1 + 0x470) = puVar3 + 0xc;
    *puVar3 = local_190[0];
    puVar3[2] = (undefined4)local_188;
    puVar3[3] = local_188._4_4_;
    puVar3[4] = (undefined4)uStack_180;
    puVar3[5] = uStack_180._4_4_;
    *(undefined8 *******)(puVar3 + 6) = local_178;
    *(undefined8 *******)(puVar3 + 8) = ppppppuStack_170;
    local_188 = (void *)((ulonglong)local_188 & 0xffffffffffffff00);
    *(undefined1 *)(puVar3 + 10) = 1;
    pppppppuVar9 = (undefined8 *******)0xf;
  }
  else {
    FUN_14041a2a0(lVar15,local_190);
    pppppppuVar9 = (undefined8 *******)ppppppuStack_170;
  }
  if ((undefined8 *******)0xf < pppppppuVar9) {
    FUN_140003040(&local_188,local_188);
  }
  local_218 = 5;
  local_214 = 1;
  lVar6 = FUN_1402d82e0("accounting_construction_tracks","Tracks");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_construction_tracks_140a709c0._8_8_;
  local_1e0 = (undefined8 ******)0x13;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_construction_tracks_140a709c0._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined2 *)(local_1f0 + 0x10) = s_construction_tracks_140a709c0._16_2_;
  local_1f0[0x12] = s_construction_tracks_140a709c0[0x12];
  local_1f0[0x13] = '\0';
  local_res8 = (undefined8 ********)((ulonglong)local_res8._4_4_ << 0x20);
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)((longlong)&local_res8 + 4);
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 8;
  local_214 = 1;
  lVar6 = FUN_1402d82e0("accounting_construction_buildings","Buildings");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_construction_buildings_140a70b10._8_8_;
  local_1e0 = (undefined8 ******)0x16;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_construction_buildings_140a70b10._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined4 *)(local_1f0 + 0x10) = s_construction_buildings_140a70b10._16_4_;
  *(undefined2 *)(local_1f0 + 0x14) = s_construction_buildings_140a70b10._20_2_;
  local_1f0[0x16] = '\0';
  local_res8 = (undefined8 ********)((ulonglong)local_res8 & 0xffffffff00000000);
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)((longlong)&local_res8 + 4);
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 6;
  local_214 = 1;
  lVar6 = FUN_1402d82e0("accounting_construction_stations","Platforms");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_construction_stations_140a70ac0._8_8_;
  local_1e0 = (undefined8 ******)0x15;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_construction_stations_140a70ac0._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined4 *)(local_1f0 + 0x10) = s_construction_stations_140a70ac0._16_4_;
  local_1f0[0x14] = s_construction_stations_140a70ac0[0x14];
  local_1f0[0x15] = '\0';
  local_res8 = (undefined8 ********)((ulonglong)local_res8 & 0xffffffff00000000);
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)((longlong)&local_res8 + 4);
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 7;
  local_214 = 1;
  lVar6 = FUN_1402d82e0("accounting_construction_depots","Depots");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_construction_depots_140a70a80._8_8_;
  local_1e0 = (undefined8 ******)0x13;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_construction_depots_140a70a80._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined2 *)(local_1f0 + 0x10) = s_construction_depots_140a70a80._16_2_;
  local_1f0[0x12] = s_construction_depots_140a70a80[0x12];
  local_1f0[0x13] = '\0';
  local_res8 = (undefined8 ********)((ulonglong)local_res8 & 0xffffffff00000000);
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)((longlong)&local_res8 + 4);
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 4;
  local_214 = 1;
  lVar6 = FUN_1402d82e0("accounting_construction_total","Total construction");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_construction_total_140a70bb0._8_8_;
  local_1e0 = (undefined8 ******)0x12;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_construction_total_140a70bb0._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined2 *)(local_1f0 + 0x10) = s_construction_total_140a70bb0._16_2_;
  local_1f0[0x12] = '\0';
  local_res8 = (undefined8 ********)((ulonglong)local_res8 & 0xffffffff00000000);
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)((longlong)&local_res8 + 4);
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_190[0] = 2;
  lVar6 = FUN_1402d82e0("accounting_trains","Trains");
  local_188 = (void *)0x0;
  uStack_180 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_178 = pppppppuVar8;
  ppppppuStack_170 = pppppppuVar8;
  FUN_140002c00(&local_188,lVar6);
  local_168 = 1;
  puVar3 = *(undefined4 **)(param_1 + 0x470);
  if (puVar3 < *(undefined4 **)(param_1 + 0x478)) {
    *(undefined4 **)(param_1 + 0x470) = puVar3 + 0xc;
    *puVar3 = local_190[0];
    puVar3[2] = (undefined4)local_188;
    puVar3[3] = local_188._4_4_;
    puVar3[4] = (undefined4)uStack_180;
    puVar3[5] = uStack_180._4_4_;
    *(undefined8 *******)(puVar3 + 6) = local_178;
    *(undefined8 *******)(puVar3 + 8) = ppppppuStack_170;
    local_188 = (void *)((ulonglong)local_188 & 0xffffffffffffff00);
    *(undefined1 *)(puVar3 + 10) = 1;
    pppppppuVar9 = (undefined8 *******)0xf;
  }
  else {
    FUN_14041a2a0(lVar15,local_190);
    pppppppuVar9 = (undefined8 *******)ppppppuStack_170;
  }
  if ((undefined8 *******)0xf < pppppppuVar9) {
    FUN_140003040(&local_188,local_188);
  }
  local_218 = 9;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_purchases","New trains");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_trains_purchases_140a70b50._8_8_;
  local_1e0 = (undefined8 ******)0x10;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_purchases_140a70b50._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  local_1f0[0x10] = '\0';
  local_res8 = (undefined8 ********)0x200000000;
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)local_res10;
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 10;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_sales","Train sales");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1e0 = (undefined8 ******)0xc;
  local_1d8 = (undefined8 ******)0xf;
  local_1f0 = (char *)s_trains_sales_140a70c80._0_8_;
  uStack_1e8 = (ulonglong)(uint)s_trains_sales_140a70c80._8_4_;
  local_res8 = (undefined8 ********)((ulonglong)local_res8 & 0xffffffff00000000);
  local_238 = (undefined8 **********)&local_res8;
  uStack_230 = (undefined4 *)((longlong)&local_res8 + 4);
  local_228 = local_238;
  puStack_220 = (undefined1 *)uStack_230;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0xb;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_running","Running costs");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1e0 = (undefined8 ******)0xe;
  local_1d8 = (undefined8 ******)0xf;
  local_1f0 = (char *)s_trains_running_140a70c40._0_8_;
  uStack_1e8 = (ulonglong)
               CONCAT24(s_trains_running_140a70c40._12_2_,s_trains_running_140a70c40._8_4_);
  local_238 = (undefined8 **********)0x200000000;
  uStack_230 = (undefined4 *)CONCAT44(uStack_230._4_4_,3);
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0xc;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_maintenance","Maintenance");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_trains_maintenance_140a70bf8._8_8_;
  local_1e0 = (undefined8 ******)0x12;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_maintenance_140a70bf8._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined2 *)(local_1f0 + 0x10) = s_trains_maintenance_140a70bf8._16_2_;
  local_1f0[0x12] = '\0';
  local_238 = (undefined8 **********)0x200000000;
  uStack_230 = (undefined4 *)CONCAT44(uStack_230._4_4_,3);
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0xd;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_intervention","Interventions");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_trains_intervention_140a70d30._8_8_;
  local_1e0 = (undefined8 ******)0x13;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_intervention_140a70d30._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined2 *)(local_1f0 + 0x10) = s_trains_intervention_140a70d30._16_2_;
  local_1f0[0x12] = s_trains_intervention_140a70d30[0x12];
  local_1f0[0x13] = '\0';
  local_238 = (undefined8 **********)0x200000000;
  uStack_230 = (undefined4 *)CONCAT44(uStack_230._4_4_,3);
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0xe;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_running_distance","Running distance");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_trains_running_distance_140a70cd8._8_8_;
  local_1e0 = (undefined8 ******)0x17;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_running_distance_140a70cd8._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined4 *)(local_1f0 + 0x10) = s_trains_running_distance_140a70cd8._16_4_;
  *(undefined2 *)(local_1f0 + 0x14) = s_trains_running_distance_140a70cd8._20_2_;
  local_1f0[0x16] = s_trains_running_distance_140a70cd8[0x16];
  local_1f0[0x17] = '\0';
  local_1d0 = 2;
  local_238 = (undefined8 **********)0x200000000;
  uStack_230 = (undefined4 *)CONCAT44(uStack_230._4_4_,3);
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0xf;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_running_time","Running time");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_trains_running_time_140a70c90._8_8_;
  local_1e0 = (undefined8 ******)0x13;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_running_time_140a70c90._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  *(undefined2 *)(local_1f0 + 0x10) = s_trains_running_time_140a70c90._16_2_;
  local_1f0[0x12] = s_trains_running_time_140a70c90[0x12];
  local_1f0[0x13] = '\0';
  local_1d0 = 3;
  local_238 = (undefined8 **********)0x200000000;
  uStack_230 = (undefined4 *)CONCAT44(uStack_230._4_4_,3);
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0x10;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_stop_time","Scheduled stop");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar24 = s_trains_stop_time_140a70df8._8_8_;
  local_1e0 = (undefined8 ******)0x10;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_stop_time_140a70df8._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar24;
  local_1f0[0x10] = '\0';
  uVar24 = _UNK_140aacd28;
  pppppuVar5 = DAT_140aacd20;
  local_1d0 = 3;
  local_228 = (undefined8 *********)DAT_140aacd20;
  puStack_220 = (undefined1 *)_UNK_140aacd28;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0x12;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_extra_stop_time","Extra stop");
  local_210 = 0;
  uStack_208 = 0;
  lVar23 = -1;
  do {
    lVar23 = lVar23 + 1;
  } while (*(char *)(lVar6 + lVar23) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6);
  local_1f0 = (char *)0x0;
  uStack_1e8 = 0;
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  local_1f0 = (char *)FUN_140003270(0x20);
  uVar10 = s_trains_extra_stop_time_140a70da8._8_8_;
  local_1e0 = (undefined8 ******)0x16;
  local_1d8 = (undefined8 ******)0x1f;
  *(undefined8 *)local_1f0 = s_trains_extra_stop_time_140a70da8._0_8_;
  *(undefined8 *)(local_1f0 + 8) = uVar10;
  *(undefined4 *)(local_1f0 + 0x10) = s_trains_extra_stop_time_140a70da8._16_4_;
  *(undefined2 *)(local_1f0 + 0x14) = s_trains_extra_stop_time_140a70da8._20_2_;
  local_1f0[0x16] = '\0';
  local_1d0 = 3;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  if (*(ulonglong *)(param_1 + 0x488) < *(ulonglong *)(param_1 + 0x490)) {
    *(ulonglong *)(param_1 + 0x488) = *(ulonglong *)(param_1 + 0x488) + 0x88;
    FUN_140419920();
  }
  else {
    FUN_14041a420(puVar11,&local_218);
  }
  FUN_140353c00(&local_218);
  local_218 = 0x14;
  local_214 = 2;
  lVar6 = FUN_1402d82e0("accounting_trains_late_arrival_time","Late arrival >10s");
  local_210 = 0;
  uStack_208 = 0;
  do {
    lVar20 = lVar20 + 1;
  } while (*(char *)(lVar6 + lVar20) != '\0');
  local_200 = pppppppuVar8;
  local_1f8 = pppppppuVar8;
  FUN_140002c00(&local_210,lVar6,lVar20);
  FUN_140003200(&local_1f0);
  local_1e0 = pppppppuVar8;
  local_1d8 = pppppppuVar8;
  FUN_140002c00(&local_1f0,"trains_late_arrival_time",0x18);
  local_1d0 = 3;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230._4_4_ = (undefined4)((ulonglong)&local_218 >> 0x20);
  uStack_230._0_4_ = SUB84(&local_218,0);
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x13;
  local_214 = 2;
  uVar10 = FUN_1402d82e0("accounting_trains_signal_stop_time","Signal wait");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"trains_signal_stop_time");
  local_1d0 = 3;
  local_238 = (undefined8 **********)0x200000000;
  uStack_230._0_4_ = 3;
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x15;
  local_214 = 2;
  uVar10 = FUN_1402d82e0("accounting_trains_collisions","Collisions");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"trains_collisions");
  local_1d0 = 10;
  local_238 = (undefined8 **********)0x200000000;
  uStack_230 = (undefined4 *)CONCAT44(uStack_230._4_4_,3);
  local_228 = &local_238;
  puStack_220 = (undefined1 *)((longlong)&uStack_230 + 4);
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x11;
  local_214 = 2;
  uVar10 = FUN_1402d82e0("accounting_trains_departure","Departures");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"trains_departures");
  local_1d0 = 10;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x2b;
  local_214 = 2;
  uVar10 = FUN_1402d82e0("accounting_train_arrived_full","Arrived full");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"train_arrived_full");
  local_1d0 = 10;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x28;
  local_214 = 2;
  uVar10 = FUN_1402d82e0("accounting_train_departed_full","Departed full");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"train_departed_full");
  local_1d0 = 10;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_190[0] = 3;
  uVar10 = FUN_1402d82e0("accounting_ticketing","Ticketing");
  FUN_140002b40(&local_188,uVar10);
  local_168 = 1;
  FUN_140418a20(lVar15,local_190);
  FUN_140002d30(&local_188);
  local_218 = 0x16;
  local_214 = 3;
  uVar10 = FUN_1402d82e0("accounting_ticketing_fares","Fares");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"ticketing_fares");
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x17;
  local_214 = 3;
  uVar10 = FUN_1402d82e0("accounting_ticketing_refunds","Refunds");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"ticketing_refunds");
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x18;
  local_214 = 3;
  uVar10 = FUN_1402d82e0("accounting_ticketing_compensations","Compensations");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"ticketing_compensations");
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_190[0] = 5;
  uVar10 = FUN_1402d82e0("accounting_cash_total","$ Total");
  FUN_140002b40(&local_188,uVar10);
  local_168 = 0;
  FUN_140418a20(lVar15,local_190);
  FUN_140002d30(&local_188);
  local_214 = 5;
  local_218 = uVar19;
  uVar10 = FUN_1402d82e0("accounting_cash_total","$ Total");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"cash_total");
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  local_1d0 = uVar19;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 1;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_190[0] = 4;
  uVar10 = FUN_1402d82e0("accounting_pax","Passengers");
  FUN_140002b40(&local_188,uVar10);
  local_168 = 1;
  FUN_140418a20(lVar15,local_190);
  FUN_140002d30(&local_188);
  local_218 = 0x20;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_new","New pax");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_new");
  local_1d0 = 1;
  local_res8 = (undefined8 ********)0x100000000;
  local_228 = (undefined8 *********)&local_res8;
  puStack_220 = local_res10;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x27;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_never_serviced","Never serviced");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_never_serviced");
  local_1d0 = 1;
  local_res8 = (undefined8 ********)0x100000000;
  local_228 = (undefined8 *********)&local_res8;
  puStack_220 = local_res10;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x26;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_spawned","Started trip");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_spawned");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x21;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_boarded","Boarded train");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_boarded");
  local_1d0 = 10;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x29;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_walked_in","Walked in");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_walked_in");
  local_1d0 = 10;
  local_res8 = (undefined8 ********)0x100000000;
  local_228 = (undefined8 *********)&local_res8;
  puStack_220 = local_res10;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x2a;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_walked_out","Walked out");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_walked_out");
  local_1d0 = 10;
  local_res8 = (undefined8 ********)0x100000000;
  local_228 = (undefined8 *********)&local_res8;
  puStack_220 = local_res10;
  FUN_140417ef0(local_1c8,&local_228);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x1f;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_reached_transfer","Reached transfer");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_reached_transfer");
  local_1d0 = 10;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x1a;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_reached_destination","Reached destination");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_reached_destination");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x24;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_found_station_too_full","Compen.: full station");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_found_station_too_full");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x22;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_waited_too_long","Compen.: waited too long");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_waited_too_long");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x1c;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_lost","Compen.: lost");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_lost");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x25;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_intervention","Compen.: intervention");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_intervention");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x1e;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_compensated","Compensated");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_compensated");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x23;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_refused_fare","Refund: too high fare");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_refused_fare");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x1b;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_reached_depot","Refund: reached depot");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_reached_depot");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  local_218 = 0x1d;
  local_214 = 4;
  uVar10 = FUN_1402d82e0("accounting_pax_refunded","Refunded (incl. compen.)");
  FUN_140002b40(&local_210,uVar10);
  FUN_140002b40(&local_1f0,"pax_refunded");
  local_1d0 = 1;
  local_228 = (undefined8 *********)pppppuVar5;
  puStack_220 = (undefined1 *)uVar24;
  local_238 = &local_228;
  uStack_230 = &local_218;
  FUN_140417ef0(local_1c8,&local_238);
  local_198 = 0;
  FUN_140418ae0(puVar11,&local_218);
  FUN_140353c00(&local_218);
  ppppppppuVar21 = (undefined8 ********)*puVar11;
  ppppppppuVar22 = *(undefined8 *********)(param_1 + 0x488);
  local_res8 = ppppppppuVar22;
  if (ppppppppuVar21 != ppppppppuVar22) {
    do {
      puVar11 = puVar1;
      if (*(undefined8 **)(param_1 + 0x4a8) != (undefined8 *)0x0) {
        puVar13 = *(undefined8 **)(param_1 + 0x4a8);
        do {
          if (*(int *)(puVar13 + 4) < *(int *)((longlong)ppppppppuVar21 + 4)) {
            puVar14 = (undefined8 *)*puVar13;
          }
          else {
            puVar14 = (undefined8 *)puVar13[1];
            puVar11 = puVar13;
          }
          puVar13 = puVar14;
        } while (puVar14 != (undefined8 *)0x0);
      }
      if ((puVar11 == puVar1) || (*(int *)((longlong)ppppppppuVar21 + 4) < *(int *)(puVar11 + 4))) {
        puVar11 = (undefined8 *)FUN_14041b110(puVar1,local_res10);
        puVar11 = (undefined8 *)*puVar11;
      }
      piVar12 = (int *)puVar11[6];
      if (piVar12 < (int *)puVar11[7]) {
        puVar11[6] = piVar12 + 1;
        *piVar12 = *(int *)ppppppppuVar21;
      }
      else {
        lVar15 = (longlong)piVar12 - puVar11[5] >> 2;
        if (lVar15 == 0) {
          lVar15 = 1;
LAB_140409a2b:
          piVar12 = (int *)thunk_FUN_140983da8(lVar15 * 4);
        }
        else {
          lVar15 = lVar15 * 2;
          if (lVar15 != 0) goto LAB_140409a2b;
          piVar12 = (int *)0x0;
        }
        pvVar7 = (void *)puVar11[5];
        piVar16 = piVar12;
        if (pvVar7 != (void *)puVar11[6]) {
          _Size = (longlong)puVar11[6] - (longlong)pvVar7;
          pvVar7 = memmove(piVar12,pvVar7,_Size);
          piVar16 = (int *)((longlong)pvVar7 + ((longlong)_Size >> 2) * 4);
        }
        *piVar16 = *(int *)ppppppppuVar21;
        if ((void *)puVar11[5] != (void *)0x0) {
          free((void *)puVar11[5]);
        }
        puVar11[5] = piVar12;
        puVar11[6] = piVar16 + 1;
        puVar11[7] = piVar12 + lVar15;
        ppppppppuVar22 = local_res8;
      }
      puVar11 = puVar2;
      if (*(undefined8 **)(param_1 + 0x4d8) != (undefined8 *)0x0) {
        puVar13 = *(undefined8 **)(param_1 + 0x4d8);
        do {
          if (*(int *)(puVar13 + 4) < *(int *)ppppppppuVar21) {
            puVar14 = (undefined8 *)*puVar13;
          }
          else {
            puVar14 = (undefined8 *)puVar13[1];
            puVar11 = puVar13;
          }
          puVar13 = puVar14;
        } while (puVar14 != (undefined8 *)0x0);
      }
      if ((puVar11 == puVar2) || (*(int *)ppppppppuVar21 < *(int *)(puVar11 + 4))) {
        puVar11 = (undefined8 *)FUN_14041b2e0(puVar2,local_res18);
        puVar11 = (undefined8 *)*puVar11;
      }
      *(int *)(puVar11 + 5) = *(int *)ppppppppuVar21;
      *(int *)((longlong)puVar11 + 0x2c) = *(int *)((longlong)ppppppppuVar21 + 4);
      ppppppppuVar18 = ppppppppuVar21 + 1;
      if ((undefined8 ********)(puVar11 + 6) != ppppppppuVar18) {
        if ((undefined8 *******)0xf < ppppppppuVar21[4]) {
          ppppppppuVar18 = (undefined8 ********)*ppppppppuVar18;
        }
        FUN_140030630(puVar11 + 6,ppppppppuVar18,ppppppppuVar21[3]);
      }
      ppppppppuVar18 = ppppppppuVar21 + 5;
      if ((undefined8 ********)(puVar11 + 10) != ppppppppuVar18) {
        if ((undefined8 *******)0xf < ppppppppuVar21[8]) {
          ppppppppuVar18 = (undefined8 ********)*ppppppppuVar18;
        }
        FUN_140030630(puVar11 + 10,ppppppppuVar18,ppppppppuVar21[7]);
      }
      *(int *)(puVar11 + 0xe) = *(int *)(ppppppppuVar21 + 9);
      FUN_140418fc0(puVar11 + 0xf,ppppppppuVar21 + 10);
      *(undefined1 *)(puVar11 + 0x15) = *(undefined1 *)(ppppppppuVar21 + 0x10);
      ppppppppuVar21 = ppppppppuVar21 + 0x11;
    } while (ppppppppuVar21 != ppppppppuVar22);
  }
  return;
}


// Incoming references
// 0xc217d0 DATA caller none
// 0x489c07 UNCONDITIONAL_CALL caller 140489ad0
