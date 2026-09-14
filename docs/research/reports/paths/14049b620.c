// Candidate VA 14049b620; RVA 0x49b620
// Ghidra inferred prototype: undefined FUN_14049b620()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14049b620(longlong param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong local_res8;
  undefined8 local_18;
  undefined8 uStack_10;
  
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Presence");
  if ((((0xbc < (int)param_2[1]) && (FUN_1402f2ee0(param_1,param_2), 0xbc < (int)param_2[1])) &&
      ((**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x18), 0xbc < (int)param_2[1])) &&
     ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x20,1,1), 0xbc < (int)param_2[1])) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@U?$pair@_JH@eastl@@$03@serde@@YAXPEBV?$fixed_vector@U?$pair@_JH@eastl@@$03$00Vallocator@2@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = *(longlong *)(param_1 + 0x30) - *(longlong *)(param_1 + 0x28) >> 4;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar1 = *(longlong *)(param_1 + 0x30);
    for (lVar2 = *(longlong *)(param_1 + 0x28); lVar2 != lVar1; lVar2 = lVar2 + 0x10) {
      (**(code **)(*param_2 + 8))
                (param_2,"??$visit@_JH@serde@@YAXPEBU?$pair@_JH@eastl@@PEAUSerializer@0@@Z");
      (**(code **)(*param_2 + 0x18))(param_2,lVar2,8,0);
      (**(code **)(*param_2 + 0x18))(param_2,lVar2 + 8,4);
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  local_18 = _DAT_140aad610;
  uStack_10 = _UNK_140aad618;
  if ((int)param_2[1] - 0xbdU < 0x28) {
    FUN_1402f23e0(&local_18,param_2);
  }
  FUN_140323720(param_1 + 0x90,param_2);
                    /* WARNING: Could not recover jumptable at 0x00014049b7a7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc266b4 DATA caller none
// 0xb0de38 DATA caller none
// 0xb0de48 DATA caller none
// 0x4b20d4 UNCONDITIONAL_CALL caller 1404b1e10
