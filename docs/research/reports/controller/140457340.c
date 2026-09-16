
void FUN_140457340(longlong param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
                  longlong *param_5)

{
  char cVar1;
  longlong *plVar2;
  undefined8 *puVar3;
  uint uVar4;
  longlong lVar5;
  undefined4 uStack_160;
  undefined4 uStack_15c;
  undefined8 uStack_158;
  undefined8 uStack_150;
  undefined8 uStack_148;
  uint local_140;
  undefined4 local_13c;
  uint local_138;
  uint uStack_124;
  undefined4 local_118;
  undefined4 uStack_114;
  undefined8 uStack_110;
  undefined8 local_108;
  undefined2 uStack_100;
  undefined4 uStack_fe;
  undefined2 uStack_fa;
  undefined8 local_f8;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined4 uStack_e0;
  undefined4 uStack_dc;
  undefined4 local_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  undefined4 uStack_cc;
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
  undefined1 local_68 [56];
  undefined8 local_30;

  if (*(char *)(param_3 + 0x1d0) != '\0') {
    cVar1 = *(char *)(param_3 + 0x5f0);
    if ((((cVar1 == '\0') || (*(longlong *)(param_3 + 0x5d8) == 0)) ||
        (lVar5 = FUN_14033f710(*(longlong *)(param_1 + 0x890) + 0x280), lVar5 == 0)) ||
       (local_b8 = FUN_1403e44d0(lVar5,*(undefined8 *)(param_3 + 0x5e0)), local_b8 == 0)) {
      local_b8 = 0;
    }
    uStack_160 = 0;
    uStack_15c = CONCAT31(uStack_15c._1_3_,1);
    uStack_158 = 0;
    uStack_150 = 0;
    uStack_148 = 0xffffffff00000000;
    local_13c = 0xffffffff;
    local_138 = 0xffffffff;
    uStack_124 = uStack_124 & 0xffffff00;
    if (*(char *)(param_3 + 0x5d0) != '\0') {
      uStack_160 = *(undefined4 *)(param_3 + 0x5a0);
      uStack_15c = *(undefined4 *)(param_3 + 0x5a4);
      uStack_158 = *(undefined8 *)(param_3 + 0x5a8);
      uStack_150 = *(undefined8 *)(param_3 + 0x5b0);
      uStack_148 = *(undefined8 *)(param_3 + 0x5b8);
      local_138 = *(uint *)(param_3 + 0x5c8);
      if (cVar1 != '\0') {
        local_13c = *(undefined4 *)(param_3 + 0x5e8);
      }
    }
    uVar4 = local_140 >> 8;
    local_140 = local_140 & 0xffffff00;
    if ((local_b8 == 0) || (*(longlong *)(local_b8 + 0x50) == *(longlong *)(local_b8 + 0x58))) {
      local_140 = CONCAT31((int3)uVar4,1);
    }
    local_30 = 0;
    puVar3 = (undefined8 *)param_5[7];
    if (puVar3 != (undefined8 *)0x0) {
      local_30 = (**(code **)*puVar3)(puVar3,local_68);
    }
    if (*(char *)(param_3 + 0x4b0) == '\0') {
      local_e8 = 0xffffffff;
      uStack_e4 = 0;
      uStack_110._0_4_ = 0;
      uStack_110._4_4_ = 0;
      uStack_d0 = 0x101;
      uStack_cc = 0;
      local_d8 = 0;
      uStack_d4 = 0;
      uStack_100 = 0x101;
      uStack_fe = 0;
      uStack_fa = 0;
      local_f8 = 0;
      local_c8 = 0;
    }
    else {
      local_e8 = *(undefined4 *)(param_3 + 0x3a0);
      uStack_e4 = *(undefined4 *)(param_3 + 0x3a4);
      uStack_110._0_4_ = *(undefined4 *)(param_3 + 0x3a8);
      uStack_110._4_4_ = *(undefined4 *)(param_3 + 0x3ac);
      local_d8 = *(undefined4 *)(param_3 + 0x3b0);
      uStack_d4 = *(undefined4 *)(param_3 + 0x3b4);
      uStack_d0 = *(undefined4 *)(param_3 + 0x3b8);
      uStack_cc = *(undefined4 *)(param_3 + 0x3bc);
      local_c8 = *(undefined8 *)(param_3 + 0x3c0);
    }
    uStack_b0 = CONCAT44(uStack_15c,uStack_160);
    local_a8 = uStack_158;
    uStack_a0 = uStack_150;
    uStack_90 = CONCAT44(local_13c,local_140);
    local_98 = uStack_148;
    local_88 = CONCAT44(local_13c,local_138);
    uStack_80 = (ulonglong)local_138;
    local_78 = (ulonglong)uStack_124 << 0x20;
    local_118 = *(undefined4 *)(param_3 + 0xb8);
    uStack_114 = *(undefined4 *)(param_3 + 0xbc);
    local_108 = *(undefined8 *)(param_3 + 200);
    lVar5 = param_3 + 0x290;
    if (*(char *)(param_3 + 0x4b0) == '\0') {
      lVar5 = 0;
    }
    uStack_dc = uStack_110._4_4_;
    uStack_e0 = (undefined4)uStack_110;
    uStack_110 = *(undefined8 *)(param_3 + 0xc0);
    FUN_14045c0d0(param_1,&local_118,lVar5,&local_e8,&local_b8,param_2,param_3 + 8);
    plVar2 = (longlong *)param_5[7];
    if (plVar2 != (longlong *)0x0) {
      (**(code **)(*plVar2 + 0x20))(plVar2,plVar2 != param_5);
      param_5[7] = 0;
    }
    return;
  }
  plVar2 = (longlong *)param_5[7];
  if (plVar2 == (longlong *)0x0) {
    return;
  }
  (**(code **)(*plVar2 + 0x20))(plVar2,plVar2 != param_5);
  param_5[7] = 0;
  return;
}
