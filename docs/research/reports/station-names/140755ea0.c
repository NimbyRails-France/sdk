// Candidate VA 140755ea0; RVA 0x755ea0
// Ghidra inferred prototype: undefined FUN_140755ea0()

/* WARNING: Type propagation algorithm not settling */

void FUN_140755ea0(longlong *param_1,undefined8 *param_2,longlong param_3,longlong *param_4)

{
  code *pcVar1;
  void *pvVar2;
  undefined1 auVar3 [16];
  undefined1 auVar4 [16];
  undefined1 auVar5 [16];
  undefined1 auVar6 [16];
  undefined4 uVar7;
  undefined1 uVar8;
  char cVar9;
  int iVar10;
  longlong lVar11;
  undefined8 uVar12;
  char *pcVar13;
  longlong lVar14;
  undefined8 *puVar15;
  undefined8 uVar16;
  undefined1 (*pauVar17) [16];
  ulonglong uVar18;
  void *pvVar19;
  longlong *plVar20;
  int *piVar21;
  undefined8 *******pppppppuVar22;
  undefined **_Memory;
  int *piVar23;
  ulonglong uVar24;
  undefined8 *puVar25;
  undefined1 *puVar26;
  undefined1 auVar27 [8];
  longlong lVar28;
  int iVar29;
  undefined8 *puVar30;
  longlong lVar31;
  bool bVar32;
  undefined8 local_res8;
  undefined8 *local_res10;
  longlong local_res18;
  undefined8 in_stack_fffffffffffffe78;
  undefined4 uVar33;
  undefined1 local_168 [8];
  undefined8 uStack_160;
  undefined8 *local_158;
  ulonglong local_150;
  undefined1 local_148 [8];
  undefined8 uStack_140;
  size_t local_138;
  ulonglong uStack_130;
  undefined8 *******local_128;
  undefined1 *puStack_120;
  size_t local_118;
  ulonglong local_110;
  int local_108 [4];
  longlong local_f8;
  int local_f0;
  undefined1 local_e8;
  undefined7 uStack_e7;
  undefined8 local_d8;
  ulonglong local_d0;
  undefined **local_c8 [3];
  ulonglong local_b0;
  undefined ***local_90;
  undefined1 (*local_88 [2]) [16];
  ulonglong local_78;
  ulonglong local_70;

  if (*param_1 == 0) {
    return;
  }
  if (param_1[1] == 0) {
    return;
  }
  iVar29 = (int)(DAT_140b77ee0 - DAT_140b77ed8 >> 2) + -1;
  local_108[0] = 0;
  lVar14 = DAT_140b77ed8;
  local_res10 = param_2;
  local_res18 = param_3;
  local_108[2] = iVar29;
  lVar11 = FUN_14046f4a0(DAT_140b77ed8,DAT_140b77ee0,*param_1 + 0x3a0);
  local_108[1] = (int)(lVar11 - lVar14 >> 2);
  piVar23 = local_108;
  if (-1 < local_108[1]) {
    piVar23 = local_108 + 1;
  }
  piVar21 = local_108 + 2;
  if (local_108[1] <= iVar29) {
    piVar21 = piVar23;
  }
  local_f0 = *piVar21;
  local_f8 = *(longlong *)(param_3 + 600);
  if (local_f8 == 0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  lVar14 = param_1[4];
  lVar11 = FUN_1402d82e0("station_editor_basics","Basics");
  _local_168 = ZEXT816(0);
  local_158 = (undefined8 *)0x0;
  local_150 = 0;
  lVar31 = -1;
  lVar28 = -1;
  do {
    lVar28 = lVar28 + 1;
  } while (*(char *)(lVar11 + lVar28) != '\0');
  FUN_140002c00(local_168,lVar11);
  uVar8 = FUN_140572e60(param_2,param_4,local_168,(char)lVar14);
  *(undefined1 *)((longlong)param_1 + 0x24) = uVar8;
  uVar7 = DAT_140aabae4;
  if ((char)param_1[4] == '\0') goto LAB_140756d36;
  FUN_14055b8e0(param_4,&DAT_140b8dfe0);
  (**(code **)(*param_4 + 8))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_4 + 0xa8);
  uVar12 = FUN_1402d82e0("station_name_edit","Name:");
  (*pcVar1)(param_4,uVar12,0x14);
  uVar33 = (undefined4)((ulonglong)in_stack_fffffffffffffe78 >> 0x20);
  if (*(char *)(*param_1 + 0x40) == '\0') {
    FUN_140019ca0(local_148,*param_1 + 0x20);
    FUN_140019ca0(&local_128,local_148);
    if ((char)param_1[5] != '\0') {
      (**(code **)(*param_4 + 0xe8))(param_4,0xa62);
    }
    FUN_14055b8e0(param_4,&DAT_140b8e0e0);
    uVar12 = FUN_140572730(param_4,local_168,*(undefined8 *)*param_1,"user_name");
    in_stack_fffffffffffffe78 = CONCAT44(uVar33,0x260);
    FUN_140822c70(uVar12,local_148,0xff,0,in_stack_fffffffffffffe78);
    auVar27 = (undefined1  [8])local_148;
    if (0xf < uStack_130) {
      auVar27 = local_148;
    }
    pppppppuVar22 = &local_128;
    if (0xf < local_110) {
      pppppppuVar22 = local_128;
    }
    if ((local_118 != local_138) ||
       ((local_118 != 0 && (iVar29 = memcmp(pppppppuVar22,(void *)auVar27,local_118), iVar29 != 0)))
       ) {
      FUN_140585b80(param_1 + 0x47,local_148);
    }
    FUN_140002d30(&local_128);
    FUN_140002d30(local_148);
  }
  else {
    FUN_140019ca0(local_c8,param_1[1] + 8);
    *(undefined1 *)(param_4 + 4) = 1;
    *(undefined4 *)((longlong)param_4 + 0x24) = 2;
    FUN_14055b8e0(param_4,&DAT_140b8e0e0);
    (**(code **)(*param_4 + 8))(param_4);
    *(undefined1 *)(param_4 + 3) = 1;
    *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
    (**(code **)(*param_4 + 0xa0))(param_4,local_c8,0x11);
    FUN_14055a690(param_4 + 3,DAT_140aac308);
    pcVar1 = *(code **)(*param_4 + 0x80);
    uVar12 = *param_2;
    local_res8._0_5_ = (uint5)(uint)(float)local_res8;
    _local_168 = ZEXT816(0);
    local_158 = (undefined8 *)0x0;
    local_150 = 0;
    pcVar13 = (char *)FUN_140003270(0x20);
    uVar16 = s_icon_refresh_svg_140a9c4a8._8_8_;
    local_168 = (undefined1  [8])pcVar13;
    local_158 = (undefined8 *)&DAT_00000010;
    local_150 = 0x1f;
    *(undefined8 *)pcVar13 = s_icon_refresh_svg_140a9c4a8._0_8_;
    *(undefined8 *)(pcVar13 + 8) = uVar16;
    pcVar13[0x10] = '\0';
    uVar12 = FUN_14081dff0(uVar12,local_88,local_168,local_res8);
    cVar9 = (*pcVar1)(param_4,uVar12,0);
    if (0xf < local_150) {
      FUN_140003040(local_168,local_168);
    }
    if (cVar9 != '\0') {
      local_res8._0_5_ = CONCAT14(1,*(int *)(*param_1 + 0x44) + 1);
      *(undefined ****)((longlong)param_1 + 0x274) = local_res8;
    }
    (**(code **)(*param_4 + 0x18))(param_4);
    FUN_140002d30(local_c8);
  }
  puVar15 = (undefined8 *)0x0;
  (**(code **)(*param_4 + 0x18))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8dfe0);
  (**(code **)(*param_4 + 8))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8e060);
  (**(code **)(*param_4 + 200))(param_4);
  local_res8 = (undefined ***)CONCAT44(local_res8._4_4_,(uint)*(byte *)(*param_1 + 0x40));
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  lVar14 = FUN_140572730(param_4,local_168,*(undefined8 *)*param_1,"use_automatic_name");
  uVar12 = FUN_1402d82e0("station_name_use_auto","Use automatic name");
  *(longlong *)(lVar14 + 8) = *(longlong *)(lVar14 + 8) + 1;
  uVar24 = (*(ulonglong *)(lVar14 + 8) >> 0x21 ^ *(ulonglong *)(lVar14 + 8)) * -0xae502812aa7333;
  uVar24 = (uVar24 >> 0x21 ^ uVar24) * -0x3b314601e57a13ad;
  FUN_140822e50(lVar14,uVar24 >> 0x21 ^ uVar24,uVar12,&local_res8);
  if ((float)local_res8 != (float)(uint)*(byte *)(*param_1 + 0x40)) {
    bVar32 = (float)local_res8 != 0.0;
    local_res8._0_2_ = CONCAT11(1,bVar32);
    *(undefined2 *)(param_1 + 0x4c) = (undefined2)local_res8;
  }
  (**(code **)(*param_4 + 0x18))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8dfe0);
  (**(code **)(*param_4 + 8))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_4 + 0xa8);
  uVar12 = FUN_1402d82e0("station_label_mode","Label:");
  (*pcVar1)(param_4,uVar12,0x14);
  _local_168 = ZEXT816(0);
  local_158 = (undefined8 *)0x0;
  lVar14 = FUN_1402d82e0("station_label_mode_full",&DAT_140a900e0);
  _local_148 = ZEXT816(0);
  local_138 = 0;
  uStack_130 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar14 + lVar11) != '\0');
  FUN_140002c00(local_148,lVar14);
  if (uStack_160 < local_158) {
    *uStack_160 = local_148;
    uStack_160[1] = uStack_140;
    uStack_160[2] = local_138;
    uStack_160[3] = uStack_130;
    uStack_160 = uStack_160 + 4;
  }
  else {
    lVar14 = (longlong)uStack_160 - (longlong)local_168 >> 5;
    if (lVar14 == 0) {
      lVar14 = 1;
LAB_14075645b:
      puVar15 = (undefined8 *)thunk_FUN_140983da8(lVar14 << 5);
      auVar27 = local_168;
      puVar30 = puVar15;
      puVar25 = uStack_160;
    }
    else {
      lVar14 = lVar14 * 2;
      auVar27 = local_168;
      puVar30 = puVar15;
      puVar25 = uStack_160;
      if (lVar14 != 0) goto LAB_14075645b;
    }
    for (; auVar27 != (undefined1  [8])puVar25;
        auVar27 = (undefined1  [8])((longlong)auVar27 + 0x20)) {
      *puVar15 = 0;
      puVar15[1] = 0;
      puVar15[2] = 0;
      puVar15[3] = 0;
      uVar12 = *(undefined8 *)((longlong)auVar27 + 8);
      *puVar15 = *(undefined8 *)auVar27;
      puVar15[1] = uVar12;
      uVar12 = *(undefined8 *)((longlong)auVar27 + 0x18);
      puVar15[2] = *(undefined8 *)((longlong)auVar27 + 0x10);
      puVar15[3] = uVar12;
      *(undefined8 *)((longlong)auVar27 + 0x10) = 0;
      *(undefined8 *)((longlong)auVar27 + 0x18) = 0xf;
      *(undefined1 *)auVar27 = 0;
      puVar15 = puVar15 + 4;
    }
    *puVar15 = local_148;
    puVar15[1] = uStack_140;
    puVar15[2] = local_138;
    puVar15[3] = uStack_130;
    local_res8 = (undefined ***)(puVar15 + 4);
    pvVar19 = uStack_160;
    auVar27 = local_168;
    if (local_168 != (undefined1  [8])uStack_160) {
      do {
        FUN_140002d30(auVar27);
        auVar27 = (undefined1  [8])((longlong)auVar27 + 0x20);
      } while (auVar27 != (undefined1  [8])pvVar19);
    }
    if (local_168 != (undefined1  [8])0x0) {
      free((void *)local_168);
    }
    uStack_160 = local_res8;
    local_158 = puVar30 + lVar14 * 4;
    local_168 = (undefined1  [8])puVar30;
  }
  puVar15 = (undefined8 *)0x0;
  lVar14 = FUN_1402d82e0("station_label_mode_name_pop","Name and pax");
  _local_148 = ZEXT816(0);
  local_138 = 0;
  uStack_130 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar14 + lVar11) != '\0');
  FUN_140002c00(local_148,lVar14);
  if (uStack_160 < local_158) {
    *uStack_160 = local_148;
    uStack_160[1] = uStack_140;
    uStack_160[2] = local_138;
    uStack_160[3] = uStack_130;
    uStack_160 = uStack_160 + 4;
  }
  else {
    lVar14 = (longlong)uStack_160 - (longlong)local_168 >> 5;
    if (lVar14 == 0) {
      lVar14 = 1;
LAB_1407565b6:
      puVar15 = (undefined8 *)thunk_FUN_140983da8(lVar14 << 5);
      auVar27 = local_168;
      puVar30 = puVar15;
    }
    else {
      lVar14 = lVar14 * 2;
      auVar27 = local_168;
      puVar30 = puVar15;
      if (lVar14 != 0) goto LAB_1407565b6;
    }
    for (; puVar25 = uStack_160, auVar27 != (undefined1  [8])uStack_160;
        auVar27 = (undefined1  [8])((longlong)auVar27 + 0x20)) {
      *puVar15 = 0;
      puVar15[1] = 0;
      puVar15[2] = 0;
      puVar15[3] = 0;
      uVar12 = *(undefined8 *)((longlong)auVar27 + 8);
      *puVar15 = *(undefined8 *)auVar27;
      puVar15[1] = uVar12;
      uVar12 = *(undefined8 *)((longlong)auVar27 + 0x18);
      puVar15[2] = *(undefined8 *)((longlong)auVar27 + 0x10);
      puVar15[3] = uVar12;
      *(undefined8 *)((longlong)auVar27 + 0x10) = 0;
      *(undefined8 *)((longlong)auVar27 + 0x18) = 0xf;
      *(undefined1 *)auVar27 = 0;
      puVar15 = puVar15 + 4;
    }
    *puVar15 = local_148;
    puVar15[1] = uStack_140;
    puVar15[2] = local_138;
    puVar15[3] = uStack_130;
    local_res8 = (undefined ***)(puVar15 + 4);
    for (auVar27 = local_168; auVar27 != (undefined1  [8])puVar25;
        auVar27 = (undefined1  [8])((longlong)auVar27 + 0x20)) {
      FUN_140002d30(auVar27);
    }
    if (local_168 != (undefined1  [8])0x0) {
      free((void *)local_168);
    }
    uStack_160 = local_res8;
    local_158 = puVar30 + lVar14 * 4;
    local_168 = (undefined1  [8])puVar30;
  }
  lVar14 = FUN_1402d82e0("station_label_mode_name_only","Name only");
  _local_148 = ZEXT816(0);
  local_138 = 0;
  uStack_130 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar14 + lVar11) != '\0');
  FUN_140002c00(local_148,lVar14);
  if (uStack_160 < local_158) {
    *uStack_160 = local_148;
    uStack_160[1] = uStack_140;
    uStack_160[2] = local_138;
    uStack_160[3] = uStack_130;
    auVar3[0xf] = 0;
    auVar3._0_15_ = stack0xfffffffffffffeb9;
    _local_148 = auVar3 << 8;
    uVar24 = 0xf;
    uStack_160 = uStack_160 + 4;
  }
  else {
    FUN_1404ba220(local_168,local_148);
    uVar24 = uStack_130;
  }
  if (0xf < uVar24) {
    FUN_140003040(local_148,local_148);
  }
  lVar14 = FUN_1402d82e0("station_label_mode_icon_only","Icon only");
  _local_148 = ZEXT816(0);
  local_138 = 0;
  uStack_130 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar14 + lVar11) != '\0');
  FUN_140002c00(local_148,lVar14);
  if (uStack_160 < local_158) {
    *uStack_160 = local_148;
    uStack_160[1] = uStack_140;
    uStack_160[2] = local_138;
    uStack_160[3] = uStack_130;
    auVar4[0xf] = 0;
    auVar4._0_15_ = stack0xfffffffffffffeb9;
    _local_148 = auVar4 << 8;
    uVar24 = 0xf;
    uStack_160 = uStack_160 + 4;
  }
  else {
    FUN_1404ba220(local_168,local_148);
    uVar24 = uStack_130;
  }
  if (0xf < uVar24) {
    FUN_140003040(local_148,local_148);
  }
  lVar14 = FUN_1402d82e0("station_label_mode_dot_only","Dot only");
  _local_148 = ZEXT816(0);
  local_138 = 0;
  uStack_130 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar14 + lVar11) != '\0');
  FUN_140002c00(local_148,lVar14);
  if (uStack_160 < local_158) {
    *uStack_160 = local_148;
    uStack_160[1] = uStack_140;
    uStack_160[2] = local_138;
    uStack_160[3] = uStack_130;
    auVar5[0xf] = 0;
    auVar5._0_15_ = stack0xfffffffffffffeb9;
    _local_148 = auVar5 << 8;
    uVar24 = 0xf;
    uStack_160 = uStack_160 + 4;
  }
  else {
    FUN_1404ba220(local_168,local_148);
    uVar24 = uStack_130;
  }
  if (0xf < uVar24) {
    FUN_140003040(local_148,local_148);
  }
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  iVar29 = (**(code **)(*param_4 + 0x120))(param_4,local_168,*(undefined4 *)(*param_1 + 0x3a8));
  puVar15 = uStack_160;
  uVar33 = (undefined4)((ulonglong)in_stack_fffffffffffffe78 >> 0x20);
  auVar27 = local_168;
  if (iVar29 != *(int *)(*param_1 + 0x3a8)) {
    local_res8._0_5_ = CONCAT14(1,iVar29);
    *(undefined ****)((longlong)param_1 + 0x27c) = local_res8;
  }
  for (; auVar27 != (undefined1  [8])puVar15; auVar27 = (undefined1  [8])((longlong)auVar27 + 0x20))
  {
    FUN_140002d30(auVar27);
    uVar33 = (undefined4)((ulonglong)in_stack_fffffffffffffe78 >> 0x20);
  }
  if (local_168 != (undefined1  [8])0x0) {
    free((void *)local_168);
  }
  (**(code **)(*param_4 + 0x18))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8dfe0);
  (**(code **)(*param_4 + 8))(param_4);
  local_res8 = (undefined ***)CONCAT44(local_res8._4_4_,*(undefined4 *)(*param_1 + 0x398));
  FUN_14055b8e0(param_4,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_4 + 0xa8);
  uVar12 = FUN_1402d82e0("station_pop_area","Population radius:");
  (*pcVar1)(param_4,uVar12,0x14);
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  uVar12 = FUN_140572730(param_4,local_168,*(undefined8 *)*param_1,"size_factor");
  FUN_140822f10(uVar12);
  *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42380000;
  *(undefined1 *)(param_4 + 5) = 1;
  pcVar1 = *(code **)(*param_4 + 0xa0);
  uStack_160._0_4_ = (undefined4)uStack_140;
  local_168 = (undefined1  [8])((double)*(float *)(*param_1 + 0x398) * DAT_140aabd98);
  uStack_160._4_4_ = uStack_140._4_4_;
  puStack_120 = local_168;
  local_128 = (undefined8 *******)0xa;
  uStack_140._0_4_ = 9;
  local_148 = (undefined1  [8])&DAT_140a8f018;
  uStack_140._4_4_ = 0;
  FUN_140021e90(local_c8,local_148,&local_128);
  (*pcVar1)(param_4,local_c8,0x11);
  FUN_140002d30(local_c8);
  if ((float)local_res8 != *(float *)(*param_1 + 0x398)) {
    local_res8._0_5_ = CONCAT14(1,(float)local_res8);
    *(undefined ****)((longlong)param_1 + 0x264) = local_res8;
  }
  (**(code **)(*param_4 + 0x18))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8dfe0);
  (**(code **)(*param_4 + 8))(param_4);
  iVar29 = local_f0;
  lVar14 = (longlong)local_f0;
  local_res8 = (undefined ***)CONCAT44(local_res8._4_4_,(float)local_f0);
  FUN_14055b8e0(param_4,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_4 + 0xa8);
  uVar12 = FUN_1402d82e0("station_max_plat_pax","Platforms pax capacity:");
  (*pcVar1)(param_4,uVar12,0x14);
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  uVar12 = FUN_140572730(param_4,local_168,*(undefined8 *)*param_1,"station_max_plat_pax");
  FUN_140822f10(uVar12,uVar12,(float)((DAT_140b77ee0 - DAT_140b77ed8 >> 2) - 1),&local_res8,
                CONCAT44(uVar33,uVar7));
  *(undefined4 *)((longlong)param_4 + 0x2c) = 0x428c0000;
  *(undefined1 *)(param_4 + 5) = 1;
  pcVar1 = *(code **)(*param_4 + 0xa0);
  puVar15 = (undefined8 *)FUN_140572550(local_88,*(undefined4 *)(DAT_140b77ed8 + lVar14 * 4));
  local_168 = (undefined1  [8])puVar15;
  if (0xf < (ulonglong)puVar15[3]) {
    local_168 = (undefined1  [8])*puVar15;
  }
  uStack_160 = (undefined8 *)puVar15[2];
  uStack_140 = local_168;
  local_148 = (undefined1  [8])0xd;
  local_128 = (undefined8 *******)((longlong)&DAT_140a8f27e + 2);
  puStack_120 = (undefined1 *)0x6;
  FUN_140021e90(local_c8,&local_128,local_148);
  (*pcVar1)(param_4,local_c8,0x11);
  FUN_140002d30(local_c8);
  FUN_140002d30(local_88);
  iVar10 = lroundf();
  if (iVar10 != iVar29) {
    iVar29 = lround((double)iVar10);
    local_res8._0_5_ = CONCAT14(1,*(undefined4 *)(DAT_140b77ed8 + (longlong)iVar29 * 4));
    *(undefined ****)((longlong)param_1 + 0x284) = local_res8;
  }
  (**(code **)(*param_4 + 0x18))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8dfe0);
  (**(code **)(*param_4 + 8))(param_4);
  local_res8 = (undefined ***)CONCAT44(local_res8._4_4_,(uint)*(byte *)(*param_1 + 0x3a4));
  FUN_14055b8e0(param_4,&DAT_140b8e060);
  (**(code **)(*param_4 + 200))(param_4);
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  uVar12 = FUN_140572730(param_4,local_168,*(undefined8 *)*param_1,"transfer_overflow_into_hall");
  uVar16 = FUN_1402d82e0("station_transfer_overflow_into_hall","Transfers overflow into hall");
  FUN_140822e00(uVar12,uVar16,&local_res8);
  if ((float)local_res8 != (float)(uint)*(byte *)(*param_1 + 0x3a4)) {
    bVar32 = (float)local_res8 != 0.0;
    local_res8._0_2_ = CONCAT11(1,bVar32);
    *(undefined2 *)((longlong)param_1 + 0x28c) = (undefined2)local_res8;
  }
  (**(code **)(*param_4 + 0x18))(param_4);
