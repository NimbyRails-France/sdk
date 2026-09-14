// Candidate VA 1404bfcd0; RVA 0x4bfcd0
// Ghidra inferred prototype: undefined FUN_1404bfcd0()

void FUN_1404bfcd0(longlong param_1,longlong *param_2)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  longlong *plVar4;
  undefined8 local_res8;
  
  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_KUSignalTextures@Rules@model@nimby@@@serde@@YAXPEBV?$hash_map@_KUSignalTextures@Rules@model@nimby@@U?$hash@_K@eastl@@U?$equal_to@_K@6@Vallocator@6@$0A@@eastl@@PEAUSerializer@0@@Z"
            );
  local_res8 = *(undefined8 *)(param_1 + 0x18);
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  plVar1 = *(longlong **)(param_1 + 8);
  lVar3 = *plVar1;
  plVar4 = plVar1;
  if (lVar3 == 0) {
    lVar3 = plVar1[1];
    while (plVar4 = plVar4 + 1, lVar3 == 0) {
      lVar3 = plVar4[1];
    }
  }
  lVar2 = plVar1[*(longlong *)(param_1 + 0x10)];
  while (lVar3 != lVar2) {
    (**(code **)(*param_2 + 0x18))(param_2,lVar3,8,1);
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Rules::SignalTextures");
    if (((0xd2 < (int)param_2[1]) && (FUN_1402f21e0(lVar3 + 8,param_2), 0xd2 < (int)param_2[1])) &&
       ((**(code **)(*param_2 + 0x18))(param_2,lVar3 + 0x28,1,1), 0xd2 < (int)param_2[1])) {
      (**(code **)(*param_2 + 0x18))(param_2,lVar3 + 0x30,8,1);
    }
    if (0xde < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,lVar3 + 0x38,4,0);
    }
    if (((0xd2 < (int)param_2[1]) && (FUN_1402f21e0(lVar3 + 0x40,param_2), 0xd2 < (int)param_2[1]))
       && (FUN_1402f21e0(lVar3 + 0x60,param_2), 0xd2 < (int)param_2[1])) {
      FUN_1404beb80(lVar3 + 0x80,param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    lVar3 = *(longlong *)(lVar3 + 0x98);
    while (lVar3 == 0) {
      plVar1 = plVar4 + 1;
      plVar4 = plVar4 + 1;
      lVar3 = *plVar1;
    }
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc275d8 DATA caller none
// 0x4b515c UNCONDITIONAL_CALL caller 1404b4d40
