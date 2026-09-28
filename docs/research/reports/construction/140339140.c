
undefined8 * FUN_140339140(undefined8 *param_1,undefined8 *param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  
  *param_1 = *param_2;
  puVar5 = param_2 + 2;
  *(undefined1 *)(param_1 + 1) = *(undefined1 *)(param_2 + 1);
  if (param_1 + 2 != puVar5) {
    if (0xf < (ulonglong)param_2[5]) {
      puVar5 = (undefined8 *)*puVar5;
    }
    FUN_140030630(param_1 + 2,puVar5,param_2[4]);
  }
  *(undefined4 *)(param_1 + 6) = *(undefined4 *)(param_2 + 6);
  param_1[7] = param_2[7];
  uVar1 = *(undefined4 *)((longlong)param_2 + 0x44);
  uVar2 = *(undefined4 *)(param_2 + 9);
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x4c);
  *(undefined4 *)(param_1 + 8) = *(undefined4 *)(param_2 + 8);
  *(undefined4 *)((longlong)param_1 + 0x44) = uVar1;
  *(undefined4 *)(param_1 + 9) = uVar2;
  *(undefined4 *)((longlong)param_1 + 0x4c) = uVar3;
  param_1[10] = param_2[10];
  *(undefined4 *)(param_1 + 0xb) = *(undefined4 *)(param_2 + 0xb);
  *(undefined4 *)((longlong)param_1 + 0x5c) = *(undefined4 *)((longlong)param_2 + 0x5c);
  *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)(param_2 + 0xc);
  *(undefined1 *)((longlong)param_1 + 100) = *(undefined1 *)((longlong)param_2 + 100);
  *(undefined4 *)(param_1 + 0xd) = *(undefined4 *)(param_2 + 0xd);
  *(undefined1 *)((longlong)param_1 + 0x6c) = *(undefined1 *)((longlong)param_2 + 0x6c);
  *(undefined1 *)((longlong)param_1 + 0x6d) = *(undefined1 *)((longlong)param_2 + 0x6d);
  *(undefined4 *)(param_1 + 0xe) = *(undefined4 *)(param_2 + 0xe);
  if (param_1 + 0xf != param_2 + 0xf) {
    FUN_14032fe20(param_1 + 0xf,param_2[0xf],param_2[0x10]);
  }
  if (param_1 + 0x12 != param_2 + 0x12) {
    FUN_1403416a0(param_1 + 0x12,param_2[0x12],param_2[0x13]);
  }
  uVar4 = param_2[0x16];
  param_1[0x15] = param_2[0x15];
  param_1[0x16] = uVar4;
  uVar4 = param_2[0x18];
  param_1[0x17] = param_2[0x17];
  param_1[0x18] = uVar4;
  return param_1;
}

