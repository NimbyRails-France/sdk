// Candidate VA 140560870; RVA 0x560870
// Ghidra inferred prototype: undefined FUN_140560870()

void FUN_140560870(longlong param_1,char *param_2,undefined4 *param_3)

{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  int iVar4;
  undefined1 local_18 [16];

  FUN_14055d140(param_1,local_18);
  if (*(longlong *)(param_1 + 0x98) != *(longlong *)(param_1 + 0x90)) {
    FUN_14055b3c0(*(longlong *)(param_1 + 0x98) + -0x78,param_1 + 0x18);
  }
  FUN_14055a9b0(param_1 + 0x18,*(undefined8 *)(param_1 + 8));
  iVar4 = 0;
  pcVar3 = param_2;
  while ((pcVar3 != (char *)0x0 && (cVar1 = *pcVar3, pcVar3 = pcVar3 + 1, cVar1 != '\0'))) {
    iVar4 = iVar4 + 1;
  }
  if (((*(longlong *)(param_1 + 0x1d8) != 0) && (param_2 != (char *)0x0)) &&
     (param_3 != (undefined4 *)0x0)) {
    uVar2 = FUN_14050cc40(*(longlong *)(param_1 + 0x1d8),param_2,iVar4,*param_3);
    *param_3 = uVar2;
  }
  FUN_14055af70(param_1 + 0x18,*(undefined8 *)(param_1 + 0x1d8));
  FUN_14055ba40(param_1);
  return;
}


// Incoming references
// 0xc2d8d8 DATA caller none
// 0xa83560 DATA caller none
