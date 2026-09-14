// Candidate VA 1403856e0; RVA 0x3856e0
// Ghidra inferred prototype: undefined FUN_1403856e0()

ulonglong FUN_1403856e0(longlong *param_1,longlong param_2,double param_3,int param_4)

{
  double *in_RAX;
  double *pdVar1;
  double *pdVar2;
  longlong *plVar3;
  double dVar4;
  
  plVar3 = (longlong *)*param_1;
  if (plVar3 != (longlong *)param_1[1]) {
    pdVar1 = (double *)(plVar3 + 2);
    do {
      if (param_2 == *plVar3) {
        dVar4 = param_3;
        if (param_4 != *(char *)(pdVar1 + 1)) {
          dVar4 = DAT_140aabd08 - param_3;
        }
        in_RAX = pdVar1 + -1;
        if (*pdVar1 < pdVar1[-1]) {
          in_RAX = pdVar1;
        }
        if (*in_RAX <= dVar4) {
          pdVar2 = pdVar1 + -1;
          if (pdVar1[-1] < *pdVar1) {
            pdVar2 = pdVar1;
          }
          if (dVar4 < *pdVar2 || dVar4 == *pdVar2) {
            return CONCAT71((int7)((ulonglong)in_RAX >> 8),1);
          }
        }
      }
      plVar3 = plVar3 + 4;
      pdVar1 = pdVar1 + 4;
    } while (plVar3 != (longlong *)param_1[1]);
  }
  return (ulonglong)in_RAX & 0xffffffffffffff00;
}


// Incoming references
// 0x3857a5 UNCONDITIONAL_CALL caller 140385760
// 0x3857bf UNCONDITIONAL_CALL caller 140385760
// 0x3857d9 UNCONDITIONAL_CALL caller 140385760
// 0x3857f3 UNCONDITIONAL_CALL caller 140385760
// 0x444b1a UNCONDITIONAL_CALL caller 140444230
// 0x444b46 UNCONDITIONAL_CALL caller 140444230
// 0x444cc5 UNCONDITIONAL_CALL caller 140444230
// 0x444ce4 UNCONDITIONAL_CALL caller 140444230
// 0x38584f UNCONDITIONAL_CALL caller 140385820
// 0x385869 UNCONDITIONAL_CALL caller 140385820
// 0x42802d UNCONDITIONAL_CALL caller none
