// Candidate VA 140790c40; RVA 0x790c40
// Ghidra inferred prototype: undefined FUN_140790c40()

void FUN_140790c40(undefined8 *param_1,longlong *param_2)

{
  code *pcVar1;
  undefined8 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined8 *puVar6;
  undefined1 uVar7;
  char cVar8;
  int iVar9;
  int iVar10;
  longlong lVar11;
  undefined8 uVar12;
  undefined8 *puVar13;
  longlong lVar14;
  ulonglong uVar15;
  undefined8 *puVar16;
  longlong lVar17;
  undefined8 *local_res8;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 *local_88;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  ulonglong uStack_60;
  undefined1 local_58 [48];

  FUN_14055b8e0(param_2,&DAT_140b8df60);
  cVar8 = DAT_140b5eca8;
  lVar11 = FUN_1402d82e0("station_editor","Station properties");
  local_78 = (char *)0x0;
  uStack_70 = 0;
  puVar13 = (undefined8 *)0x0;
  local_68 = 0;
  uStack_60 = 0;
  lVar17 = -1;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar11 + lVar14) != '\0');
  FUN_140002c00(&local_78,lVar11);
  uVar7 = FUN_1405cc370(param_1[1],param_2,&local_78,cVar8);
  *(undefined1 *)*param_1 = uVar7;
  if (DAT_140b5eca8 == '\0') {
    return;
  }
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_2 + 0xa8);
  uVar12 = FUN_1402d82e0("station_name_edit","Name:");
  (*pcVar1)(param_2,uVar12,0x14);
  if (*(char *)param_1[2] == '\0') {
    if (DAT_140be285e != '\0') {
      (**(code **)(*param_2 + 0xe8))(param_2,0xa62);
    }
    FUN_14055b8e0(param_2,&DAT_140b8e0e0);
    uVar12 = FUN_140572730(param_2,&local_98,**(undefined8 **)param_1[4],"station_user_name");
    FUN_140822c70(uVar12,param_1[6],0xff,0,0xa62);
  }
  else {
    lVar11 = *(longlong *)param_1[3] + 8;
    if (*(char *)(*(longlong *)param_1[4] + 0x40) == '\0') {
      lVar11 = *(longlong *)param_1[4] + 0x20;
    }
    FUN_140019ca0(&local_98,lVar11);
    *(undefined1 *)(param_2 + 4) = 1;
    *(undefined4 *)((longlong)param_2 + 0x24) = 2;
    FUN_14055b8e0(param_2,&DAT_140b8e0e0);
    (**(code **)(*param_2 + 8))(param_2);
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    (**(code **)(*param_2 + 0xa0))(param_2,&local_98,0x11);
    *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
    *(undefined1 *)(param_2 + 5) = 1;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
    *(undefined1 *)(param_2 + 6) = 1;
    pcVar1 = *(code **)(*param_2 + 0x80);
    uVar12 = *(undefined8 *)param_1[1];
    local_res8._0_5_ = (uint5)(uint)local_res8;
    local_78 = (char *)0x0;
    uStack_70 = 0;
    local_68 = 0;
    uStack_60 = 0;
    local_78 = (char *)FUN_140003270(0x20);
    uVar5 = s_icon_refresh_svg_140a9c4a8._12_4_;
    uVar4 = s_icon_refresh_svg_140a9c4a8._8_4_;
    uVar3 = s_icon_refresh_svg_140a9c4a8._4_4_;
    local_68 = 0x10;
    uStack_60 = 0x1f;
    *(undefined4 *)local_78 = s_icon_refresh_svg_140a9c4a8._0_4_;
    *(undefined4 *)(local_78 + 4) = uVar3;
    *(undefined4 *)(local_78 + 8) = uVar4;
    *(undefined4 *)(local_78 + 0xc) = uVar5;
    local_78[0x10] = '\0';
    uVar12 = FUN_14081dff0(uVar12,local_58,&local_78,local_res8);
    cVar8 = (*pcVar1)(param_2,uVar12,0);
    if (0xf < uStack_60) {
      FUN_140003040(&local_78,local_78);
    }
    if (cVar8 != '\0') {
      *(int *)param_1[5] = *(int *)param_1[5] + 1;
    }
    (**(code **)(*param_2 + 0x18))(param_2);
    FUN_140002d30(&local_98);
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  (**(code **)(*param_2 + 200))(param_2);
  local_res8 = (undefined8 *)CONCAT44(local_res8._4_4_,(uint)*(byte *)param_1[2]);
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  pcVar1 = *(code **)(*param_2 + 0xf0);
  uVar12 = FUN_1402d82e0("station_name_use_auto","Use automatic name");
  (*pcVar1)(param_2,uVar12,&local_res8);
  *(bool *)param_1[7] = (uint)local_res8 != 0;
  (**(code **)(*param_2 + 0x18))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_2 + 0xa8);
  uVar12 = FUN_1402d82e0("station_label_mode","Label:");
  (*pcVar1)(param_2,uVar12,0x14);
  local_98 = (undefined8 *)0x0;
  uStack_90 = (undefined8 *)0x0;
  local_88 = (undefined8 *)0x0;
  lVar11 = FUN_1402d82e0("station_label_mode_full",&DAT_140a900e0);
  local_78 = (char *)0x0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar11 + lVar14) != '\0');
  FUN_140002c00(&local_78,lVar11);
  if (uStack_90 < local_88) {
    *(undefined4 *)uStack_90 = (undefined4)local_78;
    *(undefined4 *)((longlong)uStack_90 + 4) = local_78._4_4_;
    *(undefined4 *)(uStack_90 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)uStack_90 + 0xc) = uStack_70._4_4_;
    uStack_90[2] = local_68;
    uStack_90[3] = uStack_60;
    uStack_90 = uStack_90 + 4;
  }
  else {
    lVar11 = (longlong)uStack_90 - (longlong)local_98 >> 5;
    if (lVar11 == 0) {
      lVar11 = 1;
LAB_140791052:
      puVar13 = (undefined8 *)thunk_FUN_140983da8(lVar11 << 5);
      puVar2 = local_98;
      puVar16 = puVar13;
    }
    else {
      lVar11 = lVar11 * 2;
      puVar2 = local_98;
      puVar16 = puVar13;
      if (lVar11 != 0) goto LAB_140791052;
    }
    for (; puVar6 = uStack_90, puVar2 != uStack_90; puVar2 = puVar2 + 4) {
      *puVar13 = 0;
      puVar13[1] = 0;
      puVar13[2] = 0;
      puVar13[3] = 0;
      uVar12 = puVar2[1];
      *puVar13 = *puVar2;
      puVar13[1] = uVar12;
      uVar12 = puVar2[3];
      puVar13[2] = puVar2[2];
      puVar13[3] = uVar12;
      puVar2[2] = 0;
      puVar2[3] = 0xf;
      *(undefined1 *)puVar2 = 0;
      puVar13 = puVar13 + 4;
    }
    *(undefined4 *)puVar13 = (undefined4)local_78;
    *(undefined4 *)((longlong)puVar13 + 4) = local_78._4_4_;
    *(undefined4 *)(puVar13 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)puVar13 + 0xc) = uStack_70._4_4_;
    puVar13[2] = local_68;
    puVar13[3] = uStack_60;
    local_res8 = puVar13 + 4;
    for (puVar13 = local_98; puVar13 != puVar6; puVar13 = puVar13 + 4) {
      FUN_140002d30(puVar13);
    }
    if (local_98 != (undefined8 *)0x0) {
      free(local_98);
    }
    uStack_90 = local_res8;
    local_88 = puVar16 + lVar11 * 4;
    local_98 = puVar16;
  }
  puVar13 = (undefined8 *)0x0;
  lVar11 = FUN_1402d82e0("station_label_mode_name_pop","Name and pax");
  local_78 = (char *)0x0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar11 + lVar14) != '\0');
  FUN_140002c00(&local_78,lVar11);
  if (uStack_90 < local_88) {
    *(undefined4 *)uStack_90 = (undefined4)local_78;
    *(undefined4 *)((longlong)uStack_90 + 4) = local_78._4_4_;
    *(undefined4 *)(uStack_90 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)uStack_90 + 0xc) = uStack_70._4_4_;
    uStack_90[2] = local_68;
    uStack_90[3] = uStack_60;
    uStack_90 = uStack_90 + 4;
  }
  else {
    lVar11 = (longlong)uStack_90 - (longlong)local_98 >> 5;
    if (lVar11 == 0) {
      lVar11 = 1;
LAB_140791193:
      puVar13 = (undefined8 *)thunk_FUN_140983da8(lVar11 << 5);
      puVar2 = local_98;
      puVar16 = puVar13;
    }
    else {
      lVar11 = lVar11 * 2;
      puVar2 = local_98;
      puVar16 = puVar13;
      if (lVar11 != 0) goto LAB_140791193;
    }
    for (; puVar6 = uStack_90, puVar2 != uStack_90; puVar2 = puVar2 + 4) {
      *puVar13 = 0;
      puVar13[1] = 0;
      puVar13[2] = 0;
      puVar13[3] = 0;
      uVar12 = puVar2[1];
      *puVar13 = *puVar2;
      puVar13[1] = uVar12;
      uVar12 = puVar2[3];
      puVar13[2] = puVar2[2];
      puVar13[3] = uVar12;
      puVar2[2] = 0;
      puVar2[3] = 0xf;
      *(undefined1 *)puVar2 = 0;
      puVar13 = puVar13 + 4;
    }
    *(undefined4 *)puVar13 = (undefined4)local_78;
    *(undefined4 *)((longlong)puVar13 + 4) = local_78._4_4_;
    *(undefined4 *)(puVar13 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)puVar13 + 0xc) = uStack_70._4_4_;
    puVar13[2] = local_68;
    puVar13[3] = uStack_60;
    local_res8 = puVar13 + 4;
    for (puVar13 = local_98; puVar13 != puVar6; puVar13 = puVar13 + 4) {
      FUN_140002d30(puVar13);
    }
    if (local_98 != (undefined8 *)0x0) {
      free(local_98);
    }
    uStack_90 = local_res8;
    local_88 = puVar16 + lVar11 * 4;
    local_98 = puVar16;
  }
  puVar13 = (undefined8 *)0x0;
  lVar11 = FUN_1402d82e0("station_label_mode_name_only","Name only");
  local_78 = (char *)0x0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar11 + lVar14) != '\0');
  FUN_140002c00(&local_78,lVar11);
  if (uStack_90 < local_88) {
    *(undefined4 *)uStack_90 = (undefined4)local_78;
    *(undefined4 *)((longlong)uStack_90 + 4) = local_78._4_4_;
    *(undefined4 *)(uStack_90 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)uStack_90 + 0xc) = uStack_70._4_4_;
    uStack_90[2] = local_68;
    uStack_90[3] = uStack_60;
    uStack_90 = uStack_90 + 4;
    goto LAB_14079138c;
  }
  lVar11 = (longlong)uStack_90 - (longlong)local_98 >> 5;
  if (lVar11 == 0) {
    lVar11 = 1;
LAB_1407912d0:
    puVar13 = (undefined8 *)thunk_FUN_140983da8(lVar11 << 5);
    puVar2 = local_98;
    puVar16 = puVar13;
  }
  else {
    lVar11 = lVar11 * 2;
    puVar2 = local_98;
    puVar16 = puVar13;
    if (lVar11 != 0) goto LAB_1407912d0;
  }
  for (; puVar6 = uStack_90, puVar2 != uStack_90; puVar2 = puVar2 + 4) {
    *puVar13 = 0;
    puVar13[1] = 0;
    puVar13[2] = 0;
    puVar13[3] = 0;
    uVar12 = puVar2[1];
    *puVar13 = *puVar2;
    puVar13[1] = uVar12;
    uVar12 = puVar2[3];
    puVar13[2] = puVar2[2];
    puVar13[3] = uVar12;
    puVar2[2] = 0;
    puVar2[3] = 0xf;
    *(undefined1 *)puVar2 = 0;
    puVar13 = puVar13 + 4;
  }
  *(undefined4 *)puVar13 = (undefined4)local_78;
  *(undefined4 *)((longlong)puVar13 + 4) = local_78._4_4_;
  *(undefined4 *)(puVar13 + 1) = (undefined4)uStack_70;
  *(undefined4 *)((longlong)puVar13 + 0xc) = uStack_70._4_4_;
  puVar13[2] = local_68;
  puVar13[3] = uStack_60;
  local_res8 = puVar13 + 4;
  for (puVar13 = local_98; puVar13 != puVar6; puVar13 = puVar13 + 4) {
    FUN_140002d30(puVar13);
  }
  if (local_98 != (undefined8 *)0x0) {
    free(local_98);
  }
  uStack_90 = local_res8;
  local_88 = puVar16 + lVar11 * 4;
  local_98 = puVar16;
