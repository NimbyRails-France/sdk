// Candidate VA 14043ef00; RVA 0x43ef00
// Ghidra inferred prototype: undefined FUN_14043ef00()

void FUN_14043ef00(longlong *param_1,longlong *param_2)

{
  undefined8 *puVar1;
  undefined4 *puVar2;
  short sVar3;
  longlong lVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined8 uVar9;
  undefined4 uVar10;
  longlong lVar11;
  int *piVar12;
  int *piVar13;
  longlong lVar14;
  int iVar15;
  int iVar16;
  char cVar17;
  int local_res8 [2];
  int local_res18 [2];
  int local_res20 [2];
  int local_48 [4];
  undefined4 local_38;
  undefined4 uStack_34;
  undefined8 uStack_30;

  if ((param_1[2] == 0) || (lVar11 = FUN_14033f7f0(*param_2 + 0x180), lVar11 == 0)) {
    if (*param_1 != 0) {
      *(undefined1 *)((longlong)param_1 + 0x44) = 1;
    }
    *(int *)((longlong)param_1 + 0x3c) = *(int *)((longlong)param_1 + 0x3c) + 1;
    return;
  }
  if ((char)param_1[5] != '\0') {
    *(undefined1 *)(param_1 + 5) = 0;
    return;
  }
  cVar17 = *(char *)((longlong)param_1 + 0xc);
  iVar16 = (int)param_1[6] + 1;
  *(int *)(param_1 + 6) = iVar16;
  sVar3 = *(short *)((longlong)param_1 + 0x26);
  iVar15 = (int)(*(longlong *)(lVar11 + 0x120) - *(longlong *)(lVar11 + 0x118) >> 3) * 0x2fa0be83 +
           -1;
  if (sVar3 != -1) {
    local_48[1] = (int)sVar3;
    local_48[2] = iVar15;
    local_48[0] = 0;
    piVar13 = local_48;
    if (-1 < sVar3) {
      piVar13 = local_48 + 1;
    }
    piVar12 = local_48 + 2;
    if (sVar3 <= iVar15) {
      piVar12 = piVar13;
    }
    iVar15 = *piVar12;
  }
  if (iVar16 <= iVar15) {
    return;
  }
  lVar11 = *param_1;
  if (lVar11 == 0) {
    *(undefined1 *)(param_1 + 5) = 1;
    return;
  }
  iVar15 = (int)(*(longlong *)(lVar11 + 0x58) - *(longlong *)(lVar11 + 0x50) >> 3);
  if (iVar15 < 0) {
    iVar16 = FUN_140360e20(-*(int *)((longlong)param_1 + 0x34),-iVar15);
  }
  else {
    iVar16 = *(int *)((longlong)param_1 + 0x34) % iVar15;
    if (iVar16 < 0) {
      iVar16 = iVar16 + iVar15;
    }
  }
  if (*(int *)((longlong)param_1 + 0x2c) == iVar16) {
    *(int *)((longlong)param_1 + 0x3c) = *(int *)((longlong)param_1 + 0x3c) + 1;
  }
  local_res18[0] = *(int *)((longlong)param_1 + 0x2c) + 1;
  iVar15 = (int)(*(longlong *)(lVar11 + 0x58) - *(longlong *)(lVar11 + 0x50) >> 3);
  if (iVar15 < 0) {
    local_res18[0] = FUN_140360e20(-local_res18[0],-iVar15);
  }
  else {
    local_res18[0] = local_res18[0] % iVar15;
    if (local_res18[0] < 0) {
      local_res18[0] = local_res18[0] + iVar15;
    }
  }
  *(int *)((longlong)param_1 + 0x2c) = local_res18[0];
  lVar4 = *(longlong *)(lVar11 + 0x50);
  local_res20[0] = (int)(*(longlong *)(lVar11 + 0x58) - lVar4 >> 3);
  local_res8[0] = 0;
  piVar13 = local_res8;
  if (-1 < local_res18[0]) {
    piVar13 = local_res18;
  }
  piVar12 = local_res20;
  if (local_res18[0] <= local_res20[0]) {
    piVar12 = piVar13;
  }
  lVar14 = (longlong)*(int *)(lVar4 + 4 + (longlong)*piVar12 * 8) * 0x20;
  puVar1 = (undefined8 *)(lVar14 + *(longlong *)(lVar11 + 0x68));
  uVar9 = puVar1[1];
  puVar2 = (undefined4 *)(lVar14 + 0x10 + *(longlong *)(lVar11 + 0x68));
  uVar5 = *puVar2;
  uVar6 = puVar2[1];
  uVar7 = puVar2[2];
  uVar8 = puVar2[3];
  uVar10 = *(undefined4 *)(lVar4 + (longlong)*piVar12 * 8);
  uStack_34 = (undefined4)((ulonglong)*puVar1 >> 0x20);
  _local_38 = CONCAT44(uStack_34,uVar10);
  uStack_30._0_4_ = (undefined4)uVar9;
  uStack_30._4_4_ = (undefined4)((ulonglong)uVar9 >> 0x20);
  *(bool *)(param_1 + 5) = cVar17 == '\0';
  *(undefined4 *)(param_1 + 1) = uVar10;
  *(undefined4 *)((longlong)param_1 + 0xc) = uStack_34;
  *(undefined4 *)(param_1 + 2) = (undefined4)uStack_30;
  *(undefined4 *)((longlong)param_1 + 0x14) = uStack_30._4_4_;
  *(undefined4 *)(param_1 + 3) = uVar5;
  *(undefined4 *)((longlong)param_1 + 0x1c) = uVar6;
  *(undefined4 *)(param_1 + 4) = uVar7;
  *(undefined4 *)((longlong)param_1 + 0x24) = uVar8;
  if ((param_1[2] != 0) &&
     (uStack_30 = uVar9, lVar11 = FUN_14033f7f0(*param_2 + 0x180), lVar11 != 0)) {
    uVar10 = FUN_140355740(param_1 + 1,
                           (*(longlong *)(lVar11 + 0x120) - *(longlong *)(lVar11 + 0x118) >> 3) *
                           -0x7d05f417d05f417d);
    *(undefined4 *)(param_1 + 6) = uVar10;
    return;
  }
  *(undefined1 *)((longlong)param_1 + 0x44) = 1;
  return;
}


// Incoming references
// 0xc22fac DATA caller none
// 0xb072f4 DATA caller none
// 0xb07308 DATA caller none
// 0xb07318 DATA caller none
// 0x44a808 UNCONDITIONAL_CALL caller 14044a330
// 0x444165 UNCONDITIONAL_CALL caller 140444030
// 0x45bb03 UNCONDITIONAL_CALL caller 14045b3d0
// 0x45e4a0 UNCONDITIONAL_CALL caller 14045dd80
// 0x460d39 UNCONDITIONAL_CALL caller 140460610
// 0x460159 UNCONDITIONAL_CALL caller 14045fa30
// 0x45c833 UNCONDITIONAL_CALL caller 14045c0d0
// 0x804dba UNCONDITIONAL_CALL caller 140804000
