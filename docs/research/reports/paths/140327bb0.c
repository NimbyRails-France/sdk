// Candidate VA 140327bb0; RVA 0x327bb0
// Ghidra inferred prototype: undefined FUN_140327bb0()

void FUN_140327bb0(longlong *param_1,longlong *param_2)

{
  longlong lVar1;
  ulonglong uVar2;
  longlong lVar3;
  ulonglong local_res8;
  
  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@UConflict@model@nimby@@@serde@@YAXPEAV?$vector@UConflict@model@nimby@@Vallocator@eastl@@@eastl@@PEAUDeserializer@0@@Z"
            );
  local_res8 = (param_1[1] - *param_1 >> 4) * -0x5555555555555555;
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  lVar1 = param_1[1] - *param_1 >> 4;
  uVar2 = lVar1 * -0x5555555555555555;
  if (uVar2 - local_res8 != 0) {
    if (uVar2 < local_res8) {
      FUN_140337730(param_1,local_res8 + lVar1 * 0x5555555555555555);
    }
    else {
      param_1[1] = local_res8 * 0x30 + *param_1;
    }
  }
  lVar1 = param_1[1];
  for (lVar3 = *param_1; lVar3 != lVar1; lVar3 = lVar3 + 0x30) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Conflict");
    FUN_14030fc40(lVar3,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc18fa4 DATA caller none
// 0x310b71 UNCONDITIONAL_CALL caller 140310850
// 0x310b86 UNCONDITIONAL_CALL caller 140310850
// 0x310b9b UNCONDITIONAL_CALL caller 140310850
// 0x310bb0 UNCONDITIONAL_CALL caller 140310850
