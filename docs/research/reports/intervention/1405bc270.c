// Candidate VA 1405bc270; RVA 0x5bc270
// Ghidra inferred prototype: undefined FUN_1405bc270()

longlong * FUN_1405bc270(longlong param_1,undefined8 param_2,undefined8 param_3)

{
  longlong lVar1;
  longlong *plVar2;
  int local_res8 [2];
  longlong local_28 [4];

  plVar2 = (longlong *)FUN_1405bfb90(param_1 + 0x228);
  lVar1 = *(longlong *)(param_1 + 0x250);
  local_res8[0] = (**(code **)(*plVar2 + 0x28))(plVar2);
  FUN_14074c5a0(lVar1 + 0x4e8,local_28,param_3,local_res8,(longlong)local_res8[0]);
  *(longlong *)(local_28[0] + 8) = *(longlong *)(local_28[0] + 8) + 1;
  plVar2[3] = *(longlong *)(local_28[0] + 8);
  return plVar2;
}


// Incoming references
// 0xc2f2e8 DATA caller none
// 0x5a7a76 UNCONDITIONAL_CALL caller 1405a6e60
// 0x668668 UNCONDITIONAL_CALL caller 1406678f0
// 0x668959 UNCONDITIONAL_CALL caller 1406678f0
// 0x6f75df UNCONDITIONAL_CALL caller 1406f6f50
// 0x7fe82c UNCONDITIONAL_CALL caller 1407fd860
// 0x7ff80a UNCONDITIONAL_CALL caller 1407ff5f0
// 0x6bd24f UNCONDITIONAL_CALL caller 1406bce90
