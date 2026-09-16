// Candidate VA 1406b7c00; RVA 0x6b7c00
// Ghidra inferred prototype: undefined FUN_1406b7c00()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1406b7c00(longlong param_1,longlong *param_2,longlong param_3)

{
  longlong *plVar1;
  longlong lVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  double dVar5;
  double dVar6;
  int iVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  longlong lVar12;
  longlong lVar13;
  ulonglong *puVar14;
  int iVar15;
  undefined8 uVar16;
  uint uVar17;
  longlong lVar18;
  ulonglong *puVar19;
  float fVar20;
  double dVar21;
  double dVar22;
  undefined4 uVar23;
  undefined4 uVar24;
  undefined4 uStackX_1c;
  undefined8 local_188;
  double dStack_180;
  undefined8 local_178;
  undefined8 local_170;
  longlong local_168;
  double local_160;
  double local_158;
  longlong local_150;
  longlong local_148;
  undefined8 *local_140;
  undefined8 local_128;
  undefined8 uStack_120;
  undefined8 local_118;
  undefined2 uStack_110;
  undefined2 uStack_10e;
  undefined4 uStack_10c;
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  double dStack_e8;
  undefined8 uStack_e0;
  uint local_d8;
  undefined4 uStack_d4;
  undefined8 uStack_d0;
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
  double local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;

  lVar13 = *(longlong *)(param_3 + 600);
  if ((lVar13 == 0) || (lVar2 = *(longlong *)(param_3 + 0x260), lVar2 == 0)) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  local_170 = *(undefined8 *)(lVar13 + 0x408);
  plVar1 = (longlong *)(lVar13 + 0x890);
  local_140 = (undefined8 *)(lVar13 + 0x428);
  FUN_140362640(param_3 + 0x38,&local_188,param_3,param_1 + 8);
  lVar12 = *(longlong *)(lVar2 + 0x2200);
  uVar3 = *(ulonglong *)(param_1 + 8);
  lVar18 = *(longlong *)(lVar12 + 0x350);
  for (puVar14 = *(ulonglong **)(lVar18 + (uVar3 % (ulonglong)*(uint *)(lVar12 + 0x358)) * 8);
      puVar14 != (ulonglong *)0x0; puVar14 = (ulonglong *)puVar14[9]) {
    if (uVar3 == *puVar14) goto LAB_1406b7cc0;
  }
  puVar14 = *(ulonglong **)(lVar18 + *(longlong *)(lVar12 + 0x358) * 8);
LAB_1406b7cc0:
  puVar19 = puVar14 + 1;
  if (puVar14 == *(ulonglong **)(lVar18 + *(longlong *)(lVar12 + 0x358) * 8)) {
    puVar19 = (ulonglong *)0x0;
  }
  if (puVar19 == (ulonglong *)0x0) {
    return;
  }
  if (uVar3 == 0) {
    return;
  }
  local_148 = FUN_14033f780(*plVar1 + 0x200,uVar3);
  if (local_148 == 0) {
    return;
  }
  puVar10 = (undefined8 *)FUN_14045f300();
  dVar6 = DAT_140aac2c0;
  dVar5 = DAT_140aac298;
  dVar22 = DAT_140aab8f0;
  if (puVar10 == (undefined8 *)0x0) {
    return;
  }
  iVar7 = 0;
  iVar15 = 0;
  if (*(char *)(puVar10 + 0x3a) != '\0') {
    iVar7 = (int)(((double)puVar10[0x2b] * DAT_140aabc48 + (double)puVar10[0x29]) * DAT_140aabf88);
    dVar21 = exp(((double)puVar10[0x2c] * DAT_140aabc48 + (double)puVar10[0x2a]) * DAT_140aab8f0);
    dVar21 = atan(dVar21);
    iVar15 = (int)(dVar21 * dVar6 - dVar5);
  }
  local_160 = (double)iVar7 * DAT_140aab8d8;
  local_158 = (double)iVar15 * DAT_140aab8d8;
  FUN_140284630(&local_188,&local_160);
  FUN_1406ab1b0(param_1 + 0x10,lVar13,lVar2,lVar13 + 0x428,lVar13 + 0x780,plVar1,puVar19 + 1,
                lVar2 + 0x20,&local_188);
  if (*(int *)(param_1 + 0x748) != *(int *)(param_1 + 0x6e8)) {
    *(undefined1 *)(param_1 + 0x75c) = 0;
  }
  *(int *)(param_1 + 0x748) = *(int *)(param_1 + 0x6e8);
  if (*(char *)(param_1 + 0x75c) != '\0') {
    FUN_14032a5d0(param_1 + 0x4d0);
  }
  if ((*(char *)(puVar10 + 0xbe) == '\0') || (puVar10[0xbb] == 0)) {
    puVar11 = (undefined8 *)0x0;
LAB_1406b7eb3:
    local_160 = 0.0;
  }
  else {
    puVar11 = (undefined8 *)FUN_14033f710(*plVar1 + 0x280);
    if (puVar11 == (undefined8 *)0x0) goto LAB_1406b7eb3;
    local_160 = (double)FUN_1403e44d0(puVar11,puVar10[0xbc]);
  }
  *(undefined4 *)(param_1 + 0x6e0) = 2;
  *(longlong *)(param_1 + 0x5b8) = lVar13 + 0x780;
  *(undefined4 *)(param_1 + 0x6f4) = *(undefined4 *)(param_1 + 0x758);
  *(undefined1 *)(param_1 + 0x6e4) = 0;
  *(undefined8 *)(param_1 + 0x6f8) = 0x41800000;
  lVar12 = FUN_1402d82e0("traineditor_tz_orders","Orders");
  lVar18 = -1;
  do {
    lVar18 = lVar18 + 1;
  } while (*(char *)(lVar12 + lVar18) != '\0');
  FUN_140030630(param_1 + 0x718,lVar12);
  dVar21 = (double)param_2[0x23] * DAT_140aabf90;
  dVar22 = exp((double)param_2[0x24] * dVar22);
  dVar22 = atan(dVar22);
  iVar7 = (int)(dVar22 * dVar6 - dVar5);
  uVar8 = FUN_140488630(*(undefined8 *)(lVar13 + 0x410),CONCAT44(iVar7,(int)dVar21));
  if (puVar11 == (undefined8 *)0x0) {
    uVar16 = 0;
  }
  else {
    uVar16 = *puVar11;
  }
  uVar9 = FUN_1403e7c10(lVar13 + 0x890,uVar16);
  uVar23 = uVar8;
  if (((*(longlong *)(param_1 + 0x728) == 0) || (uVar23 = uVar9, *(int *)(param_1 + 0x700) == 0)) ||
     (uVar23 = uVar8, *(int *)(param_1 + 0x700) == 1)) {
    *(undefined4 *)(param_1 + 0x740) = uVar23;
  }
  fVar20 = DAT_140aac3d0;
  *(undefined4 *)(param_1 + 0x74c) = *(undefined4 *)(param_1 + 0x740);
  *(undefined4 *)(param_1 + 0x5c0) = *(undefined4 *)(param_1 + 0x740);
  *(float *)(param_1 + 0x618) =
       fVar20 / (float)(1 << ((byte)*(undefined4 *)(param_1 + 0x704) & 0x1f));
  FUN_14058dc50(param_1 + 0x610);
  fVar20 = DAT_140aac530 / *(float *)(param_1 + 0x618);
  *(longlong *)(param_1 + 0x5a8) = local_148;
  *(undefined8 **)(param_1 + 0x5b0) = puVar10;
  *(undefined8 **)(param_1 + 0x598) = puVar11;
  *(double *)(param_1 + 0x5a0) = local_160;
  *(float *)(param_1 + 0x4c0) = *(float *)(param_1 + 0x618);
  *(float *)(param_1 + 0x750) = fVar20;
  fVar20 = *(float *)(*param_2 + 0x1550);
  if (*(float *)(*param_2 + 0x1550) < 0.0) {
    fVar20 = DAT_140aabae4;
  }
  uStackX_1c = CONCAT31((int3)((uint)iVar7 >> 8),1);
  *(undefined4 *)(param_1 + 0x6d8) = 0xffffffff;
  *(ulonglong *)(param_1 + 0x620) = CONCAT44(uStackX_1c,fVar20 * *(float *)(param_1 + 0x754));
  iVar7 = *(int *)(lVar2 + 0x20f8) % 0x93a80;
  iVar15 = iVar7;
  if (iVar7 < 0) {
    iVar15 = iVar7 + 0x93a80;
  }
  iVar15 = (iVar15 + *(int *)(param_1 + 0x74c)) % 0x93a80;
  if (iVar15 < 0) {
    iVar15 = iVar15 + 0x93a80;
  }
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0x93a80;
  }
  iVar7 = (iVar7 + *(int *)(param_1 + 0x74c)) % 0x15180;
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0x15180;
  }
  if (*(char *)(param_1 + 0x75c) == '\0') {
    if (iVar15 / 0x15180 == *(int *)(param_1 + 0x6ec)) goto LAB_1406b8167;
  }
  else {
    *(int *)(param_1 + 0x6ec) = iVar15 / 0x15180;
LAB_1406b8167:
    *(int *)(param_1 + 0x6d8) = iVar7;
  }
  if (*(char *)(puVar10 + 0x96) == '\0') {
    *(undefined4 *)(param_1 + 0x760) = 0;
LAB_1406b83f0:
    *(undefined1 *)(param_1 + 0x7d8) = 0;
    goto LAB_1406b83f7;
  }
  uStack_d0 = *puVar10;
  local_128 = puVar10[0x77];
  uVar16 = puVar10[0x75];
  uVar4 = puVar10[0x76];
  uVar17 = 0xffffffff;
  uStack_120 = 0;
  local_108 = 0;
  uStack_f0 = 0;
  local_d8 = 0xffffffff;
  local_118 = 0;
  uStack_100 = 0;
  dStack_e8 = 0.0;
  uStack_110 = 0x101;
  local_f8 = CONCAT62(local_f8._2_6_,0x101);
  uStack_e0 = CONCAT62(uStack_e0._2_6_,0x101);
  lVar13 = FUN_14044ffe0(lVar2 + 0xa0,lVar13 + 0x400);
  puVar11 = local_140;
  if (lVar13 != 0) {
    uVar17 = *(uint *)(lVar13 + 0xb8) | 0xff000000;
    local_d8 = uVar17;
  }
  if (*(int *)(param_1 + 0x760) == 1) {
    local_98 = puVar10[0x58];
    uStack_90 = puVar10[0x59];
    local_88 = puVar10[0x5a];
    uStack_80 = puVar10[0x5b];
    local_78 = (double)puVar10[0x5c];
    uStack_70 = puVar10[0x5d];
    local_b8 = local_128;
    uStack_b0 = uStack_120;
    local_68 = CONCAT44(uStack_d4,local_d8);
    uStack_60 = uStack_d0;
    local_a8 = local_118;
    uStack_a0 = CONCAT44(uStack_10c,CONCAT22(uStack_10e,uStack_110));
    local_c8 = uVar16;
    uStack_c0 = uVar4;
    FUN_14066fa20(&local_c8);
    goto LAB_1406b83f7;
  }
  if (*(int *)(param_1 + 0x760) != 2) goto LAB_1406b83f0;
  FUN_140647a30(&local_160,param_2,local_140);
  if ((local_150 == 0) ||
     (puVar11 = (undefined8 *)FUN_14032c420(*puVar11), puVar11 == (undefined8 *)0x0)) {
    uVar8 = (undefined4)local_108;
    uVar23 = local_108._4_4_;
    uVar9 = (undefined4)uStack_100;
    uVar24 = uStack_100._4_4_;
  }
  else {
    uStack_f0 = *puVar11;
    local_178._0_2_ = CONCAT11(*(undefined1 *)((longlong)puVar11 + 0x2c),1);
    local_f8 = local_178;
    local_188._0_4_ = (undefined4)uStack_f0;
    local_188._4_4_ = (undefined4)((ulonglong)uStack_f0 >> 0x20);
    local_178._0_2_ = CONCAT11(*(undefined1 *)((longlong)puVar11 + 0x2c),0xff);
    dStack_180 = local_158;
    dStack_e8 = local_158;
    uStack_e0 = local_178;
    uVar8 = (undefined4)local_188;
    uVar23 = local_188._4_4_;
    uVar9 = SUB84(local_158,0);
    uVar24 = (int)((ulonglong)local_158 >> 0x20);
    local_188 = uStack_f0;
  }
  uStack_a0 = CONCAT44(uStack_10c,CONCAT22(uStack_10e,uStack_110));
  if (*(char *)((longlong)param_2 + 0xf1) == '\0') {
LAB_1406b8343:
    if (*(char *)(param_1 + 0x7d8) == '\0') {
      local_b8 = local_128;
      uStack_b0 = uStack_120;
      local_a8 = local_118;
      local_98 = CONCAT44(uVar23,uVar8);
      uStack_90 = CONCAT44(uVar24,uVar9);
      local_88 = local_f8;
      uStack_80 = uStack_f0;
      local_78 = dStack_e8;
      uStack_70 = uStack_e0;
      local_68 = CONCAT44(uStack_d4,local_d8);
      uStack_60 = uStack_d0;
      local_c8 = uVar16;
      uStack_c0 = uVar4;
      FUN_14066fa20(&local_c8);
      goto LAB_1406b83f7;
    }
  }
  else if (*(char *)(param_1 + 0x7d8) == '\0') {
    *(undefined8 *)(param_1 + 0x768) = uVar16;
    *(undefined8 *)(param_1 + 0x770) = uVar4;
    *(undefined1 *)(param_1 + 0x7d8) = 1;
    *(undefined4 *)(param_1 + 0x778) = (undefined4)local_128;
    *(undefined4 *)(param_1 + 0x77c) = local_128._4_4_;
    *(undefined4 *)(param_1 + 0x780) = (undefined4)uStack_120;
    *(undefined4 *)(param_1 + 0x784) = uStack_120._4_4_;
    *(undefined8 *)(param_1 + 0x788) = local_118;
    *(undefined8 *)(param_1 + 0x790) = uStack_a0;
    *(undefined4 *)(param_1 + 0x798) = uVar8;
    *(undefined4 *)(param_1 + 0x79c) = uVar23;
    *(undefined4 *)(param_1 + 0x7a0) = uVar9;
    *(undefined4 *)(param_1 + 0x7a4) = uVar24;
    *(undefined8 *)(param_1 + 0x7a8) = local_f8;
    *(undefined8 *)(param_1 + 0x7b0) = uStack_f0;
    *(double *)(param_1 + 0x7b8) = dStack_e8;
    *(undefined8 *)(param_1 + 0x7c0) = uStack_e0;
    *(uint *)(param_1 + 0x7c8) = local_d8;
    *(undefined4 *)(param_1 + 0x7cc) = uStack_d4;
    *(undefined4 *)(param_1 + 2000) = (undefined4)uStack_d0;
    *(undefined4 *)(param_1 + 0x7d4) = uStack_d0._4_4_;
    goto LAB_1406b8343;
  }
  *(undefined8 *)(param_1 + 0x768) = uVar16;
  *(undefined8 *)(param_1 + 0x770) = uVar4;
  *(uint *)(param_1 + 0x7c8) = uVar17;
  *(undefined4 *)(param_1 + 0x778) = (undefined4)local_128;
  *(undefined4 *)(param_1 + 0x77c) = local_128._4_4_;
  *(undefined4 *)(param_1 + 0x780) = (undefined4)uStack_120;
  *(undefined4 *)(param_1 + 0x784) = uStack_120._4_4_;
  *(undefined8 *)(param_1 + 0x788) = local_118;
  *(undefined8 *)(param_1 + 0x790) = uStack_a0;
  local_c8 = *(undefined8 *)(param_1 + 0x768);
  uStack_c0 = *(undefined8 *)(param_1 + 0x770);
  local_b8 = *(undefined8 *)(param_1 + 0x778);
  uStack_b0 = *(undefined8 *)(param_1 + 0x780);
  local_a8 = *(undefined8 *)(param_1 + 0x788);
  uStack_a0 = *(undefined8 *)(param_1 + 0x790);
  local_98 = *(undefined8 *)(param_1 + 0x798);
  uStack_90 = *(undefined8 *)(param_1 + 0x7a0);
  local_88 = *(undefined8 *)(param_1 + 0x7a8);
  uStack_80 = *(undefined8 *)(param_1 + 0x7b0);
  local_78 = *(double *)(param_1 + 0x7b8);
  uStack_70 = *(undefined8 *)(param_1 + 0x7c0);
  local_68 = *(undefined8 *)(param_1 + 0x7c8);
  uStack_60 = *(undefined8 *)(param_1 + 2000);
  FUN_14066fa20(&local_c8);
