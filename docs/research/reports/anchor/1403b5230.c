
undefined8 * FUN_1403b5230(undefined8 *param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  
  *param_1 = param_2;
  puVar1 = param_1 + 1;
  *puVar1 = 0;
  param_1[2] = 0;
  *(undefined4 *)(param_1 + 3) = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  FUN_1403cf010(puVar1);
  puVar2 = (undefined8 *)param_1[5];
  for (puVar3 = (undefined8 *)param_1[4]; puVar3 != puVar2; puVar3 = puVar3 + 1) {
    if ((void *)*puVar3 != (void *)0x0) {
      free((void *)*puVar3);
    }
  }
  param_1[5] = param_1[4];
  FUN_1403cf010(puVar1);
  *(undefined4 *)(param_1 + 0xb) = 0x3f800000;
  *(undefined8 *)((longlong)param_1 + 0x5c) = 0x40000000;
  param_1[9] = 1;
  param_1[8] = &DAT_140b5abc0;
  param_1[10] = 0;
  FUN_1403b5440(param_1 + 0xd);
  FUN_1403b5440(param_1 + 0x1d);
  FUN_1403b5440(param_1 + 0x2d);
  puVar1 = param_1 + 0x3d;
  *(undefined4 *)((longlong)param_1 + 0x201) = 0;
  *(undefined2 *)((longlong)param_1 + 0x205) = 0;
  *(undefined1 *)((longlong)param_1 + 0x207) = 0;
  *puVar1 = puVar1;
  param_1[0x3e] = puVar1;
  param_1[0x3f] = 0;
  *(undefined1 *)(param_1 + 0x40) = 0;
  param_1[0x41] = 0;
  param_1[0x43] = &DAT_140a6d430;
  param_1[0x44] = 0;
  param_1[0x45] = 0;
  param_1[0x46] = 0;
  param_1[0x49] = 0;
  puVar1 = param_1 + 0x4a;
  *(undefined4 *)((longlong)param_1 + 0x269) = 0;
  *(undefined2 *)((longlong)param_1 + 0x26d) = 0;
  *(undefined1 *)((longlong)param_1 + 0x26f) = 0;
  *puVar1 = puVar1;
  param_1[0x4b] = puVar1;
  param_1[0x4c] = 0;
  *(undefined1 *)(param_1 + 0x4d) = 0;
  param_1[0x4e] = 0;
  puVar1 = param_1 + 0x50;
  *(undefined4 *)((longlong)param_1 + 0x299) = 0;
  *(undefined2 *)((longlong)param_1 + 0x29d) = 0;
  *(undefined1 *)((longlong)param_1 + 0x29f) = 0;
  *puVar1 = puVar1;
  param_1[0x51] = puVar1;
  param_1[0x52] = 0;
  *(undefined1 *)(param_1 + 0x53) = 0;
  param_1[0x54] = 0;
  puVar1 = param_1 + 0x56;
  *(undefined4 *)((longlong)param_1 + 0x2c9) = 0;
  *(undefined2 *)((longlong)param_1 + 0x2cd) = 0;
  *(undefined1 *)((longlong)param_1 + 0x2cf) = 0;
  *puVar1 = puVar1;
  param_1[0x57] = puVar1;
  param_1[0x58] = 0;
  *(undefined1 *)(param_1 + 0x59) = 0;
  param_1[0x5a] = 0;
  puVar1 = param_1 + 0x5c;
  *(undefined4 *)((longlong)param_1 + 0x2f9) = 0;
  *(undefined2 *)((longlong)param_1 + 0x2fd) = 0;
  *(undefined1 *)((longlong)param_1 + 0x2ff) = 0;
  *puVar1 = puVar1;
  param_1[0x5d] = puVar1;
  param_1[0x5e] = 0;
  *(undefined1 *)(param_1 + 0x5f) = 0;
  param_1[0x60] = 0;
  param_1[0x62] = 0;
  param_1[99] = 0;
  param_1[100] = 0;
  *(undefined4 *)(param_1 + 0x69) = 0x3f800000;
  *(undefined8 *)((longlong)param_1 + 0x34c) = 0x40000000;
  param_1[0x67] = 1;
  param_1[0x66] = &DAT_140b5abc0;
  param_1[0x68] = 0;
  return param_1;
}

