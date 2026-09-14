// Candidate VA 140427ef0; RVA 0x427ef0
// Ghidra inferred prototype: undefined FUN_140427ef0()

undefined8 * FUN_140427ef0(undefined8 *param_1,longlong param_2)

{
  undefined8 uVar1;
  int iVar2;
  undefined8 uVar3;
  
  iVar2 = *(int *)(param_2 + 0x30);
  uVar3 = *(undefined8 *)(param_2 + 0x48);
  uVar1 = *(undefined8 *)(param_2 + 0x50);
  *param_1 = *(undefined8 *)(param_2 + 0x40);
  param_1[1] = uVar3;
  param_1[2] = uVar1;
  if (iVar2 == 4) {
    *(char *)(param_1 + 2) = -*(char *)(param_1 + 2);
  }
  return param_1;
}


// Incoming references
// 0x42ad22 DATA caller 140428790
