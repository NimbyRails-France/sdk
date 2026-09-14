
undefined8 * FUN_1403f1a30(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  void *pvVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  longlong lVar9;
  void *_Memory;
  
  if (param_1 != param_2) {
    uVar3 = *param_1;
    *param_1 = uVar3;
    param_1[2] = param_1[2];
    uVar4 = *param_1;
    param_1[1] = uVar3;
    *param_1 = *param_2;
    *param_2 = uVar4;
    uVar3 = param_1[1];
    param_1[1] = param_2[1];
    param_2[1] = uVar3;
    uVar3 = param_1[2];
    param_1[2] = param_2[2];
    param_2[2] = uVar3;
  }
  puVar1 = param_2 + 3;
  puVar2 = param_1 + 3;
  if (puVar2 != puVar1) {
    uVar3 = *puVar2;
    *puVar2 = 0;
    uVar4 = param_1[5];
    param_1[5] = 0;
    pvVar5 = (void *)*puVar2;
    *puVar2 = uVar3;
    param_1[4] = uVar3;
    param_1[5] = uVar4;
    if (pvVar5 != (void *)0x0) {
      free(pvVar5);
    }
    uVar3 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar3;
    uVar3 = param_1[4];
    param_1[4] = param_2[4];
    param_2[4] = uVar3;
    uVar3 = param_1[5];
    param_1[5] = param_2[5];
    param_2[5] = uVar3;
  }
  uVar3 = param_2[7];
  uVar4 = param_2[6];
  uVar6 = param_2[9];
  param_2[6] = &DAT_140a6d430;
  uVar7 = param_2[8];
  param_2[8] = 0;
  param_2[7] = 0;
  param_2[9] = 0;
  uVar8 = param_2[0xc];
  param_2[0xc] = 0;
  pvVar5 = (void *)param_1[6];
  param_1[7] = uVar3;
  lVar9 = param_1[9];
  param_1[6] = uVar4;
  param_1[8] = uVar7;
  param_1[9] = uVar6;
  param_1[0xc] = uVar8;
  if (lVar9 != 0) {
    _Memory = pvVar5;
    if ((0xfff < (lVar9 + 0x18U & 0xfffffffffffffff8) + lVar9 * 8) &&
       (_Memory = *(void **)((longlong)pvVar5 + -8),
       0x1f < (ulonglong)((longlong)pvVar5 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
  }
  *(undefined4 *)(param_1 + 0xd) = *(undefined4 *)(param_2 + 0xd);
  return param_1;
}

