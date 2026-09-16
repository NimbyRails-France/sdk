
undefined8
FUN_140443ac0(longlong param_1,longlong *param_2,undefined8 param_3,longlong param_4,char param_5,
             int param_6,char param_7)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined8 uVar7;
  longlong lVar8;
  longlong lVar9;
  undefined4 uVar10;
  bool bVar11;
  int iVar12;
  undefined4 uVar13;
  longlong lVar14;
  longlong lVar15;
  longlong lVar16;
  ulonglong *puVar17;
  longlong *plVar18;
  ulonglong *puVar19;
  longlong lVar20;
  undefined8 uVar21;
  float fVar22;
  ulonglong local_res10 [2];
  int local_res20 [2];
  ulonglong uVar23;
  ulonglong local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  int local_150 [2];
  undefined8 local_148;
  undefined8 uStack_140;
  undefined8 local_138;
  undefined8 local_e8;
  undefined4 uStack_e0;
  uint uStack_dc;
  undefined8 local_d8;
  undefined1 uStack_d0;
  undefined1 uStack_cf;
  undefined6 uStack_ce;
  undefined1 local_c8;
  uint7 uStack_c7;
  undefined8 uStack_c0;
  undefined1 local_b8;
  uint7 uStack_b7;
  void *local_b0 [4];
  void *local_90;

  if ((char)param_2[5] == '\0') {
    lVar14 = *param_2;
    if ((lVar14 == 0) || (*(longlong *)(lVar14 + 0x50) != *(longlong *)(lVar14 + 0x58))) {
      uVar23 = param_2[2];
      bVar11 = false;
      if (uVar23 != 0) {
        uStack_140 = *(undefined8 *)((longlong)param_2 + 0x2c);
        uStack_160 = *(undefined8 *)((longlong)param_2 + 0x2c);
        local_158 = CONCAT44(local_158._4_4_,(int)param_2[8]);
        local_138 = local_158;
        bVar11 = true;
        local_168 = uVar23;
        local_148 = uVar23;
      }
    }
    else {
      bVar11 = false;
    }
  }
  else {
    bVar11 = false;
  }
  lVar14 = *param_2;
  if (*(char *)(param_4 + 0x5d0) == '\0') {
    *(undefined8 *)(param_4 + 0xa8) = 1;
    if (*(char *)(param_4 + 0xb0) != '\0') {
      return 0;
    }
    *(undefined1 *)(param_4 + 0xb0) = 1;
    return 0;
  }
  if (!bVar11) {
    if ((((*(char *)(param_4 + 0x5f0) != '\0') && (*(longlong *)(param_4 + 0x5d8) != 0)) &&
        (lVar14 = FUN_14033f710(**(longlong **)(param_1 + 0x38) + 0x280), lVar14 != 0)) &&
       (lVar15 = FUN_1403e44d0(lVar14,*(undefined8 *)(param_4 + 0x5e0)), lVar15 != 0)) {
      FUN_140442740(param_1,lVar14,lVar15,param_3,param_4,3);
    }
    *(undefined1 *)(param_4 + 0x5d0) = 0;
    *(undefined1 *)(param_4 + 0x510) = 0;
    *(undefined1 *)(param_4 + 0x558) = 0;
    *(undefined1 *)(param_4 + 0x5f0) = 0;
    *(undefined1 *)(param_4 + 0x4f0) = 0;
    *(undefined8 *)(param_4 + 0xd0) = 0;
    return 0;
  }
  if ((local_148 == 0) ||
     (lVar15 = FUN_14033f7f0(**(longlong **)(param_1 + 0x30) + 0x180), lVar15 == 0)) {
LAB_140443fdb:
    *(undefined8 *)(param_4 + 0xa8) = 1;
    if (*(char *)(param_4 + 0xb0) == '\0') {
      *(undefined1 *)(param_4 + 0xb0) = 1;
    }
    return 0;
  }
  uVar10 = uStack_140._4_4_;
  lVar16 = FUN_1404225e0(lVar15,uStack_140._4_4_);
  if (lVar16 == 0) goto LAB_140443fdb;
  *(undefined4 *)(param_4 + 0x5c8) = uVar10;
  iVar12 = (int)uStack_140;
  if (lVar14 != 0) {
    local_res20[0] = (int)uStack_140;
    lVar3 = *(longlong *)(lVar14 + 0x50);
    local_150[0] = (int)(*(longlong *)(lVar14 + 0x58) - lVar3 >> 3);
    local_res10[0] = local_res10[0] & 0xffffffff00000000;
    puVar19 = local_res10;
    if (-1 < (int)uStack_140) {
      puVar19 = (ulonglong *)local_res20;
    }
    puVar17 = (ulonglong *)local_150;
    if ((int)uStack_140 <= local_150[0]) {
      puVar17 = puVar19;
    }
    lVar20 = (longlong)*(int *)(lVar3 + 4 + (longlong)(int)*puVar17 * 8) * 0x20;
    puVar1 = (undefined8 *)(lVar20 + *(longlong *)(lVar14 + 0x68));
    uStack_140 = puVar1[1];
    puVar2 = (undefined8 *)(lVar20 + 0x10 + *(longlong *)(lVar14 + 0x68));
    uVar21 = *puVar2;
    uVar7 = puVar2[1];
    local_148 = CONCAT44((int)((ulonglong)*puVar1 >> 0x20),
                         *(undefined4 *)(lVar3 + (longlong)(int)*puVar17 * 8));
    *(ulonglong *)(param_4 + 0x5a0) = local_148;
    *(undefined8 *)(param_4 + 0x5a8) = uStack_140;
    *(undefined8 *)(param_4 + 0x5b0) = uVar21;
    *(undefined8 *)(param_4 + 0x5b8) = uVar7;
  }
  if (*(char *)(param_4 + 0x5f0) != '\0') {
    *(int *)(param_4 + 0x5e8) = iVar12;
  }
  if (lVar14 != 0) {
    if (*(longlong *)(lVar14 + 0x58) - *(longlong *)(lVar14 + 0x50) == 8) {
      *(longlong *)(param_4 + 0x5c0) = *(longlong *)(param_4 + 0x5c0) + 0x93a80;
    }
    else {
      iVar12 = (*(int *)(param_4 + 0x5a0) - param_6) % 0x93a80;
      if (iVar12 < 0) {
        iVar12 = iVar12 + 0x93a80;
      }
      *(longlong *)(param_4 + 0x5c0) = *(longlong *)(param_4 + 0x5c0) + (longlong)iVar12;
    }
  }
  if (param_7 == '\0') {
    return 1;
  }
  local_168 = *(ulonglong *)(param_4 + 0xb8);
  uStack_160 = *(undefined8 *)(param_4 + 0xc0);
  local_158._0_1_ = (char)*(undefined8 *)(param_4 + 200);
  local_158 = CONCAT71((int7)((ulonglong)*(undefined8 *)(param_4 + 200) >> 8),-(char)local_158);
  FUN_140377010(local_b0,*(undefined8 *)(param_1 + 0x28),&local_168,
                SUB84((double)*(float *)(param_4 + 0x40),0));
  iVar12 = FUN_14036b180(lVar16,local_b0);
  if (iVar12 < 0) {
    if ((*(longlong *)(lVar16 + 0x78) != 0) &&
       (lVar14 = FUN_14032c420(**(undefined8 **)(param_1 + 0x28)), lVar14 != 0)) {
      if ((*(int *)(lVar15 + 0xfc) != 1) ||
         ((*(int *)(lVar15 + 0x100) != 1 ||
          (lVar14 = *(longlong *)(lVar16 + 0xf0), lVar14 == *(longlong *)(lVar16 + 0xf8))))) {
        lVar14 = lVar16 + 8;
      }
      uVar23 = (ulonglong)(*(int *)(lVar16 + 0xc0) + *(int *)(lVar16 + 0xe0) != 0);
      plVar18 = (longlong *)FUN_14036c5f0(*(undefined8 *)(param_1 + 0x30),&local_148,lVar14);
      lVar3 = plVar18[4];
      uVar13 = *(undefined4 *)((longlong)plVar18 + 0x24);
      lVar20 = plVar18[5];
      uVar4 = *(undefined4 *)((longlong)plVar18 + 0x2c);
      lVar8 = plVar18[2];
      uVar5 = *(undefined4 *)((longlong)plVar18 + 0x14);
      lVar9 = plVar18[3];
      uVar6 = *(undefined4 *)((longlong)plVar18 + 0x1c);
      lVar14 = plVar18[1];
      if (*plVar18 != 0) {
        local_e8 = 0;
        uStack_e0 = 0;
        local_d8 = 0;
        uStack_d0 = *(int *)(lVar16 + 0xe0) + *(int *)(lVar16 + 0xc0) != 0;
        uStack_cf = 0;
        local_c8 = 0;
        local_b8 = 0;
        *(longlong *)(param_4 + 0x220) = *plVar18;
        *(longlong *)(param_4 + 0x228) = lVar14;
        *(int *)(param_4 + 0x230) = (int)lVar8;
        *(undefined4 *)(param_4 + 0x234) = uVar5;
        *(int *)(param_4 + 0x238) = (int)lVar9;
        *(undefined4 *)(param_4 + 0x23c) = uVar6;
        *(int *)(param_4 + 0x240) = (int)lVar3;
        *(undefined4 *)(param_4 + 0x244) = uVar13;
        *(int *)(param_4 + 0x248) = (int)lVar20;
        *(undefined4 *)(param_4 + 0x24c) = uVar4;
        *(undefined8 *)(param_4 + 0x250) = 0;
        *(ulonglong *)(param_4 + 600) = (ulonglong)uStack_dc << 0x20;
        *(undefined8 *)(param_4 + 0x260) = 0;
        *(ulonglong *)(param_4 + 0x268) = CONCAT62(uStack_ce,(ushort)(byte)uStack_d0);
        *(ulonglong *)(param_4 + 0x270) = (ulonglong)uStack_c7 << 8;
        *(undefined8 *)(param_4 + 0x278) = uStack_c0;
        *(ulonglong *)(param_4 + 0x280) = (ulonglong)uStack_b7 << 8;
        if (*(char *)(param_4 + 0x288) == '\0') {
          *(undefined1 *)(param_4 + 0x288) = 1;
        }
        *(float *)(param_4 + 600) = *(float *)(lVar16 + 200) * DAT_140aab908;
        if (param_5 == '\0') {
          fVar22 = *(float *)(lVar16 + 0xc4);
          if (*(float *)(lVar16 + 0xc4) <= DAT_140aabae4) {
            fVar22 = DAT_140aabae4;
          }
          *(float *)(param_4 + 0x250) = fVar22;
          uVar13 = *(undefined4 *)(lVar16 + 0xb4);
        }
        else {
          *(undefined4 *)(param_4 + 0x250) = *(undefined4 *)(param_4 + 0x5b8);
          uVar13 = *(undefined4 *)(param_4 + 0x5b0);
        }
        *(undefined4 *)(param_4 + 0x254) = uVar13;
        iVar12 = FUN_1403554e0(param_4 + 0x5a0,lVar15 + 0x118,uVar10,(int)lVar3,uVar23);
        lVar15 = ((longlong)iVar12 +
                 (*(longlong *)(param_4 + 0x5c0) -
                 *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x20))) * 1000000;
        lVar14 = 0;
        if (0 < lVar15) {
          lVar14 = lVar15;
        }
        *(longlong *)(param_4 + 0x260) = lVar14;
        goto LAB_140443f93;
      }
    }
    local_res10[0] = 1;
    *(undefined8 *)(param_4 + 0xa8) = 1;
    if (*(char *)(param_4 + 0xb0) == '\0') {
      *(undefined1 *)(param_4 + 0xb0) = 1;
    }
    uVar21 = 0;
  }
  else {
    local_168 = local_168 & 0xffffffff00000000;
    uStack_160 = 0;
    *(ulonglong *)(param_4 + 0x5f8) = local_168;
    *(undefined8 *)(param_4 + 0x600) = 0;
    if (*(char *)(param_4 + 0x608) == '\0') {
      *(undefined1 *)(param_4 + 0x608) = 1;
      uVar21 = 1;
      goto LAB_140443fc2;
    }
LAB_140443f93:
    uVar21 = 1;
  }
LAB_140443fc2:
  if (local_b0[0] == (void *)0x0) {
    return uVar21;
  }
  if (local_b0[0] != local_90) {
    free(local_b0[0]);
    return uVar21;
  }
  return uVar21;
}
