// Candidate VA 140327cb0; RVA 0x327cb0
// Ghidra inferred prototype: undefined FUN_140327cb0()

void FUN_140327cb0(longlong *param_1,longlong *param_2)

{
  longlong *plVar1;
  longlong *plVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  ulonglong local_res8;
  longlong local_res10;
  longlong local_res18 [2];
  
  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_J$01@serde@@YAXPEAV?$vector_set@_JU?$less@_J@eastl@@Vallocator@2@V?$fixed_vector@_J$01$00Vallocator@eastl@@@2@@eastl@@PEAUDeserializer@0@@Z"
            );
  local_res8 = param_1[1] - *param_1 >> 3;
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  uVar5 = 0;
  if (local_res8 != 0) {
    do {
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
      plVar1 = (longlong *)param_1[1];
      plVar2 = (longlong *)*param_1;
      local_res18[0] = local_res10;
      uVar4 = (longlong)plVar1 - (longlong)plVar2 >> 3;
      while (uVar3 = uVar4, 0 < (longlong)uVar3) {
        uVar4 = uVar3 >> 1;
        if (plVar2[uVar4] < local_res10) {
          plVar2 = plVar2 + uVar4 + 1;
          uVar4 = uVar3 + (-1 - uVar4);
        }
      }
      if ((plVar2 == plVar1) || (local_res10 < *plVar2)) {
        if ((plVar1 == (longlong *)param_1[2]) || (plVar2 != plVar1)) {
          FUN_14033d4e0(param_1,plVar2,local_res18);
        }
        else {
          *plVar1 = local_res10;
          param_1[1] = param_1[1] + 8;
        }
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < local_res8);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc18fb0 DATA caller none
// 0x310be4 UNCONDITIONAL_CALL caller 140310850
// 0x310c4d UNCONDITIONAL_CALL caller 140310850
// 0x310c62 UNCONDITIONAL_CALL caller 140310850
