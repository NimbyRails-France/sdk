
void FUN_1404508a0(longlong param_1,undefined8 *param_2)

{
  longlong lVar1;
  longlong *plVar2;
  void *_Dst;
  longlong *plVar3;
  uint uVar4;
  longlong *plVar5;
  ulonglong uVar6;
  longlong lVar7;
  ulonglong uVar8;

  plVar2 = (longlong *)param_2[1];
  for (plVar5 = (longlong *)*param_2; plVar5 != plVar2; plVar5 = plVar5 + 8) {
    lVar7 = *plVar5;
    uVar8 = lVar7 >> 0x10;
    uVar4 = (uint)((ulonglong)lVar7 >> 0x10);
    uVar6 = (ulonglong)(uVar4 >> ((byte)*(undefined4 *)(param_1 + 0x34) & 0x1f));
    if ((ulonglong)(*(longlong *)(param_1 + 0x50) - *(longlong *)(param_1 + 0x48) >> 3) <= uVar6) {
      FUN_140324fb0(param_1 + 0x48);
      lVar7 = *plVar5;
    }
    lVar1 = uVar6 * 8;
    if (*(longlong *)(lVar1 + *(longlong *)(param_1 + 0x48)) == 0) {
      _Dst = (void *)thunk_FUN_140983da8(*(undefined4 *)(param_1 + 0x30));
      memset(_Dst,0,(ulonglong)*(uint *)(param_1 + 0x30));
      *(void **)(lVar1 + *(longlong *)(param_1 + 0x48)) = _Dst;
      lVar7 = *plVar5;
    }
    uVar4 = uVar4 & *(uint *)(param_1 + 0x40);
    uVar6 = (uVar8 & 0xffffffff) >> ((byte)*(undefined4 *)(param_1 + 0x34) & 0x3f);
    if (((uVar6 < (ulonglong)(*(longlong *)(param_1 + 0x50) - *(longlong *)(param_1 + 0x48) >> 3))
        && (uVar4 < *(uint *)(param_1 + 0x38))) &&
       (lVar1 = *(longlong *)(*(longlong *)(param_1 + 0x48) + uVar6 * 8), lVar1 != 0)) {
      plVar3 = (longlong *)((ulonglong)uVar4 * 0x40 + lVar1);
    }
    else {
      plVar3 = (longlong *)0x0;
    }
    *plVar3 = lVar7;
    FUN_14045abc0(plVar3 + 1);
  }
  return;
}
