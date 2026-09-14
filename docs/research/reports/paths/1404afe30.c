// Candidate VA 1404afe30; RVA 0x4afe30
// Ghidra inferred prototype: undefined FUN_1404afe30()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1404afe30(longlong param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong local_res8;
  undefined8 local_18;
  undefined8 uStack_10;
  
  if (0x7c < (int)param_2[1]) {
    FUN_1402f2f80();
  }
  if ((((0x7c < (int)param_2[1]) && (FUN_1402f2f80(param_1 + 0x30,param_2), 0x7c < (int)param_2[1]))
      && (FUN_1402f2ee0(param_1 + 0x60,param_2), 0x7c < (int)param_2[1])) &&
     ((FUN_1402f2ee0(param_1 + 0x78,param_2), 0x7c < (int)param_2[1] &&
      ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x90,1,1), 0x7c < (int)param_2[1])))) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x94);
  }
  if (0x81 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x98,1,1);
  }
  if (0xd1 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x99,1,1);
  }
  if (0x96 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xa0,8,1);
  }
  if (0x7c < (int)param_2[1]) {
    FUN_14031aad0(param_1 + 0xa8,param_2);
  }
  if (0x7e < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@USpeedChange@Path@model@nimby@@@serde@@YAXPEBV?$vector@USpeedChange@Path@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = (*(longlong *)(param_1 + 200) - *(longlong *)(param_1 + 0xc0) >> 2) *
                 -0x5555555555555555;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar1 = *(longlong *)(param_1 + 200);
    for (lVar2 = *(longlong *)(param_1 + 0xc0); lVar2 != lVar1; lVar2 = lVar2 + 0xc) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Path::SpeedChange");
      if (0x7e < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,lVar2);
      }
      if (0x83 < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,lVar2 + 4);
      }
      if (0x7e < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,lVar2 + 8);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xb0 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0xd8);
  }
  if (0xc4 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0xe0);
  }
  local_res8 = 0;
  if ((int)param_2[1] - 0x7dU < 0x38) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  }
  local_18 = _DAT_140aad610;
  uStack_10 = _UNK_140aad618;
  if ((int)param_2[1] - 0x7dU < 0x68) {
    FUN_1402f23e0(&local_18,param_2);
  }
  FUN_140323720(param_1 + 0xf0,param_2);
  return;
}


// Incoming references
// 0xc26b28 DATA caller none
// 0xb0f2b0 DATA caller none
// 0xb0f2c0 DATA caller none
// 0x4b1694 UNCONDITIONAL_CALL caller 1404b1660
