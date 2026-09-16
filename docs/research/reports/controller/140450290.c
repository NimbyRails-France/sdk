
void FUN_140450290(uint *param_1,undefined8 param_2,undefined8 *param_3)

{
  void *_Dst;
  longlong lVar1;
  uint uVar2;
  uint uVar3;
  longlong *plVar4;
  longlong lVar5;
  ulonglong uVar6;
  longlong *plVar7;

  plVar7 = (longlong *)param_3[1];
  for (plVar4 = (longlong *)*param_3; plVar4 != plVar7; plVar4 = plVar4 + 199) {
    lVar1 = *plVar4;
    uVar2 = (uint)((ulonglong)lVar1 >> 0x10);
    uVar6 = (ulonglong)(uVar2 >> ((byte)param_1[1] & 0x1f));
    if ((ulonglong)(*(longlong *)(param_1 + 8) - *(longlong *)(param_1 + 6) >> 3) <= uVar6) {
      FUN_140324fb0(param_1 + 6);
    }
    lVar5 = uVar6 * 8;
    if (*(longlong *)(lVar5 + *(longlong *)(param_1 + 6)) == 0) {
      _Dst = (void *)thunk_FUN_140983da8(*param_1);
      memset(_Dst,0,(ulonglong)*param_1);
      *(void **)(lVar5 + *(longlong *)(param_1 + 6)) = _Dst;
    }
    uVar2 = param_1[4] & uVar2;
    uVar6 = (lVar1 >> 0x10 & 0xffffffffU) >> ((byte)param_1[1] & 0x3f);
    if (((uVar6 < (ulonglong)(*(longlong *)(param_1 + 8) - *(longlong *)(param_1 + 6) >> 3)) &&
        (uVar2 < param_1[2])) &&
       (lVar1 = *(longlong *)(*(longlong *)(param_1 + 6) + uVar6 * 8), lVar1 != 0)) {
      lVar1 = (ulonglong)uVar2 * 0x638 + lVar1;
    }
    else {
      lVar1 = 0;
    }
    FUN_14045ac60(lVar1);
  }
  plVar4 = *(longlong **)(param_1 + 8);
  for (plVar7 = *(longlong **)(param_1 + 6); plVar7 != plVar4; plVar7 = plVar7 + 1) {
    lVar1 = *plVar7;
    if (lVar1 != 0) {
      uVar2 = param_1[2];
      uVar6 = 0;
      lVar5 = lVar1;
      if (uVar2 != 0) {
        do {
          if ((*(short *)(lVar5 + 6) != -1) && (*(short *)(lVar5 + 6) != 0)) {
            FUN_1403e0d90(uVar6 * 0x638 + 8 + lVar1,param_2);
          }
          uVar3 = (int)uVar6 + 1;
          uVar6 = (ulonglong)uVar3;
          lVar5 = lVar5 + 0x638;
        } while (uVar3 < uVar2);
      }
    }
  }
  return;
}
