// Candidate VA 1404c2a90; RVA 0x4c2a90
// Ghidra inferred prototype: undefined FUN_1404c2a90()

void FUN_1404c2a90(longlong *param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong local_res8;
  
  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@UMotion@model@nimby@@@serde@@YAXPEBV?$vector@UMotion@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
            );
  local_res8 = (param_1[1] - *param_1 >> 3) * 0x2697cc8aef46c0f7;
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  lVar1 = param_1[1];
  for (lVar2 = *param_1; lVar2 != lVar1; lVar2 = lVar2 + 0x638) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion");
    FUN_1404b1e10(lVar2,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc27710 DATA caller none
// 0x4b607d UNCONDITIONAL_CALL caller 1404b5ea0
// 0x4f62d6 UNCONDITIONAL_CALL caller 1404f6120
