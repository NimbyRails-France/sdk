
void FUN_140476c20(undefined1 param_1,undefined8 *param_2,undefined8 *param_3)

{
  undefined8 uVar1;
  longlong lVar2;
  void *pvVar3;
  void *_Memory;
  
  *param_2 = *param_3;
  param_2[1] = param_3[1];
  param_2[2] = param_3[2];
  uVar1 = param_3[3];
  param_3[3] = &DAT_140a6d430;
  param_2[3] = uVar1;
  uVar1 = param_3[4];
  param_3[4] = 0;
  param_2[4] = uVar1;
  uVar1 = param_3[5];
  param_3[5] = 0;
  param_2[5] = uVar1;
  uVar1 = param_3[6];
  param_3[6] = 0;
  param_2[6] = uVar1;
  *(undefined1 *)(param_2 + 7) = param_1;
  *(undefined1 *)(param_2 + 8) = *(undefined1 *)(param_3 + 8);
  *(undefined1 *)((longlong)param_2 + 0x41) = *(undefined1 *)((longlong)param_3 + 0x41);
  *(undefined1 *)((longlong)param_2 + 0x42) = *(undefined1 *)((longlong)param_3 + 0x42);
  param_2[9] = param_3[9];
  param_3[9] = 0;
  lVar2 = param_3[6];
  if (lVar2 != 0) {
    pvVar3 = (void *)param_3[3];
    _Memory = pvVar3;
    if ((0xfff < (lVar2 + 0x18U & 0xfffffffffffffff8) + lVar2 * 0x20) &&
       (_Memory = *(void **)((longlong)pvVar3 + -8),
       0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
    param_3[3] = &DAT_140a6d430;
    param_3[4] = 0;
    param_3[5] = 0;
    param_3[6] = 0;
    param_3[9] = 0;
  }
  return;
}

