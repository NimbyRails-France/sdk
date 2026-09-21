// Candidate VA 14074e180; RVA 0x74e180
// Ghidra inferred prototype: undefined FUN_14074e180()

void FUN_14074e180(longlong param_1)

{
  undefined8 *puVar1;
  longlong *plVar2;
  undefined8 uVar3;
  longlong local_res8;
  undefined1 local_98 [32];
  char local_78;
  undefined1 local_70 [32];
  undefined1 local_50 [32];
  undefined1 local_30 [40];

  puVar1 = *(undefined8 **)(param_1 + 0x48);
  plVar2 = *(longlong **)(param_1 + 0x50);
  FUN_14047eae0(plVar2[1],*plVar2 + 0x400);
  uVar3 = FUN_1407428d0(puVar1[1],local_50);
  FUN_140247b90(local_70,*puVar1);
  FUN_1404abc50(local_98,*plVar2,plVar2[1],local_70,uVar3);
  FUN_140002d30(local_30);
  FUN_1404ac9b0(*(undefined8 *)(param_1 + 0x40),local_98);
  if (local_78 != '\0') {
    FUN_140002d30(local_98);
  }
  FUN_14026c9e0(&local_res8);
  if (local_res8 < 0x7ffffffffd050f7f) {
    local_res8 = local_res8 + 50000000;
  }
  else {
    local_res8 = 0x7fffffffffffffff;
  }
  FUN_14026ee80(&local_res8);
  *(undefined1 *)(param_1 + 8) = 3;
  FUN_140002380(param_1);
  return;
}


// Incoming references
// 0xc3797c DATA caller none
// 0xa2d000 DATA caller none
