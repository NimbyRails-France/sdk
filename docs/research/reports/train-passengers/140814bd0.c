// Candidate VA 140814bd0; RVA 0x814bd0
// Ghidra inferred prototype: undefined FUN_140814bd0()

longlong FUN_140814bd0(longlong param_1,longlong param_2,longlong param_3,undefined8 param_4,
                      float param_5,float param_6,float param_7,float param_8,float param_9,
                      float param_10,float param_11,float param_12,undefined4 param_13,
                      undefined4 param_14,uint param_15,char *param_16,char *param_17,
                      undefined8 *param_18)

{
  undefined8 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  char cVar4;
  byte bVar5;
  int iVar6;
  longlong lVar7;
  undefined8 *puVar8;
  undefined8 uVar9;
  byte *pbVar10;
  uint *puVar11;
  uint uVar12;
  undefined4 uVar13;
  uint uVar14;
  undefined4 uVar15;
  undefined8 uVar16;
  undefined4 *puVar17;
  longlong lVar18;
  uint *_Dst;
  void *pvVar19;
  ulonglong uVar20;
  undefined4 *puVar21;
  char *pcVar22;
  char ****ppppcVar23;
  longlong lVar24;
  int iVar25;
  char *pcVar26;
  float *pfVar27;
  uint *puVar28;
  longlong lVar29;
  char *****pppppcVar30;
  float fVar31;
  undefined4 uVar32;
  undefined4 uVar33;
  undefined4 uVar34;
  undefined4 uVar35;
  undefined4 in_XMM3_Dc;
  undefined4 uVar36;
  undefined4 in_XMM3_Dd;
  undefined4 uVar37;
  float fVar38;
  undefined1 auVar39 [16];
  undefined1 auVar40 [16];
  undefined1 auVar41 [16];
  float fVar43;
  float fVar44;
  float fVar45;
  float fVar46;
  float fVar47;
  undefined1 auVar48 [16];
  float fVar49;
  float fVar50;
  undefined8 in_stack_fffffffffffffc78;
  undefined4 uVar51;
  float local_378;
  float fStack_374;
  float local_370;
  float local_36c;
  char local_361;
  char ****local_358;
  undefined8 uStack_350;
  ulonglong local_348;
  ulonglong local_340;
  float local_338;
  float fStack_334;
  float fStack_330;
  float fStack_32c;
  undefined8 local_328;
  longlong local_320 [3];
  longlong local_308 [4];
  undefined1 local_2e8;
  undefined4 local_2e4;
  undefined1 local_2e0;
  float local_2dc;
  undefined1 local_2d8;
  float local_2d4;
  void *local_2a8;
  undefined1 local_290 [24];
  undefined1 local_278 [24];
  float local_260;
  float local_25c;
  longlong local_258 [4];
  undefined1 local_238;
  undefined4 local_234;
  undefined1 local_230;
  float local_22c;
  undefined1 local_228;
  float local_224;
  float local_1b0;
  float local_1ac;
  float local_1a8;
  undefined4 local_1a4;
  undefined4 local_1a0;
  undefined1 local_198 [8];
  ulonglong local_190;
  int local_184;
  uint *local_180;
  uint *local_178;
  uint *local_170;
  undefined8 *local_108;
  undefined4 local_f0;
  undefined4 local_ec;
  char *local_e8;
  char *local_e0;
  undefined1 auVar42 [16];

  uVar15 = (undefined4)((ulonglong)param_4 >> 0x20);
  uVar51 = (undefined4)((ulonglong)in_stack_fffffffffffffc78 >> 0x20);
  auVar39._8_4_ = in_XMM3_Dc;
  auVar39._0_8_ = param_4;
  auVar39._12_4_ = in_XMM3_Dd;
  pcVar26 = param_16;
  if (param_17 != (char *)0x0) {
    pcVar26 = param_17;
  }
  uVar16 = FUN_1402f20b0(pcVar26);
  FUN_14055d040(local_308,param_3,uVar16);
  fVar44 = DAT_140aabae4;
  lVar24 = param_3 + 0x16f8;
  local_1a8 = param_12;
  local_1a4 = param_13;
  local_1a0 = param_14;
  uVar12 = param_15 & 0x40;
  if (uVar12 == 0) {
    if ((param_15 & 0x40000000) == 0) {
      fVar43 = *(float *)(param_3 + 0x6178);
      puVar17 = (undefined4 *)(param_3 + 0x616c);
      puVar21 = (undefined4 *)(param_3 + 0x6170);
      pfVar27 = (float *)(param_3 + 0x6174);
    }
    else {
      fVar43 = *(float *)(param_3 + 0x6188);
      puVar17 = (undefined4 *)(param_3 + 0x617c);
      puVar21 = (undefined4 *)(param_3 + 0x6180);
      pfVar27 = (float *)(param_3 + 0x6184);
    }
  }
  else {
    fVar43 = *(float *)(*(longlong *)(param_3 + 0x6198) + 8);
    if ((param_15 & 0x40000000) == 0) {
      fVar43 = *(float *)(param_3 + 0x6158) + fVar43 + *(float *)(param_3 + 0x6158);
      puVar17 = (undefined4 *)(param_3 + 0x614c);
      puVar21 = (undefined4 *)(param_3 + 0x6150);
      pfVar27 = (float *)(param_3 + 0x6154);
    }
    else {
      fVar43 = *(float *)(param_3 + 0x6168) + fVar43 + *(float *)(param_3 + 0x6168);
      puVar17 = (undefined4 *)(param_3 + 0x615c);
      puVar21 = (undefined4 *)(param_3 + 0x6160);
      pfVar27 = (float *)(param_3 + 0x6164);
    }
  }
  fVar31 = *pfVar27;
  uVar2 = *puVar21;
  uVar3 = *puVar17;
  if ((param_17 != (char *)0x0) && ((param_15 & 4) != 0)) {
    iVar25 = 0;
    pcVar22 = param_17;
    do {
      cVar4 = *pcVar22;
      pcVar22 = pcVar22 + 1;
      if (cVar4 == '\0') break;
      iVar25 = iVar25 + 1;
    } while (pcVar22 != (char *)0x0);
    uVar13 = FUN_1404faac0(param_17,iVar25,0x40);
    lVar18 = FUN_140507c40(lVar24,uVar13,param_17);
    if (lVar18 != 0) {
      local_260 = (*(float *)(lVar18 + 0x54) - fVar31) * (fVar44 / param_12);
      local_25c = (*(float *)(lVar18 + 0x58) - fVar43) * (fVar44 / param_12);
      local_1b0 = local_260;
      local_1ac = local_25c;
    }
  }
  uVar14 = FUN_14081bda0(local_198);
  if (local_178 < local_170) {
    *local_178 = uVar14;
    goto LAB_140814efd;
  }
  lVar18 = (longlong)local_178 - (longlong)local_180 >> 2;
  if (lVar18 == 0) {
    lVar29 = 1;
LAB_140814e72:
    lVar18 = lVar29 * 4;
    _Dst = (uint *)thunk_FUN_140983da8(lVar18);
    puVar28 = local_178;
  }
  else {
    lVar29 = lVar18 * 2;
    if (lVar29 != 0) goto LAB_140814e72;
    _Dst = (uint *)0x0;
    lVar18 = lVar18 << 3;
    puVar28 = local_178;
  }
  puVar11 = local_180;
  local_178 = _Dst;
  if (local_180 != puVar28) {
    pvVar19 = memmove(_Dst,local_180,(longlong)puVar28 - (longlong)local_180);
    local_178 = (uint *)((longlong)pvVar19 + ((longlong)puVar28 - (longlong)puVar11 >> 2) * 4);
  }
  *local_178 = uVar14;
  if (local_180 != (uint *)0x0) {
    free(local_180);
  }
  local_170 = (uint *)((longlong)_Dst + lVar18);
  local_180 = _Dst;
LAB_140814efd:
  local_178 = local_178 + 1;
  fVar47 = 0.0;
  if (*(float *)*param_18 != 0.0) {
    local_230 = 1;
    local_22c = *(float *)*param_18;
  }
  if (*(float *)param_18[1] != 0.0) {
    local_228 = 1;
    local_224 = *(float *)param_18[1];
  }
  local_238 = 1;
  local_234 = 3;
  FUN_14055b8e0(local_258,param_18[2]);
  (**(code **)(local_258[0] + 8))(local_258);
  FUN_1407fe960(param_18[3],local_258);
  (**(code **)(local_258[0] + 0x18))(local_258);
  local_178 = local_178 + -1;
  if (local_184 != 0) {
    FUN_14081be40(local_198,0,0);
    FUN_14081c160(local_198,0,0);
    FUN_14081be40(local_198,0,1);
    FUN_14081c160(local_198,0,1);
  }
  local_e8 = param_16;
  local_e0 = param_17;
  lVar18 = local_190 + (ulonglong)uVar14 * 0x10;
  fVar50 = *(float *)(lVar18 + 8) + fVar31;
  fVar49 = *(float *)(lVar18 + 0xc) + fVar43;
  auVar40._4_12_ = auVar39._4_12_;
  auVar40._0_4_ = (float)param_4 - fVar50 * param_6;
  iVar25 = (int)auVar40._0_4_;
  if ((iVar25 != -0x80000000) && ((float)iVar25 != auVar40._0_4_)) {
    auVar42._0_8_ = auVar40._0_8_;
    auVar42._8_4_ = uVar15;
    auVar42._12_4_ = uVar15;
    auVar41._8_8_ = auVar42._8_8_;
    auVar41._4_4_ = auVar40._0_4_;
    auVar41._0_4_ = auVar40._0_4_;
    uVar14 = movmskps((int)local_190,auVar41);
    local_190 = (ulonglong)(uVar14 & 1);
    auVar40 = ZEXT416((uint)(float)(int)(iVar25 - (uVar14 & 1)));
  }
  fVar38 = auVar40._0_4_;
  auVar48._0_4_ = param_5 - fVar49 * param_7;
  iVar25 = (int)auVar48._0_4_;
  if ((iVar25 != -0x80000000) && ((float)iVar25 != auVar48._0_4_)) {
    auVar48._4_4_ = auVar48._0_4_;
    auVar48._8_8_ = 0;
    uVar14 = movmskps((int)local_190,auVar48);
    auVar48._0_4_ = (float)(int)(iVar25 - (uVar14 & 1));
  }
  if (uVar12 == 0) {
    puVar1 = (undefined8 *)(param_3 + 0x3ae8);
    if ((param_15 >> 0x1a & 1) == 0) {
      uVar15 = *(undefined4 *)(param_3 + 0x62c0);
      uVar13 = *(undefined4 *)(param_3 + 0x62c4);
      uVar32 = *(undefined4 *)(param_3 + 0x62c8);
      uVar33 = *(undefined4 *)(param_3 + 0x62cc);
      uVar34 = *(undefined4 *)(param_3 + 0x62d0);
      uVar35 = *(undefined4 *)(param_3 + 0x62d4);
      uVar36 = *(undefined4 *)(param_3 + 0x62d8);
      uVar37 = *(undefined4 *)(param_3 + 0x62dc);
      uVar16 = *(undefined8 *)(param_3 + 0x62e0);
    }
    else {
      uVar15 = *(undefined4 *)(param_3 + 0x62e8);
      uVar13 = *(undefined4 *)(param_3 + 0x62ec);
      uVar32 = *(undefined4 *)(param_3 + 0x62f0);
      uVar33 = *(undefined4 *)(param_3 + 0x62f4);
      uVar34 = *(undefined4 *)(param_3 + 0x62f8);
      uVar35 = *(undefined4 *)(param_3 + 0x62fc);
      uVar36 = *(undefined4 *)(param_3 + 0x6300);
      uVar37 = *(undefined4 *)(param_3 + 0x6304);
      uVar16 = *(undefined8 *)(param_3 + 0x6308);
    }
    if (lVar24 != 0) {
      iVar25 = *(int *)(param_3 + 0x3c70);
      lVar18 = (longlong)iVar25;
      if (iVar25 < 0x10) {
        *(int *)(param_3 + 0x3c70) = iVar25 + 1;
        *(undefined8 **)(param_3 + 0x3c78 + lVar18 * 0x30) = puVar1;
        uVar9 = *(undefined8 *)(param_3 + 0x3af0);
        puVar8 = (undefined8 *)(param_3 + 0x3c80 + lVar18 * 0x30);
        *puVar8 = *puVar1;
        puVar8[1] = uVar9;
        uVar9 = *(undefined8 *)(param_3 + 0x3b00);
        puVar8 = (undefined8 *)(param_3 + 0x3c90 + lVar18 * 0x30);
        *puVar8 = *(undefined8 *)(param_3 + 0x3af8);
        puVar8[1] = uVar9;
        *(undefined8 *)(param_3 + 0x3ca0 + lVar18 * 0x30) = *(undefined8 *)(param_3 + 0x3b08);
        *(undefined4 *)puVar1 = uVar15;
        *(undefined4 *)(param_3 + 0x3aec) = uVar13;
        *(undefined4 *)(param_3 + 0x3af0) = uVar32;
        *(undefined4 *)(param_3 + 0x3af4) = uVar33;
        *(undefined4 *)(param_3 + 0x3af8) = uVar34;
        *(undefined4 *)(param_3 + 0x3afc) = uVar35;
        *(undefined4 *)(param_3 + 0x3b00) = uVar36;
        *(undefined4 *)(param_3 + 0x3b04) = uVar37;
        *(undefined8 *)(param_3 + 0x3b08) = uVar16;
      }
    }
  }
  else {
    uVar16 = *(undefined8 *)(param_3 + 0x6198);
    if (lVar24 != 0) {
      iVar25 = *(int *)(param_3 + 0x4698);
      if (iVar25 < 8) {
        *(int *)(param_3 + 0x4698) = iVar25 + 1;
        puVar1 = (undefined8 *)(param_3 + 0x1950);
        *(undefined8 **)(param_3 + 0x46a0 + (longlong)iVar25 * 0x10) = puVar1;
        *(undefined8 *)(param_3 + 0x46a8 + (longlong)iVar25 * 0x10) = *puVar1;
        *puVar1 = uVar16;
      }
    }
  }
  iVar25 = 0;
  pcVar22 = pcVar26;
  while ((pcVar22 != (char *)0x0 && (cVar4 = *pcVar22, pcVar22 = pcVar22 + 1, cVar4 != '\0'))) {
    iVar25 = iVar25 + 1;
  }
  local_f0 = uVar3;
  local_ec = uVar2;
  uVar15 = FUN_1404faac0(pcVar26,iVar25,0x40);
  uVar20 = FUN_140507c40(lVar24,uVar15,pcVar26);
  fVar45 = fVar50;
  fVar46 = fVar49;
  if ((param_15 & 4) != 0) {
    if (uVar20 == 0) {
      if (param_8 <= param_10) {
        param_8 = param_10;
      }
      fVar45 = param_8 + fVar31;
      if (param_9 <= param_11) {
        param_9 = param_11;
      }
      fVar46 = param_9 + fVar43;
    }
    else {
      fVar45 = fVar31 + param_8;
      fVar46 = fVar43 + param_9;
    }
  }
  lVar18 = -1;
  if (((param_15 & 0x8000000) != 0) && (uVar20 == 0)) {
    local_358 = (char ****)0x0;
    uStack_350 = 0;
    local_348 = 0;
    local_340 = 0;
    lVar29 = -1;
    do {
      lVar29 = lVar29 + 1;
    } while (param_17[lVar29] != '\0');
    FUN_140002c00(&local_358,param_17);
    FUN_14081dce0(param_3,&local_338);
    if (0xf < local_340) {
      pppppcVar30 = (char *****)local_358;
      if ((0xfff < local_340 + 1) &&
         (pppppcVar30 = (char *****)local_358[-1],
         (char *)0x1f < (char *)((longlong)local_358 + (-8 - (longlong)pppppcVar30)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppcVar30);
    }
    if ((local_338 != 0.0) && (fStack_334 != 0.0)) {
      auVar48._0_4_ = fStack_334;
      fVar38 = local_338;
    }
  }
  local_370 = fVar45;
  local_36c = fVar46;
  if (((param_15 & 0x10000000) != 0) && (uVar20 == 0)) {
    local_358 = (char ****)0x0;
    uStack_350 = 0;
    lVar29 = -1;
    do {
      lVar29 = lVar29 + 1;
    } while (param_17[lVar29] != '\0');
    local_348 = uVar20;
    local_340 = uVar20;
    FUN_140002c00(&local_358,param_17);
    FUN_14081dce0(param_3,&local_338);
    if (0xf < local_340) {
      pppppcVar30 = (char *****)local_358;
      if ((0xfff < local_340 + 1) &&
         (pppppcVar30 = (char *****)local_358[-1],
         (char *)0x1f < (char *)((longlong)local_358 + (-8 - (longlong)pppppcVar30)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppcVar30);
    }
    if ((fStack_330 != 0.0) && (fStack_32c != 0.0)) {
      local_370 = fStack_330;
      local_36c = fStack_32c;
    }
  }
  pcVar26 = param_17;
  if (param_17 == (char *)0x0) {
    pcVar26 = param_16;
  }
  local_378 = fVar38;
  fStack_374 = auVar48._0_4_;
  iVar25 = FUN_140507e00(lVar24,pcVar26,param_16,&local_378,CONCAT44(uVar51,param_15));
  if ((uVar12 != 0) && (lVar24 != 0)) {
    iVar6 = *(int *)(param_3 + 0x4698);
    if (0 < iVar6) {
      *(int *)(param_3 + 0x4698) = iVar6 + -1;
      **(undefined8 **)(param_3 + 0x4690 + (longlong)iVar6 * 0x10) =
           *(undefined8 *)(param_3 + 0x4698 + (longlong)iVar6 * 0x10);
    }
  }
  if (iVar25 != 0) {
    if (((lVar24 != 0) && (lVar29 = *(longlong *)(param_3 + 0x5e70), lVar29 != 0)) &&
       (lVar7 = *(longlong *)(lVar29 + 0xa8), lVar7 != 0)) {
      fVar43 = *(float *)(param_3 + 0x3b80);
      *(undefined4 *)(lVar7 + 0x74) = 0;
      fVar45 = *(float *)(lVar7 + 0x78) + *(float *)(lVar7 + 0x2c);
      *(float *)(lVar7 + 0x2c) = fVar45;
      *(undefined4 *)(lVar7 + 0x80) = 0x7fffffff;
      fVar31 = fVar49;
      if ((fVar49 == 0.0) && (fVar31 = *(float *)(lVar7 + 0x7c), *(float *)(lVar7 + 0x7c) <= fVar49)
         ) {
        fVar31 = fVar49;
      }
      fVar31 = fVar31 + fVar43;
      *(float *)(lVar7 + 0x78) = fVar31;
      *(undefined4 *)(lVar7 + 0x98) = 0;
      if ((*(uint *)(lVar7 + 4) & 0x1000) != 0) {
        local_378 = *(float *)(lVar29 + 0x4c);
        fStack_374 = fVar45 - fVar44;
        local_370 = *(float *)(lVar29 + 0x54);
        local_36c = fVar31 + fVar44;
        FUN_1404fc370(lVar29 + 0x68,&local_378,0);
      }
      *(undefined4 *)(lVar7 + 0x70) = 6;
      *(undefined8 *)(lVar7 + 0x88) = 0;
      *(undefined8 *)(lVar7 + 0x98) = 0;
      *(undefined4 *)(lVar7 + 0x90) = 0;
    }
    if (*(float *)*param_18 != 0.0) {
      local_2e0 = 1;
      local_2dc = *(float *)*param_18;
    }
    if (*(float *)param_18[1] != 0.0) {
      local_2d8 = 1;
      local_2d4 = *(float *)param_18[1];
    }
    local_2e8 = 1;
    local_2e4 = 3;
    FUN_14055b8e0(local_308,param_18[2]);
    (**(code **)(local_308[0] + 8))(local_308);
    FUN_1407fe960(param_18[3],local_308);
    (**(code **)(local_308[0] + 0x18))(local_308);
  }
  if ((param_15 & 0x8000000) != 0) {
    if ((lVar24 == 0) || (lVar29 = *(longlong *)(param_3 + 0x5e70), lVar29 == 0)) {
      fVar44 = 0.0;
      fVar43 = 0.0;
    }
    else {
      fVar44 = *(float *)(lVar29 + 0x4c);
      fVar43 = *(float *)(lVar29 + 0x50);
    }
    local_358 = (char ****)0x0;
    uStack_350 = 0;
    local_348 = 0;
    local_340 = 0;
    lVar29 = -1;
    do {
      lVar29 = lVar29 + 1;
    } while (param_17[lVar29] != '\0');
    FUN_140002c00(&local_358,param_17);
    FUN_14081dce0(param_3,&local_338);
    if (0xf < local_340) {
      ppppcVar23 = local_358;
      if ((0xfff < local_340 + 1) &&
         (ppppcVar23 = (char ****)local_358[-1],
         0x1f < (ulonglong)((longlong)local_358 + (-8 - (longlong)ppppcVar23)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(ppppcVar23);
    }
    local_358 = (char ****)0x0;
    uStack_350 = 0;
    local_348 = 0;
    local_340 = 0;
    lVar29 = -1;
    do {
      lVar29 = lVar29 + 1;
    } while (param_17[lVar29] != '\0');
    local_338 = fVar44;
    fStack_334 = fVar43;
    FUN_140002c00(&local_358,param_17);
    pppppcVar30 = &local_358;
    if (0xf < local_340) {
      pppppcVar30 = (char *****)local_358;
    }
    local_378 = (float)((uint)local_378 & 0xffffff00);
    local_361 = '\x17';
    cVar4 = *(char *)pppppcVar30;
    while (cVar4 != '\0') {
      pppppcVar30 = (char *****)((longlong)pppppcVar30 + 1);
      cVar4 = *(char *)pppppcVar30;
    }
    FUN_14041a770(&local_378);
    pfVar27 = &local_378;
    if (local_361 < '\0') {
      pfVar27 = (float *)CONCAT44(fStack_374,local_378);
    }
    uVar14 = 0x811c9dc5;
    bVar5 = *(byte *)pfVar27;
    pbVar10 = (byte *)((longlong)pfVar27 + 1);
    while (bVar5 != 0) {
      uVar14 = uVar14 * 0x1000193 ^ (uint)bVar5;
      bVar5 = *pbVar10;
      pbVar10 = pbVar10 + 1;
    }
    FUN_140824520(param_3 + 0x6028,local_320,pbVar10,&local_378,uVar14);
    *(ulonglong *)(local_320[0] + 0x18) = CONCAT44(fStack_334,local_338);
    *(ulonglong *)(local_320[0] + 0x20) = CONCAT44(fStack_32c,fStack_330);
    *(undefined8 *)(local_320[0] + 0x28) = local_328;
    if ((local_361 < '\0') && ((void *)CONCAT44(fStack_374,local_378) != (void *)0x0)) {
      free((void *)CONCAT44(fStack_374,local_378));
    }
    if (0xf < local_340) {
      pppppcVar30 = (char *****)local_358;
      if ((0xfff < local_340 + 1) &&
         (pppppcVar30 = (char *****)local_358[-1],
         (char *)0x1f < (char *)((longlong)local_358 + (-8 - (longlong)pppppcVar30)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppcVar30);
    }
  }
  if ((param_15 & 0x10000000) != 0) {
    if ((lVar24 == 0) || (lVar29 = *(longlong *)(param_3 + 0x5e70), lVar29 == 0)) {
      fVar44 = 0.0;
    }
    else {
      fVar47 = *(float *)(lVar29 + 0x54);
      fVar44 = *(float *)(lVar29 + 0x58);
    }
    local_358 = (char ****)0x0;
    uStack_350 = 0;
    local_348 = 0;
    local_340 = 0;
    lVar29 = -1;
    do {
      lVar29 = lVar29 + 1;
    } while (param_17[lVar29] != '\0');
    FUN_140002c00(&local_358,param_17);
    FUN_14081dce0(param_3,&local_338);
    if (0xf < local_340) {
      ppppcVar23 = local_358;
      if ((0xfff < local_340 + 1) &&
         (ppppcVar23 = (char ****)local_358[-1],
         0x1f < (ulonglong)((longlong)local_358 + (-8 - (longlong)ppppcVar23)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(ppppcVar23);
    }
    local_358 = (char ****)0x0;
    uStack_350 = 0;
    local_348 = 0;
    local_340 = 0;
    do {
      lVar18 = lVar18 + 1;
    } while (param_17[lVar18] != '\0');
    fStack_330 = fVar47;
    fStack_32c = fVar44;
    FUN_140002c00(&local_358,param_17,lVar18);
    pppppcVar30 = &local_358;
    if (0xf < local_340) {
      pppppcVar30 = (char *****)local_358;
    }
    local_378 = (float)((uint)local_378 & 0xffffff00);
    local_361 = '\x17';
    cVar4 = *(char *)pppppcVar30;
    while (cVar4 != '\0') {
      pppppcVar30 = (char *****)((longlong)pppppcVar30 + 1);
      cVar4 = *(char *)pppppcVar30;
    }
    FUN_14041a770(&local_378);
    pfVar27 = &local_378;
    if (local_361 < '\0') {
      pfVar27 = (float *)CONCAT44(fStack_374,local_378);
    }
    cVar4 = *(char *)pfVar27;
    while (cVar4 != '\0') {
      pfVar27 = (float *)((longlong)pfVar27 + 1);
      cVar4 = *(char *)pfVar27;
    }
    FUN_140824520(param_3 + 0x6028,local_320);
    *(ulonglong *)(local_320[0] + 0x18) = CONCAT44(fStack_334,local_338);
    *(ulonglong *)(local_320[0] + 0x20) = CONCAT44(fStack_32c,fStack_330);
    *(undefined8 *)(local_320[0] + 0x28) = local_328;
    if ((local_361 < '\0') && ((void *)CONCAT44(fStack_374,local_378) != (void *)0x0)) {
      free((void *)CONCAT44(fStack_374,local_378));
    }
    if (0xf < local_340) {
      pppppcVar30 = (char *****)local_358;
      if ((0xfff < local_340 + 1) &&
         (pppppcVar30 = (char *****)local_358[-1],
         (char *)0x1f < (char *)((longlong)local_358 + (-8 - (longlong)pppppcVar30)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppcVar30);
    }
  }
  if (lVar24 != 0) {
    if ((*(char *)(param_2 + 100) != '\0') &&
       (lVar18 = *(longlong *)(param_3 + 0x5e70), lVar18 != 0)) {
      *(undefined4 *)(lVar18 + 0x18c) = 0;
      *(undefined4 *)(lVar18 + 0x180) = 0;
    }
    lVar18 = *(longlong *)(param_3 + 0x5e70);
    if (lVar18 != 0) {
      if ((*(int **)(lVar18 + 0xa8) != (int *)0x0) &&
         ((**(int **)(lVar18 + 0xa8) != 1 || ((*(uint *)(lVar18 + 0x48) & 0x4000) == 0)))) {
        FUN_140506ed0(lVar24);
        FUN_140505bf0(lVar24,*(undefined8 *)(*(longlong *)(param_3 + 0x5e70) + 0xa8));
      }
      *(undefined8 *)(param_3 + 0x5e70) = 0;
    }
  }
  if ((uVar12 == 0) && (lVar24 != 0)) {
    iVar6 = *(int *)(param_3 + 0x3c70);
    lVar24 = (longlong)iVar6;
    if (0 < iVar6) {
      *(int *)(param_3 + 0x3c70) = iVar6 + -1;
      puVar8 = *(undefined8 **)(param_3 + 0x3c48 + lVar24 * 0x30);
      puVar1 = (undefined8 *)(param_3 + 0x3c50 + lVar24 * 0x30);
      uVar16 = puVar1[1];
      *puVar8 = *puVar1;
      puVar8[1] = uVar16;
      puVar1 = (undefined8 *)(param_3 + 0x3c60 + lVar24 * 0x30);
      uVar16 = puVar1[1];
      puVar8[2] = *puVar1;
      puVar8[3] = uVar16;
      puVar8[4] = *(undefined8 *)(param_3 + 0x3c70 + lVar24 * 0x30);
    }
  }
  *(bool *)param_1 = iVar25 != 0;
  *(float *)(param_1 + 4) = fVar38;
  *(float *)(param_1 + 8) = auVar48._0_4_;
  *(float *)(param_1 + 0xc) = fVar50;
  *(float *)(param_1 + 0x10) = fVar49;
  if (local_108 != (undefined8 *)0x0) {
    (**(code **)*local_108)(local_108,1);
  }
  FUN_14055c040(local_258);
  FUN_140567d00(local_278);
  FUN_140567d00(local_290);
  if (local_2a8 != (void *)0x0) {
    free(local_2a8);
  }
  return param_1;
}


// Incoming references
// 0xc3b2f4 DATA caller none
// 0x80e54d UNCONDITIONAL_CALL caller 14080e3d0
