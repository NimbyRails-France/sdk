
undefined8 * FUN_1404565e0(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  void *pvVar6;

  if (param_1 != param_2) {
    uVar4 = *param_1;
    *param_1 = uVar4;
    param_1[2] = param_1[2];
    param_1[1] = uVar4;
    uVar4 = *param_1;
    *param_1 = *param_2;
    *param_2 = uVar4;
    uVar4 = param_1[1];
    param_1[1] = param_2[1];
    param_2[1] = uVar4;
    uVar4 = param_1[2];
    param_1[2] = param_2[2];
    param_2[2] = uVar4;
  }
  puVar1 = param_2 + 3;
  puVar2 = param_1 + 3;
  if (puVar2 != puVar1) {
    uVar4 = *puVar2;
    *puVar2 = 0;
    uVar5 = param_1[5];
    param_1[5] = 0;
    pvVar6 = (void *)*puVar2;
    *puVar2 = uVar4;
    param_1[4] = uVar4;
    param_1[5] = uVar5;
    if (pvVar6 != (void *)0x0) {
      free(pvVar6);
    }
    uVar4 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar4;
    uVar4 = param_1[4];
    param_1[4] = param_2[4];
    param_2[4] = uVar4;
    uVar4 = param_1[5];
    param_1[5] = param_2[5];
    param_2[5] = uVar4;
  }
  if (param_1 + 6 != param_2 + 6) {
    FUN_14033b630(param_1 + 6);
    uVar3 = *(undefined4 *)(param_1 + 0xb);
    uVar4 = param_1[10];
    param_1[10] = param_2[10];
    *(undefined4 *)(param_1 + 0xb) = *(undefined4 *)(param_2 + 0xb);
    param_2[10] = uVar4;
    *(undefined4 *)(param_2 + 0xb) = uVar3;
    uVar4 = param_1[7];
    param_1[7] = param_2[7];
    param_2[7] = uVar4;
    uVar4 = param_1[8];
    param_1[8] = param_2[8];
    param_2[8] = uVar4;
    uVar4 = param_1[9];
    param_1[9] = param_2[9];
    param_2[9] = uVar4;
  }
  puVar1 = param_2 + 0x18;
  puVar2 = param_1 + 0x18;
  LOCK();
  param_1[0xc] = param_2[0xc];
  UNLOCK();
  param_1[0xd] = param_2[0xd];
  if (puVar2 != puVar1) {
    uVar4 = *puVar2;
    *puVar2 = 0;
    uVar5 = param_1[0x1a];
    param_1[0x1a] = 0;
    pvVar6 = (void *)*puVar2;
    *puVar2 = uVar4;
    param_1[0x19] = uVar4;
    param_1[0x1a] = uVar5;
    if (pvVar6 != (void *)0x0) {
      free(pvVar6);
    }
    uVar4 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar4;
    uVar4 = param_1[0x19];
    param_1[0x19] = param_2[0x19];
    param_2[0x19] = uVar4;
    uVar4 = param_1[0x1a];
    param_1[0x1a] = param_2[0x1a];
    param_2[0x1a] = uVar4;
  }
  puVar1 = param_2 + 0x1b;
  puVar2 = param_1 + 0x1b;
  if (puVar2 != puVar1) {
    uVar4 = *puVar2;
    *puVar2 = 0;
    uVar5 = param_1[0x1d];
    param_1[0x1d] = 0;
    pvVar6 = (void *)*puVar2;
    *puVar2 = uVar4;
    param_1[0x1c] = uVar4;
    param_1[0x1d] = uVar5;
    if (pvVar6 != (void *)0x0) {
      free(pvVar6);
    }
    uVar4 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar4;
    uVar4 = param_1[0x1c];
    param_1[0x1c] = param_2[0x1c];
    param_2[0x1c] = uVar4;
    uVar4 = param_1[0x1d];
    param_1[0x1d] = param_2[0x1d];
    param_2[0x1d] = uVar4;
  }
  if (param_1 + 0x1e != param_2 + 0x1e) {
    FUN_14033b630(param_1 + 0x1e);
    uVar3 = *(undefined4 *)(param_1 + 0x23);
    uVar4 = param_1[0x22];
    param_1[0x22] = param_2[0x22];
    *(undefined4 *)(param_1 + 0x23) = *(undefined4 *)(param_2 + 0x23);
    param_2[0x22] = uVar4;
    *(undefined4 *)(param_2 + 0x23) = uVar3;
    uVar4 = param_1[0x1f];
    param_1[0x1f] = param_2[0x1f];
    param_2[0x1f] = uVar4;
    uVar4 = param_1[0x20];
    param_1[0x20] = param_2[0x20];
    param_2[0x20] = uVar4;
    uVar4 = param_1[0x21];
    param_1[0x21] = param_2[0x21];
    param_2[0x21] = uVar4;
  }
  LOCK();
  param_1[0x24] = param_2[0x24];
  UNLOCK();
  param_1[0x25] = param_2[0x25];
  return param_1;
}
