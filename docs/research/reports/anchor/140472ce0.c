
undefined8 * FUN_140472ce0(undefined8 *param_1,longlong param_2)

{
  undefined8 *puVar1;
  
  if (param_2 != 0) {
    puVar1 = (undefined8 *)((longlong)param_1 + 0xc4);
    do {
      memset(param_1 + 1,0,0xf8);
      *param_1 = 0;
      *(undefined8 *)((longlong)puVar1 + -0xbc) = 0;
      *(undefined8 *)((longlong)puVar1 + -0xb4) = 0;
      *(undefined2 *)((longlong)puVar1 + -0xac) = 0;
      *(undefined8 *)((longlong)puVar1 + -0xa4) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x9c) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x94) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x8c) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x84) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x7c) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x74) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x6c) = 0;
      *(undefined8 *)((longlong)puVar1 + -100) = 0;
      *(undefined1 **)((longlong)puVar1 + -0x5c) = &DAT_140a6d430;
      *(undefined8 *)((longlong)puVar1 + -0x54) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x4c) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x44) = 0;
      *(undefined8 *)((longlong)puVar1 + -0x2c) = 0;
      *(undefined4 *)((longlong)puVar1 + -4) = 0x3f800000;
      *puVar1 = 0x40000000;
      *(undefined8 *)((longlong)puVar1 + -0x14) = 1;
      *(undefined8 **)((longlong)puVar1 + -0x1c) = &DAT_140b5abc0;
      *(undefined8 *)((longlong)puVar1 + -0xc) = 0;
      *(undefined8 *)((longlong)puVar1 + 0xc) = 0;
      *(undefined8 *)((longlong)puVar1 + 0x14) = 0;
      *(undefined8 *)((longlong)puVar1 + 0x1c) = 0;
      *(undefined1 *)((longlong)puVar1 + 0x24) = 0;
      *(undefined8 *)((longlong)puVar1 + 0x2c) = 0;
      *(undefined8 *)((longlong)puVar1 + 0x34) = 0;
      param_1 = param_1 + 0x20;
      puVar1 = puVar1 + 0x20;
      param_2 = param_2 + -1;
    } while (param_2 != 0);
  }
  return param_1;
}

