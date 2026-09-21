// Candidate VA 14050cc40; RVA 0x50cc40
// Ghidra inferred prototype: undefined FUN_14050cc40()

undefined4 FUN_14050cc40(longlong param_1,undefined8 param_2,undefined4 param_3,undefined4 param_4)

{
  longlong lVar1;
  int iVar2;
  undefined4 local_res20 [2];
  undefined1 local_28 [16];

  if ((((param_1 != 0) && (lVar1 = *(longlong *)(param_1 + 0x4778), lVar1 != 0)) &&
      (*(longlong *)(lVar1 + 0xa8) != 0)) &&
     (local_res20[0] = param_4, iVar2 = FUN_14050abd0(local_28,param_1), iVar2 != 0)) {
    FUN_14050c850(param_1 + 0x2570,lVar1 + 0x68,local_28,local_res20,param_2,param_3);
    param_4 = local_res20[0];
  }
  return param_4;
}


// Incoming references
// 0xc2a548 DATA caller none
// 0x5608fe UNCONDITIONAL_CALL caller 140560870
