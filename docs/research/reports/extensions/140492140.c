// Candidate VA 140492140; RVA 0x492140
// Ghidra inferred prototype: undefined FUN_140492140()

ulonglong * FUN_140492140(longlong param_1,ulonglong param_2)

{
  ulonglong *puVar1;
  ulonglong *puVar2;

  puVar1 = *(ulonglong **)
            (*(longlong *)(param_1 + 0x128) + (param_2 % (ulonglong)*(uint *)(param_1 + 0x130)) * 8)
  ;
  while( true ) {
    if (puVar1 == (ulonglong *)0x0) {
      return (ulonglong *)0x0;
    }
    if (param_2 == *puVar1) break;
    puVar1 = (ulonglong *)puVar1[4];
  }
  puVar2 = (ulonglong *)0x0;
  if (puVar1 != *(ulonglong **)(*(longlong *)(param_1 + 0x128) + *(longlong *)(param_1 + 0x130) * 8)
     ) {
    puVar2 = puVar1 + 1;
  }
  return puVar2;
}


// Incoming references
// 0x4278c6 UNCONDITIONAL_CALL caller 1404277a0
// 0x4926a6 UNCONDITIONAL_CALL caller 1404925d0
// 0x492589 UNCONDITIONAL_CALL caller 1404924f0
// 0x30da06 UNCONDITIONAL_CALL caller 14030d940
// 0x79d4a3 UNCONDITIONAL_CALL caller 14079cd60