LAB_140756d36:
  FUN_1403fd2d0(*param_1 + 0x48,&local_128,*(undefined8 *)(local_f8 + 0x420),&DAT_140a4d304,0);
  local_158 = (undefined8 *)0x1;
  local_150 = 0xf;
  uStack_160 = (undefined8 *)0x0;
  local_168 = (undefined1  [8])((ulonglong)DAT_140a4d470 & 0xffffffffffff00ff);
  lVar14 = FUN_1402d82e0("lineeditor_tags","Tags:");
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar14 + lVar11) != '\0');
  pauVar17 = (undefined1 (*) [16])FUN_14029cb50(local_168,0,lVar14);
  _local_148 = *pauVar17;
  local_138 = *(size_t *)pauVar17[1];
  uStack_130 = *(ulonglong *)(pauVar17[1] + 8);
  *(undefined8 *)pauVar17[1] = 0;
  *(undefined8 *)(pauVar17[1] + 8) = 0xf;
  (*pauVar17)[0] = 0;
  FUN_14029c760(local_88,(ulonglong)local_res8 & 0xff);
  if (0xf < uStack_130) {
    auVar27 = local_148;
    if ((0xfff < uStack_130 + 1) &&
       (auVar27 = *(undefined1 (*) [8])((longlong)local_148 + -8),
       0x1f < (ulonglong)((longlong)local_148 + (-8 - (longlong)auVar27)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free((void *)auVar27);
  }
  if (0xf < local_150) {
    auVar27 = local_168;
    if ((0xfff < local_150 + 1) &&
       (auVar27 = *(undefined1 (*) [8])((longlong)local_168 + -8),
       0x1f < (ulonglong)((longlong)local_168 + (-8 - (longlong)auVar27)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free((void *)auVar27);
  }
  local_158 = (undefined8 *)0x0;
  local_150 = 0xf;
  auVar6[0xf] = 0;
  auVar6._0_15_ = stack0xfffffffffffffe99;
  _local_168 = auVar6 << 8;
  if (0xf < local_110) {
    pppppppuVar22 = local_128;
    if ((0xfff < local_110 + 1) &&
       (pppppppuVar22 = (undefined8 *******)local_128[-1],
       0x1f < (ulonglong)((longlong)local_128 + (-8 - (longlong)pppppppuVar22)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppppuVar22);
  }
  local_118 = 0;
  local_110 = 0xf;
  local_128 = (undefined8 *******)((ulonglong)local_128 & 0xffffffffffffff00);
  local_res8 = (undefined ***)CONCAT71(local_res8._1_7_,*(undefined1 *)((longlong)param_1 + 0x21));
  _local_148 = ZEXT816(0);
  local_138 = 0;
  uStack_130 = 0;
  pauVar17 = (undefined1 (*) [16])local_88;
  if (0xf < local_70) {
    pauVar17 = local_88[0];
  }
  if (local_78 < 0x8000000000000000) {
    if (local_78 < 0x10) {
      local_138 = local_78;
      uStack_130 = 0xf;
      _local_148 = *pauVar17;
      uVar8 = *(undefined1 *)((longlong)param_1 + 0x21);
    }
    else {
      uVar18 = local_78 | 0xf;
      uVar24 = 0x7fffffffffffffff;
      if ((uVar18 < 0x8000000000000000) && (uVar24 = uVar18, uVar18 < 0x16)) {
        uVar24 = 0x16;
      }
      pvVar19 = (void *)FUN_140003270(uVar24 + 1);
      local_148 = (undefined1  [8])pvVar19;
      local_138 = local_78;
      uStack_130 = uVar24;
      memcpy(pvVar19,pauVar17,local_78 + 1);
      uVar8 = (undefined1)local_res8;
    }
    puVar15 = local_res10;
    uVar8 = FUN_140572e60(local_res10,param_4,local_148,uVar8);
    *(undefined1 *)((longlong)param_1 + 0x25) = uVar8;
    FUN_1403fd2d0(*param_1 + 0x48,&local_e8,*(undefined8 *)(local_f8 + 0x420),&DAT_140a4aefc,0);
    puVar26 = &local_e8;
    if (0xf < local_d0) {
      puVar26 = (undefined1 *)CONCAT71(uStack_e7,local_e8);
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar26);
    if (*(char *)((longlong)param_1 + 0x21) != '\0') {
      *(undefined4 *)((longlong)param_4 + 0x34) = 0x437a0000;
      *(undefined1 *)(param_4 + 3) = 1;
      *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
      *(undefined1 *)(param_4 + 6) = 1;
      FUN_14058dbe0(param_1 + 6,puVar15,param_4,*(undefined8 *)(local_f8 + 0x420),*param_1 + 0x48,
                    *(undefined8 *)(local_f8 + 0x1598));
    }
    uVar8 = *(undefined1 *)((longlong)param_1 + 0x22);
    lVar14 = FUN_1402d82e0("station_editor_walk_links","Walk links");
    _local_168 = ZEXT816(0);
    local_158 = (undefined8 *)0x0;
    local_150 = 0;
    lVar11 = -1;
    do {
      lVar11 = lVar11 + 1;
    } while (*(char *)(lVar14 + lVar11) != '\0');
    FUN_140002c00(local_168,lVar14);
    uVar8 = FUN_140572e60(local_res10,param_4,local_168,uVar8);
    *(undefined1 *)((longlong)param_1 + 0x26) = uVar8;
    if (*(char *)((longlong)param_1 + 0x22) != '\0') {
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      (**(code **)(*param_4 + 8))(param_4);
      local_res8 = (undefined ***)CONCAT44(local_res8._4_4_,*(undefined4 *)(*param_1 + 0x39c));
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar1 = *(code **)(*param_4 + 0xa8);
      uVar12 = FUN_1402d82e0("station_max_walk","Auto walk transfer:");
      (*pcVar1)(param_4,uVar12,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      uVar12 = FUN_140572730(param_4,local_168,*(undefined8 *)*param_1,"walk_factor");
      FUN_140822f10(uVar12);
      *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42380000;
      *(undefined1 *)(param_4 + 5) = 1;
      pcVar1 = *(code **)(*param_4 + 0xa0);
      local_168 = (undefined1  [8])((double)*(float *)(*param_1 + 0x39c) * DAT_140aabd98);
      uStack_140 = local_168;
      local_148 = (undefined1  [8])0xa;
      local_128 = (undefined8 *******)&DAT_140a8f018;
      puStack_120 = (undefined1 *)0x9;
      FUN_140021e90(local_c8,&local_128,local_148);
      (*pcVar1)(param_4,local_c8);
      if (0xf < local_b0) {
        _Memory = local_c8[0];
        if ((0xfff < local_b0 + 1) &&
           (_Memory = (undefined **)local_c8[0][-1],
           0x1f < (ulonglong)((longlong)local_c8[0] + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory);
      }
      if ((float)local_res8 != *(float *)(*param_1 + 0x39c)) {
        local_res8._0_5_ = CONCAT14(1,(float)local_res8);
        *(undefined ****)((longlong)param_1 + 0x26c) = local_res8;
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      *(undefined4 *)((longlong)param_4 + 0x34) = 0x437a0000;
      *(undefined1 *)(param_4 + 6) = 1;
      *(undefined1 *)(param_4 + 3) = 1;
      *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
      *(undefined1 *)(param_4 + 6) = 1;
      FUN_140754f40(param_1 + 0x2c,local_res10,local_res18,param_4);
    }
    lVar14 = FUN_1402d82e0("station_editor_stop_select_signals","Stop selection signals");
    _local_168 = ZEXT816(0);
    local_158 = (undefined8 *)0x0;
    local_150 = 0;
    do {
      lVar31 = lVar31 + 1;
    } while (*(char *)(lVar14 + lVar31) != '\0');
    FUN_140002c00(local_168,lVar14,lVar31);
    puVar15 = local_res10;
    uVar8 = FUN_140572e60(local_res10,param_4);
    *(undefined1 *)((longlong)param_1 + 0x27) = uVar8;
    if (*(char *)((longlong)param_1 + 0x23) != '\0') {
      *(undefined4 *)((longlong)param_4 + 0x34) = 0x437a0000;
      *(undefined1 *)(param_4 + 6) = 1;
      *(undefined1 *)(param_4 + 3) = 1;
      *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
      *(undefined1 *)(param_4 + 6) = 1;
      FUN_140755460(param_1 + 0x3d,puVar15);
      lVar14 = local_res18;
      *(undefined4 *)((longlong)param_4 + 0x34) = 0x41f00000;
      *(undefined1 *)(param_4 + 6) = 1;
      *(undefined1 *)(param_4 + 3) = 1;
      *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
      plVar20 = *(longlong **)(local_res18 + 0x200);
      pcVar1 = *(code **)(*plVar20 + 0x20);
      local_res8 = local_c8;
      local_c8[0] = std::
                    _Func_impl_no_alloc<`public:_void___cdecl_nimby::shell::StationEditor::Impl::StationBasics::shell(nimby::shell::Context&___ptr64,nimby::shell::UITxn&___ptr64,nimby::shell::ui::Declare&___ptr64)___ptr64'::`56'::<lambda_1>,bool,nimby::model::DBSession_const&___ptr64,nimby::model::Signal_const*___ptr64>
                    ::vftable;
      local_90 = local_c8;
      uVar12 = FUN_1406c7ae0("station editor stop selection",*(undefined8 *)*param_1);
      plVar20 = (longlong *)(*pcVar1)(plVar20,local_168,puVar15,lVar14,param_4,uVar12,1,local_c8);
      lVar14 = plVar20[1];
      param_1[0x52] = *plVar20;
      param_1[0x53] = lVar14;
      pcVar1 = *(code **)(*param_4 + 0xc0);
      uVar12 = FUN_1402d82e0("lineeditor_alt_signal",
                             "When a train reaches this path signal, it will be capable of changing its destination platform if its current choice is busy, and only if it belongs to this station.\nOnly trains whose current line stop is configured for multiple platforms will be able to change their destination platform.\nOnly path signals with a direct path to the main and all secondary platforms will properly enable this feature."
                            );
      (*pcVar1)(param_4,uVar12);
    }
    if (0xf < local_d0) {
      pvVar2 = (void *)CONCAT71(uStack_e7,local_e8);
      pvVar19 = pvVar2;
      if ((0xfff < local_d0 + 1) &&
         (pvVar19 = *(void **)((longlong)pvVar2 + -8),
         0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)pvVar19)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar19);
    }
    local_d8 = 0;
    local_d0 = 0xf;
    local_e8 = 0;
    if (0xf < local_70) {
      pauVar17 = local_88[0];
      if ((0xfff < local_70 + 1) &&
         (pauVar17 = *(undefined1 (**) [16])(local_88[0][-1] + 8),
         0x1f < (ulonglong)((longlong)local_88[0] + (-8 - (longlong)pauVar17)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pauVar17);
    }
    return;
  }
                    /* WARNING: Subroutine does not return */
  FUN_140001c70();
}


// Incoming references
// 0xc37c4c DATA caller none
// 0x759b67 UNCONDITIONAL_CALL caller 140759af0
