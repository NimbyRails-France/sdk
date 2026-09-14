// Candidate VA 140427f20; RVA 0x427f20
// Ghidra inferred prototype: undefined FUN_140427f20()

undefined8 * FUN_140427f20(undefined8 *param_1,longlong param_2)

{
  undefined8 uVar1;
  ulonglong uVar2;
  undefined8 local_18;
  
  uVar1 = *(undefined8 *)(param_2 + 0x48);
  local_18 = *(undefined8 *)(param_2 + 0x50);
  if (*(int *)(param_2 + 0x30) == 4) {
    uVar2 = (ulonglong)local_18 >> 8;
    local_18 = CONCAT71((int7)uVar2,-(char)local_18);
  }
  *param_1 = *(undefined8 *)(param_2 + 0x40);
  param_1[1] = uVar1;
  param_1[2] = local_18;
  *(char *)(param_1 + 2) = -*(char *)(param_1 + 2);
  return param_1;
}


// Incoming references
// 0xc22964 DATA caller none
// 0x42ae68 DATA caller 140428790
