
void FUN_140458750(longlong param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
                  undefined8 *param_5)

{
  uint uVar1;
  longlong lVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined8 uVar9;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined8 uStack_e0;
  undefined8 uStack_d8;
  undefined8 uStack_d0;
  undefined8 uStack_c8;
  uint local_c0;
  undefined4 local_bc;
  uint local_b8;
  undefined4 uStack_b4;
  uint uStack_b0;
  undefined4 uStack_ac;
  undefined4 uStack_a8;
  uint uStack_a4;
  undefined4 local_98;
  undefined4 uStack_94;
  undefined8 uStack_90;
  undefined8 local_88;
  undefined2 uStack_80;
  undefined4 uStack_7e;
  undefined2 uStack_7a;
  undefined8 local_78;
  undefined8 local_68;
  undefined8 uStack_60;
  longlong local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  ulonglong uStack_20;
  longlong local_18;

  if (*(char *)(param_3 + 0x1d0) != '\0') {
    if ((((*(char *)(param_3 + 0x5f0) == '\0') || (*(longlong *)(param_3 + 0x5d8) == 0)) ||
        (lVar2 = FUN_14033f710(*(longlong *)(param_1 + 0x890) + 0x280), lVar2 == 0)) ||
       (local_58 = FUN_1403e44d0(lVar2,*(undefined8 *)(param_3 + 0x5e0)), local_58 == 0)) {
      local_58 = 0;
    }
    local_bc = 0xffffffff;
    local_b8 = 0xffffffff;
    uStack_e0 = CONCAT35(uStack_e0._5_3_,0x100000000);
    uStack_d8 = 0;
    uStack_d0 = 0;
    uStack_c8 = 0xffffffff00000000;
    uStack_ac = 0;
    uStack_a8 = 0;
    uStack_a4 = uStack_a4 & 0xffffff00;
    if (*(char *)(param_3 + 0x5d0) != '\0') {
      uStack_e0 = *(undefined8 *)(param_3 + 0x5a0);
      uStack_d8 = *(undefined8 *)(param_3 + 0x5a8);
      local_b8 = *(uint *)(param_3 + 0x5c8);
      uStack_d0 = *(undefined8 *)(param_3 + 0x5b0);
      uStack_c8 = *(undefined8 *)(param_3 + 0x5b8);
      if (*(char *)(param_3 + 0x5f0) != '\0') {
        local_bc = *(undefined4 *)(param_3 + 0x5e8);
      }
    }
    uVar1 = local_c0 >> 8;
    local_c0 = local_c0 & 0xffffff00;
    if ((local_58 == 0) || (*(longlong *)(local_58 + 0x50) == *(longlong *)(local_58 + 0x58))) {
      local_c0 = CONCAT31((int3)uVar1,1);
    }
    if (*(char *)(param_3 + 0x4b0) == '\0') {
      uVar9 = 0;
      uVar5 = 0x101;
      uVar6 = 0;
      uVar3 = 0;
      uVar4 = 0;
      uVar7 = 0xffffffff;
      uVar8 = 0;
      uStack_90._0_4_ = 0;
      uStack_90._4_4_ = 0;
      uStack_80 = 0x101;
      uStack_7e = 0;
      uStack_7a = 0;
      local_78 = 0;
    }
    else {
      uVar7 = *(undefined4 *)(param_3 + 0x3a0);
      uVar8 = *(undefined4 *)(param_3 + 0x3a4);
      uStack_90._0_4_ = *(undefined4 *)(param_3 + 0x3a8);
      uStack_90._4_4_ = *(undefined4 *)(param_3 + 0x3ac);
      uVar3 = *(undefined4 *)(param_3 + 0x3b0);
      uVar4 = *(undefined4 *)(param_3 + 0x3b4);
      uVar5 = *(undefined4 *)(param_3 + 0x3b8);
      uVar6 = *(undefined4 *)(param_3 + 0x3bc);
      uVar9 = *(undefined8 *)(param_3 + 0x3c0);
    }
    uStack_50 = uStack_e0;
    uStack_30 = CONCAT44(local_bc,local_c0);
    lVar2 = param_3 + 0x290;
    if (*(char *)(param_3 + 0x4b0) == '\0') {
      lVar2 = 0;
    }
    local_68 = *param_5;
    uStack_60 = param_5[1];
    local_48 = uStack_d8;
    uStack_40 = uStack_d0;
    local_28 = CONCAT44(local_bc,local_b8);
    uStack_20 = (ulonglong)local_b8;
    local_38 = uStack_c8;
    local_18 = (ulonglong)uStack_a4 << 0x20;
    local_98 = *(undefined4 *)(param_3 + 0xb8);
    uStack_94 = *(undefined4 *)(param_3 + 0xbc);
    local_88 = *(undefined8 *)(param_3 + 200);
    _local_e8 = CONCAT44(uVar8,uVar7);
    uStack_e0 = CONCAT44(uStack_90._4_4_,(undefined4)uStack_90);
    uStack_d8 = CONCAT44(uVar4,uVar3);
    uStack_d0 = CONCAT44(uVar6,uVar5);
    uStack_c8 = uVar9;
    uStack_b4 = local_bc;
    uStack_b0 = local_b8;
    uStack_90 = *(undefined8 *)(param_3 + 0xc0);
    FUN_14045dd80(param_1,&local_98,lVar2,&local_e8,&local_58,param_2,param_3 + 8);
    return;
  }
  return;
}
