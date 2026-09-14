
undefined8 * FUN_1403ee610(undefined8 *param_1)

{
  undefined8 *puVar1;
  
  puVar1 = param_1 + 3;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  *(undefined4 *)((longlong)param_1 + 0x31) = 0;
  *(undefined2 *)((longlong)param_1 + 0x35) = 0;
  *(undefined1 *)((longlong)param_1 + 0x37) = 0;
  *puVar1 = puVar1;
  param_1[4] = puVar1;
  param_1[5] = 0;
  *(undefined1 *)(param_1 + 6) = 0;
  param_1[7] = 0;
  param_1[0xf] = &DAT_140a6d430;
  puVar1 = param_1 + 0x1a;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x12] = 0;
  param_1[0x15] = 0;
  *(undefined4 *)(param_1 + 0x16) = 0;
  param_1[0x17] = 0;
  param_1[0x18] = 0;
  param_1[0x19] = 0;
  *(undefined4 *)((longlong)param_1 + 0xe9) = 0;
  *(undefined2 *)((longlong)param_1 + 0xed) = 0;
  *(undefined1 *)((longlong)param_1 + 0xef) = 0;
  *puVar1 = puVar1;
  param_1[0x1b] = puVar1;
  param_1[0x1c] = 0;
  *(undefined1 *)(param_1 + 0x1d) = 0;
  param_1[0x1e] = 0;
  return param_1;
}

