// Candidate VA 1404282f0; RVA 0x4282f0
// Ghidra inferred prototype: undefined FUN_1404282f0()

float * FUN_1404282f0(longlong param_1)

{
  float fVar1;
  float *in_RAX;
  float *pfVar2;
  float *pfVar3;
  
  if (param_1 == 0) {
    return in_RAX;
  }
  pfVar3 = (float *)(param_1 + 0x80);
  fVar1 = *(float *)(param_1 + 0x84);
  if (DAT_140aab904 <= fVar1) {
    pfVar2 = (float *)(param_1 + 0x84);
    if (*pfVar3 <= fVar1 && fVar1 != *pfVar3) {
      pfVar2 = pfVar3;
    }
    return pfVar2;
  }
  return pfVar3;
}


// Incoming references
// 0x429e6d DATA caller 140428790
