// Candidate VA 14031aad0; RVA 0x31aad0
// Ghidra inferred prototype: undefined FUN_14031aad0()

void FUN_14031aad0(longlong *param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong local_res8;
  
  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_J@serde@@YAXPEBV?$vector@_JVallocator@eastl@@@eastl@@PEAUSerializer@0@@Z");
  local_res8 = param_1[1] - *param_1 >> 3;
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  lVar1 = param_1[1];
  for (lVar2 = *param_1; lVar2 != lVar1; lVar2 = lVar2 + 8) {
    (**(code **)(*param_2 + 0x18))(param_2,lVar2,8,0);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc18170 DATA caller none
// 0x4b6459 UNCONDITIONAL_CALL caller 1404b5ea0
// 0x310049 UNCONDITIONAL_CALL caller 14030fde0
// 0x4b587e UNCONDITIONAL_CALL caller 1404b5860
// 0x4b58b0 UNCONDITIONAL_CALL caller 1404b5860
// 0x4b58e2 UNCONDITIONAL_CALL caller 1404b5860
// 0x4b5917 UNCONDITIONAL_CALL caller 1404b5860
// 0x4b594f UNCONDITIONAL_CALL caller 1404b5860
// 0x4b5987 UNCONDITIONAL_CALL caller 1404b5860
// 0x4b59bf UNCONDITIONAL_CALL caller 1404b5860
// 0x4b5a96 UNCONDITIONAL_CALL caller 1404b5860
// 0x314fdd UNCONDITIONAL_CALL caller 140314af0
// 0x31500d UNCONDITIONAL_CALL caller 140314af0
// 0x4aff2c UNCONDITIONAL_CALL caller 1404afe30
// 0x3dc1c6 UNCONDITIONAL_CALL caller 1403dc160
// 0x3dc1e5 UNCONDITIONAL_CALL caller 1403dc160
// 0x3dc218 UNCONDITIONAL_CALL caller 1403dc160
// 0x3dc266 UNCONDITIONAL_CALL caller 1403dc160
// 0x3dc2a0 UNCONDITIONAL_CALL caller 1403dc160
// 0x3dc2d9 UNCONDITIONAL_CALL caller 1403dc160
// 0x2f63db UNCONDITIONAL_CALL caller 1402f6130
// 0x2f63eb UNCONDITIONAL_CALL caller 1402f6130
// 0x304971 UNCONDITIONAL_CALL caller 1403048f0
