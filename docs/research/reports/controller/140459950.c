
void FUN_140459950(longlong *param_1,ulonglong param_2)

{
  longlong lVar1;
  undefined8 *puVar2;
  void *_Dst;
  undefined8 *puVar3;
  ulonglong uVar4;
  void *pvVar5;
  void *_Memory;
  longlong local_28 [4];

  LOCK();
  param_1[0xc] = 0;
  UNLOCK();
  uVar4 = param_2 - 1 >> 1 | param_2 - 1;
  param_1[0xd] = (param_2 >> 3) + param_2;
  uVar4 = uVar4 >> 2 | uVar4;
  uVar4 = uVar4 >> 4 | uVar4;
  uVar4 = uVar4 >> 8 | uVar4;
  uVar4 = uVar4 >> 0x10 | uVar4;
  uVar4 = (uVar4 >> 0x1f & 0x1fffffffe | uVar4 * 2) + 2;
  if (uVar4 != param_1[1] - *param_1 >> 3) {
    if (uVar4 == 0) {
      _Dst = (void *)0x0;
      pvVar5 = (void *)0x0;
    }
    else {
      _Dst = (void *)thunk_FUN_140983da8(uVar4 * 8);
      pvVar5 = (void *)(uVar4 * 8 + (longlong)_Dst);
      memset(_Dst,0,uVar4 * 8);
    }
    _Memory = _Dst;
    if (param_1 != local_28) {
      lVar1 = *param_1;
      *param_1 = lVar1;
      param_1[1] = lVar1;
      param_1[2] = param_1[2];
      _Memory = (void *)*param_1;
      *param_1 = (longlong)_Dst;
      param_1[1] = (longlong)pvVar5;
      param_1[2] = (longlong)pvVar5;
    }
    if (_Memory != (void *)0x0) {
      free(_Memory);
    }
    lVar1 = param_1[3];
    if ((ulonglong)(param_1[4] - lVar1 >> 4) < uVar4) {
      FUN_14032b730(param_1 + 3,uVar4 - (param_1[4] - lVar1 >> 4));
    }
    else {
      param_1[4] = uVar4 * 0x10 + lVar1;
    }
  }
  puVar2 = (undefined8 *)param_1[1];
  for (puVar3 = (undefined8 *)*param_1; puVar3 != puVar2; puVar3 = puVar3 + 1) {
    LOCK();
    *puVar3 = 0;
    UNLOCK();
  }
  FUN_14033b630(param_1 + 6);
  return;
}
