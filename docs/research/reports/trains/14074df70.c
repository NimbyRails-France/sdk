// Candidate VA 14074df70; RVA 0x74df70
// Ghidra inferred prototype: undefined FUN_14074df70()

void FUN_14074df70(longlong param_1)

{
  longlong local_res8 [4];
  undefined1 local_38 [32];
  char local_18;
  
  FUN_1404a1710(local_38,**(undefined8 **)(param_1 + 0x50),(*(undefined8 **)(param_1 + 0x50))[1]);
  FUN_1402f1690(*(undefined8 *)(param_1 + 0x40),local_38);
  if (local_18 != '\0') {
    FUN_140025470(local_38);
  }
  FUN_14026c9e0(local_res8);
  if (local_res8[0] < 0x7ffffffffd050f7f) {
    local_res8[0] = local_res8[0] + 50000000;
  }
  else {
    local_res8[0] = 0x7fffffffffffffff;
  }
  FUN_14026ee80(local_res8);
  *(undefined1 *)(param_1 + 8) = 3;
  FUN_140002380(param_1);
  return;
}


// Incoming references
// 0xc37958 DATA caller none
// 0xa2cfe0 DATA caller none
