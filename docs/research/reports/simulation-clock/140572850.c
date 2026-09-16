// Candidate VA 140572850; RVA 0x572850
// Ghidra inferred prototype: undefined FUN_140572850()

undefined8 FUN_140572850(undefined8 param_1,longlong param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  longlong lVar5;
  longlong lVar6;
  int iVar7;
  undefined4 local_1a8;
  undefined4 uStack_1a4;
  undefined4 uStack_1a0;
  undefined4 uStack_19c;
  undefined4 local_188;
  undefined4 local_184;
  undefined4 local_180;
  undefined4 local_17c;
  undefined8 local_178;
  undefined8 *puStack_170;
  longlong local_168;
  longlong lStack_160;
  char **local_150;
  undefined8 local_148;
  undefined8 local_140;
  undefined8 local_138;
  longlong local_130;
  undefined8 uStack_128;
  undefined8 local_120;
  undefined8 local_118;
  undefined8 local_110;
  undefined8 uStack_108;
  undefined8 uStack_f8;
  undefined4 local_f0;
  undefined4 uStack_ec;
  undefined4 uStack_e8;
  undefined4 uStack_e4;
  undefined4 local_e0;
  undefined4 uStack_dc;
  undefined4 uStack_d8;
  undefined4 uStack_d4;
  char *local_d0;
  undefined4 local_c8;
  undefined4 local_c4;
  undefined1 *local_c0;
  undefined4 local_b8;
  undefined4 local_b4;
  char *local_b0;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined1 *local_a0;
  undefined4 local_98;
  undefined4 local_94;
  char *local_90;
  undefined4 local_88;
  undefined4 local_84;
  undefined *local_80;
  undefined4 local_78;
  undefined4 local_74;
  undefined *local_70;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_58;
  undefined8 uStack_50;
  undefined4 local_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;

  iVar7 = (int)(param_2 / 86400000000);
  if (((longlong)iVar7 * 86400000000 - param_2 != 0) && (param_2 <= (longlong)iVar7 * 86400000000))
  {
    iVar7 = iVar7 + -1;
  }
  FUN_140483380(&local_1a8,param_2 + (longlong)iVar7 * -86400000000);
  FUN_140499a40(&local_188,iVar7);
  uVar2 = uStack_1a0;
  uVar1 = uStack_1a4;
  uVar3 = FUN_140499830(local_184);
  uVar4 = FUN_140499970(local_188);
  lVar5 = FUN_1402d82e0("full_date_time",
                        "{weekday} {day} {month}, {year} / {hours:02d}:{mins:02d}:{secs:02d}");
  lVar6 = -1;
  do {
    lVar6 = lVar6 + 1;
  } while (*(char *)(lVar5 + lVar6) != '\0');
  local_168 = CONCAT44(local_168._4_4_,local_17c);
  local_178 = CONCAT44(local_178._4_4_,local_180);
  local_110 = local_178;
  local_150 = &local_d0;
  local_d0 = "weekday";
  local_c4 = uStack_19c;
  local_c0 = &DAT_140a49b98;
  local_b4 = uStack_19c;
  local_b0 = "month";
  local_a4 = uStack_19c;
  local_a0 = &DAT_140a49ba4;
  local_94 = uStack_19c;
  local_90 = "hours";
  local_84 = uStack_19c;
  local_80 = &DAT_140a74d54;
  local_74 = uStack_19c;
  local_70 = &DAT_140a74d5c;
  local_130 = local_168;
  uStack_128 = lStack_160;
  local_64 = uStack_19c;
  local_178 = 0x4000000001111c2c;
  uStack_108 = puStack_170;
  local_58 = local_1a8;
  local_48 = uVar1;
  local_1a8 = uVar2;
  uStack_f8 = uStack_50;
  local_148 = 7;
  local_f0 = uVar1;
  uStack_ec = uStack_44;
  uStack_e8 = uStack_40;
  uStack_e4 = uStack_3c;
  local_c8 = 0;
  local_e0 = uVar2;
  uStack_dc = uStack_1a4;
  uStack_d8 = uStack_1a0;
  uStack_d4 = uStack_19c;
  local_b8 = 1;
  local_a8 = 2;
  local_98 = 3;
  local_88 = 4;
  local_78 = 5;
  local_68 = 6;
  puStack_170 = &local_140;
  local_168 = lVar5;
  lStack_160 = lVar6;
  local_140 = uVar4;
  local_138 = param_1;
  local_120 = uVar3;
  local_118 = param_1;
  FUN_140021e90(param_1,&local_168,&local_178);
  return param_1;
}


// Incoming references
// 0xc2e220 DATA caller none
// 0x577aed UNCONDITIONAL_CALL caller 140577980
// 0x594d0c UNCONDITIONAL_CALL caller 1405941c0
// 0x5fd48b UNCONDITIONAL_CALL caller 1405fca20
// 0x5ffc4f UNCONDITIONAL_CALL caller 1405ff1e0
// 0x656477 UNCONDITIONAL_CALL caller 140655030
// 0x6816b5 UNCONDITIONAL_CALL caller 140680c70
// 0x6bb28a UNCONDITIONAL_CALL caller 1406badf0
// 0x6c238b UNCONDITIONAL_CALL caller 1406c2040
