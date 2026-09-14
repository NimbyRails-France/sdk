// Candidate VA 140428150; RVA 0x428150
// Ghidra inferred prototype: undefined FUN_140428150()

longlong FUN_140428150(undefined8 param_1,longlong *param_2)

{
  longlong lVar1;
  
  lVar1 = *param_2;
  if (lVar1 != param_2[1]) {
    *param_2 = lVar1 + 0x20;
    return lVar1;
  }
  return 0;
}


// Incoming references
// 0x42d9ad DATA caller 140428790
// 0x42d9b4 DATA caller 140428790
