
undefined8 * FUN_140450030(undefined8 param_1,undefined8 *param_2,longlong param_3,longlong param_4)

{
  undefined8 uVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 *puVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  char local_38 [16];
  longlong local_28;

  *(undefined2 *)(param_2 + 2) = 0x101;
  *param_2 = 0;
  param_2[1] = 0;
  param_2[3] = 0;
  param_2[4] = 0;
  *(undefined2 *)(param_2 + 5) = 0x101;
  if (param_4 == 0) {
    return param_2;
  }
  lVar2 = FUN_14033f780(*(longlong *)(param_3 + 0x490) + 0x200,param_4);
  lVar3 = FUN_14045f300(param_1,param_4);
  if (lVar2 == 0) {
    return param_2;
  }
  if (lVar3 == 0) {
    return param_2;
  }
  if (*(char *)(lVar3 + 0x1d0) == '\0') {
    return param_2;
  }
  uVar1 = *(undefined8 *)(lVar3 + 0xc0);
  *param_2 = *(undefined8 *)(lVar3 + 0xb8);
  param_2[1] = uVar1;
  param_2[2] = *(undefined8 *)(lVar3 + 200);
  local_58 = *(undefined8 *)(lVar3 + 0xb8);
  uStack_50 = *(undefined8 *)(lVar3 + 0xc0);
  local_48 = *(undefined8 *)(lVar3 + 200);
  FUN_14044ba00(SUB84((double)*(float *)(lVar3 + 0x40),0),local_38,param_3,&local_58,lVar3 + 0xe0,
                (double)*(float *)(lVar3 + 0x40));
  if (local_28 == 0) {
    local_58 = 0;
    uVar5 = 0;
    uVar6 = 0;
    local_48 = CONCAT62(local_48._2_6_,0x101);
  }
  else {
    puVar4 = (undefined8 *)FUN_14032c420(*(undefined8 *)(param_3 + 0x28));
    local_58 = 0;
    uStack_50 = 0;
    local_48 = CONCAT62(local_48._2_6_,0x101);
    if (puVar4 == (undefined8 *)0x0) goto LAB_14045017e;
    local_58 = *puVar4;
    uVar5 = (undefined4)local_38._8_8_;
    uVar6 = SUB84(local_38._8_8_,4);
    local_48 = CONCAT71(CONCAT61(local_48._2_6_,*(undefined1 *)((longlong)puVar4 + 0x2c)),
                        -local_38[0]);
  }
  uStack_50 = CONCAT44(uVar6,uVar5);
LAB_14045017e:
  param_2[3] = local_58;
  param_2[4] = uStack_50;
  param_2[5] = local_48;
  return param_2;
}
