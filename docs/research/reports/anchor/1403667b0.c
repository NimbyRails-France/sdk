
void FUN_1403667b0(longlong *param_1)

{
  longlong lVar1;
  void *pvVar2;
  void *_Memory;
  
  lVar1 = param_1[3];
  if (lVar1 != 0) {
    pvVar2 = (void *)*param_1;
    _Memory = pvVar2;
    if ((0xfff < (lVar1 + 0x14U & 0xfffffffffffffffc) + lVar1 * 8) &&
       (_Memory = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
    *param_1 = (longlong)&DAT_140a6d430;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    param_1[6] = 0;
  }
  return;
}

