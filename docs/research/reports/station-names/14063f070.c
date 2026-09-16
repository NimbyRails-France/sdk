// Candidate VA 14063f070; RVA 0x63f070
// Ghidra inferred prototype: undefined FUN_14063f070()

void FUN_14063f070(undefined8 *param_1,longlong *param_2,undefined8 param_3,undefined8 param_4,
                  longlong param_5,longlong param_6,longlong param_7,undefined1 param_8,
                  undefined8 param_9)

{
  int *piVar1;
  longlong *plVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  undefined2 uVar6;
  undefined4 uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  longlong lVar14;
  longlong *plVar15;
  longlong lVar16;
  longlong lVar17;
  undefined8 *puVar18;
  undefined8 *puVar19;
  longlong lVar20;
  void *pvVar21;
  longlong lVar22;
  ulonglong uVar23;
  void *pvVar24;
  void *pvVar25;
  float fVar26;
  undefined8 in_stack_fffffffffffffe38;
  undefined4 uVar27;
  undefined2 local_180 [2];
  undefined4 local_17c;
  undefined4 local_178;
  undefined8 local_174;
  undefined4 local_16c;
  longlong local_150;
  undefined8 local_148;
  longlong local_140;
  longlong local_138;
  undefined8 local_100 [3];
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined4 uStack_e0;
  undefined4 uStack_dc;
  uint local_d8;
  undefined8 local_c8;
  undefined8 uStack_c0;
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined4 local_78;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined4 uStack_6c;
  undefined4 local_68;
  undefined4 uStack_64;
  undefined4 uStack_60;
  undefined4 uStack_5c;

  uVar27 = (undefined4)((ulonglong)in_stack_fffffffffffffe38 >> 0x20);
  *param_1 = 0;
  FUN_1402531c0(param_1 + 8);
  lVar22 = param_5 + 0x428;
  param_1[0x77] = 0;
  param_1[0x78] = 0;
  *(undefined4 *)(param_1 + 0x79) = 0;
  local_148 = *(undefined8 *)(param_5 + 0x408);
  lVar20 = param_6 + 0xa0;
  local_138 = param_5 + 0x890;
  lVar14 = *param_2;
  plVar15 = *(longlong **)(lVar14 + 0x1580);
  uVar6 = *(undefined2 *)(lVar14 + 0x15b8);
  fVar26 = *(float *)(lVar14 + 0x1550);
  if (*(float *)(lVar14 + 0x1550) < 0.0) {
    fVar26 = DAT_140aabae4;
  }
  local_140 = lVar22;
  FUN_1401b2200(plVar15,0x1e,(short)param_2[0x14],*(undefined2 *)((longlong)param_2 + 0xa4),
                CONCAT44(uVar27,fVar26 * DAT_140aabb98));
  fVar26 = DAT_140aabd78;
  puVar19 = &DAT_140b77c60;
  if (0xf < DAT_140b77c78) {
    puVar19 = DAT_140b77c60;
  }
  local_17c = 0x41400000;
  local_178 = 0x24;
  local_174 = 0x5f555555;
  local_16c = 0;
  local_180[0] = uVar6;
  (**(code **)(*plVar15 + 0x138))
            (plVar15,local_180,
             (*(float *)(param_2 + 0x2a) + *(float *)(param_2 + 0x29)) - DAT_140aabd78,
             (*(float *)((longlong)param_2 + 0x154) + *(float *)((longlong)param_2 + 0x14c)) -
             DAT_140aac1d0,puVar19,0);
  local_17c = 0x41400000;
  local_178 = 0x24;
  local_174 = 0x5f555555;
  local_16c = 0;
  local_180[0] = uVar6;
  (**(code **)(*plVar15 + 0x138))
            (plVar15,local_180,(*(float *)(param_2 + 0x2a) + *(float *)(param_2 + 0x29)) - fVar26,
             (*(float *)((longlong)param_2 + 0x154) + *(float *)((longlong)param_2 + 0x14c)) -
             fVar26,&DAT_140a8dd80,0);
  FUN_1401b2400(plVar15);
  FUN_1406328b0(param_1,param_2,param_4,param_6,param_7,param_8);
  FUN_1406236d0(param_1[0x4c],param_2,param_3,param_5,param_6,param_7,param_9);
  uVar27 = (undefined4)((ulonglong)param_9 >> 0x20);
  if ((*(int *)(param_7 + 0x20) == 1) || (*(char *)(param_7 + 4) != '\0')) {
    cVar5 = *(char *)(param_7 + 4);
    uVar7 = *(undefined4 *)(param_7 + 0x10);
    local_150 = *(longlong *)(param_7 + 8);
    FUN_140639480(param_1,param_2);
    if ((local_150 != 0) && (cVar5 != '\0')) {
      FUN_140639c10(param_1,param_2,param_4,lVar22,param_5 + 0x780,local_150,CONCAT44(uVar27,uVar7),
                    0,99999999999,1,cVar5);
    }
  }
  FUN_14063da50(param_1,param_2,param_4,lVar22,param_5 + 0x780,param_5 + 0x890,param_6 + 0x20,
                param_6 + 0x1658);
  puVar19 = (undefined8 *)param_1[0x33];
  for (puVar18 = (undefined8 *)param_1[0x32]; puVar18 != puVar19; puVar18 = puVar18 + 0xe) {
    local_c8 = *puVar18;
    uStack_c0 = puVar18[1];
    local_b8 = puVar18[2];
    uStack_b0 = puVar18[3];
    local_a8 = puVar18[4];
    uStack_a0 = puVar18[5];
    local_98 = puVar18[6];
    uStack_90 = puVar18[7];
    local_88 = puVar18[8];
    uStack_80 = puVar18[9];
    local_78 = *(undefined4 *)(puVar18 + 10);
    uStack_74 = *(undefined4 *)((longlong)puVar18 + 0x54);
    uStack_70 = *(undefined4 *)(puVar18 + 0xb);
    uStack_6c = *(undefined4 *)((longlong)puVar18 + 0x5c);
    local_68 = *(undefined4 *)(puVar18 + 0xc);
    uStack_64 = *(undefined4 *)((longlong)puVar18 + 100);
    uStack_60 = *(undefined4 *)(puVar18 + 0xd);
    uStack_5c = *(undefined4 *)((longlong)puVar18 + 0x6c);
    FUN_14063e270(local_68,param_2);
  }
  FUN_140633f80(param_1,param_2,local_148,param_5 + 0x400,lVar20,param_6 + 0x20,param_7);
  FUN_14063d700(param_1,param_2,param_7,*(undefined8 *)(param_6 + 0x2200),lVar20,local_140);
  FUN_140353a70(local_180,param_1 + 0x29);
  FUN_14063c9e0(param_1,param_2,param_3);
  FUN_14063eae0(param_1,param_2);
  lVar22 = param_1[0x4c];
  pvVar25 = (void *)0x0;
  lVar20 = *(longlong *)(lVar22 + 0x2e8);
  lVar14 = *(longlong *)(lVar22 + 0x2f0);
  lVar16 = *(longlong *)(lVar22 + 0x2b8);
  pvVar21 = pvVar25;
  pvVar24 = pvVar25;
  for (puVar19 = *(undefined8 **)(lVar22 + 0x280); puVar19 != (undefined8 *)(lVar22 + 0x280);
      puVar19 = (undefined8 *)*puVar19) {
    lVar17 = puVar19[3];
    uVar23 = (ulonglong)(*(short *)(lVar17 + 0x8c) != -1);
    pvVar21 = (void *)((longlong)pvVar21 + uVar23);
    pvVar24 = (void *)((longlong)pvVar24 +
                      (ulonglong)(*(short *)(lVar17 + 0x88) != -1) +
                      (*(short *)(lVar17 + 0x8a) != -1) + uVar23);
  }
  param_1[4] = lVar14;
  param_1[1] = param_1[0x73] + param_1[0x48] + lVar20;
  param_1[3] = param_1[0x49];
  param_1[2] = param_1[0x74] + param_1[0x49] + lVar14;
  param_1[5] = param_1[0x74];
  param_1[0xf] = param_1[0x6d] + param_1[0x42] + lVar16;
  param_1[6] = pvVar24;
  param_1[7] = pvVar21;
  for (puVar19 = (undefined8 *)param_1[0x3b]; puVar19 != param_1 + 0x3b;
      puVar19 = (undefined8 *)*puVar19) {
    if (puVar19[5] != 0) {
      LOCK();
      piVar1 = (int *)(puVar19[5] + 8);
      *piVar1 = *piVar1 + 1;
      UNLOCK();
    }
    lVar22 = puVar19[4];
    plVar15 = (longlong *)puVar19[5];
    param_1[6] = param_1[6] + (ulonglong)(*(short *)(lVar22 + 0x18) != -1);
    lVar20 = param_1[6] + (ulonglong)(*(short *)(lVar22 + 0x1a) != -1);
    param_1[6] = lVar20;
    param_1[6] = (ulonglong)(*(short *)(lVar22 + 0x1c) != -1) + lVar20;
    param_1[7] = param_1[7] + (ulonglong)(*(short *)(lVar22 + 0x1c) != -1);
    if (plVar15 != (longlong *)0x0) {
      LOCK();
      plVar2 = plVar15 + 1;
      lVar22 = *plVar2;
      *(int *)plVar2 = (int)*plVar2 + -1;
      UNLOCK();
      if ((int)lVar22 == 1) {
        (**(code **)*plVar15)(plVar15);
        LOCK();
        piVar1 = (int *)((longlong)plVar15 + 0xc);
        iVar3 = *piVar1;
        *piVar1 = *piVar1 + -1;
        UNLOCK();
        if (iVar3 == 1) {
          (**(code **)(*plVar15 + 8))(plVar15);
        }
      }
    }
  }
  puVar19 = param_1 + 0x66;
  puVar18 = (undefined8 *)*puVar19;
  if (puVar18 != puVar19) {
    lVar22 = param_1[6];
    do {
      lVar22 = lVar22 + (ulonglong)(*(short *)(puVar18[3] + 0x90) != -1);
      param_1[6] = lVar22;
      lVar22 = (ulonglong)(*(short *)(puVar18[3] + 0x92) != -1) + lVar22;
      param_1[6] = lVar22;
      lVar22 = (ulonglong)(*(short *)(puVar18[3] + 0x94) != -1) + lVar22;
      param_1[6] = lVar22;
      puVar18 = (undefined8 *)*puVar18;
    } while (puVar18 != puVar19);
  }
  FUN_140626490(param_1 + 0x16,param_7);
  param_1[0x33] = param_1[0x32];
  FUN_1402531c0(param_1 + 0x29);
  local_e8 = 0x4f0000ff;
  puVar19 = param_1 + 0x35;
  uStack_e4 = 0x4fff0000;
  uStack_e0 = 0x40400000;
  uStack_dc = 0x42c80000;
  local_d8 = local_d8 & 0xffffff00;
  pvVar21 = pvVar25;
  if (puVar19 != local_100) {
    FUN_1406451d0(puVar19);
    pvVar21 = (void *)*puVar19;
    *puVar19 = 0;
    pvVar25 = (void *)param_1[0x36];
    param_1[0x36] = 0;
    param_1[0x37] = 0;
  }
  *(undefined4 *)(param_1 + 0x38) = local_e8;
  *(undefined4 *)((longlong)param_1 + 0x1c4) = uStack_e4;
  *(undefined4 *)(param_1 + 0x39) = uStack_e0;
  *(undefined4 *)((longlong)param_1 + 0x1cc) = uStack_dc;
  *(uint *)(param_1 + 0x3a) = local_d8;
  for (pvVar24 = pvVar21; pvVar24 != pvVar25; pvVar24 = (void *)((longlong)pvVar24 + 0x38)) {
    FUN_140002d30((longlong)pvVar24 + 0x18);
  }
  if (pvVar21 != (void *)0x0) {
    free(pvVar21);
  }
  param_1[0x49] = 0;
  for (puVar19 = (undefined8 *)param_1[0x3b]; puVar19 != param_1 + 0x3b;
      puVar19 = (undefined8 *)*puVar19) {
    if (puVar19[5] != 0) {
      LOCK();
      piVar1 = (int *)(puVar19[5] + 8);
      *piVar1 = *piVar1 + 1;
      UNLOCK();
    }
    lVar22 = puVar19[4];
    plVar15 = (longlong *)puVar19[5];
    uVar8 = *(uint *)(lVar22 + 0x28);
    uVar9 = *(uint *)(lVar22 + 0xe0);
    iVar3 = *(int *)(lVar22 + 0x200);
    iVar10 = *(int *)(lVar22 + 0x1fc);
    iVar11 = *(int *)(lVar22 + 0x2c);
    iVar12 = *(int *)(lVar22 + 0x1f8);
    iVar13 = *(int *)(lVar22 + 0xe4);
    if (plVar15 != (longlong *)0x0) {
      LOCK();
      plVar2 = plVar15 + 1;
      lVar22 = *plVar2;
      *(int *)plVar2 = (int)*plVar2 + -1;
      UNLOCK();
      if ((int)lVar22 == 1) {
        (**(code **)*plVar15)(plVar15);
        LOCK();
        piVar1 = (int *)((longlong)plVar15 + 0xc);
        iVar4 = *piVar1;
        *piVar1 = *piVar1 + -1;
        UNLOCK();
        if (iVar4 == 1) {
          (**(code **)(*plVar15 + 8))(plVar15);
        }
      }
    }
    param_1[0x49] =
         param_1[0x49] +
         (longlong)(iVar3 * iVar10 * iVar12) + ((ulonglong)uVar9 + (ulonglong)uVar8 * 2) * 8 +
         (ulonglong)(uint)(iVar13 << 2) + (ulonglong)(uint)(iVar11 << 2) + 0x210;
  }
  return;
}


// Incoming references
// 0xc31a78 DATA caller none
// 0x63fa7c UNCONDITIONAL_CALL caller 14063f8b0