LAB_1406b83f7:
  local_188 = local_170;
  if (*(char *)(puVar10 + 0x96) == '\0') {
    uVar8 = 0;
  }
  else {
    uVar8 = SUB84((double)puVar10[0x79] * _DAT_140aabdf0,0);
  }
  dStack_180 = (double)param_1;
  local_178 = lVar2;
  local_168 = param_1;
  FUN_1406b85c0(&local_188,0xe,uVar8,0xff22ffff);
  FUN_1406b8ad0(&local_170,0xe,*(undefined1 *)(param_1 + 0x4a8));
  if ((*(char *)(puVar10 + 0x96) == '\0') ||
     (lVar13 = FUN_14043e0c0(puVar10 + 0x52,puVar10[0x75],*(longlong *)(lVar2 + 0x28) * 10000),
     (double)lVar13 * DAT_140aab938 < DAT_140aabe78)) {
    uVar8 = 0;
  }
  else {
    uVar8 = round();
  }
  FUN_1406b85c0(&local_188,0xc,uVar8,0xff2222ff);
  FUN_1406b8ad0(&local_170,0xc,*(undefined1 *)(param_1 + 0x4aa));
  if (*(char *)(puVar10 + 0x96) == '\0') {
    uVar8 = 0;
  }
  else {
    uVar8 = SUB84((double)puVar10[0x7b] - (double)puVar10[0x78],0);
    if ((double)puVar10[0x7b] - (double)puVar10[0x78] <= 0.0) {
      uVar8 = 0;
    }
  }
  FUN_1406b85c0(&local_188,10,uVar8,0xff22ff22);
  FUN_1406b8ad0(&local_170,10,*(undefined1 *)(param_1 + 0x4a9));
  uVar3 = puVar19[7];
  if ((longlong)uVar3 < 0) {
    uVar8 = SUB84((double)uVar3,0);
  }
  else {
    uVar8 = SUB84((double)(longlong)uVar3,0);
  }
  FUN_1406b85c0(&local_188,0,uVar8,0xffff22bb);
  FUN_1406b8ad0(&local_170,0,*(undefined1 *)(param_1 + 0x4ab));
  return;
}


// Incoming references
// 0xc33d94 DATA caller none
// 0xb2db6c DATA caller none
// 0xb2dbe8 DATA caller none
// 0xa92bd8 DATA caller none
