// Candidate VA 1404bc6e0; RVA 0x4bc6e0
// Ghidra inferred prototype: undefined FUN_1404bc6e0()

void FUN_1404bc6e0(longlong param_1,longlong *param_2)

{
  longlong lVar1;

  (**(code **)(*param_2 + 8))
            (param_2,"??$visit@E$0CA@@serde@@YAXPEBV?$array@E$0CA@@std@@PEAUSerializer@0@@Z");
  lVar1 = param_1 + 0x20;
  for (; param_1 != lVar1; param_1 = param_1 + 1) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1,1,1);
  }
                    /* WARNING: Could not recover jumptable at 0x0001404bc742. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc27290 DATA caller none
// 0x4b65e0 UNCONDITIONAL_CALL caller 1404b5ea0
