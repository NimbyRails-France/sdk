// Candidate VA 14032e3a0; RVA 0x32e3a0
// Ghidra inferred prototype: undefined FUN_14032e3a0()

void FUN_14032e3a0(longlong *param_1,longlong *param_2)

{
  longlong *plVar1;
  longlong *plVar2;
  longlong *plVar3;
  longlong local_res8;

  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_JUScript@model@nimby@@@serde@@YAXPEBV?$map@_JUScript@model@nimby@@U?$less@_J@eastl@@Vallocator@5@@eastl@@PEAUSerializer@0@@Z"
            );
  local_res8 = param_1[4];
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  plVar3 = (longlong *)param_1[1];
  while (plVar3 != param_1) {
    (**(code **)(*param_2 + 0x18))(param_2,plVar3 + 4,8,0);
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Script");
    FUN_140318cc0(plVar3 + 5,param_2);
    (**(code **)(*param_2 + 0x10))();
    plVar1 = (longlong *)*plVar3;
    if (plVar1 == (longlong *)0x0) {
      plVar1 = (longlong *)plVar3[2];
      plVar2 = (longlong *)0x0;
      if (plVar3 == (longlong *)*plVar1) {
        do {
          plVar3 = plVar1;
          plVar1 = (longlong *)plVar3[2];
        } while (plVar3 == (longlong *)*plVar1);
        plVar2 = (longlong *)*plVar3;
      }
      if (plVar2 != plVar1) {
        plVar3 = plVar1;
      }
    }
    else {
      for (plVar2 = (longlong *)plVar1[1]; plVar3 = plVar1, plVar2 != (longlong *)0x0;
          plVar2 = (longlong *)plVar2[1]) {
        plVar1 = plVar2;
      }
    }
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc1991c DATA caller none
// 0xaf5d10 DATA caller none
// 0xaf5d20 DATA caller none
// 0x323e91 UNCONDITIONAL_CALL caller 140323bb0
// 0x323e9d UNCONDITIONAL_CALL caller 140323bb0
