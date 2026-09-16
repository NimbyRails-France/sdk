// Candidate VA 1403bd470; RVA 0x3bd470
// Ghidra inferred prototype: undefined FUN_1403bd470()

longlong * FUN_1403bd470(uint *param_1,longlong param_2)

{
  short sVar1;
  longlong *plVar2;
  void *_Dst;
  uint uVar3;
  longlong lVar4;
  ulonglong uVar5;

  lVar4 = param_2;
  plVar2 = (longlong *)FUN_1403ca140();
  if (plVar2 != (longlong *)0x0) {
    return plVar2;
  }
  sVar1 = (short)((ulonglong)lVar4 >> 0x30);
  if ((sVar1 == -1) || (sVar1 != 2)) {
LAB_1403bd622:
    plVar2 = (longlong *)0x0;
  }
  else {
    uVar3 = (uint)((ulonglong)lVar4 >> 0x10);
    uVar5 = (ulonglong)(uVar3 >> ((byte)param_1[1] & 0x1f));
    if ((ulonglong)(*(longlong *)(param_1 + 8) - *(longlong *)(param_1 + 6) >> 3) <= uVar5) {
      FUN_140324fb0(param_1 + 6);
    }
    if (*(longlong *)(*(longlong *)(param_1 + 6) + uVar5 * 8) == 0) {
      _Dst = (void *)thunk_FUN_140983da8(*param_1);
      memset(_Dst,0,(ulonglong)*param_1);
      *(void **)(*(longlong *)(param_1 + 6) + uVar5 * 8) = _Dst;
    }
    plVar2 = (longlong *)0x0;
    uVar3 = param_1[4] & uVar3;
    uVar5 = (lVar4 >> 0x10 & 0xffffffffU) >> ((byte)param_1[1] & 0x3f);
    if (((uVar5 < (ulonglong)(*(longlong *)(param_1 + 8) - *(longlong *)(param_1 + 6) >> 3)) &&
        (uVar3 < param_1[2])) &&
       (lVar4 = *(longlong *)(*(longlong *)(param_1 + 6) + uVar5 * 8), lVar4 != 0)) {
      plVar2 = (longlong *)((ulonglong)uVar3 * 0xf8 + lVar4);
    }
    sVar1 = (short)((ulonglong)*plVar2 >> 0x30);
    if ((sVar1 != -1) && (sVar1 != 0)) {
      if ((ushort)param_2 <= (ushort)*plVar2) goto LAB_1403bd622;
      FUN_14034e280(plVar2);
    }
    plVar2[5] = 0;
    plVar2[0xf] = 0;
    plVar2[1] = 0;
    plVar2[2] = 0;
    plVar2[3] = 0;
    plVar2[4] = 0xf;
    *(undefined1 *)(plVar2 + 1) = 0;
    plVar2[6] = 0;
    plVar2[7] = 0;
    plVar2[8] = 0;
    plVar2[9] = 0;
    plVar2[10] = 0xf;
    *(undefined1 *)(plVar2 + 7) = 0;
    plVar2[0xb] = 0;
    plVar2[0xc] = 0;
    plVar2[0xd] = 0;
    plVar2[0xe] = 0xf;
    *(undefined1 *)(plVar2 + 0xb) = 0;
    plVar2[0x10] = 0;
    plVar2[0x11] = 0;
    plVar2[0x12] = 0;
    plVar2[0x13] = 0;
    plVar2[0x14] = 0;
    plVar2[0x15] = 0;
    plVar2[0x16] = 0;
    plVar2[0x17] = 0;
    plVar2[0x18] = 0;
    plVar2[0x19] = 0;
    plVar2[0x1a] = 0;
    plVar2[0x1b] = 0;
    plVar2[0x1c] = 0;
    plVar2[0x1d] = 0;
    plVar2[0x1e] = 0;
    *plVar2 = param_2;
  }
  return plVar2;
}


// Incoming references
// 0xc1f454 DATA caller none
// 0xb00638 DATA caller none
// 0xb00670 DATA caller none
// 0x3b17a6 UNCONDITIONAL_CALL caller 1403b16e0
// 0x39150f UNCONDITIONAL_CALL caller 1403914d0
// 0x3a4464 UNCONDITIONAL_CALL caller 1403a4290
// 0x396eae UNCONDITIONAL_CALL caller 140396e40
// 0x39783e UNCONDITIONAL_CALL caller 1403977d0
