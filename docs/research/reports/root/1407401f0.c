// Candidate VA 1407401f0; RVA 0x7401f0
// Ghidra inferred prototype: undefined FUN_1407401f0()

void FUN_1407401f0(longlong param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  undefined8 *local_158;
  undefined8 *puStack_150;
  undefined8 local_148;
  undefined8 local_138;
  undefined8 uStack_130;
  undefined8 local_128;
  undefined8 uStack_120;
  undefined8 local_118;
  undefined8 uStack_110;
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  undefined8 local_d8;
  undefined8 uStack_d0;
  undefined4 local_c8;
  undefined4 uStack_c4;
  undefined4 uStack_c0;
  undefined4 uStack_bc;
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined4 local_98;
  undefined4 uStack_94;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  local_158 = (undefined8 *)0x0;
  puStack_150 = (undefined8 *)0x0;
  local_148 = 0;
  if ((*(int *)(param_1 + 0x2d8) == 0) || (1000 < *(int *)(param_1 + 0x2d8))) {
    local_138 = 0;
    uStack_110 = 0;
    local_d8 = 0;
    local_b8 = 0;
    uStack_130 = 0;
    local_128 = 0;
    uStack_120 = 0;
    local_118 = 0;
    uStack_d0 = 0;
    uStack_b0 = 0;
    local_a8 = 0;
    uStack_a0 = 0;
    local_98 = 0;
    uStack_94 = 0;
    uStack_90 = 0;
    uStack_8c = 0;
    local_88 = 0;
    uStack_80 = 0;
    local_78 = 0;
    uStack_70 = 0;
    local_68 = 0;
    uStack_60 = 0;
    local_58 = 0;
    uStack_50 = 0;
    local_48 = 0;
    uStack_40 = 0;
    local_38 = 0;
    uStack_30 = 0;
    local_28 = 0;
    uStack_24 = 0;
    uStack_20 = 0;
    uStack_1c = 0;
    puVar3 = &local_a8;
  }
  else {
    puVar3 = (undefined8 *)FUN_14024ac30(param_1 + 0x208,&local_a8);
    local_138 = *puVar3;
    uStack_130 = puVar3[1];
    local_128 = puVar3[2];
    uStack_120 = puVar3[3];
    local_118 = puVar3[4];
    uStack_110 = puVar3[5];
    local_108 = puVar3[6];
    uStack_100 = puVar3[7];
    local_f8 = puVar3[8];
    uStack_f0 = puVar3[9];
    local_e8 = puVar3[10];
    uStack_e0 = puVar3[0xb];
    local_d8 = puVar3[0xc];
    uStack_d0 = puVar3[0xd];
    local_c8 = *(undefined4 *)(puVar3 + 0xe);
    uStack_c4 = *(undefined4 *)((longlong)puVar3 + 0x74);
    uStack_c0 = *(undefined4 *)(puVar3 + 0xf);
    uStack_bc = *(undefined4 *)((longlong)puVar3 + 0x7c);
    local_b8 = puVar3[0x10];
    uStack_b0 = puVar3[0x11];
    puVar3 = &local_138;
  }
  FUN_140347e20(param_1 + 0x540,puVar3,param_2,&local_158);
  puVar2 = puStack_150;
  puVar3 = local_158;
  if (local_158 != puStack_150) {
    puVar4 = local_158 + 4;
    do {
      local_138 = puVar4[-4];
      uStack_130 = puVar4[-3];
      local_128 = CONCAT44(local_128._4_4_,*(undefined4 *)(puVar4 + -2));
      uStack_110 = puVar4[1];
      puVar4[1] = 0;
      local_118 = *puVar4;
      *puVar4 = 0;
      uStack_120 = puVar4[-1];
      puVar4[-1] = 0;
      local_f8 = puVar4[4];
      puVar4[4] = 0;
      uStack_100 = puVar4[3];
      puVar4[3] = 0;
      local_108 = puVar4[2];
      puVar4[2] = 0;
      FUN_14073df80(param_1 + 0x408,&local_138);
      puVar1 = puVar4 + 5;
      puVar4 = puVar4 + 9;
    } while (puVar1 != puVar2);
  }
  if (puVar3 != puVar2) {
    puVar4 = puVar3 + 3;
    do {
      FUN_140015c60(puVar4 + 3);
      FUN_140015e70(puVar4);
      puVar1 = puVar4 + 6;
      puVar4 = puVar4 + 9;
    } while (puVar1 != puVar2);
  }
  if (puVar3 != (undefined8 *)0x0) {
    free(puVar3);
  }
  return;
}


// Incoming references
// 0xc371fc DATA caller none
// 0x727d6c UNCONDITIONAL_CALL caller 1407272d0
