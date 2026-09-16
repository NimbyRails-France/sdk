
void FUN_1404575f0(longlong *param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;
  longlong lVar9;
  longlong lVar10;
  void *pvVar11;
  void *pvVar12;

  lVar1 = *param_1;
  lVar2 = param_1[1];
  lVar3 = param_1[2];
  lVar4 = param_1[3];
  *param_1 = (longlong)&DAT_140a6d430;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  lVar5 = param_1[6];
  param_1[6] = 0;
  lVar6 = param_2[2];
  lVar7 = *param_2;
  lVar8 = param_2[1];
  *param_2 = (longlong)&DAT_140a6d430;
  param_2[1] = 0;
  param_2[2] = 0;
  lVar9 = param_2[3];
  param_2[3] = 0;
  lVar10 = param_2[6];
  param_2[6] = 0;
  pvVar11 = (void *)*param_1;
  param_1[2] = lVar6;
  lVar6 = param_1[3];
  *param_1 = lVar7;
  param_1[1] = lVar8;
  param_1[3] = lVar9;
  param_1[6] = lVar10;
  if (lVar6 != 0) {
    pvVar12 = pvVar11;
    if ((0xfff < (lVar6 + 0x18U & 0xfffffffffffffff8) + lVar6 * 0x10) &&
       (pvVar12 = *(void **)((longlong)pvVar11 + -8),
       0x1f < (ulonglong)((longlong)pvVar11 + (-8 - (longlong)pvVar12)))) goto LAB_140457734;
    free(pvVar12);
  }
  lVar6 = param_2[3];
  pvVar11 = (void *)*param_2;
  *param_2 = lVar1;
  param_2[1] = lVar2;
  param_2[2] = lVar3;
  param_2[3] = lVar4;
  param_2[6] = lVar5;
  if (lVar6 != 0) {
    pvVar12 = pvVar11;
    if ((0xfff < lVar6 * 0x10 + (lVar6 + 0x18U & 0xfffffffffffffff8)) &&
       (pvVar12 = *(void **)((longlong)pvVar11 + -8),
       0x1f < (ulonglong)((longlong)pvVar11 + (-8 - (longlong)pvVar12)))) {
LAB_140457734:
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar12);
  }
  return;
}
