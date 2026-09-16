// Candidate VA 140355740; RVA 0x355740
// Ghidra inferred prototype: undefined FUN_140355740()

int FUN_140355740(longlong param_1,int param_2)

{
  short sVar1;
  int *piVar2;
  int *piVar3;
  int local_res8 [4];
  int local_res18 [2];
  int local_res20 [2];

  sVar1 = *(short *)(param_1 + 0x1c);
  if (sVar1 == -1) {
    return 0;
  }
  local_res18[0] = (int)sVar1;
  local_res20[0] = param_2 + -1;
  local_res8[0] = 0;
  piVar3 = local_res8;
  if (-1 < sVar1) {
    piVar3 = local_res18;
  }
  piVar2 = local_res20;
  if ((int)sVar1 <= param_2 + -1) {
    piVar2 = piVar3;
  }
  return *piVar2;
}


// Incoming references
// 0x43f0ff UNCONDITIONAL_CALL caller 14043ef00
// 0x35a82a UNCONDITIONAL_CALL caller 14035a1f0
// 0x35a979 UNCONDITIONAL_CALL caller 14035a1f0
// 0x35aa78 UNCONDITIONAL_CALL caller 14035a1f0
// 0x35ad15 UNCONDITIONAL_CALL caller 14035a1f0
// 0x35adf2 UNCONDITIONAL_CALL caller 14035a1f0
// 0x590d4a UNCONDITIONAL_CALL caller 1405907a0
// 0x592cea UNCONDITIONAL_CALL caller 140592c10
