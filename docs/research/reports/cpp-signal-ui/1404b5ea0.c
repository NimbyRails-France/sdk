// Candidate VA 1404b5ea0; RVA 0x4b5ea0
// Ghidra inferred prototype: undefined FUN_1404b5ea0()

void FUN_1404b5ea0(longlong param_1,longlong *param_2)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong *plVar5;
  longlong lVar6;
  ulonglong local_res8;
  undefined8 local_res10;

  local_res8 = local_res8 & 0xffffffff00000000;
  if ((int)param_2[1] - 1U < 0xc5) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::sync::saves::legacy::ModelSave");
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_1403232c0(param_1,param_2);
    }
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_140323380(param_1 + 0x18,param_2);
    }
    if ((int)param_2[1] - 0x3eU < 0x88) {
      FUN_14031a940(param_1 + 0x30,param_2);
    }
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_1404c2220(param_1 + 0x48,param_2);
    }
    local_res8 = CONCAT44(local_res8._4_4_,1);
    if ((int)param_2[1] - 0xeU < 0x3a) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@ULineState14@serde@@@serde@@YAXPEBV?$vector@ULineState14@serde@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
                );
      local_res10 = 0;
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    FUN_1404c3af0(0,0);
    if ((int)param_2[1] - 1U < 0xc5) {
      FUN_1404c22e0(param_1 + 0x60,param_2);
    }
    if ((int)param_2[1] - 0xacU < 0x1a) {
      FUN_1404c23a0(param_1 + 0x78,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xc5 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::ModelSave");
    FUN_1404b5860(param_1 + 0x90,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  local_res8 = CONCAT44(local_res8._4_4_,2);
  if ((int)param_2[1] - 1U < 0xbc) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@ULegacyMotion115@serde@@@serde@@YAXPEBV?$vector@ULegacyMotion115@serde@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res10 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  FUN_1404c3be0(0,0);
  if (0xbc < (int)param_2[1]) {
    FUN_1404c2a90(param_1 + 0x210,param_2);
  }
  local_res8 = CONCAT44(local_res8._4_4_,4);
  if ((int)param_2[1] - 0xbcU < 2) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UShiftState@model@nimby@@@serde@@YAXPEBV?$vector@UShiftState@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res10 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (3 < (int)param_2[1]) {
    FUN_1404c2b50(param_1 + 0x228,param_2);
  }
  local_res8 = CONCAT44(local_res8._4_4_,8);
  if ((int)param_2[1] - 4U < 99) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UStationStateDynTx15@serde@@@serde@@YAXPEBV?$vector@UStationStateDynTx15@serde@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res10 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (3 < (int)param_2[1]) {
    FUN_1404c2c10(param_1 + 0x240,param_2);
  }
  if (0x9d < (int)param_2[1]) {
    FUN_1404c2ce0(param_1 + 600,param_2);
  }
  if ((int)param_2[1] - 10U < 0x8c) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UAccounting@legacy@saves@sync@nimby@@@serde@@YAXPEBV?$vector@UAccounting@legacy@saves@sync@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = (*(longlong *)(param_1 + 0x278) - *(longlong *)(param_1 + 0x270) >> 3) *
                 0x6db6db6db6db6db7;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar2 = *(longlong *)(param_1 + 0x278);
    for (lVar4 = *(longlong *)(param_1 + 0x270); lVar4 != lVar2; lVar4 = lVar4 + 0x38) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::sync::saves::legacy::Accounting");
      if ((9 < (int)param_2[1]) &&
         ((**(code **)(*param_2 + 0x18))(param_2,lVar4,8,0), 9 < (int)param_2[1])) {
        FUN_1404bae30(lVar4 + 8,param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x95 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::AccountingArchive");
    if (0x95 < (int)param_2[1]) {
      FUN_1404bb1d0(param_1 + 0x288,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xd4 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::SimScriptState");
    FUN_1404b5ac0(param_1 + 0x2b8,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x31 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::TagRegistry");
    if (((0x31 < (int)param_2[1]) &&
        ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x2e8,8,0), 0x31 < (int)param_2[1])) &&
       (FUN_1404ba780(param_1 + 0x2f0,param_2), 0x31 < (int)param_2[1])) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@_J_J@serde@@YAXPEBV?$hash_multimap@_J_JU?$hash@_J@eastl@@U?$equal_to@_J@2@Vallocator@2@$0A@@eastl@@PEAUSerializer@0@@Z"
                );
      local_res8 = *(ulonglong *)(param_1 + 0x338);
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
      plVar1 = *(longlong **)(param_1 + 0x328);
      lVar2 = *plVar1;
      plVar5 = plVar1;
      if (lVar2 == 0) {
        plVar5 = plVar1 + 1;
        lVar2 = *plVar5;
        while (lVar2 == 0) {
          plVar5 = plVar5 + 1;
          lVar2 = *plVar5;
        }
      }
      lVar4 = plVar1[*(longlong *)(param_1 + 0x330)];
      while (lVar2 != lVar4) {
        (**(code **)(*param_2 + 0x18))(param_2,lVar2,8,0);
        (**(code **)(*param_2 + 0x18))(param_2,lVar2 + 8,8);
        lVar2 = *(longlong *)(lVar2 + 0x10);
        while (lVar2 == 0) {
          plVar5 = plVar5 + 1;
          lVar2 = *plVar5;
        }
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
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
      FUN_14031aad0(param_1 + 0x390,param_2);
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
      FUN_1404ae8a0(param_1 + 0x3c0,param_2);
      (**(code **)(*param_2 + 0x10))(param_2);
    }
  }
  if (0x67 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Bank");
    if ((0x67 < (int)param_2[1]) &&
       ((**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x4f0), 0x67 < (int)param_2[1])) {
      FUN_1404bb720(param_1 + 0x4f8,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (4 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Rules");
    FUN_1404b4d40(param_1 + 0x510,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x6f < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Versioning");
    if ((0x6f < (int)param_2[1]) && (FUN_1404bc6e0(param_1 + 0xb58,param_2), 0x6f < (int)param_2[1])
       ) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@V?$array@E$0CA@@std@@@serde@@YAXPEBV?$vector@V?$array@E$0CA@@std@@V?$allocator@V?$array@E$0CA@@std@@@2@@std@@PEAUSerializer@0@@Z"
                );
      local_res8 = *(longlong *)(param_1 + 0xb80) - *(longlong *)(param_1 + 0xb78) >> 5;
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
      lVar2 = *(longlong *)(param_1 + 0xb80);
      lVar4 = *(longlong *)(param_1 + 0xb78);
      lVar3 = lVar4;
      for (; lVar4 != lVar2; lVar4 = lVar4 + 0x20) {
        lVar3 = lVar3 + 0x20;
        (**(code **)(*param_2 + 8))
                  (param_2,"??$visit@E$0CA@@serde@@YAXPEBV?$array@E$0CA@@std@@PEAUSerializer@0@@Z");
        for (lVar6 = lVar4; lVar6 != lVar3; lVar6 = lVar6 + 1) {
          (**(code **)(*param_2 + 0x18))(param_2,lVar6,1,1);
        }
        (**(code **)(*param_2 + 0x10))(param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0x2e < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::UISettings");
    FUN_1404b3760(param_1 + 0xb90,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  return;
}


// Incoming references
// 0xc26d44 DATA caller none
// 0x49f0f9 UNCONDITIONAL_CALL caller 14049f050
// 0x4f4830 UNCONDITIONAL_CALL caller 1404f3520
