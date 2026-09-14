
void FUN_140714000(longlong *param_1)

{
  longlong lVar1;
  undefined8 *puVar2;
  void *pvVar3;
  void *_Memory;
  longlong lVar4;
  undefined8 *_Memory_00;
  longlong lVar5;
  longlong lVar6;
  
  lVar4 = param_1[3];
  if (lVar4 != 0) {
    lVar5 = 0;
    lVar6 = lVar5;
    do {
      if (-1 < *(char *)(lVar5 + *param_1)) {
        lVar1 = param_1[1];
        _Memory_00 = *(undefined8 **)(lVar6 + 0x18 + lVar1);
        if (_Memory_00 != (undefined8 *)0x0) {
          do {
            FUN_1402450d0(lVar6 + lVar1 + 8,*_Memory_00);
            puVar2 = (undefined8 *)_Memory_00[1];
            free(_Memory_00);
            _Memory_00 = puVar2;
          } while (puVar2 != (undefined8 *)0x0);
          lVar4 = param_1[3];
        }
      }
      lVar5 = lVar5 + 1;
      lVar6 = lVar6 + 0x38;
    } while (lVar5 != lVar4);
    pvVar3 = (void *)*param_1;
    _Memory = pvVar3;
    if ((0xfff < lVar4 * 0x38 + (lVar4 + 0x18U & 0xfffffffffffffff8)) &&
       (_Memory = *(void **)((longlong)pvVar3 + -8),
       0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)_Memory)))) {
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

