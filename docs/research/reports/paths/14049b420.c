// Candidate VA 14049b420; RVA 0x49b420
// Ghidra inferred prototype: undefined FUN_14049b420()

void FUN_14049b420(longlong param_1,longlong *param_2)

{
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::Path::Ride");
  if (0x7c < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Path::Trace");
    if (0x7c < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1,4,0);
      if (0x7c < (int)param_2[1]) {
        FUN_1402f2ee0(param_1 + 8,param_2);
        if (0x7c < (int)param_2[1]) {
          (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x20);
        }
      }
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0x7c < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x28);
      if (0x7c < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x30);
        if (0x7c < (int)param_2[1]) {
          (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x38);
        }
      }
    }
  }
  if (0x80 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x40,4,0);
  }
                    /* WARNING: Could not recover jumptable at 0x00014049b50b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc2669c DATA caller none
// 0x4b16b5 UNCONDITIONAL_CALL caller 1404b1660
// 0x4b1483 UNCONDITIONAL_CALL caller 1404b0c50
// 0x4b1990 UNCONDITIONAL_CALL caller 1404b1940
