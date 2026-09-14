
void FUN_1404cad40(longlong *param_1,ulonglong param_2)

{
  longlong lVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  
  puVar3 = (undefined8 *)param_1[1];
  if ((ulonglong)((param_1[2] - (longlong)puVar3 >> 3) * -0x71c71c71c71c71c7) < param_2) {
    lVar5 = (longlong)puVar3 - *param_1 >> 3;
    lVar6 = lVar5 * -0x71c71c71c71c71c7;
    uVar8 = lVar5 * 0x1c71c71c71c71c72;
    if (lVar6 == 0) {
      uVar8 = 1;
    }
    uVar7 = lVar6 + param_2;
    if (uVar8 < uVar7) {
      uVar8 = uVar7;
    }
    lVar5 = 0;
    if (uVar8 != 0) {
      lVar5 = thunk_FUN_140983da8(uVar8 * 0x48);
    }
    lVar6 = FUN_140486ab0(*param_1,param_1[1],lVar5);
    if (param_2 != 0) {
      puVar3 = (undefined8 *)(lVar6 + 0x20);
      uVar7 = param_2;
      do {
        puVar3[2] = 0;
        puVar3[3] = 0;
        puVar3[-4] = 0;
        puVar3[-3] = 0;
        puVar3[-2] = &DAT_140a6d430;
        puVar3[-1] = 0;
        *puVar3 = 0;
        puVar3[1] = 0;
        puVar3[4] = 0;
        puVar3 = puVar3 + 9;
        uVar7 = uVar7 - 1;
      } while (uVar7 != 0);
    }
    lVar1 = param_1[1];
    for (lVar4 = *param_1; lVar4 != lVar1; lVar4 = lVar4 + 0x48) {
      FUN_140353b70(lVar4 + 0x10);
    }
    if ((void *)*param_1 != (void *)0x0) {
      free((void *)*param_1);
    }
    *param_1 = lVar5;
    param_1[1] = lVar6 + param_2 * 0x48;
    param_1[2] = lVar5 + uVar8 * 0x48;
    return;
  }
  if (param_2 != 0) {
    puVar2 = puVar3 + 4;
    uVar8 = param_2;
    do {
      puVar3[1] = 0;
      puVar3[2] = 0;
      puVar3[3] = 0;
      puVar3[4] = 0;
      puVar3[5] = 0;
      puVar3[6] = 0;
      puVar3[7] = 0;
      puVar3[8] = 0;
      *puVar3 = 0;
      puVar3 = puVar3 + 9;
      puVar2[-3] = 0;
      puVar2[-2] = &DAT_140a6d430;
      puVar2[-1] = 0;
      *puVar2 = 0;
      puVar2[1] = 0;
      puVar2[4] = 0;
      uVar8 = uVar8 - 1;
      puVar2 = puVar2 + 9;
    } while (uVar8 != 0);
  }
  param_1[1] = param_1[1] + param_2 * 0x48;
  return;
}

