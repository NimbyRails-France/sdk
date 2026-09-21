// Candidate VA 14055c4b0; RVA 0x55c4b0
// Ghidra inferred prototype: undefined FUN_14055c4b0()

uint FUN_14055c4b0(longlong param_1)

{
  uint *puVar1;
  float *pfVar2;
  float fVar3;
  char cVar4;
  undefined4 uVar5;
  longlong lVar6;
  float fVar7;
  uint uVar8;
  uint uVar9;
  ulonglong uVar10;
  ulonglong uVar11;
  ulonglong uVar12;
  longlong *plVar13;
  longlong lVar14;
  float fVar15;
  float fVar16;
  uint local_res8 [2];

  cVar4 = *(char *)(param_1 + 0x120);
  *(undefined1 *)(param_1 + 0x120) = 0;
  if (cVar4 != '\0') {
    return 0;
  }
  uVar8 = FUN_14081bda0((longlong *)(param_1 + 0xc0));
  uVar12 = (ulonglong)uVar8;
  if (*(longlong *)(param_1 + 0xe0) != *(longlong *)(param_1 + 0xd8)) {
    uVar10 = (ulonglong)*(uint *)(*(longlong *)(param_1 + 0xe0) + -4);
    lVar6 = *(longlong *)(param_1 + 0xc0);
    uVar9 = *(uint *)(lVar6 + 4 + uVar10 * 0x24);
    uVar11 = (ulonglong)uVar9;
    if (uVar9 == 0xffffffff) {
      *(uint *)(lVar6 + 4 + uVar10 * 0x24) = uVar8;
      puVar1 = (uint *)(lVar6 + uVar12 * 0x24);
      *puVar1 = *puVar1 | 0x400;
    }
    else {
      uVar9 = *(uint *)(lVar6 + 8 + uVar11 * 0x24);
      while (uVar9 != 0xffffffff) {
        uVar11 = (ulonglong)uVar9;
        uVar9 = *(uint *)(lVar6 + 8 + uVar11 * 0x24);
      }
      lVar14 = lVar6 + uVar11 * 0x24;
      uVar5 = *(undefined4 *)(lVar14 + 8);
      puVar1 = (uint *)(lVar6 + uVar12 * 0x24);
      *puVar1 = *puVar1 | 0x400;
      *(undefined4 *)(lVar6 + 8 + uVar12 * 0x24) = uVar5;
      *(uint *)(lVar14 + 8) = uVar8;
    }
  }
  plVar13 = (longlong *)(param_1 + 0xc0);
  local_res8[0] = uVar8;
  if (*(longlong *)(param_1 + 0x98) != *(longlong *)(param_1 + 0x90)) {
    FUN_14055b3c0(*(longlong *)(param_1 + 0x98) + -0x78,(char *)(param_1 + 0x18));
  }
  fVar3 = *(float *)(param_1 + 0xb0);
  if (*(char *)(param_1 + 0x18) != '\0') {
    *(uint *)(*plVar13 + uVar12 * 0x24) =
         *(uint *)(*plVar13 + uVar12 * 0x24) & 0xfffffc1f | *(uint *)(param_1 + 0x1c);
  }
  if (*(char *)(param_1 + 0x20) != '\0') {
    *(uint *)(*plVar13 + uVar12 * 0x24) =
         *(uint *)(*plVar13 + uVar12 * 0x24) & 0xffffffe0 | *(uint *)(param_1 + 0x24);
  }
  if ((*(char *)(param_1 + 0x28) != '\0') || (*(char *)(param_1 + 0x30) != '\0')) {
    lVar6 = *plVar13;
    fVar15 = fVar3 * *(float *)(param_1 + 0x2c);
    fVar16 = fVar3 * *(float *)(param_1 + 0x34);
    *(float *)(lVar6 + 0x1c + uVar12 * 0x24) = fVar15;
    *(float *)(lVar6 + 0x20 + uVar12 * 0x24) = fVar16;
    uVar9 = *(uint *)(lVar6 + uVar12 * 0x24);
    if (fVar15 == 0.0) {
      uVar9 = uVar9 & 0xfffff7ff;
    }
    else {
      uVar9 = uVar9 | 0x800;
    }
    if (fVar16 == 0.0) {
      uVar9 = uVar9 & 0xffffefff;
    }
    else {
      uVar9 = uVar9 | 0x1000;
    }
    *(uint *)(lVar6 + uVar12 * 0x24) = uVar9;
  }
  if (*(char *)(param_1 + 0x38) != '\0') {
    fVar15 = *(float *)(param_1 + 0x40);
    fVar16 = *(float *)(param_1 + 0x44);
    fVar7 = *(float *)(param_1 + 0x48);
    pfVar2 = (float *)(*plVar13 + 0xc + uVar12 * 0x24);
    *pfVar2 = *(float *)(param_1 + 0x3c) * fVar3;
    pfVar2[1] = fVar15 * fVar3;
    pfVar2[2] = fVar16 * fVar3;
    pfVar2[3] = fVar7 * fVar3;
  }
  FUN_14055ba40(param_1);
  FUN_140290a60(param_1 + 0xf0,local_res8);
  return uVar8;
}


// Incoming references
// 0xc2d590 DATA caller none
// 0x55c70e UNCONDITIONAL_CALL caller 14055c6f0
// 0x55c96c UNCONDITIONAL_CALL caller 14055c950
// 0x55c9a7 UNCONDITIONAL_CALL caller 14055c990
// 0x55c9ec UNCONDITIONAL_CALL caller 14055c9c0
// 0x55ca1c UNCONDITIONAL_CALL caller 14055ca00
// 0x55ccfa UNCONDITIONAL_CALL caller 14055cc40
// 0x55cd62 UNCONDITIONAL_CALL caller 14055cd40
// 0x55cdb2 UNCONDITIONAL_CALL caller 14055cd80
// 0x55ce03 UNCONDITIONAL_CALL caller 14055cdd0
// 0x55ce3c UNCONDITIONAL_CALL caller 14055ce20
// 0x55ce83 UNCONDITIONAL_CALL caller 14055ce50
// 0x55ca51 UNCONDITIONAL_CALL caller 14055ca30
// 0x55cb24 UNCONDITIONAL_CALL caller 14055ca70
// 0x55cc08 UNCONDITIONAL_CALL caller 14055cb30
// 0x55cc31 UNCONDITIONAL_CALL caller 14055cc10
// 0x55cd3b UNCONDITIONAL_CALL caller 14055cd20
// 0x55cf2a UNCONDITIONAL_CALL caller 14055cea0
