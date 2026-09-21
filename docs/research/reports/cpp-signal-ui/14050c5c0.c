// Candidate VA 14050c5c0; RVA 0x50c5c0
// Ghidra inferred prototype: undefined FUN_14050c5c0()

void FUN_14050c5c0(longlong param_1,ulonglong param_2,int *param_3,int param_4,float *param_5,
                  float *param_6,float *param_7,undefined8 param_8,undefined4 param_9,
                  undefined8 *param_10)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  float fVar5;
  float fVar6;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float fStack_54;
  float fStack_50;
  float fStack_4c;

  if ((param_2 & 0x10) == 0) {
    if ((param_2 & 0x20) == 0) {
      iVar3 = param_3[0x34];
      piVar4 = param_3 + 0x20;
      piVar2 = param_3;
    }
    else {
      iVar3 = param_3[0x36];
      piVar4 = param_3 + 0x2a;
      piVar2 = param_3 + 10;
    }
  }
  else {
    iVar3 = param_3[0x35];
    piVar4 = param_3 + 0x2a;
    piVar2 = param_3 + 10;
  }
  if (*piVar2 == 0) {
    local_58 = *param_6;
    fStack_54 = param_6[1];
    fStack_50 = param_6[2];
    fStack_4c = param_6[3];
    FUN_1404fc370(param_1,&local_58,0,param_3[0x1e]);
    local_64 = (float)param_3[0x3e];
    local_58 = *param_6;
    fStack_54 = param_6[1];
    fStack_50 = param_6[2];
    fStack_4c = param_6[3];
    fVar5 = local_64 + local_64;
    local_60 = fStack_50;
    if (fStack_50 < fVar5) {
      local_60 = fVar5;
    }
    local_5c = fStack_4c;
    if (fStack_4c < fVar5) {
      local_5c = fVar5;
    }
    local_5c = local_5c - fVar5;
    local_60 = local_60 - fVar5;
    local_68 = local_64 + local_58;
    local_64 = local_64 + fStack_54;
    FUN_1404fc370(param_1,&local_68,0,piVar2[2]);
  }
  else {
    local_58 = *param_6;
    fStack_54 = param_6[1];
    fStack_50 = param_6[2];
    fStack_4c = param_6[3];
    if (*piVar2 == 2) {
      FUN_1404fc970();
    }
    else {
      FUN_1404fc810(param_1,&local_58,piVar2 + 2,DAT_140a033b8);
    }
  }
  if (param_4 != 0) {
    if (*piVar4 == 1) {
      local_58 = *param_7;
      fStack_54 = param_7[1];
      fStack_50 = param_7[2];
      fStack_4c = param_7[3];
      FUN_1404fc810(param_1,&local_58,piVar4 + 2,DAT_140a033b8);
    }
    else {
      local_58 = *param_7;
      fStack_54 = param_7[1];
      fStack_50 = param_7[2];
      fStack_4c = param_7[3];
      if (*piVar4 == 2) {
        FUN_1404fc970(param_1,&local_58,piVar4 + 2,DAT_140a033b8);
      }
      else {
        FUN_1404fc370(param_1,&local_58,0,piVar4[2]);
      }
    }
  }
  iVar1 = param_3[0x37];
  local_58 = *param_5;
  fStack_54 = param_5[1];
  fStack_50 = param_5[2];
  fStack_4c = param_5[3];
  if (param_1 != 0) {
    fVar5 = fStack_4c;
    if (fStack_4c < 0.0) {
      fVar5 = 0.0;
    }
    (*(code *)param_10[2])(*param_10,*(undefined4 *)(param_10 + 1),param_8,param_9);
    local_60 = fStack_50;
    if (fStack_50 <= 0.0) {
      local_60 = 0.0;
    }
    fVar6 = fVar5 * DAT_140aab9dc;
    local_68 = local_58;
    local_64 = (fVar6 + fStack_54) - *(float *)(param_10 + 1) * DAT_140aab9dc;
    local_5c = fVar5 - (fVar6 + *(float *)(param_10 + 1) * DAT_140aab9dc);
    if (local_5c <= fVar6) {
      local_5c = fVar6;
    }
    FUN_1404fcad0(param_1,&local_68,param_8,param_9,param_10,iVar1,iVar3);
  }
  return;
}


// Incoming references
// 0xc2a4c4 DATA caller none
// 0xb15d74 DATA caller none
// 0xb15d84 DATA caller none
// 0x50cbe7 UNCONDITIONAL_CALL caller 14050c850
