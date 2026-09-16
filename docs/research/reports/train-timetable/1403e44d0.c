// Candidate VA 1403e44d0; RVA 0x3e44d0
// Ghidra inferred prototype: undefined FUN_1403e44d0()

ulonglong * FUN_1403e44d0(longlong param_1,ulonglong param_2)

{
  ulonglong *puVar1;
  ulonglong *puVar2;

  puVar1 = *(ulonglong **)
            (*(longlong *)(param_1 + 0x1b8) + (param_2 % (ulonglong)*(uint *)(param_1 + 0x1c0)) * 8)
  ;
  while( true ) {
    if (puVar1 == (ulonglong *)0x0) {
      return (ulonglong *)0x0;
    }
    if (param_2 == *puVar1) break;
    puVar1 = (ulonglong *)puVar1[0x3b];
  }
  puVar2 = (ulonglong *)0x0;
  if (puVar1 != *(ulonglong **)(*(longlong *)(param_1 + 0x1b8) + *(longlong *)(param_1 + 0x1c0) * 8)
     ) {
    puVar2 = puVar1 + 1;
  }
  return puVar2;
}


// Incoming references
// 0x43f19a UNCONDITIONAL_CALL caller 14043f150
// 0x3e8c4c UNCONDITIONAL_CALL caller 1403e8be0
// 0x3e8c6a UNCONDITIONAL_CALL caller 1403e8be0
// 0x3ed05b UNCONDITIONAL_CALL caller 1403ecff0
// 0x3e9155 UNCONDITIONAL_CALL caller 1403e9110
// 0x305fe0 UNCONDITIONAL_CALL caller 140305f50
// 0x3087c7 UNCONDITIONAL_CALL caller 140308710
// 0x3ed293 UNCONDITIONAL_CALL caller 1403ed1a0
// 0x4921c6 UNCONDITIONAL_CALL caller 140492190
// 0x443b93 UNCONDITIONAL_CALL caller 140443ac0
// 0x457131 UNCONDITIONAL_CALL caller 1404570c0
// 0x4587c0 UNCONDITIONAL_CALL caller 140458750
// 0x45db78 UNCONDITIONAL_CALL caller 14045db00
// 0x45d8f8 UNCONDITIONAL_CALL caller 14045d880
// 0x4573dc UNCONDITIONAL_CALL caller 140457340
// 0x4a4ee9 UNCONDITIONAL_CALL caller 1404a48e0
// 0x598ef6 UNCONDITIONAL_CALL caller 140598a30
// 0x599677 UNCONDITIONAL_CALL caller 140599130
// 0x59f599 UNCONDITIONAL_CALL caller 14059efc0
// 0x64d4f7 UNCONDITIONAL_CALL caller 14064d450
// 0x6f8aff UNCONDITIONAL_CALL caller 1406f8770
// 0x70a085 UNCONDITIONAL_CALL caller 140709c20
// 0x80410c UNCONDITIONAL_CALL caller 140804000
// 0x7f25b1 UNCONDITIONAL_CALL caller 1407f19a0
// 0x6b7ea5 UNCONDITIONAL_CALL caller 1406b7c00
// 0x6b9a81 UNCONDITIONAL_CALL caller 1406b8ca0
// 0x6bb4cc UNCONDITIONAL_CALL caller 1406badf0
// 0x49231d UNCONDITIONAL_CALL caller 140492220
// 0x42422b UNCONDITIONAL_CALL caller 140424090
