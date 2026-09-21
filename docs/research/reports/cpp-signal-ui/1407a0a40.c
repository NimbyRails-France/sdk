// Candidate VA 1407a0a40; RVA 0x7a0a40
// Ghidra inferred prototype: undefined FUN_1407a0a40()
// Entry bytes: 48 8b c4 55 53 56 57 41 54 41 55 41 56 41 57 48

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1407a0a40(undefined8 *param_1,longlong *param_2)

{
  undefined1 *puVar1;
  code *pcVar2;
  char *****pppppcVar3;
  void *pvVar4;
  size_t sVar5;
  ulonglong uVar6;
  char ******ppppppcVar7;
  size_t _Size;
  undefined1 uVar8;
  char cVar9;
  char cVar10;
  int iVar11;
  uint uVar12;
  undefined8 uVar13;
  longlong *plVar14;
  undefined8 *puVar15;
  undefined8 *******pppppppuVar16;
  longlong *plVar17;
  char *******pppppppcVar18;
  undefined8 uVar19;
  char *pcVar20;
  void *pvVar21;
  undefined8 *******_Buf1;
  char *******pppppppcVar22;
  char *pcVar23;
  ulonglong uVar24;
  undefined4 *puVar25;
  char ******ppppppcVar26;
  char ******ppppppcVar27;
  longlong lVar28;
  longlong lVar29;
  int iVar30;
  longlong *plVar31;
  uint local_res8;
  uint uStackX_c;
  char ******local_res10;
  char ******local_res18;
  char *******local_res20;
  undefined4 uVar33;
  char ****ppppcVar32;
  undefined8 in_stack_fffffffffffffdd0;
  ulonglong uVar34;
  char *******local_228;
  longlong lStack_220;
  size_t local_218;
  char ******ppppppcStack_210;
  undefined8 *******local_208;
  undefined8 uStack_200;
  size_t local_1f8;
  ulonglong uStack_1f0;
  undefined8 *******local_1e8;
  undefined8 uStack_1e0;
  size_t local_1d8;
  ulonglong uStack_1d0;
  char *******local_1c8;
  undefined8 uStack_1c0;
  size_t local_1b8;
  char ******ppppppcStack_1b0;
  longlong *local_1a8;
  undefined4 *local_1a0;
  longlong *local_198;
  char *******local_190;
  char *******local_188;
  undefined8 uStack_180;
  ulonglong local_178;
  ulonglong uStack_170;
  longlong *local_168;
  undefined4 *local_160;
  longlong *local_158;
  void *local_150;
  undefined8 uStack_148;
  undefined8 local_140;
  ulonglong uStack_138;
  ulonglong local_130 [2];
  size_t local_120;
  ulonglong uStack_118;
  undefined8 *******local_110;
  longlong lStack_108;
  longlong local_100;
  ulonglong uStack_f8;
  undefined8 *******local_f0 [3];
  ulonglong local_d8;
  undefined1 local_d0;
  undefined7 uStack_cf;
  size_t local_c0;
  ulonglong uStack_b8;
  undefined1 local_b0 [40];
  undefined1 local_88;
  undefined7 uStack_87;
  size_t local_78;
  ulonglong uStack_70;

  uVar12 = (uint)((ulonglong)in_stack_fffffffffffffdd0 >> 0x20);
  FUN_14055b8e0(param_2,&DAT_140b8df60);
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  (**(code **)(*param_2 + 8))(param_2);
  cVar9 = DAT_140b5ecac;
  local_228 = (char *******)0x0;
  lStack_220 = 0;
  local_218 = 0;
  ppppppcStack_210 = (char ******)0x0;
  lVar28 = -1;
  do {
    lVar28 = lVar28 + 1;
  } while (*(char *)(*(longlong *)param_1[2] + lVar28) != '\0');
  FUN_140002c00(&local_228);
  puVar1 = (undefined1 *)*param_1;
  uVar8 = FUN_1405cc370(param_1[1],param_2,&local_228,cVar9);
  *puVar1 = uVar8;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined4 *)((longlong)param_2 + 0x3c) = 0x40800000;
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  pcVar2 = *(code **)(*param_2 + 0x80);
  cVar9 = *(char *)(param_1[3] + 0x2468);
  uVar13 = *(undefined8 *)param_1[1];
  uStackX_c = uStackX_c & 0xffffff00;
  local_228 = (char *******)0x0;
  lStack_220 = 0;
  local_218 = 0;
  ppppppcStack_210 = (char ******)0x0;
  local_228 = (char *******)FUN_140003270(0x20);
  uVar19 = s_icon_add_signal_svg_140a9f680._8_8_;
  local_218 = _DAT_140aaca50;
  ppppppcStack_210 = _UNK_140aaca58;
  *local_228 = (char ******)s_icon_add_signal_svg_140a9f680._0_8_;
  local_228[1] = (char ******)uVar19;
  *(undefined2 *)(local_228 + 2) = s_icon_add_signal_svg_140a9f680._16_2_;
  *(char *)((longlong)local_228 + 0x12) = s_icon_add_signal_svg_140a9f680[0x12];
  *(char *)((longlong)local_228 + 0x13) = '\0';
  uVar13 = FUN_14081dff0(uVar13,local_b0,&local_228);
  cVar9 = (*pcVar2)(param_2,uVar13,-(cVar9 != '\0') & 2);
  if ((char ******)0xf < ppppppcStack_210) {
    pppppppcVar22 = local_228;
    if ((0xfff < (longlong)ppppppcStack_210 + 1U) &&
       (pppppppcVar22 = (char *******)local_228[-1],
       (char *)0x1f < (char *)((longlong)local_228 + (-8 - (longlong)pppppppcVar22)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppppcVar22);
  }
  if (cVar9 != '\0') {
    *(bool *)(param_1[3] + 0x2468) = *(char *)(param_1[3] + 0x2468) == '\0';
  }
  pcVar2 = *(code **)(*param_2 + 0xc0);
  uVar13 = FUN_1402d82e0("signal_editor_save_template","Save as template");
  (*pcVar2)(param_2,uVar13);
  (**(code **)(*param_2 + 0x18))(param_2);
  if (DAT_140b5ecac == '\0') {
LAB_1407a258c:
    FUN_1405a1f30(param_1[3] + 0x21b8,param_1[1],param_1[0x15],param_2);
    if (*(int *)param_1[0x16] == 0) {
      return;
    }
    *(undefined4 *)((longlong)param_2 + 0x34) = 0;
    *(undefined1 *)(param_2 + 6) = 1;
    FUN_14055b8e0(param_2,&DAT_140b8dfe0);
    (**(code **)(*param_2 + 8))(param_2);
    *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41c00000;
    *(undefined1 *)(param_2 + 5) = 1;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    *(undefined1 *)(param_2 + 7) = 1;
    *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
    *(undefined8 *)((longlong)param_2 + 0x44) = 0x40800000;
    pcVar2 = *(code **)(*param_2 + 0x100);
    uVar13 = *(undefined8 *)param_1[1];
    uStackX_c = uStackX_c & 0xffffff00;
    local_228 = (char *******)0x0;
    lStack_220 = 0;
    local_218 = 0;
    ppppppcStack_210 = (char ******)0x0;
    local_228 = (char *******)FUN_140003270(0x20);
    uVar19 = s_icon_alert_outline_svg_140a83cd8._8_8_;
    local_218 = _DAT_140aaca80;
    ppppppcStack_210 = _UNK_140aaca88;
    *local_228 = (char ******)s_icon_alert_outline_svg_140a83cd8._0_8_;
    local_228[1] = (char ******)uVar19;
    *(undefined4 *)(local_228 + 2) = s_icon_alert_outline_svg_140a83cd8._16_4_;
    *(undefined2 *)((longlong)local_228 + 0x14) = s_icon_alert_outline_svg_140a83cd8._20_2_;
    *(char *)((longlong)local_228 + 0x16) = '\0';
    uVar13 = FUN_14081dff0(uVar13,local_b0,&local_228);
    (*pcVar2)(param_2,uVar13);
    if ((char ******)0xf < ppppppcStack_210) {
      pppppppcVar22 = local_228;
      if ((0xfff < (longlong)ppppppcStack_210 + 1U) &&
         (pppppppcVar22 = (char *******)local_228[-1],
         (char *)0x1f < (char *)((longlong)local_228 + (-8 - (longlong)pppppppcVar22)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppppcVar22);
    }
    iVar11 = *(int *)param_1[0x16];
    if ((iVar11 == 2) || (iVar11 == 3)) {
      pcVar23 =
      "The signal position in the track is too close to another track. Move it away from overlapping tracks."
      ;
      pcVar20 = "signal_warn_too_close_desc";
    }
    else if (iVar11 == 1) {
      pcVar23 =
      "The block of track protected by this signal is too long. Introduce more signals in the block, like balises, to make it shorter."
      ;
      pcVar20 = "signal_warn_max_length_desc";
    }
    else {
      if (iVar11 != 4) goto LAB_1407a2755;
      pcVar23 = "One way signals at platform dead ends are ignored";
      pcVar20 = "signal_warn_ignored_plat_desc";
    }
  }
  else {
    if (*(int *)param_1[4] != -1) {
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      (**(code **)(*param_2 + 8))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_2 + 0xa8);
      uVar13 = FUN_1402d82e0("signal_type","Type:");
      (*pcVar2)(param_2,uVar13,0x14);
      *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41e00000;
      *(undefined1 *)(param_2 + 5) = 1;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x41e00000;
      *(undefined1 *)(param_2 + 6) = 1;
      *(undefined1 *)(param_2 + 7) = 1;
      *(undefined4 *)((longlong)param_2 + 0x3c) = 0x40800000;
      *(undefined4 *)(param_2 + 8) = 0;
      *(undefined4 *)((longlong)param_2 + 0x44) = 0;
      *(undefined4 *)(param_2 + 9) = 0;
      local_res20 = (char *******)(param_1 + 6);
      uVar34 = (ulonglong)uVar12 << 0x20;
      uVar33 = 0;
      FUN_1407388b0(param_1[1],param_2,*local_res20,*(undefined4 *)(param_1[7] + 0x30),0,uVar34);
      FUN_14055b8e0(param_2,&DAT_140b8e0e0);
      iVar11 = (**(code **)(*param_2 + 0x120))(param_2,param_1[8],*(undefined4 *)param_1[4]);
      if (iVar11 != *(int *)param_1[4]) {
        *(undefined1 *)param_1[9] = 1;
        *(undefined4 *)(param_1[10] + 0x30) =
             *(undefined4 *)(*(longlong *)param_1[0xb] + (longlong)iVar11 * 4);
        local_res20 = (char *******)(param_1 + 6);
      }
      local_res18 = (char ******)(param_1 + 7);
      plVar31 = param_2 + 9;
      plVar17 = param_2 + 8;
      puVar25 = (undefined4 *)((longlong)param_2 + 0x44);
      (**(code **)(*param_2 + 0x18))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      (**(code **)(*param_2 + 8))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_2 + 0xa8);
      uVar13 = FUN_1402d82e0("signal_name_edit","Name:");
      (*pcVar2)(param_2,uVar13,0x14);
      ppppppcVar27 = (char ******)(param_1 + 7);
      local_1a8 = plVar17;
      local_1a0 = puVar25;
      local_198 = plVar31;
      if (*(char *)(*ppppppcVar27 + 1) == '\0') {
        local_188 = (char *******)(param_1 + 6);
        local_res10 = ppppppcVar27;
        if (DAT_140be285c != '\0') {
          local_168 = plVar17;
          local_160 = puVar25;
          local_158 = plVar31;
          (**(code **)(*param_2 + 0xe8))(param_2,0xa62);
          local_res10 = local_res18;
        }
        local_158 = param_2 + 9;
        local_160 = (undefined4 *)((longlong)param_2 + 0x44);
        local_168 = param_2 + 8;
        local_188 = (char *******)(param_1 + 6);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        uVar13 = FUN_140572730(param_2,&local_208,**ppppppcVar27,"signal_user_name");
        FUN_140822c70(uVar13,param_1[10] + 0x10,0x1e,0,CONCAT44(uVar33,0xa62));
        local_res18 = local_res10;
        local_190 = (char *******)(param_1 + 6);
        cVar9 = FUN_14018ecf0(*ppppppcVar27 + 2,param_1[10] + 0x10);
        if (cVar9 == '\0') {
          *(undefined1 *)param_1[9] = 1;
          local_res18 = local_res10;
          local_1a8 = local_168;
          local_1a0 = local_160;
          local_198 = local_158;
          local_190 = local_188;
        }
      }
      else {
        FUN_1403a7830(*ppppppcVar27,local_f0);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        (**(code **)(*param_2 + 0xa0))(param_2,local_f0,0x11);
        FUN_140002d30(local_f0);
        local_190 = (char *******)(param_1 + 6);
      }
      local_res8 = (uint)*(byte *)(*local_res18 + 1);
      *(undefined4 *)((longlong)param_2 + 0x2c) = 0x428c0000;
      *(undefined1 *)(param_2 + 5) = 1;
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x140;
      pcVar2 = *(code **)(*param_2 + 0xf0);
      uVar13 = FUN_1402d82e0("signal_name_auto",&DAT_140a57948);
      (*pcVar2)(param_2,uVar13,&local_res8);
      if (local_res8 != *(byte *)(*local_res18 + 1)) {
        *(bool *)(param_1[10] + 8) = local_res8 != 0;
        *(undefined1 *)param_1[9] = 1;
        local_res20 = local_190;
        puVar25 = local_1a0;
        plVar17 = local_1a8;
        plVar31 = local_198;
      }
      (**(code **)(*param_2 + 0x18))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      (**(code **)(*param_2 + 8))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_2 + 0xa8);
      uVar13 = FUN_1402d82e0("signal_appearance","Appearance:");
      (*pcVar2)(param_2,uVar13,0x14);
      ppppppcVar27 = local_res18;
      *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41e00000;
      *(undefined1 *)(param_2 + 5) = 1;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x41e00000;
      *(undefined1 *)(param_2 + 6) = 1;
      *(undefined1 *)(param_2 + 7) = 1;
      *(undefined4 *)((longlong)param_2 + 0x3c) = 0x40800000;
      *(undefined4 *)plVar17 = 0;
      *puVar25 = 0;
      *(undefined4 *)plVar31 = 0;
      uVar34 = uVar34 & 0xffffffff00000000;
      ppppcVar32 = (*local_res18)[7];
      FUN_1407388b0(param_1[1],param_2,*local_res20,*(undefined4 *)(*local_res18 + 6),ppppcVar32,
                    uVar34);
      FUN_14055b8e0(param_2,&DAT_140b8e0e0);
      plVar17 = (longlong *)param_1[0xc];
      if (0xf < (ulonglong)plVar17[3]) {
        plVar17 = (longlong *)*plVar17;
      }
      uVar24 = (ulonglong)ppppcVar32 & 0xffffffff00000000;
      cVar9 = (**(code **)(*param_2 + 0x68))(param_2,plVar17,0,DAT_140aac474,uVar24);
      uVar6 = _UNK_140aac918;
      sVar5 = _DAT_140aac910;
      uVar33 = (undefined4)(uVar24 >> 0x20);
      if (cVar9 != '\0') {
        plVar14 = (longlong *)(**(code **)(*param_2 + 0x20))(param_2);
        *(undefined4 *)((longlong)plVar14 + 0x2c) = 0x43980000;
        *(undefined1 *)(plVar14 + 5) = 1;
        *(undefined1 *)(plVar14 + 4) = 1;
        *(undefined4 *)((longlong)plVar14 + 0x24) = 3;
        (**(code **)(*plVar14 + 8))(plVar14);
        plVar17 = *(longlong **)param_1[0xd];
        plVar31 = (longlong *)((longlong *)param_1[0xd])[1];
        if (plVar17 != plVar31) {
          uVar8 = (undefined1)local_res8;
          do {
            local_res8 = (uint)(*(longlong *)param_1[0xe] == *plVar17);
            *(undefined4 *)((longlong)plVar14 + 0x34) = 0x41f00000;
            *(undefined1 *)(plVar14 + 6) = 1;
            *(undefined1 *)(plVar14 + 7) = 1;
            *(undefined8 *)((longlong)plVar14 + 0x3c) = 0x40800000;
            *(undefined8 *)((longlong)plVar14 + 0x44) = 0x40800000;
            *(undefined1 *)(plVar14 + 3) = 1;
            *(undefined4 *)((longlong)plVar14 + 0x1c) = 0xa0;
            *(undefined1 *)(plVar14 + 4) = 1;
            *(undefined4 *)((longlong)plVar14 + 0x24) = 2;
            FUN_14028fc90(&local_d0,**ppppppcVar27,0);
            uVar13 = FUN_140290400(&local_88,*plVar17);
            puVar15 = (undefined8 *)FUN_1400254f0(uVar13,&DAT_140a5b24c,1);
            local_150 = (void *)*puVar15;
            uStack_148 = puVar15[1];
            local_140 = puVar15[2];
            uStack_138 = puVar15[3];
            *(undefined1 *)puVar15 = 0;
            puVar15[2] = 0;
            puVar15[3] = 0xf;
            FUN_14029c760(&local_1e8,uVar8);
            if (0xf < uStack_138) {
              pvVar21 = local_150;
              if ((0xfff < uStack_138 + 1) &&
                 (pvVar21 = *(void **)((longlong)local_150 + -8),
                 0x1f < (ulonglong)((longlong)local_150 + (-8 - (longlong)pvVar21)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar21);
            }
            if (0xf < uStack_70) {
              pvVar4 = (void *)CONCAT71(uStack_87,local_88);
              pvVar21 = pvVar4;
              if ((0xfff < uStack_70 + 1) &&
                 (pvVar21 = *(void **)((longlong)pvVar4 + -8),
                 0x1f < (ulonglong)((longlong)pvVar4 + (-8 - (longlong)pvVar21)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar21);
            }
            local_78 = sVar5;
            uStack_70 = uVar6;
            local_88 = 0;
            if (0xf < uStack_b8) {
              pvVar4 = (void *)CONCAT71(uStack_cf,local_d0);
              pvVar21 = pvVar4;
              if ((0xfff < uStack_b8 + 1) &&
                 (pvVar21 = *(void **)((longlong)pvVar4 + -8),
                 0x1f < (ulonglong)((longlong)pvVar4 + (-8 - (longlong)pvVar21)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar21);
            }
            uVar12 = local_res8;
            local_c0 = sVar5;
            uStack_b8 = uVar6;
            local_d0 = 0;
            pppppppuVar16 = &local_1e8;
            if (0xf < uStack_1d0) {
              pppppppuVar16 = local_1e8;
            }
            (**(code **)(*plVar14 + 0x10))(plVar14,0,&local_res8,pppppppuVar16);
            *(undefined4 *)((longlong)plVar14 + 0x2c) = 0x41e00000;
            *(undefined1 *)(plVar14 + 5) = 1;
            *(undefined4 *)((longlong)plVar14 + 0x34) = 0x41e00000;
            *(undefined1 *)(plVar14 + 6) = 1;
            *(undefined1 *)(plVar14 + 7) = 1;
            *(undefined8 *)((longlong)plVar14 + 0x3c) = 0x40800000;
            *(undefined8 *)((longlong)plVar14 + 0x44) = 0x40800000;
            uVar34 = uVar34 & 0xffffffff00000000;
            lVar28 = *plVar17;
            FUN_1407388b0(param_1[1],plVar14,*local_res20,*(undefined4 *)(*local_res18 + 6),lVar28,
                          uVar34);
            uVar33 = (undefined4)((ulonglong)lVar28 >> 0x20);
            *(undefined4 *)((longlong)plVar14 + 0x34) = 0x41e00000;
            *(undefined1 *)(plVar14 + 6) = 1;
            *(undefined1 *)(plVar14 + 3) = 1;
            *(undefined4 *)((longlong)plVar14 + 0x1c) = 0xa0;
            (**(code **)(*plVar14 + 0xa0))(plVar14,plVar17 + 1);
            (**(code **)(*plVar14 + 0x18))(plVar14);
            if ((local_res8 != 0) && (uVar12 != local_res8)) {
              *(longlong *)(param_1[10] + 0x38) = *plVar17;
              *(undefined1 *)param_1[9] = 1;
            }
            if (0xf < uStack_1d0) {
              pppppppuVar16 = local_1e8;
              if ((0xfff < uStack_1d0 + 1) &&
                 (pppppppuVar16 = (undefined8 *******)local_1e8[-1],
                 0x1f < (ulonglong)((longlong)local_1e8 + (-8 - (longlong)pppppppuVar16)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pppppppuVar16);
            }
            plVar17 = plVar17 + 5;
            ppppppcVar27 = local_res18;
          } while (plVar17 != plVar31);
        }
        (**(code **)(*plVar14 + 0x18))(plVar14);
        (**(code **)(*param_2 + 0x28))(param_2);
        (**(code **)(*param_2 + 0x70))(param_2);
        ppppppcVar27 = (char ******)(param_1 + 7);
        local_res18 = ppppppcVar27;
      }
      (**(code **)(*param_2 + 0x18))(param_2);
      ppppppcVar26 = local_res18;
      if (*(char *)(param_1[5] + 0x30) != '\0') {
        FUN_14055b8e0(param_2,&DAT_140b8dfe0);
        (**(code **)(*param_2 + 8))(param_2);
        FUN_14055b8e0(param_2,&DAT_140b8e060);
        pcVar2 = *(code **)(*param_2 + 0xa8);
        uVar13 = FUN_1402d82e0("signal_direction","Direction:");
        (*pcVar2)(param_2,uVar13,0x14);
        cVar9 = *(char *)(*ppppppcVar27 + 10);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        pcVar2 = *(code **)(*param_2 + 0x90);
        uVar13 = FUN_1402d82e0("signal_flip",&DAT_140a9ecf0);
        cVar10 = (*pcVar2)(param_2,uVar13,0);
        iVar11 = -(int)cVar9;
        if (cVar10 == '\0') {
          iVar11 = (int)cVar9;
        }
        if (iVar11 != *(char *)(*local_res18 + 10)) {
          *(undefined1 *)param_1[9] = 1;
          *(char *)(param_1[10] + 0x50) = (char)iVar11;
        }
        (**(code **)(*param_2 + 0x18))(param_2);
        ppppppcVar26 = (char ******)(param_1 + 7);
      }
      ppppppcVar27 = ppppppcVar26;
      if (*(char *)(param_1[5] + 0x31) != '\0') {
        FUN_14055b8e0(param_2,&DAT_140b8dfe0);
        (**(code **)(*param_2 + 8))(param_2);
        FUN_14055b8e0(param_2,&DAT_140b8e060);
        pcVar2 = *(code **)(*param_2 + 0xa8);
        uVar13 = FUN_1402d82e0("signal_limits_block","Bounds other signals:");
        (*pcVar2)(param_2,uVar13,0x14);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        ppppppcVar7 = local_res18;
        uVar12 = (**(code **)(*param_2 + 0x120))
                           (param_2,param_1[0xf],*(undefined1 *)((longlong)*local_res18 + 0x6c));
        if (uVar12 != *(byte *)((longlong)*ppppppcVar7 + 0x6c)) {
          *(undefined1 *)param_1[9] = 1;
          *(bool *)(param_1[10] + 0x6c) = uVar12 != 0;
          ppppppcVar27 = (char ******)(param_1 + 7);
        }
        (**(code **)(*param_2 + 0x18))(param_2);
      }
      if (*(int *)param_1[5] == 4) {
        FUN_14055b8e0(param_2,&DAT_140b8dfe0);
        (**(code **)(*param_2 + 8))(param_2);
        FUN_14055b8e0(param_2,&DAT_140b8e060);
        (**(code **)(*param_2 + 200))(param_2);
        local_res8 = (uint)*(byte *)((longlong)*ppppppcVar26 + 0x6d);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        pcVar2 = *(code **)(*param_2 + 0xf0);
        uVar13 = FUN_1402d82e0("signal_check_beyond_stops","Check beyond stops");
        (*pcVar2)(param_2,uVar13,&local_res8);
        if (local_res8 != *(byte *)((longlong)*ppppppcVar26 + 0x6d)) {
          *(undefined1 *)param_1[9] = 1;
          *(bool *)(param_1[10] + 0x6d) = local_res8 != 0;
        }
        (**(code **)(*param_2 + 0x18))(param_2);
        ppppppcVar26 = ppppppcVar27;
      }
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      (**(code **)(*param_2 + 8))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_2 + 0xa8);
      uVar13 = FUN_1402d82e0("signal_cosmetic","Cosmetic:");
      (*pcVar2)(param_2,uVar13,0x14);
      pppppcVar3 = *ppppppcVar26;
      iVar11 = *(int *)(pppppcVar3 + 0xb);
      uVar12 = *(uint *)(pppppcVar3 + 0xc);
      iVar30 = *(int *)((longlong)pppppcVar3 + 0x5c);
      local_res8 = iVar30;
      FUN_14055b8e0(param_2,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_2 + 0x90);
      uVar13 = FUN_1402d82e0("signal_toggle_side",&DAT_140a9f81c);
      cVar9 = (*pcVar2)(param_2,uVar13,0);
      if (cVar9 != '\0') {
        iVar11 = iVar11 + 2;
        iVar11 = iVar11 + (iVar11 / 3 + (iVar11 >> 0x1f) +
                          (int)(((longlong)iVar11 / 3 + ((longlong)iVar11 >> 0x3f) & 0xffffffffU) >>
                               0x1f)) * -3 + -1;
      }
      FUN_14055b8e0(param_2,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_2 + 0x90);
      uVar13 = FUN_1402d82e0("signal_toggle_rotate","Rotate");
      cVar9 = (*pcVar2)(param_2,uVar13,0);
      if ((cVar9 != '\0') && (uVar12 = uVar12 + 1 & 0x80000003, (int)uVar12 < 0)) {
        uVar12 = (uVar12 - 1 | 0xfffffffc) + 1;
      }
      FUN_14055b8e0(param_2,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_2 + 0x90);
      uVar13 = FUN_1402d82e0("signal_toggle_size",&DAT_140a8d2b0);
      cVar9 = (*pcVar2)(param_2,uVar13,0);
      if (cVar9 != '\0') {
        iVar30 = (iVar30 + 1) % 5;
        local_res8 = iVar30;
      }
      pppppcVar3 = *ppppppcVar26;
      if (((iVar11 != *(int *)(pppppcVar3 + 0xb)) || (uVar12 != *(uint *)(pppppcVar3 + 0xc))) ||
         (iVar30 != *(int *)((longlong)pppppcVar3 + 0x5c))) {
        *(undefined1 *)param_1[9] = 1;
        plVar17 = param_1 + 10;
        *(int *)(*plVar17 + 0x58) = iVar11;
        *(uint *)(*plVar17 + 0x60) = uVar12;
        *(uint *)(*plVar17 + 0x5c) = local_res8;
      }
      (**(code **)(*param_2 + 0x18))(param_2);
      if (*(char *)(param_1[5] + 0x35) != '\0') {
        FUN_14055b8e0(param_2,&DAT_140b8dfe0);
        (**(code **)(*param_2 + 8))(param_2);
        FUN_14055b8e0(param_2,&DAT_140b8e060);
        pcVar2 = *(code **)(*param_2 + 0xa8);
        uVar13 = FUN_1402d82e0("signal_alert_opts","Alert on:");
        (*pcVar2)(param_2,uVar13,0x14);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        uVar12 = (**(code **)(*param_2 + 0x120))
                           (param_2,param_1[0x10],*(undefined1 *)(param_1[7] + 100));
        if (uVar12 != *(byte *)(param_1[7] + 100)) {
          *(undefined1 *)param_1[9] = 1;
          *(bool *)(param_1[10] + 100) = uVar12 != 0;
        }
        (**(code **)(*param_2 + 0x18))(param_2);
        if (*(char *)(param_1[7] + 100) != '\0') {
          FUN_14055b8e0(param_2,&DAT_140b8dfe0);
          (**(code **)(*param_2 + 8))(param_2);
          FUN_14028fde0(&local_208,*(int *)(param_1[7] + 0x68) / 0x3c);
          FUN_140019ca0(&local_1e8,&local_208);
          FUN_14055b8e0(param_2,&DAT_140b8e060);
          pcVar2 = *(code **)(*param_2 + 0xa8);
          uVar13 = FUN_1402d82e0("alert_after","After:");
          (*pcVar2)(param_2,uVar13,0x14);
          *(undefined1 *)(param_2 + 4) = 1;
          *(undefined4 *)((longlong)param_2 + 0x24) = 2;
          FUN_14055b8e0(param_2,&DAT_140b8e0e0);
          (**(code **)(*param_2 + 8))(param_2);
          *(undefined4 *)((longlong)param_2 + 0x2c) = 0x42200000;
          *(undefined1 *)(param_2 + 5) = 1;
          *(undefined1 *)(param_2 + 3) = 1;
          *(undefined4 *)((longlong)param_2 + 0x1c) = 0x140;
          (**(code **)(*param_2 + 0xe0))(param_2,&local_208,0xff,0x20,CONCAT44(uVar33,0x260));
          *(undefined1 *)(param_2 + 3) = 1;
          *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
          *(undefined1 *)(param_2 + 7) = 1;
          *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
          *(undefined8 *)((longlong)param_2 + 0x44) = 0;
          pcVar2 = *(code **)(*param_2 + 0xa8);
          uVar13 = FUN_1402d82e0("minutes","minutes");
          (*pcVar2)(param_2,uVar13,0x11);
          (**(code **)(*param_2 + 0x18))(param_2);
          pppppppuVar16 = &local_1e8;
          if (0xf < uStack_1d0) {
            pppppppuVar16 = local_1e8;
          }
          _Buf1 = &local_208;
          if (0xf < uStack_1f0) {
            _Buf1 = local_208;
          }
          if ((local_1f8 != local_1d8) ||
             ((local_1f8 != 0 && (iVar11 = memcmp(_Buf1,pppppppuVar16,local_1f8), iVar11 != 0)))) {
            *(undefined1 *)param_1[9] = 1;
            local_res20 = (char *******)CONCAT44(local_res20._4_4_,0x5a0);
            local_res18 = (char ******)((ulonglong)local_res18 & 0xffffffff00000000);
            pppppppuVar16 = &local_208;
            if (0xf < uStack_1f0) {
              pppppppuVar16 = local_208;
            }
            local_res8 = 0;
            FUN_1402ab9c0(&local_188,pppppppuVar16,local_1f8 + (longlong)pppppppuVar16,&local_res8);
            pppppppcVar22 = &local_res18;
            if (-1 < (int)local_res8) {
              pppppppcVar22 = (char *******)&local_res8;
            }
            pppppppcVar18 = (char *******)&local_res20;
            if ((int)local_res8 < 0x5a1) {
              pppppppcVar18 = pppppppcVar22;
            }
            *(int *)(param_1[10] + 0x68) = *(int *)pppppppcVar18 * 0x3c;
          }
          FUN_140002d30(&local_1e8);
          FUN_140002d30(&local_208);
          (**(code **)(*param_2 + 0x18))(param_2);
        }
      }
      if (*(char *)(param_1[5] + 0x34) != '\0') {
        FUN_14055b8e0(param_2,&DAT_140b8dfe0);
        (**(code **)(*param_2 + 8))(param_2);
        FUN_14055b8e0(param_2,&DAT_140b8e060);
        pcVar2 = *(code **)(*param_2 + 0xa8);
        uVar13 = FUN_1402d82e0("signal_filter_label","By default:");
        (*pcVar2)(param_2,uVar13,0x14);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        iVar11 = (**(code **)(*param_2 + 0x120))
                           (param_2,param_1[0x11],*(int *)(param_1[7] + 0x70) != 0);
        if ((uint)(iVar11 != 0) != *(uint *)(param_1[7] + 0x70)) {
          *(undefined1 *)param_1[9] = 1;
          *(uint *)(param_1[10] + 0x70) = (uint)(iVar11 != 0);
        }
        (**(code **)(*param_2 + 0x18))(param_2);
        *(undefined1 *)(param_2 + 7) = 1;
        *(undefined4 *)((longlong)param_2 + 0x3c) = 0;
        param_2[8] = 0x40800000;
        *(undefined4 *)(param_2 + 9) = 0;
        *(undefined4 *)((longlong)param_2 + 0x34) = 0x41c00000;
        *(undefined1 *)(param_2 + 6) = 1;
        *(undefined1 *)(param_2 + 3) = 1;
        *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
        *(undefined1 *)(param_2 + 4) = 1;
        *(undefined4 *)((longlong)param_2 + 0x24) = 2;
        (**(code **)(*param_2 + 8))(param_2);
        *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41c00000;
        *(undefined1 *)(param_2 + 5) = 1;
        *(undefined4 *)((longlong)param_2 + 0x34) = 0x41c00000;
        *(undefined1 *)(param_2 + 6) = 1;
        *(undefined1 *)(param_2 + 7) = 1;
        *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
        *(undefined8 *)((longlong)param_2 + 0x44) = 0x40800000;
        pcVar2 = *(code **)(*param_2 + 0x80);
        uVar13 = *(undefined8 *)param_1[1];
        uStackX_c = uStackX_c & 0xffffff00;
        pcVar20 = "icon_tri_right.svg";
        if (DAT_140be285d != '\0') {
          pcVar20 = "icon_tri_down.svg";
        }
        local_1e8 = (undefined8 *******)0x0;
        uStack_1e0 = 0;
        local_1d8 = 0;
        uStack_1d0 = 0;
        lVar28 = -1;
        do {
          lVar28 = lVar28 + 1;
        } while (pcVar20[lVar28] != '\0');
        FUN_140002c00(&local_1e8);
        uVar13 = FUN_14081dff0(uVar13,local_b0,&local_1e8);
        cVar9 = (*pcVar2)(param_2,uVar13,0);
        if (0xf < uStack_1d0) {
          pppppppuVar16 = local_1e8;
          if ((0xfff < uStack_1d0 + 1) &&
             (pppppppuVar16 = (undefined8 *******)local_1e8[-1],
             0x1f < (ulonglong)((longlong)local_1e8 + (-8 - (longlong)pppppppuVar16)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pppppppuVar16);
        }
        if (cVar9 != '\0') {
          *(bool *)param_1[0x12] = DAT_140be285d == '\0';
        }
        local_res10 = (char ******)((ulonglong)local_res10 & 0xffffffff00000000);
        *(undefined1 *)(param_2 + 3) = 1;
        *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
        lVar28 = FUN_1402d82e0("signal_filter_op_or",&DAT_140a50810);
        local_1c8 = (char *******)0x0;
        uStack_1c0 = 0;
        local_1b8 = 0;
        ppppppcStack_1b0 = (char ******)0x0;
        lVar29 = -1;
        do {
          lVar29 = lVar29 + 1;
        } while (*(char *)(lVar28 + lVar29) != '\0');
        FUN_140002c00(&local_1c8,lVar28);
        _Size = local_1b8;
        if (local_1b8 == 0x7fffffffffffffff) {
                    /* WARNING: Subroutine does not return */
          FUN_140001c70();
        }
        local_res20 = local_1c8;
        local_res18 = ppppppcStack_1b0;
        pppppppcVar22 = (char *******)&local_1c8;
        if ((char ******)0xf < ppppppcStack_1b0) {
          pppppppcVar22 = local_1c8;
        }
        local_208 = (undefined8 *******)0x0;
        uStack_200 = 0;
        local_1f8 = 0;
        uStack_1f0 = 0;
        uVar34 = local_1b8 + 1;
        uVar24 = 0xf;
        pppppppuVar16 = &local_208;
        if (0xf < uVar34) {
          uVar24 = uVar34 | 0xf;
          if (uVar24 < 0x8000000000000000) {
            if (uVar24 < 0x16) {
              uVar24 = 0x16;
            }
          }
          else {
            uVar24 = 0x7fffffffffffffff;
          }
          pppppppuVar16 = (undefined8 *******)FUN_140003270(uVar24 + 1);
          local_208 = pppppppuVar16;
        }
        local_1f8 = uVar34;
        uStack_1f0 = uVar24;
        *(char *)pppppppuVar16 = (char)DAT_140a4d470;
        memcpy((undefined1 *)((longlong)pppppppuVar16 + 1),pppppppcVar22,_Size);
        *(undefined1 *)((longlong)pppppppuVar16 + uVar34) = 0;
        plVar17 = (longlong *)FUN_1400254f0(&local_208,&DAT_140a4d470);
        local_228 = (char *******)*plVar17;
        lStack_220 = plVar17[1];
        local_218 = plVar17[2];
        ppppppcStack_210 = (char ******)plVar17[3];
        *(undefined1 *)plVar17 = 0;
        plVar17[2] = 0;
        plVar17[3] = 0xf;
        if (0xf < uStack_1f0) {
          pppppppuVar16 = local_208;
          if ((0xfff < uStack_1f0 + 1) &&
             (pppppppuVar16 = (undefined8 *******)local_208[-1],
             (undefined1 *)0x1f <
             (undefined1 *)((longlong)local_208 + (-8 - (longlong)pppppppuVar16)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pppppppuVar16);
        }
        local_1f8 = sVar5;
        uStack_1f0 = uVar6;
        local_208 = (undefined8 *******)((ulonglong)local_208 & 0xffffffffffffff00);
        if (_Size == 0x7fffffffffffffff) {
                    /* WARNING: Subroutine does not return */
          FUN_140001c70();
        }
        pppppppcVar22 = (char *******)&local_1c8;
        if ((char ******)0xf < local_res18) {
          pppppppcVar22 = local_res20;
        }
        local_188 = (char *******)0x0;
        uStack_180 = 0;
        local_178 = 0;
        uStack_170 = 0;
        uVar34 = _Size + 1;
        uVar24 = 0xf;
        pppppppcVar18 = (char *******)&local_188;
        if (0xf < uVar34) {
          uVar24 = uVar34 | 0xf;
          if (uVar24 < 0x8000000000000000) {
            if (uVar24 < 0x16) {
              uVar24 = 0x16;
            }
          }
          else {
            uVar24 = 0x7fffffffffffffff;
          }
          pppppppcVar18 = (char *******)FUN_140003270(uVar24 + 1);
          local_188 = pppppppcVar18;
        }
        local_178 = uVar34;
        uStack_170 = uVar24;
        *(char *)pppppppcVar18 = (char)DAT_140a4d470;
        memcpy((undefined1 *)((longlong)pppppppcVar18 + 1),pppppppcVar22,_Size);
        *(undefined1 *)((longlong)pppppppcVar18 + uVar34) = 0;
        plVar17 = (longlong *)FUN_1400254f0(&local_188,&DAT_140a4aefc);
        local_110 = (undefined8 *******)*plVar17;
        lStack_108 = plVar17[1];
        local_100 = plVar17[2];
        uStack_f8 = plVar17[3];
        *(undefined1 *)plVar17 = 0;
        plVar17[2] = 0;
        plVar17[3] = 0xf;
        if (0xf < uStack_170) {
          pppppppcVar22 = local_188;
          if ((0xfff < uStack_170 + 1) &&
             (pppppppcVar22 = (char *******)local_188[-1],
             (undefined1 *)0x1f <
             (undefined1 *)((longlong)local_188 + (-8 - (longlong)pppppppcVar22)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pppppppcVar22);
        }
        local_178 = sVar5;
        uStack_170 = uVar6;
        local_188 = (char *******)((ulonglong)local_188 & 0xffffffffffffff00);
        uVar13 = param_1[0x13];
        uVar19 = FUN_1402d82e0("signal_filter_none","<No exceptions>");
        pppppppcVar22 = (char *******)&local_228;
        if ((char ******)0xf < ppppppcStack_210) {
          pppppppcVar22 = local_228;
        }
        uVar13 = FUN_1403fd2d0(uVar13,local_b0,*(undefined8 *)(param_1[0x14] + 0x420),pppppppcVar22,
                               uVar19);
        local_130[1] = 0;
        local_120 = _DAT_140aac920;
        uStack_118 = _UNK_140aac928;
        local_130[0] = (ulonglong)DAT_140a4d470 & 0xffffffffffff00ff;
        lVar28 = FUN_1402d82e0("signal_filter_except","Except:");
        lVar29 = -1;
        do {
          lVar29 = lVar29 + 1;
        } while (*(char *)(lVar28 + lVar29) != '\0');
        puVar15 = (undefined8 *)FUN_14029cb50(local_130,0,lVar28,lVar29);
        local_150 = (void *)*puVar15;
        uStack_148 = puVar15[1];
        local_140 = puVar15[2];
        uStack_138 = puVar15[3];
        *(undefined1 *)puVar15 = 0;
        puVar15[2] = 0;
        puVar15[3] = 0xf;
        FUN_14029c760(local_f0,(undefined1)local_res8,&local_150,uVar13);
        if (0xf < uStack_138) {
          FUN_140003040(&local_150,local_150);
        }
        if (0xf < uStack_118) {
          FUN_140003040(local_130,local_130[0]);
        }
        local_120 = sVar5;
        uStack_118 = uVar6;
        local_130[0] = local_130[0] & 0xffffffffffffff00;
        FUN_140002d30(local_b0);
        pppppppuVar16 = local_f0;
        if (0xf < local_d8) {
          pppppppuVar16 = local_f0[0];
        }
        (**(code **)(*param_2 + 0xd8))(param_2,pppppppuVar16,0x11,&local_res10);
        uVar13 = param_1[0x13];
        uVar19 = FUN_1402d82e0("signal_filter_none","<No exceptions>");
        pppppppuVar16 = &local_110;
        if (0xf < uStack_f8) {
          pppppppuVar16 = local_110;
        }
        puVar15 = (undefined8 *)
                  FUN_1403fd2d0(uVar13,&local_d0,*(undefined8 *)(param_1[0x14] + 0x420),
                                pppppppuVar16,uVar19);
        if (0xf < (ulonglong)puVar15[3]) {
          puVar15 = (undefined8 *)*puVar15;
        }
        (**(code **)(*param_2 + 0xc0))(param_2,puVar15);
        FUN_140002d30(&local_d0);
        if ((int)local_res10 != 0) {
          *(bool *)param_1[0x12] = DAT_140be285d == '\0';
        }
        FUN_140002d30(local_f0);
        if (0xf < uStack_f8) {
          FUN_140003040(&local_110,local_110);
        }
        if ((char ******)0xf < ppppppcStack_210) {
          FUN_140003040(&local_228,local_228);
        }
        if ((char ******)0xf < local_res18) {
          FUN_140003040(&local_1c8,local_res20,local_res18);
        }
        (**(code **)(*param_2 + 0x18))(param_2);
        if (DAT_140be285d != '\0') {
          *(undefined4 *)((longlong)param_2 + 0x34) = 0x437a0000;
          *(undefined1 *)(param_2 + 3) = 1;
          *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
          *(undefined1 *)(param_2 + 6) = 1;
          FUN_14058dbe0(param_1[3] + 0x2088,param_1[1],param_2,
                        *(undefined8 *)(param_1[0x14] + 0x420),param_1[0x13],
                        *(undefined8 *)(param_1[0x14] + 0x1598));
        }
      }
      goto LAB_1407a258c;
    }
    if (*(char *)((longlong)param_1[5] + 0x36) != '\0') {
      return;
    }
    iVar11 = *(int *)param_1[5];
    if (iVar11 == 3) {
      *(undefined4 *)((longlong)param_2 + 0x34) = 0;
      *(undefined1 *)(param_2 + 6) = 1;
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      (**(code **)(*param_2 + 8))(param_2);
      lVar28 = FUN_14055a690(param_2 + 3);
      *(undefined1 *)(lVar28 + 0x20) = 1;
      *(undefined8 *)(lVar28 + 0x24) = 0;
      *(undefined8 *)(lVar28 + 0x2c) = 0x40800000;
      pcVar2 = *(code **)(*param_2 + 0x100);
      uVar13 = *(undefined8 *)param_1[1];
      uStackX_c = uStackX_c & 0xffffff00;
      local_1c8 = (char *******)0x0;
      uStack_1c0 = 0;
      local_1b8 = 0;
      ppppppcStack_1b0 = (char ******)0x0;
      local_1c8 = (char *******)FUN_140003270(0x20);
      uVar19 = s_icon_alert_outline_svg_140a83cd8._8_8_;
      local_1b8 = _DAT_140aaca80;
      ppppppcStack_1b0 = _UNK_140aaca88;
      *local_1c8 = (char ******)s_icon_alert_outline_svg_140a83cd8._0_8_;
      local_1c8[1] = (char ******)uVar19;
      *(undefined4 *)(local_1c8 + 2) = s_icon_alert_outline_svg_140a83cd8._16_4_;
      *(undefined2 *)((longlong)local_1c8 + 0x14) = s_icon_alert_outline_svg_140a83cd8._20_2_;
      *(char *)((longlong)local_1c8 + 0x16) = '\0';
      uVar13 = FUN_14081dff0(uVar13,local_b0,&local_1c8,CONCAT44(uStackX_c,local_res8));
      (*pcVar2)(param_2,uVar13,0xffffffff);
      if ((char ******)0xf < ppppppcStack_1b0) {
        FUN_140003040(&local_1c8,local_1c8);
      }
      pcVar23 =
      "This signal is still functional but it has been obsoleted by path signals. Replace your simple block signaling with path signals."
      ;
      pcVar20 = "signal_edit_simple_block_obsolete";
    }
    else {
      if (iVar11 != 1) {
        return;
      }
      *(undefined4 *)((longlong)param_2 + 0x34) = 0;
      *(undefined1 *)(param_2 + 6) = 1;
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      (**(code **)(*param_2 + 8))(param_2);
      lVar28 = FUN_14055a690(param_2 + 3);
      *(undefined1 *)(lVar28 + 0x20) = 1;
      *(undefined8 *)(lVar28 + 0x24) = 0;
      *(undefined8 *)(lVar28 + 0x2c) = 0x40800000;
      pcVar2 = *(code **)(*param_2 + 0x100);
      uVar13 = *(undefined8 *)param_1[1];
      uStackX_c = uStackX_c & 0xffffff00;
      local_228 = (char *******)0x0;
      lStack_220 = 0;
      local_218 = 0;
      ppppppcStack_210 = (char ******)0x0;
      local_228 = (char *******)FUN_140003270(0x20);
      uVar19 = s_icon_alert_outline_svg_140a83cd8._8_8_;
      local_218 = _DAT_140aaca80;
      ppppppcStack_210 = _UNK_140aaca88;
      *local_228 = (char ******)s_icon_alert_outline_svg_140a83cd8._0_8_;
      local_228[1] = (char ******)uVar19;
      *(undefined4 *)(local_228 + 2) = s_icon_alert_outline_svg_140a83cd8._16_4_;
      *(undefined2 *)((longlong)local_228 + 0x14) = s_icon_alert_outline_svg_140a83cd8._20_2_;
      *(char *)((longlong)local_228 + 0x16) = '\0';
      uVar13 = FUN_14081dff0(uVar13,local_b0,&local_228,CONCAT44(uStackX_c,local_res8));
      (*pcVar2)(param_2,uVar13,0xffffffff);
      FUN_140002d30(&local_228);
      pcVar23 =
      "This signal is still functional but it has been obsoleted. Some train maneuvers might be disabled in this track."
      ;
      pcVar20 = "signal_edit_plat_stop_obsolete";
    }
  }
  pcVar2 = *(code **)(*param_2 + 0xb8);
  FUN_1402d82e0(pcVar20,pcVar23);
  (*pcVar2)(param_2);
LAB_1407a2755:
  (**(code **)(*param_2 + 0x18))(param_2);
  return;
}


// Incoming references
// 0xc392c0 DATA caller none
// 0x7d63f7 UNCONDITIONAL_CALL caller 1407d6060
// 0x7d6a3f UNCONDITIONAL_CALL caller 1407d6060
