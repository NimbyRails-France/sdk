
void FUN_140478b20(longlong param_1,longlong param_2)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  void *pvVar6;
  longlong *plVar7;
  short sVar8;
  longlong *plVar9;
  longlong *plVar10;
  longlong lVar11;
  longlong *plVar12;
  void *_Memory;
  longlong *local_68;
  longlong *local_60;
  undefined8 local_58;
  longlong local_50 [3];

  *(longlong *)(param_1 + 0x2170) =
       *(longlong *)(param_1 + 0x2170) +
       (*(longlong *)(param_2 + 0x18) - *(longlong *)(param_2 + 0x10) >> 3) * 0x2697cc8aef46c0f7;
  *(longlong *)(param_1 + 0x21d0) =
       *(longlong *)(param_1 + 0x21d0) +
       (*(longlong *)(param_2 + 0x48) - *(longlong *)(param_2 + 0x40) >> 3) * 0x51b3bea3677d46cf;
  *(longlong *)(param_1 + 0x21a0) =
       *(longlong *)(param_1 + 0x21a0) +
       (*(longlong *)(param_2 + 0x30) - *(longlong *)(param_2 + 0x28) >> 6);
  local_68 = *(longlong **)(param_2 + 0x40);
  *(undefined8 *)(param_2 + 0x40) = 0;
  local_60 = *(longlong **)(param_2 + 0x48);
  *(undefined8 *)(param_2 + 0x48) = 0;
  local_58 = *(undefined8 *)(param_2 + 0x50);
  *(undefined8 *)(param_2 + 0x50) = 0;
  FUN_140485850(param_1 + 0x40,&local_68);
  plVar10 = *(longlong **)(param_2 + 0x58);
  *(undefined8 *)(param_2 + 0x58) = 0;
  plVar1 = *(longlong **)(param_2 + 0x60);
  *(undefined8 *)(param_2 + 0x60) = 0;
  local_58 = *(undefined8 *)(param_2 + 0x68);
  *(undefined8 *)(param_2 + 0x68) = 0;
  plVar9 = plVar10;
  local_68 = plVar10;
  plVar7 = plVar1;
  local_60 = plVar1;
  if (plVar10 != plVar1) {
    plVar12 = plVar10 + 4;
    do {
      FUN_140486b80(param_1 + 0x70,local_50);
      *(longlong *)(local_50[0] + 8) = *plVar10;
      *(longlong *)(local_50[0] + 0x10) = plVar12[-3];
      lVar2 = plVar12[-2];
      plVar12[-2] = (longlong)&DAT_140a6d430;
      lVar11 = plVar12[-1];
      plVar12[-1] = 0;
      lVar3 = *plVar12;
      *plVar12 = 0;
      lVar4 = plVar12[1];
      plVar12[1] = 0;
      lVar5 = plVar12[4];
      plVar12[4] = 0;
      pvVar6 = *(void **)(local_50[0] + 0x18);
      *(longlong *)(local_50[0] + 0x18) = lVar2;
      *(longlong *)(local_50[0] + 0x20) = lVar11;
      *(longlong *)(local_50[0] + 0x28) = lVar3;
      lVar2 = *(longlong *)(local_50[0] + 0x30);
      *(longlong *)(local_50[0] + 0x30) = lVar4;
      *(longlong *)(local_50[0] + 0x48) = lVar5;
      if (lVar2 != 0) {
        _Memory = pvVar6;
        if ((0xfff < lVar2 * 0x20 + (lVar2 + 0x18U & 0xfffffffffffffff8)) &&
           (_Memory = *(void **)((longlong)pvVar6 + -8),
           0x1f < (ulonglong)((longlong)pvVar6 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory);
      }
      plVar10 = plVar10 + 9;
      plVar12 = plVar12 + 9;
      plVar9 = local_68;
      plVar7 = local_60;
    } while (plVar10 != plVar1);
  }
  for (; plVar10 = local_60, plVar9 != local_60; plVar9 = plVar9 + 9) {
    local_60 = plVar7;
    FUN_140353b70(plVar9 + 2);
    plVar7 = local_60;
    local_60 = plVar10;
  }
  if (local_68 != (longlong *)0x0) {
    local_60 = plVar7;
    free(local_68);
  }
  param_1 = param_1 + 0xa0;
  plVar10 = *(longlong **)(param_2 + 0x10);
  *(undefined8 *)(param_2 + 0x10) = 0;
  plVar1 = *(longlong **)(param_2 + 0x18);
  *(undefined8 *)(param_2 + 0x18) = 0;
  local_58 = *(undefined8 *)(param_2 + 0x20);
  *(undefined8 *)(param_2 + 0x20) = 0;
  local_68 = plVar10;
  plVar9 = plVar1;
  do {
    local_60 = plVar9;
    plVar7 = local_68;
    if (plVar10 == plVar1) {
      for (; plVar7 != plVar9; plVar7 = plVar7 + 199) {
        FUN_140351080(plVar7);
      }
      if (local_68 != (longlong *)0x0) {
        free(local_68);
      }
      local_68 = *(longlong **)(param_2 + 0x28);
      *(undefined8 *)(param_2 + 0x28) = 0;
      local_60 = *(longlong **)(param_2 + 0x30);
      *(undefined8 *)(param_2 + 0x30) = 0;
      local_58 = *(undefined8 *)(param_2 + 0x38);
      *(undefined8 *)(param_2 + 0x38) = 0;
      FUN_1404509c0(param_1,&local_68);
      lVar2 = *(longlong *)(param_2 + 0x60);
      for (lVar11 = *(longlong *)(param_2 + 0x58); lVar11 != lVar2; lVar11 = lVar11 + 0x48) {
        FUN_140353b70(lVar11 + 0x10);
      }
      if (*(void **)(param_2 + 0x58) != (void *)0x0) {
        free(*(void **)(param_2 + 0x58));
      }
      lVar2 = *(longlong *)(param_2 + 0x48);
      for (lVar11 = *(longlong *)(param_2 + 0x40); lVar11 != lVar2; lVar11 = lVar11 + 0x178) {
        FUN_140351180(lVar11);
      }
      if (*(void **)(param_2 + 0x40) != (void *)0x0) {
        free(*(void **)(param_2 + 0x40));
      }
      lVar2 = *(longlong *)(param_2 + 0x30);
      for (lVar11 = *(longlong *)(param_2 + 0x28); lVar11 != lVar2; lVar11 = lVar11 + 0x40) {
        FUN_14034b770(lVar11 + 8);
      }
      if (*(void **)(param_2 + 0x28) != (void *)0x0) {
        free(*(void **)(param_2 + 0x28));
      }
      lVar2 = *(longlong *)(param_2 + 0x18);
      for (lVar11 = *(longlong *)(param_2 + 0x10); lVar11 != lVar2; lVar11 = lVar11 + 0x638) {
        FUN_140351080(lVar11);
      }
      if (*(void **)(param_2 + 0x10) != (void *)0x0) {
        free(*(void **)(param_2 + 0x10));
      }
      return;
    }
    lVar2 = *plVar10;
    lVar11 = lVar2;
    plVar9 = (longlong *)FUN_14045f300(param_1,lVar2);
    if (plVar9 == (longlong *)0x0) {
      sVar8 = (short)((ulonglong)lVar2 >> 0x30);
      if ((sVar8 != -1) && (sVar8 == 5)) {
        plVar9 = (longlong *)FUN_14045f230(param_1,lVar11 >> 0x10);
        sVar8 = (short)((ulonglong)*plVar9 >> 0x30);
        if ((sVar8 != -1) && (sVar8 != 0)) {
          if ((ushort)lVar2 <= (ushort)*plVar9) goto LAB_140478dd7;
          FUN_140351080(plVar9);
        }
        memset(plVar9 + 1,0,0x630);
        FUN_140459860(plVar9);
        *plVar9 = lVar2;
        if (plVar9 != (longlong *)0x0) goto LAB_140478dcc;
      }
    }
    else {
LAB_140478dcc:
      FUN_140450420(plVar9,plVar10);
    }
LAB_140478dd7:
    plVar10 = plVar10 + 199;
    plVar9 = local_60;
  } while( true );
}
