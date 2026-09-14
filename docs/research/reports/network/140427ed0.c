// Candidate VA 140427ed0; RVA 0x427ed0
// Ghidra inferred prototype: undefined FUN_140427ed0()

undefined8 * FUN_140427ed0(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  uVar2 = param_2[1];
  uVar1 = param_2[2];
  *param_1 = *param_2;
  param_1[1] = uVar2;
  param_1[2] = uVar1;
  *(char *)(param_1 + 2) = -*(char *)(param_1 + 2);
  return param_1;
}


// Incoming references
// 0x42f8e6 DATA caller 140428790
