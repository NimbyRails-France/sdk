
undefined8 * FUN_14044f560(undefined8 *param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  
  *param_1 = 0;
  param_1[1] = 0;
  *(undefined4 *)(param_1 + 2) = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  FUN_140461cc0();
  puVar1 = (undefined8 *)param_1[4];
  for (puVar2 = (undefined8 *)param_1[3]; puVar2 != puVar1; puVar2 = puVar2 + 1) {
    if ((void *)*puVar2 != (void *)0x0) {
      free((void *)*puVar2);
    }
  }
  param_1[4] = param_1[3];
  FUN_140461cc0(param_1);
  puVar1 = param_1 + 6;
  *puVar1 = 0;
  param_1[7] = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  FUN_140461c60(puVar1);
  puVar2 = (undefined8 *)param_1[10];
  for (puVar3 = (undefined8 *)param_1[9]; puVar3 != puVar2; puVar3 = puVar3 + 1) {
    if ((void *)*puVar3 != (void *)0x0) {
      free((void *)*puVar3);
    }
  }
  param_1[10] = param_1[9];
  FUN_140461c60(puVar1);
  *(undefined4 *)(param_1 + 0x10) = 0x3f800000;
  *(undefined8 *)((longlong)param_1 + 0x84) = 0x40000000;
  param_1[0xe] = 1;
  param_1[0xd] = &DAT_140b5abc0;
  param_1[0xf] = 0;
  param_1[0x12] = 0;
  FUN_14045a550(param_1 + 0x13);
  param_1[0x2b] = 0;
  param_1[0x2c] = 0;
  param_1[0x2d] = 0;
  param_1[0x2e] = 0;
  param_1[0x2f] = 0;
  param_1[0x30] = 0;
  param_1[0x31] = 0;
  *(undefined4 *)(param_1 + 0x36) = 0x3f800000;
  *(undefined8 *)((longlong)param_1 + 0x1b4) = 0x40000000;
  param_1[0x34] = 1;
  param_1[0x33] = &DAT_140b5abc0;
  param_1[0x35] = 0;
  param_1[0x38] = 0;
  param_1[0x3d] = 0;
  param_1[0x3e] = 0;
  param_1[0x3f] = 0;
  param_1[0x40] = 0;
  param_1[0x41] = 0;
  param_1[0x42] = 0;
  param_1[0x3b] = 0;
  param_1[0x3c] = 0;
  *(undefined4 *)(param_1 + 0x43) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x3a) = 2;
  *(undefined4 *)((longlong)param_1 + 0x21c) = 0;
  FUN_140459950(param_1 + 0x2c,0);
  param_1[0x44] = 0;
  FUN_140350c30(param_1 + 0x45,0);
  param_1[0x5d] = 0;
  param_1[0x5e] = 0;
  FUN_14034ae50(param_1 + 0x5f);
  _eh_vector_constructor_iterator_
            (param_1 + 0x8f,0x88,0x10,(_func_void_void_ptr *)&LAB_1404624b0,
             (_func_void_void_ptr *)&LAB_140351250);
  _eh_vector_constructor_iterator_
            (param_1 + 0x19f,0x88,0x10,(_func_void_void_ptr *)&LAB_1404624b0,
             (_func_void_void_ptr *)&LAB_140351240);
  param_1[0x2af] = &DAT_140a6d430;
  param_1[0x2b0] = 0;
  param_1[0x2b1] = 0;
  param_1[0x2b2] = 0;
  param_1[0x2b5] = 0;
  *(undefined1 *)(param_1 + 0x2b6) = 1;
  return param_1;
}

