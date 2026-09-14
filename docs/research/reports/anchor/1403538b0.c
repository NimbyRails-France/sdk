
void FUN_1403538b0(longlong *param_1)

{
  void *pvVar1;
  longlong lVar2;
  void *_Memory;
  longlong lVar3;
  longlong lVar4;
  
  if (param_1[3] != 0) {
    lVar3 = 0;
    lVar4 = lVar3;
    do {
      if ((-1 < *(char *)(lVar4 + *param_1)) &&
         (pvVar1 = *(void **)(param_1[1] + 8 + lVar3), pvVar1 != (void *)0x0)) {
        free(pvVar1);
      }
      lVar2 = param_1[3];
      lVar4 = lVar4 + 1;
      lVar3 = lVar3 + 0x20;
    } while (lVar4 != lVar2);
    pvVar1 = (void *)*param_1;
    _Memory = pvVar1;
    if ((0xfff < (lVar2 + 0x18U & 0xfffffffffffffff8) + lVar2 * 0x20) &&
       (_Memory = *(void **)((longlong)pvVar1 + -8),
       0x1f < (ulonglong)((longlong)pvVar1 + (-8 - (longlong)_Memory)))) {
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

