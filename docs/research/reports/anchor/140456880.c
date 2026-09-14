
longlong * FUN_140456880(longlong *param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  void *pvVar7;
  void *_Memory;
  
  lVar1 = *param_2;
  lVar2 = param_2[1];
  lVar3 = param_2[3];
  *param_2 = (longlong)&DAT_140a6d430;
  lVar4 = param_2[2];
  param_2[2] = 0;
  param_2[1] = 0;
  param_2[3] = 0;
  lVar5 = param_2[6];
  param_2[6] = 0;
  lVar6 = param_1[3];
  pvVar7 = (void *)*param_1;
  *param_1 = lVar1;
  param_1[1] = lVar2;
  param_1[2] = lVar4;
  param_1[3] = lVar3;
  param_1[6] = lVar5;
  if (lVar6 != 0) {
    _Memory = pvVar7;
    if ((0xfff < lVar6 * 0x10 + (lVar6 + 0x18U & 0xfffffffffffffff8)) &&
       (_Memory = *(void **)((longlong)pvVar7 + -8),
       0x1f < (ulonglong)((longlong)pvVar7 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
  }
  return param_1;
}

