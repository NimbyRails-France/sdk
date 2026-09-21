// Candidate VA 1402f29b0; RVA 0x2f29b0
// Ghidra inferred prototype: undefined FUN_1402f29b0()

void FUN_1402f29b0(longlong *param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong local_res8;

  (**(code **)(*param_2 + 8))(param_2,"nimby::model::ScriptInstances");
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UScriptStructInstance@model@nimby@@@serde@@YAXPEBV?$vector@UScriptStructInstance@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = (param_1[1] - *param_1 >> 4) * -0x3333333333333333;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar1 = param_1[1];
    for (lVar2 = *param_1; lVar2 != lVar1; lVar2 = lVar2 + 0x50) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::ScriptStructInstance");
      FUN_14030f6a0(lVar2,param_2);
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
                    /* WARNING: Could not recover jumptable at 0x0001402f2a8b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc16784 DATA caller none
// 0xaf29e0 DATA caller none
// 0xaf29f0 DATA caller none
// 0x310061 UNCONDITIONAL_CALL caller 14030fde0
// 0x311066 UNCONDITIONAL_CALL caller 140310e50
// 0x318e51 UNCONDITIONAL_CALL caller 140318cc0
// 0x4ba8e3 UNCONDITIONAL_CALL caller 1404ba780
// 0x3131c6 UNCONDITIONAL_CALL caller 140312660
// 0x31208d UNCONDITIONAL_CALL caller 140311c10
// 0x316125 UNCONDITIONAL_CALL caller 140315bd0
// 0x3150dc UNCONDITIONAL_CALL caller 140314af0
// 0x2f27ed UNCONDITIONAL_CALL caller 1402f2720
// 0x3ccdf8 UNCONDITIONAL_CALL caller 1403cc490
// 0x3cce13 UNCONDITIONAL_CALL caller 1403cc490
// 0x3cd7e1 UNCONDITIONAL_CALL caller 1403cd350
// 0x3cd800 UNCONDITIONAL_CALL caller 1403cd350
// 0x3d9be6 UNCONDITIONAL_CALL caller 1403d9b80
// 0x3d9c05 UNCONDITIONAL_CALL caller 1403d9b80
// 0x3d9c38 UNCONDITIONAL_CALL caller 1403d9b80
// 0x3d9c86 UNCONDITIONAL_CALL caller 1403d9b80
// 0x3d9cc0 UNCONDITIONAL_CALL caller 1403d9b80
// 0x3d9cf9 UNCONDITIONAL_CALL caller 1403d9b80
// 0x3dc566 UNCONDITIONAL_CALL caller 1403dc500
// 0x3dc585 UNCONDITIONAL_CALL caller 1403dc500
// 0x3dc5b8 UNCONDITIONAL_CALL caller 1403dc500
// 0x3dc606 UNCONDITIONAL_CALL caller 1403dc500
// 0x3dc640 UNCONDITIONAL_CALL caller 1403dc500
// 0x3dc679 UNCONDITIONAL_CALL caller 1403dc500
// 0x3147d3 UNCONDITIONAL_CALL caller 1403144f0