LAB_14079138c:
  lVar11 = FUN_1402d82e0("station_label_mode_icon_only","Icon only");
  local_78 = (char *)0x0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar11 + lVar14) != '\0');
  FUN_140002c00(&local_78,lVar11);
  if (uStack_90 < local_88) {
    *(undefined4 *)uStack_90 = (undefined4)local_78;
    *(undefined4 *)((longlong)uStack_90 + 4) = local_78._4_4_;
    *(undefined4 *)(uStack_90 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)uStack_90 + 0xc) = uStack_70._4_4_;
    uStack_90[2] = local_68;
    uStack_90[3] = uStack_60;
    local_78 = (char *)((ulonglong)local_78 & 0xffffffffffffff00);
    uVar15 = 0xf;
    uStack_90 = uStack_90 + 4;
  }
  else {
    FUN_1404ba220(&local_98,&local_78);
    uVar15 = uStack_60;
  }
  if (0xf < uVar15) {
    FUN_140003040(&local_78,local_78);
  }
  lVar11 = FUN_1402d82e0("station_label_mode_dot_only","Dot only");
  local_78 = (char *)0x0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  do {
    lVar17 = lVar17 + 1;
  } while (*(char *)(lVar11 + lVar17) != '\0');
  FUN_140002c00(&local_78,lVar11,lVar17);
  if (uStack_90 < local_88) {
    *(undefined4 *)uStack_90 = (undefined4)local_78;
    *(undefined4 *)((longlong)uStack_90 + 4) = local_78._4_4_;
    *(undefined4 *)(uStack_90 + 1) = (undefined4)uStack_70;
    *(undefined4 *)((longlong)uStack_90 + 0xc) = uStack_70._4_4_;
    uStack_90[2] = local_68;
    uStack_90[3] = uStack_60;
    local_78 = (char *)((ulonglong)local_78 & 0xffffffffffffff00);
    uVar15 = 0xf;
    uStack_90 = uStack_90 + 4;
  }
  else {
    FUN_1404ba220(&local_98,&local_78);
    uVar15 = uStack_60;
  }
  if (0xf < uVar15) {
    FUN_140003040(&local_78,local_78);
  }
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  iVar9 = (**(code **)(*param_2 + 0x120))(param_2,&local_98,*(undefined4 *)param_1[8]);
  puVar2 = uStack_90;
  puVar13 = local_98;
  if (iVar9 != *(int *)param_1[8]) {
    *(int *)param_1[9] = iVar9;
  }
  for (; puVar13 != puVar2; puVar13 = puVar13 + 4) {
    FUN_140002d30(puVar13);
  }
  if (local_98 != (undefined8 *)0x0) {
    free(local_98);
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  (**(code **)(*param_2 + 200))(param_2);
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  (**(code **)(*param_2 + 8))(param_2);
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
  pcVar1 = *(code **)(*param_2 + 0x90);
  uVar12 = FUN_1402d82e0(&DAT_140a9e420,&DAT_140a9e0bc);
  cVar8 = (*pcVar1)(param_2,uVar12,0);
  if (cVar8 != '\0') {
    *(undefined1 *)param_1[10] = 1;
  }
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0;
  pcVar1 = *(code **)(*param_2 + 0x80);
  uVar12 = *(undefined8 *)param_1[1];
  local_res8._0_5_ = (uint5)(uint)local_res8;
  local_78 = (char *)0x0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  local_78 = (char *)FUN_140003270(0x20);
  uVar5 = s_icon_pin_outline_svg_140a83c18._12_4_;
  uVar4 = s_icon_pin_outline_svg_140a83c18._8_4_;
  uVar3 = s_icon_pin_outline_svg_140a83c18._4_4_;
  local_68 = 0x14;
  uStack_60 = 0x1f;
  *(undefined4 *)local_78 = s_icon_pin_outline_svg_140a83c18._0_4_;
  *(undefined4 *)(local_78 + 4) = uVar3;
  *(undefined4 *)(local_78 + 8) = uVar4;
  *(undefined4 *)(local_78 + 0xc) = uVar5;
  *(undefined4 *)(local_78 + 0x10) = s_icon_pin_outline_svg_140a83c18._16_4_;
  local_78[0x14] = '\0';
  uVar12 = FUN_14081dff0(uVar12,local_58,&local_78,local_res8);
  cVar8 = (*pcVar1)(param_2,uVar12,0);
  if (0xf < uStack_60) {
    FUN_140003040(&local_78,local_78);
  }
  puVar13 = uStack_90;
  if ((cVar8 != '\0') &&
     (puVar2 = *(undefined8 **)(param_1[0xb] + 0x6d0), puVar2 != (undefined8 *)0x0)) {
    uStack_90 = (undefined8 *)CONCAT71(uStack_90._1_7_,1);
    puVar16 = uStack_90;
    local_98._0_4_ = SUB84(puVar2,0);
    local_98._4_4_ = (undefined4)((ulonglong)puVar2 >> 0x20);
    uStack_90._4_4_ = SUB84(puVar13,4);
    DAT_140b81900._0_4_ = (undefined4)local_98;
    DAT_140b81900._4_4_ = local_98._4_4_;
    DAT_140b81908._0_4_ = (undefined4)uStack_90;
    DAT_140b81908._4_4_ = uStack_90._4_4_;
    local_98 = puVar2;
    uStack_90 = puVar16;
    if (DAT_140b81920 != '\x06') {
      DAT_140b81920 = '\x06';
    }
  }
  if (*(int *)(*(longlong *)param_1[3] + 0x78) != 0) {
    iVar10 = *(int *)(*(longlong *)param_1[3] + 0x78) / 2;
    iVar9 = 1;
    if (1 < iVar10) {
      iVar9 = iVar10;
    }
    if (iVar9 != 0) {
      *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
      *(undefined1 *)(param_2 + 5) = 1;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
      *(undefined1 *)(param_2 + 6) = 1;
      *(undefined1 *)(param_2 + 7) = 1;
      *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
      *(undefined8 *)((longlong)param_2 + 0x44) = 0;
      pcVar1 = *(code **)(*param_2 + 0x80);
      local_res8._0_5_ = (uint5)(uint)local_res8;
      local_68 = 0xd;
      uStack_60 = 0xf;
      local_78 = (char *)s_icon_info_svg_140a875c0._0_8_;
      uStack_70 = (ulonglong)
                  CONCAT14(s_icon_info_svg_140a875c0[0xc],s_icon_info_svg_140a875c0._8_4_);
      uVar12 = FUN_14081dff0(*(undefined8 *)param_1[1],local_58,&local_78,local_res8);
      cVar8 = (*pcVar1)(param_2,uVar12,0);
      FUN_140002d30(&local_78);
      if (cVar8 != '\0') {
        DAT_140b81988 = *(undefined8 *)(param_1[0xb] + 0x6d0);
      }
    }
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  (**(code **)(*param_2 + 0x18))(param_2);
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41000000;
  *(undefined1 *)(param_2 + 6) = 1;
  (**(code **)(*param_2 + 200))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  (**(code **)(*param_2 + 200))(param_2);
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  pcVar1 = *(code **)(*param_2 + 0x90);
  uVar12 = FUN_1402d82e0("delete_station","Delete station");
  cVar8 = (*pcVar1)(param_2,uVar12,0);
  if (cVar8 != '\0') {
    *(undefined1 *)param_1[0xc] = 1;
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41000000;
  *(undefined1 *)(param_2 + 6) = 1;
  (**(code **)(*param_2 + 200))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  (**(code **)(*param_2 + 200))(param_2);
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  pcVar1 = *(code **)(*param_2 + 0x90);
  uVar12 = FUN_1402d82e0("open_in_station_editor","Open in station editor");
  cVar8 = (*pcVar1)(param_2,uVar12,0);
  if (cVar8 != '\0') {
    DAT_140b81940 = **(undefined8 **)param_1[4];
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  cVar8 = (**(code **)(*param_2 + 0x170))(param_2);
  if (cVar8 != '\0') {
    DAT_140be285e = *(char *)param_1[2] != *(char *)param_1[7];
  }
  return;
}


// Incoming references
// 0xc391c4 DATA caller none
// 0x7ce217 UNCONDITIONAL_CALL caller 1407cde80
// 0x7ce85f UNCONDITIONAL_CALL caller 1407cde80
