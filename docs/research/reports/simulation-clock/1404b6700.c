// Candidate VA 1404b6700; RVA 0x4b6700
// Ghidra inferred prototype: undefined FUN_1404b6700()

void FUN_1404b6700(longlong param_1,longlong *param_2)

{
  undefined8 *puVar1;
  longlong *plVar2;
  void *_Memory;
  longlong lVar3;
  undefined8 *puVar4;
  undefined8 *puVar5;
  longlong lVar6;
  ulonglong uVar7;
  undefined8 *puVar8;
  longlong lVar9;
  undefined8 *puVar10;
  ulonglong local_res8;
  longlong local_res10;
  undefined8 local_res18;
  undefined8 *local_58;
  undefined8 *puStack_50;
  undefined8 local_48;

  local_res8 = local_res8 & 0xffffffff00000000;
  if ((int)param_2[1] - 1U < 0xc5) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::sync::saves::legacy::ModelSave");
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_140323440(param_1,param_2);
    }
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_140323520(param_1 + 0x18,param_2);
    }
    if ((int)param_2[1] - 0x3eU < 0x88) {
      FUN_14031ae30(param_1 + 0x30,param_2);
    }
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_1404c2460(param_1 + 0x48,param_2);
    }
    local_58 = (undefined8 *)0x0;
    puStack_50 = (undefined8 *)0x0;
    local_48 = 0;
    local_res8 = CONCAT44(local_res8._4_4_,1);
    if ((int)param_2[1] - 0xeU < 0x3a) {
      FUN_1404c2540(&local_58,param_2);
    }
    puVar10 = local_58;
    FUN_1404c3af0(local_58,puStack_50);
    if (puVar10 != (undefined8 *)0x0) {
      free(puVar10);
    }
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_1404c2730(param_1 + 0x60,param_2);
    }
    if ((int)param_2[1] - 0xacU < 0x1a) {
      FUN_1404c2810(param_1 + 0x78,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xc5 < (int)param_2[1]) {
    FUN_14049b7b0(param_1 + 0x90,param_2);
  }
  FUN_1404b8d60(&local_58,param_2);
  FUN_1404c3be0(local_58,puStack_50);
  if (local_58 != (undefined8 *)0x0) {
    free(local_58);
  }
  if (0xbc < (int)param_2[1]) {
    FUN_1404c2dc0(param_1 + 0x210,param_2);
  }
  local_58 = (undefined8 *)0x0;
  puStack_50 = (undefined8 *)0x0;
  local_48 = 0;
  local_res8 = CONCAT44(local_res8._4_4_,2);
  if ((int)param_2[1] - 0xbcU < 2) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UShiftState@model@nimby@@@serde@@YAXPEAV?$vector@UShiftState@model@nimby@@Vallocator@eastl@@@eastl@@PEAUDeserializer@0@@Z"
              );
    local_res10 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
    puVar4 = local_58;
    puVar8 = local_58;
    puVar1 = puStack_50;
    if (local_res10 != 0) {
      FUN_1404c6ef0(&local_58);
      puVar4 = local_58;
      puVar8 = local_58;
      puVar1 = puStack_50;
    }
    for (; puVar5 = puStack_50, puVar10 = local_58, local_58 = puVar8, puVar4 != puStack_50;
        puVar4 = puVar4 + 1) {
      puStack_50 = puVar1;
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::ShiftState");
      local_res18 = 0;
      if ((int)param_2[1] - 0xbcU < 2) {
        (**(code **)(*param_2 + 0x18))(param_2,&local_res18,8,0);
      }
      local_res18 = 0;
      if ((int)param_2[1] - 0xbcU < 2) {
        (**(code **)(*param_2 + 0x18))(param_2,&local_res18,8);
      }
      if (0xbb < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,puVar4,8);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
      puVar8 = local_58;
      puVar1 = puStack_50;
      puStack_50 = puVar5;
      local_58 = puVar10;
    }
    puStack_50 = puVar1;
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  else {
    puVar10 = (undefined8 *)0x0;
  }
  if (puVar10 != (undefined8 *)0x0) {
    free(puVar10);
  }
  if (3 < (int)param_2[1]) {
    FUN_1404c2ef0(param_1 + 0x228,param_2);
  }
  local_58 = (undefined8 *)0x0;
  puStack_50 = (undefined8 *)0x0;
  local_48 = 0;
  local_res8 = CONCAT44(local_res8._4_4_,4);
  if ((int)param_2[1] - 4U < 99) {
    FUN_1404c2fd0(&local_58,param_2);
  }
  puVar4 = puStack_50;
  puVar10 = local_58;
  if (local_58 != puStack_50) {
    puVar8 = local_58 + 6;
    do {
      _Memory = (void *)puVar8[-4];
      if ((_Memory != (void *)0x0) && (_Memory != (void *)*puVar8)) {
        free(_Memory);
      }
      puVar1 = puVar8 + 9;
      puVar8 = puVar8 + 0xf;
    } while (puVar1 != puVar4);
  }
  if (puVar10 != (undefined8 *)0x0) {
    free(puVar10);
  }
  if (3 < (int)param_2[1]) {
    FUN_1404c3130(param_1 + 0x240,param_2);
  }
  if (0x9d < (int)param_2[1]) {
    FUN_1404c3240(param_1 + 600,param_2);
  }
  plVar2 = (longlong *)(param_1 + 0x270);
  if ((int)param_2[1] - 10U < 0x8c) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UAccounting@legacy@saves@sync@nimby@@@serde@@YAXPEAV?$vector@UAccounting@legacy@saves@sync@nimby@@Vallocator@eastl@@@eastl@@PEAUDeserializer@0@@Z"
              );
    local_res8 = (*(longlong *)(param_1 + 0x278) - *plVar2 >> 3) * 0x6db6db6db6db6db7;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar3 = *(longlong *)(param_1 + 0x278);
    lVar6 = lVar3 - *plVar2 >> 3;
    uVar7 = lVar6 * 0x6db6db6db6db6db7;
    if (uVar7 - local_res8 != 0) {
      if (uVar7 < local_res8) {
        FUN_1404cb0b0(plVar2,local_res8 + lVar6 * -0x6db6db6db6db6db7);
      }
      else {
        lVar9 = local_res8 * 0x38;
        for (lVar6 = *plVar2 + lVar9; lVar6 != lVar3; lVar6 = lVar6 + 0x38) {
          FUN_140482560(lVar6 + 8);
        }
        *(longlong *)(param_1 + 0x278) = *plVar2 + lVar9;
      }
    }
    lVar3 = *(longlong *)(param_1 + 0x278);
    for (lVar6 = *plVar2; lVar6 != lVar3; lVar6 = lVar6 + 0x38) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::sync::saves::legacy::Accounting");
      if ((9 < (int)param_2[1]) &&
         ((**(code **)(*param_2 + 0x18))(param_2,lVar6,8,0), 9 < (int)param_2[1])) {
        FUN_1404bb010(lVar6 + 8,param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x95 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::AccountingArchive");
    if (0x95 < (int)param_2[1]) {
      FUN_1404bb330(param_1 + 0x288,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xd4 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::SimScriptState");
    FUN_1404b5c70(param_1 + 0x2b8,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x31 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::TagRegistry");
    if ((0x31 < (int)param_2[1]) &&
       ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x2e8,8,0), 0x31 < (int)param_2[1])) {
      FUN_1404ba940(param_1 + 0x2f0,param_2);
    }
    FUN_1404b8200(param_1 + 800,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::sync::saves::ShellState");
    if (0 < (int)param_2[1]) {
      FUN_1402f23e0(param_1 + 0x350,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if ((int)param_2[1] - 1U < 0xc5) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::sync::saves::legacy::IDGen");
    local_res8 = 0;
    if ((int)param_2[1] - 1U < 0xb4) {
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
    }
    if (0 < (int)param_2[1]) {
      FUN_14031b010(param_1 + 0x390,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xc5 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDSeq");
    if (0xc5 < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x3a8,8,0);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Clock");
    if ((0 < (int)param_2[1]) &&
       ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x3b0,8,0), 0 < (int)param_2[1])) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x3b8,8,0);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0 < (int)param_2[1]) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Company");
      FUN_1404aed20(param_1 + 0x3c0,param_2);
      (**(code **)(*param_2 + 0x10))(param_2);
    }
  }
  if (0x67 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Bank");
    if ((0x67 < (int)param_2[1]) &&
       ((**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x4f0), 0x67 < (int)param_2[1])) {
      FUN_1404bb8a0(param_1 + 0x4f8,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (4 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Rules");
    FUN_1404b51d0(param_1 + 0x510,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x6f < (int)param_2[1]) {
    FUN_14049b1d0(param_1 + 0xb58,param_2);
  }
  if (0x2e < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::UISettings");
    FUN_1404b3a00(param_1 + 0xb90,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  return;
}


// Incoming references
// 0xc26d50 DATA caller none
// 0x49e11f UNCONDITIONAL_CALL caller 14049d9a0
// 0x4ed036 UNCONDITIONAL_CALL caller 1404ec9e0
