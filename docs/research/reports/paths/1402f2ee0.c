// Candidate VA 1402f2ee0; RVA 0x2f2ee0
// Ghidra inferred prototype: undefined FUN_1402f2ee0()

void FUN_1402f2ee0(longlong param_1,longlong *param_2)

{
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::Pos");
  if (0x7a < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1,8,0);
    if (0x7a < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x28))(param_2,param_1 + 8);
      if (0x7a < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,1,0);
        if (0x7a < (int)param_2[1]) {
          (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x11,1,0);
        }
      }
    }
  }
                    /* WARNING: Could not recover jumptable at 0x0001402f2f76. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc16814 DATA caller none
// 0x2f2fac UNCONDITIONAL_CALL caller 1402f2f80
// 0x2f2fbe UNCONDITIONAL_CALL caller 1402f2f80
// 0x4b2231 UNCONDITIONAL_CALL caller 1404b1e10
// 0x49b653 UNCONDITIONAL_CALL caller 14049b620
// 0x49b482 UNCONDITIONAL_CALL caller 14049b420
// 0x4afe6a UNCONDITIONAL_CALL caller 1404afe30
// 0x4afe7c UNCONDITIONAL_CALL caller 1404afe30
// 0x4b00ff UNCONDITIONAL_CALL caller 1404b00c0
// 0x4b0111 UNCONDITIONAL_CALL caller 1404b00c0
// 0x4bdaef UNCONDITIONAL_CALL caller 1404bda00
// 0x4b2e1e UNCONDITIONAL_CALL caller 1404b2990
// 0x4b14ee UNCONDITIONAL_CALL caller 1404b14d0
// 0x3037d9 UNCONDITIONAL_CALL caller 140303760
