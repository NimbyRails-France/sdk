
undefined8 * FUN_14032dc30(undefined8 *param_1)

{
  undefined8 *puVar1;
  longlong lVar2;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0xf;
  *(undefined1 *)(param_1 + 2) = 0;
  *(undefined1 *)(param_1 + 6) = 1;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = 0;
  param_1[0x10] = &DAT_140a6d430;
  param_1[0x11] = 0;
  param_1[0x12] = 0;
  param_1[0x13] = 0;
  param_1[0x16] = 0;
  *(undefined4 *)(param_1 + 0x17) = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0x3f800000;
  *(undefined8 *)((longlong)param_1 + 0xe4) = 0x40000000;
  param_1[0x1a] = 1;
  param_1[0x19] = &DAT_140b5abc0;
  param_1[0x1b] = 0;
  puVar1 = param_1 + 0x1e;
  lVar2 = 10;
  do {
    *(undefined1 *)puVar1 = 0;
    *(undefined8 *)((longlong)puVar1 + 4) = 0;
    puVar1 = (undefined8 *)((longlong)puVar1 + 0xc);
    lVar2 = lVar2 + -1;
  } while (lVar2 != 0);
  puVar1 = param_1 + 0x2d;
  *(undefined4 *)((longlong)param_1 + 0x181) = 0;
  *(undefined2 *)((longlong)param_1 + 0x185) = 0;
  *(undefined1 *)((longlong)param_1 + 0x187) = 0;
  *puVar1 = puVar1;
  param_1[0x2e] = puVar1;
  param_1[0x2f] = 0;
  *(undefined1 *)(param_1 + 0x30) = 0;
  param_1[0x31] = 0;
  param_1[0x33] = 0;
  param_1[0x34] = 0;
  param_1[0x35] = 0;
  return param_1;
}

