// Candidate VA 14050c850; RVA 0x50c850
// Ghidra inferred prototype: undefined FUN_14050c850()

bool FUN_14050c850(uint *param_1,longlong param_2,float *param_3,uint *param_4,undefined8 param_5,
                  undefined4 param_6,undefined8 param_7,longlong param_8,longlong param_9,
                  longlong param_10)

{
  float fVar1;
  float fVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float local_e8;
  float local_e4;
  float local_e0;
  float local_dc;
  float local_d8;
  float local_d4;
  float local_d0;
  float local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  float local_b4;
  float local_b0;
  float local_ac;

  if ((((param_2 == 0) || (param_8 == 0)) || (param_10 == 0)) || (param_4 == (uint *)0x0)) {
    return false;
  }
  local_c0 = *(float *)(param_10 + 8);
  fVar12 = *(float *)(param_8 + 0xe4) + *(float *)(param_8 + 0xe4);
  fVar6 = local_c0 + fVar12;
  if (fVar6 <= param_3[2]) {
    fVar6 = param_3[2];
  }
  fVar1 = *(float *)(param_8 + 0xe8);
  param_3[2] = fVar6;
  fVar7 = local_c0 + fVar1 + fVar1;
  if (fVar7 <= param_3[3]) {
    fVar7 = param_3[3];
  }
  local_c8 = *param_3;
  fVar2 = *(float *)(param_8 + 0xf8);
  fVar8 = *(float *)(param_8 + 0xf0);
  fVar9 = *(float *)(param_8 + 0xec);
  fVar10 = param_3[1] - fVar8;
  param_3[3] = fVar7;
  fVar11 = local_c8 - fVar9;
  fVar8 = fVar8 + fVar8 + fVar7;
  fVar9 = fVar9 + fVar9 + fVar6;
  local_c4 = (fVar7 * DAT_140aab9dc + param_3[1]) - local_c0 * DAT_140aab9dc;
  local_d8 = local_c8 + *(float *)(param_8 + 0xe4) + fVar2;
  local_d4 = fVar1 + local_c4 + fVar2;
  local_d0 = local_c0 - (fVar2 + fVar2 + fVar12);
  local_b8 = local_c8 + local_c0 + *(float *)(param_8 + 0xf4);
  local_cc = local_c0 - (fVar2 + fVar2 + fVar1 + fVar1);
  local_b0 = local_c8 + fVar6;
  if (local_c8 + fVar6 < local_b8) {
    local_b0 = local_b8;
  }
  uVar4 = *param_4;
  local_b0 = local_b0 - local_b8;
  *param_1 = *param_1 & 2 | 4;
  param_7._0_4_ = uVar4;
  local_e8 = fVar11;
  local_e4 = fVar10;
  local_e0 = fVar9;
  local_dc = fVar8;
  local_bc = local_c0;
  local_b4 = local_c4;
  local_ac = local_c0;
  iVar3 = FUN_14050b8a0(local_cc,&local_e8,param_9,0);
  if (iVar3 != 0) {
    *param_1 = 0x22;
    uVar4 = (uint)(uVar4 == 0);
  }
  uVar5 = *param_1;
  if ((uVar5 & 0x10) == 0) {
    if (param_9 == 0) goto LAB_14050cb62;
LAB_14050cb33:
    if ((((*(float *)(param_9 + 0x23c) < fVar11) || (fVar9 + fVar11 <= *(float *)(param_9 + 0x23c)))
        || (*(float *)(param_9 + 0x240) < fVar10)) ||
       (fVar8 + fVar10 <= *(float *)(param_9 + 0x240))) goto LAB_14050cb62;
    uVar5 = uVar5 | 0x40;
  }
  else {
    if (((param_9 != 0) && (fVar11 <= *(float *)(param_9 + 0x23c))) &&
       ((*(float *)(param_9 + 0x23c) < fVar9 + fVar11 &&
        ((fVar10 <= *(float *)(param_9 + 0x240) && (*(float *)(param_9 + 0x240) < fVar8 + fVar10))))
       )) goto LAB_14050cb33;
    uVar5 = uVar5 | 8;
  }
  *param_1 = uVar5;
LAB_14050cb62:
  *param_4 = uVar4;
  if (*(code **)(param_8 + 0x108) != (code *)0x0) {
    (**(code **)(param_8 + 0x108))(param_2,*(undefined8 *)(param_8 + 0x100));
  }
  FUN_14050c5c0(param_2,*param_1,param_8,*param_4,&local_b8,&local_c8,&local_d8,param_5,param_6,
                param_10);
  if (*(code **)(param_8 + 0x110) != (code *)0x0) {
    (**(code **)(param_8 + 0x110))(param_2,*(undefined8 *)(param_8 + 0x100));
  }
  return (uint)param_7 != *param_4;
}


// Incoming references
// 0xc2a4e8 DATA caller none
// 0xb15dc0 DATA caller none
// 0xb15e28 DATA caller none
// 0xb15e38 DATA caller none
// 0x50ccff UNCONDITIONAL_CALL caller 14050cc40
