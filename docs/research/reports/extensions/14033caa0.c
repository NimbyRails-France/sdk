// Candidate VA 14033caa0; RVA 0x33caa0
// Ghidra inferred prototype: undefined FUN_14033caa0()

void FUN_14033caa0(longlong param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014033cac4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(IMAGE_DOS_HEADER_140000000.e_magic + *(uint *)(&DAT_14033cf34 + param_1 * 4)))();
  return;
}


// Incoming references
// 0xc1a438 DATA caller none
// 0x30f791 UNCONDITIONAL_CALL caller 14030f6a0
