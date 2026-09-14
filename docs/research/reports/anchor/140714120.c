
void FUN_140714120(longlong *param_1)

{
  longlong lVar1;
  void *pvVar2;
  void *_Memory;
  longlong lVar3;
  longlong lVar4;
  
  if (param_1[3] != 0) {
    lVar3 = 0;
    lVar4 = lVar3;
    do {
      if (-1 < *(char *)(lVar4 + *param_1)) {
        FUN_1402ff1e0(param_1[1] + 8 + lVar3);
      }
      lVar1 = param_1[3];
      lVar4 = lVar4 + 1;
      lVar3 = lVar3 + 0x68;
    } while (lVar4 != lVar1);
    pvVar2 = (void *)*param_1;
    _Memory = pvVar2;
    if ((0xfff < (lVar1 + 0x18U & 0xfffffffffffffff8) + lVar1 * 0x68) &&
       (_Memory = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    *param_1 = (longlong)&DAT_140a6d430;
    param_1[6] = 0;
  }
  return;
}

