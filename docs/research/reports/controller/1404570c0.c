
void FUN_1404570c0(longlong param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
                  undefined8 *param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  longlong lVar8;
  longlong lVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined8 uVar12;
  undefined4 local_138;
  undefined4 uStack_134;
  undefined8 uStack_130;
  undefined8 uStack_128;
  undefined8 uStack_120;
  undefined8 uStack_118;
  uint local_110;
  undefined4 local_10c;
  uint local_108;
  undefined4 uStack_104;
  uint uStack_100;
  undefined4 uStack_fc;
  undefined4 uStack_f8;
  uint local_f4;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined8 uStack_e0;
  undefined8 local_d8;
  undefined2 uStack_d0;
  undefined4 uStack_ce;
  undefined2 uStack_ca;
  undefined8 local_c8;
  longlong local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  ulonglong uStack_80;
  longlong local_78;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  undefined8 uStack_20;
  undefined8 local_18;
  undefined8 uStack_10;

  if (*(char *)(param_3 + 0x1d0) != '\0') {
    lVar9 = 0;
    if ((((*(char *)(param_3 + 0x5f0) == '\0') || (*(longlong *)(param_3 + 0x5d8) == 0)) ||
        (lVar8 = FUN_14033f710(*(longlong *)(param_1 + 0x890) + 0x280), lVar8 == 0)) ||
       (local_b8 = FUN_1403e44d0(lVar8,*(undefined8 *)(param_3 + 0x5e0)), local_b8 == 0)) {
      local_b8 = lVar9;
    }
    local_10c = 0xffffffff;
    local_108 = 0xffffffff;
    uStack_130 = CONCAT35(uStack_130._5_3_,0x100000000);
    uStack_128 = 0;
    uStack_120 = 0;
    uStack_118 = 0xffffffff00000000;
    uStack_fc = 0;
    uStack_f8 = 0;
    local_f4 = local_f4 & 0xffffff00;
    if (*(char *)(param_3 + 0x5d0) != '\0') {
      uStack_130 = *(undefined8 *)(param_3 + 0x5a0);
      uStack_128 = *(undefined8 *)(param_3 + 0x5a8);
      local_108 = *(uint *)(param_3 + 0x5c8);
      uStack_120 = *(undefined8 *)(param_3 + 0x5b0);
      uStack_118 = *(undefined8 *)(param_3 + 0x5b8);
      if (*(char *)(param_3 + 0x5f0) != '\0') {
        local_10c = *(undefined4 *)(param_3 + 0x5e8);
      }
    }
    uVar7 = local_110 >> 8;
    local_110 = local_110 & 0xffffff00;
    if ((local_b8 == 0) || (*(longlong *)(local_b8 + 0x50) == *(longlong *)(local_b8 + 0x58))) {
      local_110 = CONCAT31((int3)uVar7,1);
    }
    if (*(char *)(param_3 + 0x4b0) == '\0') {
      uVar12 = 0;
      uVar1 = 0x101;
      uVar2 = 0;
      uVar10 = 0;
      uVar11 = 0;
      uVar3 = 0xffffffff;
      uVar4 = 0;
      uStack_e0._0_4_ = 0;
      uStack_e0._4_4_ = 0;
      uStack_d0 = 0x101;
      uStack_ce = 0;
      uStack_ca = 0;
      local_c8 = 0;
      uVar5 = (undefined4)uStack_e0;
      uVar6 = uStack_e0._4_4_;
    }
    else {
      uVar3 = *(undefined4 *)(param_3 + 0x3a0);
      uVar4 = *(undefined4 *)(param_3 + 0x3a4);
      uVar10 = *(undefined4 *)(param_3 + 0x3b0);
      uVar11 = *(undefined4 *)(param_3 + 0x3b4);
      uVar1 = *(undefined4 *)(param_3 + 0x3b8);
      uVar2 = *(undefined4 *)(param_3 + 0x3bc);
      uVar12 = *(undefined8 *)(param_3 + 0x3c0);
      lVar9 = param_3 + 0x290;
      uVar5 = *(undefined4 *)(param_3 + 0x3a8);
      uVar6 = *(undefined4 *)(param_3 + 0x3ac);
    }
    local_68 = *param_5;
    uStack_60 = param_5[1];
    local_58 = param_5[2];
    uStack_50 = param_5[3];
    local_48 = param_5[4];
    uStack_40 = param_5[5];
    local_28 = param_5[8];
    uStack_20 = param_5[9];
    local_38 = param_5[6];
    uStack_30 = param_5[7];
    local_18 = param_5[10];
    uStack_10 = param_5[0xb];
    uStack_b0 = uStack_130;
    uStack_90 = CONCAT44(local_10c,local_110);
    local_98 = uStack_118;
    local_78 = (ulonglong)local_f4 << 0x20;
    local_a8 = uStack_128;
    uStack_a0 = uStack_120;
    local_88 = CONCAT44(local_10c,local_108);
    uStack_80 = (ulonglong)local_108;
    local_e8 = *(undefined4 *)(param_3 + 0xb8);
    uStack_e4 = *(undefined4 *)(param_3 + 0xbc);
    uStack_e0 = *(undefined8 *)(param_3 + 0xc0);
    local_d8 = *(undefined8 *)(param_3 + 200);
    _local_138 = CONCAT44(uVar4,uVar3);
    uStack_130 = CONCAT44(uVar6,uVar5);
    uStack_128 = CONCAT44(uVar11,uVar10);
    uStack_120 = CONCAT44(uVar2,uVar1);
    uStack_118 = uVar12;
    uStack_104 = local_10c;
    uStack_100 = local_108;
    FUN_14045b3d0(param_1,&local_e8,lVar9,&local_138,&local_b8,param_2,param_3 + 8);
  }
  return;
}
