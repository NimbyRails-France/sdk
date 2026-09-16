// Candidate VA 14080bea0; RVA 0x80bea0
// Ghidra inferred prototype: undefined FUN_14080bea0()

void FUN_14080bea0(longlong *param_1,undefined8 param_2)

{
  float *pfVar1;
  longlong lVar2;
  undefined4 uVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fStack0000000000000038;
  float fStack0000000000000040;
  undefined8 in_stack_00000078;
  undefined1 local_180 [32];
  undefined1 local_160 [32];
  undefined8 local_140;
  undefined8 uStack_138;
  undefined8 local_130;
  undefined4 local_128;
  undefined8 local_124;
  undefined8 uStack_11c;
  undefined1 local_114;
  longlong *local_108;
  undefined1 local_100 [32];
  undefined1 local_e0 [32];
  undefined8 local_c0;
  undefined8 uStack_b8;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 uStack_a0;
  undefined4 uStack_9c;
  undefined4 uStack_98;
  undefined1 local_94;
  undefined1 local_88 [16];
  undefined1 *local_78;
  undefined1 *local_70;
  undefined *local_68;
  undefined8 local_60;

  fVar6 = DAT_140aac038;
  fVar4 = DAT_140aabae4;
  fStack0000000000000040 = 0.0;
  fStack0000000000000038 = 800.0;
  fVar7 = *(float *)(*param_1 + 0x1550);
  if (fVar7 < 0.0) {
    fVar7 = DAT_140aabae4;
  }
  pfVar1 = (float *)FUN_14081d9b0(*param_1,local_88,"##trains_listing");
  fVar4 = fVar4 / fVar7;
  fVar7 = fVar6;
  if (0.0 < fVar4 * pfVar1[2]) {
    fVar7 = fVar4 * pfVar1[2] + fVar4 * *pfVar1 + fVar6;
  }
  lVar2 = FUN_14081d9b0(*param_1,local_88,"##game_window_top_bar");
  fVar5 = fVar4 * *(float *)(lVar2 + 0xc);
  if (0.0 < fVar5) {
    fVar6 = fVar5 + fVar4 * *(float *)(lVar2 + 4) + fVar6;
  }
  if (fStack0000000000000038 < 0.0) {
    fStack0000000000000038 = ((float)*(uint *)(param_1 + 0x14) - fVar7) - DAT_140aac254;
  }
  if (fStack0000000000000040 < 0.0) {
    fStack0000000000000040 = ((float)*(uint *)((longlong)param_1 + 0xa4) - fVar6) - DAT_140aac254;
  }
  FUN_140019ca0(local_180,param_2);
  FUN_140019ca0(local_160,param_2);
  local_140 = 0;
  uStack_138 = 0;
  local_130 = 0;
  local_124 = 0;
  uStack_11c = 0;
  local_114 = 1;
  local_128 = 0x40000820;
  local_108 = param_1;
  FUN_140019ca0(local_100,local_180);
  FUN_140019ca0(local_e0,local_160);
  local_c0 = local_140;
  uStack_b8 = uStack_138;
  local_b0 = (undefined4)local_130;
  local_ac = local_130._4_4_;
  local_a8 = local_128;
  local_a4 = (undefined4)local_124;
  uStack_a0 = local_124._4_4_;
  uStack_9c = (undefined4)uStack_11c;
  uStack_98 = uStack_11c._4_4_;
  local_94 = local_114;
  FUN_140002d30(local_160);
  FUN_140002d30(local_180);
  local_c0 = 0;
  uStack_b8 = 0;
  uVar3 = roundf(fVar6);
  local_b0 = roundf(fVar7);
  local_78 = (undefined1 *)&stack0x00000038;
  local_70 = (undefined1 *)&stack0x00000040;
  local_68 = &DAT_140b8e1e0;
  local_60 = in_stack_00000078;
  local_ac = uVar3;
  FUN_14080e3d0(&local_108,local_88,&local_78);
  FUN_140002d30(local_e0);
  FUN_140002d30(local_100);
  return;
}


// Incoming references
// 0xc3b03c DATA caller none
// 0x7fdecf UNCONDITIONAL_CALL caller 1407fd860
