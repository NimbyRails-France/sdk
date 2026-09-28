
undefined8 * FUN_1404d70c0(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  void *_Memory;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  *param_1 = *param_2;
  *(undefined1 *)(param_1 + 1) = *(undefined1 *)(param_2 + 1);
  FUN_140025630(param_1 + 2,param_2 + 2);
  puVar1 = param_2 + 0xf;
  *(undefined4 *)(param_1 + 6) = *(undefined4 *)(param_2 + 6);
  puVar2 = param_1 + 0xf;
  param_1[7] = param_2[7];
  uVar5 = *(undefined4 *)((longlong)param_2 + 0x44);
  uVar6 = *(undefined4 *)(param_2 + 9);
  uVar7 = *(undefined4 *)((longlong)param_2 + 0x4c);
  *(undefined4 *)(param_1 + 8) = *(undefined4 *)(param_2 + 8);
  *(undefined4 *)((longlong)param_1 + 0x44) = uVar5;
  *(undefined4 *)(param_1 + 9) = uVar6;
  *(undefined4 *)((longlong)param_1 + 0x4c) = uVar7;
  param_1[10] = param_2[10];
  *(undefined4 *)(param_1 + 0xb) = *(undefined4 *)(param_2 + 0xb);
  *(undefined4 *)((longlong)param_1 + 0x5c) = *(undefined4 *)((longlong)param_2 + 0x5c);
  *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)(param_2 + 0xc);
  *(undefined1 *)((longlong)param_1 + 100) = *(undefined1 *)((longlong)param_2 + 100);
  *(undefined4 *)(param_1 + 0xd) = *(undefined4 *)(param_2 + 0xd);
  *(undefined1 *)((longlong)param_1 + 0x6c) = *(undefined1 *)((longlong)param_2 + 0x6c);
  *(undefined1 *)((longlong)param_1 + 0x6d) = *(undefined1 *)((longlong)param_2 + 0x6d);
  *(undefined4 *)(param_1 + 0xe) = *(undefined4 *)(param_2 + 0xe);
  if (puVar2 != puVar1) {
    uVar3 = *puVar2;
    *puVar2 = 0;
    uVar4 = param_1[0x11];
    param_1[0x11] = 0;
    _Memory = (void *)*puVar2;
    *puVar2 = uVar3;
    param_1[0x10] = uVar3;
    param_1[0x11] = uVar4;
    if (_Memory != (void *)0x0) {
      free(_Memory);
    }
    uVar3 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar3;
    uVar3 = param_1[0x10];
    param_1[0x10] = param_2[0x10];
    param_2[0x10] = uVar3;
    uVar3 = param_1[0x11];
    param_1[0x11] = param_2[0x11];
    param_2[0x11] = uVar3;
  }
  FUN_140329e20(param_1 + 0x12,param_2 + 0x12);
  return param_1;
}

