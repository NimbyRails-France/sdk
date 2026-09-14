
undefined8 * FUN_14033c9b0(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined1 local_res8;
  
  param_1[1] = 0;
  *param_1 = 0;
  param_1[2] = 0;
  *param_1 = *param_2;
  *param_2 = 0;
  uVar1 = param_1[1];
  param_1[1] = param_2[1];
  param_2[1] = uVar1;
  uVar1 = param_1[2];
  param_1[2] = param_2[2];
  param_2[2] = uVar1;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[3] = 0;
  param_1[3] = param_2[3];
  param_2[3] = 0;
  uVar1 = param_1[4];
  param_1[4] = param_2[4];
  param_2[4] = uVar1;
  uVar1 = param_1[5];
  param_1[5] = param_2[5];
  uVar2 = param_2[6];
  param_2[5] = uVar1;
  param_2[6] = &DAT_140a6d430;
  param_1[6] = uVar2;
  uVar1 = param_2[7];
  param_2[7] = 0;
  param_1[7] = uVar1;
  uVar1 = param_2[8];
  param_2[8] = 0;
  param_1[8] = uVar1;
  uVar1 = param_2[9];
  param_2[9] = 0;
  param_1[9] = uVar1;
  *(undefined1 *)(param_1 + 10) = local_res8;
  *(undefined1 *)(param_1 + 0xb) = *(undefined1 *)(param_2 + 0xb);
  *(undefined1 *)((longlong)param_1 + 0x59) = *(undefined1 *)((longlong)param_2 + 0x59);
  *(undefined1 *)((longlong)param_1 + 0x5a) = *(undefined1 *)((longlong)param_2 + 0x5a);
  param_1[0xc] = param_2[0xc];
  param_2[0xc] = 0;
  *(undefined4 *)(param_1 + 0xd) = *(undefined4 *)(param_2 + 0xd);
  return param_1;
}

