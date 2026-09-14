// Candidate VA 140323720; RVA 0x323720
// Ghidra inferred prototype: undefined FUN_140323720()

void FUN_140323720(longlong param_1,longlong *param_2)

{
  if (0xe4 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"linalg::aliases::double4");
    (**(code **)(*param_2 + 0x28))(param_2,param_1);
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 8);
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x10);
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x18);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  return;
}


// Incoming references
// 0xc18a94 DATA caller none
// 0x312bc1 UNCONDITIONAL_CALL caller 140312660
// 0x49b792 UNCONDITIONAL_CALL caller 14049b620
// 0x313783 UNCONDITIONAL_CALL caller 1403131f0
// 0x4b00b7 UNCONDITIONAL_CALL caller 1404afe30
// 0x4b0377 UNCONDITIONAL_CALL caller 1404b00c0
// 0x4b164c UNCONDITIONAL_CALL caller 1404b14d0
