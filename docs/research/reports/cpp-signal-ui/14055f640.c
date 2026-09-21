// Candidate VA 14055f640; RVA 0x55f640
// Ghidra inferred prototype: undefined FUN_14055f640()

void FUN_14055f640(longlong param_1,undefined8 param_2,char *param_3)

{
  char cVar1;
  longlong lVar2;
  char *pcVar3;
  int iVar4;
  undefined1 local_18 [16];

  FUN_14055a8b0(param_1 + 0x18,&DAT_140b8e3e0);
  FUN_14055d140(param_1,local_18);
  if (*(longlong *)(param_1 + 0x98) != *(longlong *)(param_1 + 0x90)) {
    FUN_14055b3c0(*(longlong *)(param_1 + 0x98) + -0x78,param_1 + 0x18);
  }
  FUN_14055a9b0(param_1 + 0x18,*(undefined8 *)(param_1 + 8));
  lVar2 = *(longlong *)(param_1 + 0x1d8);
  iVar4 = 0;
  pcVar3 = param_3;
  while ((pcVar3 != (char *)0x0 && (cVar1 = *pcVar3, pcVar3 = pcVar3 + 1, cVar1 != '\0'))) {
    iVar4 = iVar4 + 1;
  }
  if (lVar2 != 0) {
    FUN_14050b3b0(lVar2,param_3,iVar4,*(undefined4 *)(lVar2 + 0x2ac));
  }
  FUN_14055af70(param_1 + 0x18,*(undefined8 *)(param_1 + 0x1d8));
  FUN_14055ba40(param_1);
  return;
}


// Incoming references
// 0xc2d7d0 DATA caller none
// 0xa83528 DATA caller none
