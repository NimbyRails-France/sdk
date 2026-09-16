// Candidate VA 14061acc0; RVA 0x61acc0
// Ghidra inferred prototype: undefined FUN_14061acc0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined1 *
FUN_14061acc0(undefined1 *param_1,longlong *param_2,longlong param_3,longlong param_4,int *param_5,
             char param_6,char param_7,undefined1 param_8)

{
  double dVar1;
  double dVar2;
  double dVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  longlong *plVar9;
  void *pvVar10;
  longlong lVar11;
  undefined8 uVar12;
  undefined4 *puVar13;
  undefined8 ****ppppuVar14;
  undefined1 *puVar15;
  void *_Memory;
  longlong lVar16;
  undefined8 *puVar17;
  longlong lVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  undefined1 local_res10;
  undefined1 local_res11;
  undefined4 uVar28;
  undefined8 uVar26;
  undefined4 uVar29;
  undefined4 uVar30;
  ulonglong uVar27;
  undefined8 in_stack_fffffffffffffe30;
  undefined8 uVar31;
  undefined4 uVar32;
  undefined8 in_stack_fffffffffffffe40;
  uint uVar36;
  float *pfVar33;
  undefined4 uVar37;
  ulonglong uVar34;
  undefined8 *puVar35;
  undefined8 local_1a8;
  undefined8 local_1a0;
  undefined2 local_198 [2];
  undefined4 local_194;
  undefined4 local_190;
  undefined4 local_18c;
  undefined8 local_188;
  undefined8 ***local_180 [2];
  longlong local_170;
  ulonglong local_168;
  undefined4 local_160;
  float fStack_15c;
  undefined4 uStack_158;
  float fStack_154;
  undefined8 local_150;
  undefined1 local_148;
  undefined7 uStack_147;
  undefined8 local_138;
  ulonglong uStack_130;
  float local_128;
  float local_124;
  float local_120;
  float local_11c;
  undefined2 local_118 [2];
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined8 local_108;
  undefined8 ***local_100 [2];
  longlong local_f0;
  ulonglong local_e8;
  float local_e0 [2];
  float local_d8;

  uVar36 = (uint)((ulonglong)in_stack_fffffffffffffe40 >> 0x20);
  dVar1 = *(double *)(param_3 + 0x10);
  dVar2 = *(double *)(param_3 + 0x18);
  dVar3 = (double)param_2[0x2e];
  dVar4 = (double)param_2[0x2b];
  dVar5 = (double)param_2[0x2f];
  dVar6 = (double)param_2[0x2c];
  dVar7 = (double)param_2[0x31];
  dVar8 = (double)param_2[0x32];
  lVar16 = param_4 + 8;
  if (*(char *)(param_3 + 0x40) == '\0') {
    lVar16 = param_3 + 0x20;
  }
  FUN_140019ca0(local_180,lVar16);
  lVar16 = -1;
  if (local_170 == 0) {
    lVar11 = FUN_1402d82e0("station_name_empty","<Empty name>");
    lVar18 = -1;
    do {
      lVar18 = lVar18 + 1;
    } while (*(char *)(lVar11 + lVar18) != '\0');
    FUN_140030630(local_180,lVar11);
  }
  uVar12 = FUN_140019ca0(&local_148,local_180);
  uVar31 = CONCAT71((int7)((ulonglong)in_stack_fffffffffffffe30 >> 8),1);
  uVar26 = CONCAT44((float)(dVar2 * dVar5 + dVar1 * dVar6 + dVar8),
                    (float)(dVar2 * dVar3 + dVar1 * dVar4 + dVar7));
  puVar13 = (undefined4 *)FUN_14061a900(&local_160,param_2,DAT_140aac254,uVar12,uVar26,uVar31);
  uVar28 = (undefined4)((ulonglong)uVar26 >> 0x20);
  uVar32 = (undefined4)((ulonglong)uVar31 >> 0x20);
  uVar30 = *puVar13;
  fVar25 = (float)puVar13[1];
  uVar29 = puVar13[2];
  fVar20 = (float)puVar13[3];
  uVar27 = *(ulonglong *)(puVar13 + 4);
  if (0xf < local_168) {
    ppppuVar14 = (undefined8 ****)local_180[0];
    if ((0xfff < local_168 + 1) &&
       (ppppuVar14 = (undefined8 ****)local_180[0][-1],
       0x1f < (ulonglong)((longlong)local_180[0] + (-8 - (longlong)ppppuVar14)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppuVar14);
  }
  local_150._4_4_ = (float)(uVar27 >> 0x20);
  if (*(longlong *)(param_3 + 0x68) - *(longlong *)(param_3 + 0x60) == 8) {
    fVar19 = local_150._4_4_ + DAT_140aac768;
    fVar23 = fVar25 - DAT_140aac308;
    fVar24 = fVar25;
  }
  else {
    fVar24 = 0.0;
    fVar19 = local_150._4_4_;
    fVar23 = fVar25;
  }
  local_118[0] = *(undefined2 *)(*param_2 + 0x15b8);
  local_114 = 0x41900000;
  local_110 = 0x12;
  local_10c = 0xff000000;
  local_108 = 0;
  lVar11 = param_4 + 8;
  if (*(char *)(param_3 + 0x40) == '\0') {
    lVar11 = param_3 + 0x20;
  }
  local_160 = uVar30;
  fStack_15c = fVar25;
  uStack_158 = uVar29;
  fStack_154 = fVar20;
  local_150 = uVar27;
  FUN_140019ca0(local_100,lVar11);
  if (local_f0 == 0) {
    lVar11 = FUN_1402d82e0("station_name_empty","<Empty name>");
    do {
      lVar16 = lVar16 + 1;
    } while (*(char *)(lVar11 + lVar16) != '\0');
    FUN_140030630(local_100,lVar11,lVar16);
  }
  plVar9 = *(longlong **)(*param_2 + 0x1580);
  local_1a8 = param_2;
  local_1a0 = plVar9;
  (**(code **)*plVar9)(plVar9);
  fVar25 = fStack_154;
  uVar29 = uStack_158;
  uVar30 = local_160;
  uVar26 = CONCAT44(uVar28,uStack_158);
  FUN_14061a780(&local_1a8,&local_res10,local_160,CONCAT44(fVar24,fVar23),uVar26,
                CONCAT44(uVar32,fStack_154),param_8);
  uVar28 = (undefined4)((ulonglong)uVar26 >> 0x20);
  (**(code **)*plVar9)(plVar9);
  uVar26 = CONCAT44(uVar28,fVar25);
  (**(code **)(*plVar9 + 0x38))(plVar9,uVar30,CONCAT44(fVar24,fVar23),uVar29,uVar26);
  uVar29 = (undefined4)((ulonglong)uVar26 >> 0x20);
  (**(code **)(*plVar9 + 0x88))(plVar9,0xff000000,DAT_140aabd78);
  uVar27 = local_150;
  uVar30 = DAT_140aac3d0;
  ppppuVar14 = local_100;
  if (0xf < local_e8) {
    ppppuVar14 = (undefined8 ****)local_100[0];
  }
  uVar12 = 0;
  uVar26 = CONCAT44(uVar29,DAT_140aac3d0);
  fVar20 = (float)local_150;
  (**(code **)(*plVar9 + 0x140))
            (plVar9,local_118,local_150 & 0xffffffff,fVar19,uVar26,ppppuVar14,0,
             (ulonglong)uVar36 << 0x20);
  uVar29 = DAT_140aac234;
  uVar28 = (undefined4)((ulonglong)uVar26 >> 0x20);
  fVar25 = fVar25 + fVar23;
  if (param_7 != '\0') {
    local_198[0] = *(undefined2 *)(*param_2 + 0x15b8);
    local_194 = 0x41800000;
    local_190 = 0x12;
    local_18c = 0xffffffff;
    local_188 = 0;
    FUN_140572550(local_180,(longlong)(param_5[1] + *param_5));
    FUN_140572550(&local_148,*(undefined4 *)(param_4 + 0x28));
    ppppuVar14 = local_180;
    if (0xf < local_168) {
      ppppuVar14 = (undefined8 ****)local_180[0];
    }
    uVar26 = CONCAT44(uVar28,uVar30);
    FUN_1401b32a0(plVar9,local_198,uVar27 & 0xffffffff,fVar25,uVar26,ppppuVar14,uVar12,&local_128);
    puVar15 = &local_148;
    if (0xf < uStack_130) {
      puVar15 = (undefined1 *)CONCAT71(uStack_147,local_148);
    }
    pfVar33 = local_e0;
    uVar26 = CONCAT44((int)((ulonglong)uVar26 >> 0x20),uVar30);
    FUN_1401b32a0(plVar9,local_198,uVar27 & 0xffffffff,fVar25,uVar26,puVar15,uVar12,pfVar33);
    uVar32 = (undefined4)((ulonglong)uVar12 >> 0x20);
    uVar37 = (undefined4)((ulonglong)pfVar33 >> 0x20);
    uVar30 = (undefined4)((ulonglong)uVar26 >> 0x20);
    fVar19 = (local_d8 - local_e0[0]) + (local_120 - local_128) + _DAT_140aac344;
    fVar22 = (local_11c - local_124) + DAT_140aabf18;
    fVar20 = fVar20 - fVar19 * DAT_140aab9dc;
    (**(code **)*plVar9)(plVar9);
    uVar27 = CONCAT44(uVar30,fVar22);
    (**(code **)(*plVar9 + 0x38))(plVar9,fVar20,fVar25,fVar19,uVar27);
    (**(code **)(*plVar9 + 0x70))(plVar9,0xff000000,0);
    uVar28 = DAT_140aac644;
    fVar19 = fVar22 * DAT_140aab9dc + fVar25;
    uVar34 = CONCAT44(uVar37,uVar29);
    uVar27 = uVar27 & 0xffffffff00000000;
    FUN_14072a5e0(&local_1a8,"icon_clock_w.svg",fVar20,fVar19,uVar27,puVar15,
                  CONCAT44(uVar32,0xffffffff),uVar34,DAT_140aac644);
    uVar30 = DAT_140aac534;
    fVar21 = (local_120 - local_128) * DAT_140aab9dc;
    fVar20 = fVar21 + DAT_140aac254 + fVar20;
    ppppuVar14 = local_180;
    if (0xf < local_168) {
      ppppuVar14 = (undefined8 ****)local_180[0];
    }
    uVar34 = uVar34 & 0xffffffff00000000;
    uVar32 = 0;
    uVar27 = CONCAT44((int)(uVar27 >> 0x20),DAT_140aac534);
    (**(code **)(*plVar9 + 0x140))(plVar9,local_198,fVar20,fVar19,uVar27,ppppuVar14,0,uVar34);
    fVar20 = fVar21 + DAT_140aac0a0 + fVar20;
    uVar34 = CONCAT44((int)(uVar34 >> 0x20),uVar29);
    uVar27 = uVar27 & 0xffffffff00000000;
    FUN_14072a5e0(&local_1a8,"icon_map_population_w.svg",fVar20,fVar19,uVar27,ppppuVar14,
                  CONCAT44(uVar32,0xffffffff),uVar34,uVar28);
    fVar21 = DAT_140aab9dc;
    puVar15 = &local_148;
    if (0xf < uStack_130) {
      puVar15 = (undefined1 *)CONCAT71(uStack_147,local_148);
    }
    uVar12 = 0;
    uVar26 = CONCAT44((int)(uVar27 >> 0x20),uVar30);
    (**(code **)(*plVar9 + 0x140))
              (plVar9,local_198,(local_d8 - local_e0[0]) * DAT_140aab9dc + DAT_140aac254 + fVar20,
               fVar19,uVar26,puVar15,0,uVar34 & 0xffffffff00000000);
    uVar27 = local_150;
    uVar30 = DAT_140aac3d0;
    if (param_6 != '\0') {
      if ((*(longlong *)(param_4 + 0x48) != 0) || (*(longlong *)(param_4 + 0x68) != 0)) {
        if (DAT_140b770a0 == '\0') {
          puVar17 = (undefined8 *)(param_4 + 0x58);
          if (*(longlong *)(param_4 + 0x68) == 0) {
            puVar17 = (undefined8 *)(param_4 + 0x38);
          }
        }
        else {
          puVar17 = (undefined8 *)(param_4 + 0x38);
          if (*(longlong *)(param_4 + 0x48) == 0) {
            puVar17 = (undefined8 *)(param_4 + 0x58);
          }
        }
        if (0xf < (ulonglong)puVar17[3]) {
          puVar17 = (undefined8 *)*puVar17;
        }
        puVar35 = &local_1a8;
        uVar26 = CONCAT44((int)((ulonglong)uVar26 >> 0x20),DAT_140aac3d0);
        fVar20 = (float)local_150;
        FUN_1401b32a0(plVar9,local_198,local_150 & 0xffffffff,fVar25 + fVar22,uVar26,puVar17,uVar12,
                      puVar35);
        uVar29 = (undefined4)((ulonglong)uVar26 >> 0x20);
        fVar19 = ((float)local_1a0 - (float)local_1a8) + DAT_140aac168;
        fVar25 = (local_1a0._4_4_ - local_1a8._4_4_) + DAT_140aac038;
        (**(code **)*plVar9)(plVar9);
        fVar23 = fVar23 - fVar25;
        uVar26 = CONCAT44(uVar29,fVar25);
        (**(code **)(*plVar9 + 0x38))
                  (plVar9,fVar20 - fVar19 * fVar21,CONCAT44(fVar24,fVar23),fVar19,uVar26);
        uVar29 = (undefined4)((ulonglong)uVar26 >> 0x20);
        (**(code **)(*plVar9 + 0x70))(plVar9,0xff000000,0);
        (**(code **)(*plVar9 + 0x140))
                  (plVar9,local_198,uVar27 & 0xffffffff,CONCAT44(fVar24,fVar23 + DAT_140aac168),
                   CONCAT44(uVar29,uVar30),puVar17,0,(ulonglong)puVar35 & 0xffffffff00000000);
      }
    }
    if (0xf < uStack_130) {
      pvVar10 = (void *)CONCAT71(uStack_147,local_148);
      _Memory = pvVar10;
      if ((0xfff < uStack_130 + 1) &&
         (_Memory = *(void **)((longlong)pvVar10 + -8),
         0x1f < (ulonglong)((longlong)pvVar10 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(_Memory);
    }
    local_138 = _DAT_140aac910;
    uStack_130 = _UNK_140aac918;
    local_148 = 0;
    if (0xf < local_168) {
      ppppuVar14 = (undefined8 ****)local_180[0];
      if ((0xfff < local_168 + 1) &&
         (ppppuVar14 = (undefined8 ****)local_180[0][-1],
         0x1f < (ulonglong)((longlong)local_180[0] + (-8 - (longlong)ppppuVar14)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(ppppuVar14);
    }
  }
  *param_1 = local_res10;
  param_1[1] = local_res11;
  if (0xf < local_e8) {
    ppppuVar14 = (undefined8 ****)local_100[0];
    if ((0xfff < local_e8 + 1) &&
       (ppppuVar14 = (undefined8 ****)local_100[0][-1],
       0x1f < (ulonglong)((longlong)local_100[0] + (-8 - (longlong)ppppuVar14)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppuVar14);
  }
  return param_1;
}


// Incoming references
// 0xc311f0 DATA caller none
// 0x62595d UNCONDITIONAL_CALL caller 1406236d0
