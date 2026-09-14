// Candidate VA 140673860; RVA 0x673860
// Ghidra inferred prototype: undefined FUN_140673860()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140673860(longlong param_1,undefined8 *param_2,longlong param_3,longlong *param_4)

{
  int *piVar1;
  code *pcVar2;
  bool bVar3;
  uint uVar4;
  char cVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  longlong lVar9;
  longlong *plVar10;
  undefined8 uVar11;
  ulonglong *puVar12;
  undefined8 *****pppppuVar13;
  char ****ppppcVar14;
  void *pvVar15;
  undefined1 *puVar16;
  char *****pppppcVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  undefined4 uVar20;
  longlong lVar21;
  byte bVar22;
  undefined8 **ppuVar23;
  float fVar24;
  undefined8 extraout_XMM0_Qa;
  undefined8 extraout_XMM0_Qa_00;
  undefined8 extraout_XMM0_Qa_01;
  undefined8 extraout_XMM0_Qa_02;
  undefined8 extraout_XMM0_Qa_03;
  undefined8 extraout_XMM0_Qa_04;
  undefined8 extraout_XMM0_Qa_05;
  undefined8 extraout_XMM0_Qa_06;
  undefined8 extraout_XMM0_Qa_07;
  undefined8 extraout_XMM0_Qa_08;
  undefined8 extraout_XMM0_Qa_09;
  undefined8 extraout_XMM0_Qa_10;
  undefined4 uVar25;
  uint local_res18;
  undefined4 uStackX_1c;
  undefined8 in_stack_fffffffffffff988;
  undefined4 uVar27;
  undefined8 uVar26;
  code *local_668;
  undefined8 **ppuStack_660;
  char ****local_658;
  undefined8 uStack_650;
  undefined8 local_648;
  ulonglong uStack_640;
  code *local_638;
  undefined8 **ppuStack_630;
  undefined4 local_628;
  uint uStack_624;
  int local_620;
  undefined8 *local_618;
  undefined8 uStack_610;
  undefined8 ****local_608;
  undefined8 uStack_600;
  longlong local_5f8;
  ulonglong uStack_5f0;
  undefined1 local_5e8 [40];
  char *local_5c0;
  char *local_5b8;
  char *local_5b0;
  char *local_5a8;
  char *local_5a0;
  ulonglong local_598;
  ulonglong uStack_590;
  ulonglong local_588;
  ulonglong uStack_580;
  ulonglong local_578;
  ulonglong uStack_570;
  ulonglong local_568;
  ulonglong uStack_560;
  ulonglong local_558;
  ulonglong uStack_550;
  ulonglong local_548;
  ulonglong uStack_540;
  ulonglong local_538;
  ulonglong uStack_530;
  ulonglong local_528;
  ulonglong uStack_520;
  ulonglong local_518;
  ulonglong uStack_510;
  ulonglong local_508;
  ulonglong uStack_500;
  ulonglong local_4f8;
  ulonglong uStack_4f0;
  ulonglong local_4e8;
  ulonglong uStack_4e0;
  void *local_4d8;
  undefined8 uStack_4d0;
  undefined8 local_4c8;
  ulonglong uStack_4c0;
  undefined1 local_4b8 [32];
  undefined1 local_498 [32];
  undefined1 local_478 [32];
  undefined1 local_458 [32];
  undefined1 local_438 [32];
  undefined1 local_418 [32];
  void *local_3f8;
  ulonglong local_3e0;
  undefined1 local_3d8 [32];
  undefined1 local_3b8 [32];
  undefined1 local_398 [32];
  undefined1 local_378 [32];
  undefined1 local_358 [32];
  undefined1 local_338 [32];
  undefined1 local_318 [32];
  undefined1 local_2f8 [32];
  undefined1 local_2d8 [32];
  undefined1 local_2b8 [32];
  undefined1 local_298 [32];
  undefined **local_278;
  undefined1 *local_270;
  undefined8 local_268;
  ulonglong local_260;
  undefined1 local_258 [544];
  
  uVar27 = (undefined4)((ulonglong)in_stack_fffffffffffff988 >> 0x20);
  uVar11 = 0;
  uVar18 = 0;
  if (((*(longlong *)(param_3 + 600) != 0) && (lVar9 = *(longlong *)(param_3 + 0x260), lVar9 != 0))
     && (*(char *)(lVar9 + 0x2114) != '\0')) {
    local_res18 = CONCAT31(local_res18._1_3_,1);
    if (*(int *)(lVar9 + 0x2118) == 10000) goto LAB_1406738cb;
  }
  local_res18 = local_res18 & 0xffffff00;
LAB_1406738cb:
  lVar9 = *(longlong *)(param_3 + 0x250);
  lVar21 = *(longlong *)(lVar9 + 0x28);
  local_5a0 = "icon_hourglass.svg";
  if (lVar21 != 0) {
    for (piVar1 = *(int **)(*(longlong *)(lVar9 + 0x4f0) +
                           SUB168((ZEXT816(0) << 0x40 | ZEXT816(3)) %
                                  ZEXT416(*(uint *)(lVar9 + 0x4f8)),0) * 8); piVar1 != (int *)0x0;
        piVar1 = *(int **)(piVar1 + 6)) {
      if (*piVar1 == 3) {
        if ((piVar1 != *(int **)(*(longlong *)(lVar9 + 0x4f0) + *(longlong *)(lVar9 + 0x4f8) * 8))
           && (*(ulonglong *)(piVar1 + 4) < *(ulonglong *)(piVar1 + 2))) {
          local_5c0 = "icon_hourglass.svg";
          goto LAB_140673955;
        }
        break;
      }
    }
  }
  local_5c0 = "icon_main_tracks.svg";
  if (lVar21 != 0) {
LAB_140673955:
    for (piVar1 = *(int **)(*(longlong *)(lVar9 + 0x4f0) +
                           SUB168((ZEXT816(0) << 0x40 | ZEXT816(4)) %
                                  ZEXT416(*(uint *)(lVar9 + 0x4f8)),0) * 8); piVar1 != (int *)0x0;
        piVar1 = *(int **)(piVar1 + 6)) {
      if (*piVar1 == 4) {
        if ((piVar1 != *(int **)(*(longlong *)(lVar9 + 0x4f0) + *(longlong *)(lVar9 + 0x4f8) * 8))
           && (*(ulonglong *)(piVar1 + 4) < *(ulonglong *)(piVar1 + 2))) {
          local_5b8 = "icon_hourglass.svg";
          goto LAB_140673999;
        }
        break;
      }
    }
  }
  local_5b8 = "icon_station.svg";
  if (lVar21 != 0) {
LAB_140673999:
    for (piVar1 = *(int **)(*(longlong *)(lVar9 + 0x4f0) +
                           SUB168((ZEXT816(0) << 0x40 | ZEXT816(5)) %
                                  ZEXT416(*(uint *)(lVar9 + 0x4f8)),0) * 8); piVar1 != (int *)0x0;
        piVar1 = *(int **)(piVar1 + 6)) {
      if (*piVar1 == 5) {
        if ((piVar1 != *(int **)(*(longlong *)(lVar9 + 0x4f0) + *(longlong *)(lVar9 + 0x4f8) * 8))
           && (*(ulonglong *)(piVar1 + 4) < *(ulonglong *)(piVar1 + 2))) {
          local_5b0 = "icon_hourglass.svg";
          goto LAB_1406739dd;
        }
        break;
      }
    }
  }
  local_5b0 = "icon_main_lines.svg";
  if (lVar21 != 0) {
LAB_1406739dd:
    for (piVar1 = *(int **)(*(longlong *)(lVar9 + 0x4f0) +
                           SUB168((ZEXT816(0) << 0x40 | ZEXT816(6)) %
                                  ZEXT416(*(uint *)(lVar9 + 0x4f8)),0) * 8); piVar1 != (int *)0x0;
        piVar1 = *(int **)(piVar1 + 6)) {
      if (*piVar1 == 6) {
        if ((piVar1 != *(int **)(*(longlong *)(lVar9 + 0x4f0) + *(longlong *)(lVar9 + 0x4f8) * 8))
           && (*(ulonglong *)(piVar1 + 4) < *(ulonglong *)(piVar1 + 2))) {
          local_5a8 = "icon_hourglass.svg";
          goto LAB_140673a26;
        }
        break;
      }
    }
  }
  local_5a8 = "icon_main_trains.svg";
  if (lVar21 != 0) {
LAB_140673a26:
    for (piVar1 = *(int **)(*(longlong *)(lVar9 + 0x4f0) +
                           SUB168((ZEXT816(0) << 0x40 | ZEXT816(7)) %
                                  ZEXT416(*(uint *)(lVar9 + 0x4f8)),0) * 8); piVar1 != (int *)0x0;
        piVar1 = *(int **)(piVar1 + 6)) {
      if (*piVar1 == 7) {
        if ((piVar1 != *(int **)(*(longlong *)(lVar9 + 0x4f0) + *(longlong *)(lVar9 + 0x4f8) * 8))
           && (*(ulonglong *)(piVar1 + 4) < *(ulonglong *)(piVar1 + 2))) goto LAB_140673b27;
        break;
      }
    }
  }
  local_5a0 = "icon_main_schedules.svg";
LAB_140673b27:
  fVar24 = (float)*(uint *)(param_2 + 0x14);
  local_620 = (fVar24 < _DAT_140aac4f8 ^ 1) << 2;
  uVar25 = DAT_140aac3f0;
  if (fVar24 < _DAT_140aac4f8) {
    uVar25 = DAT_140aac3c4;
  }
  bVar3 = _DAT_140aac4f0 <= fVar24;
  *(undefined1 *)(param_4 + 4) = 1;
  *(undefined4 *)((longlong)param_4 + 0x24) = 2;
  (**(code **)(*param_4 + 8))(param_4);
  *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
  *(undefined1 *)(param_4 + 5) = 1;
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  pcVar2 = *(code **)(*param_4 + 0x80);
  if ((char)local_res18 == '\0') {
    bVar22 = -(*(char *)(param_1 + 0xa8) != '\0') & 2;
  }
  else {
    bVar22 = 1;
  }
  uVar6 = *param_2;
  uStack_624 = uStack_624 & 0xffffff00;
  local_658 = (char ****)0x0;
  uStack_650 = 0;
  local_648 = 0;
  uStack_640 = 0;
  local_658 = (char ****)FUN_140003270(0x20);
  uVar7 = s_icon_main_menu_svg_140a915e8._8_8_;
  local_648 = _DAT_140aaca40;
  uStack_640 = _UNK_140aaca48;
  *local_658 = (char ***)s_icon_main_menu_svg_140a915e8._0_8_;
  local_658[1] = (char ***)uVar7;
  *(undefined2 *)(local_658 + 2) = s_icon_main_menu_svg_140a915e8._16_2_;
  *(char *)((longlong)local_658 + 0x12) = '\0';
  uVar6 = FUN_14081dff0(uVar6,local_5e8,&local_658);
  cVar5 = (*pcVar2)(param_4,uVar6,bVar22);
  if (0xf < uStack_640) {
    ppppcVar14 = local_658;
    if ((0xfff < uStack_640 + 1) &&
       (ppppcVar14 = (char ****)local_658[-1],
       (char *)0x1f < (char *)((longlong)local_658 + (-8 - (longlong)ppppcVar14)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppcVar14);
  }
  if (cVar5 != '\0') {
    *(bool *)(param_1 + 0xa8) = *(char *)(param_1 + 0xa8) == '\0';
  }
  pcVar2 = *(code **)(*param_4 + 0xc0);
  uVar6 = FUN_1402d82e0("right_bar_main","Main menu");
  (*pcVar2)(param_4,uVar6);
  *(undefined4 *)((longlong)param_4 + 0x2c) = 0x41800000;
  *(undefined1 *)(param_4 + 5) = 1;
  (**(code **)(*param_4 + 200))(param_4);
  *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
  *(undefined1 *)(param_4 + 5) = 1;
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  pcVar2 = *(code **)(*param_4 + 0x80);
  if ((char)local_res18 == '\0') {
    uVar6 = uVar11;
    if (*(int *)(param_1 + 0xa0) == 6) {
      uVar6 = 2;
    }
  }
  else {
    uVar6 = 1;
  }
  uVar7 = *param_2;
  uStack_624 = uStack_624 & 0xffffff00;
  local_658 = (char ****)0x0;
  uStack_650 = 0;
  local_648 = 0;
  uStack_640 = 0;
  local_658 = (char ****)FUN_140003270(0x20);
  uVar26 = s_icon_main_sel_svg_140a91588._8_8_;
  local_648 = _DAT_140aaca30;
  uStack_640 = _UNK_140aaca38;
  *local_658 = (char ***)s_icon_main_sel_svg_140a91588._0_8_;
  local_658[1] = (char ***)uVar26;
  *(char *)(local_658 + 2) = s_icon_main_sel_svg_140a91588[0x10];
  *(char *)((longlong)local_658 + 0x11) = '\0';
  uVar7 = FUN_14081dff0(uVar7,local_5e8,&local_658);
  cVar5 = (*pcVar2)(param_4,uVar7,uVar6);
  if (0xf < uStack_640) {
    ppppcVar14 = local_658;
    if ((0xfff < uStack_640 + 1) &&
       (ppppcVar14 = (char ****)local_658[-1],
       (char *)0x1f < (char *)((longlong)local_658 + (-8 - (longlong)ppppcVar14)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppcVar14);
  }
  if (cVar5 != '\0') {
    *(undefined4 *)(param_1 + 0xa0) = 6;
  }
  puVar8 = (undefined8 *)FUN_140726b70();
  local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
  ppuVar23 = (undefined8 **)0xffffffffffffffff;
  ppuStack_630 = (undefined8 **)0xffffffffffffffff;
  do {
    ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
  } while (local_638[(longlong)ppuStack_630] != (code)0x0);
  local_618 = puVar8;
  if (0xf < (ulonglong)puVar8[3]) {
    local_618 = (undefined8 *)*puVar8;
  }
  uStack_610 = puVar8[2];
  ppuStack_660 = &local_618;
  local_268 = 0;
  local_278 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_270 = local_258;
  local_260 = 500;
  local_668 = (code *)0xd;
  FUN_140022830(&local_278,&local_638,&local_668);
  local_658 = (char ****)0x0;
  uStack_650 = 0;
  local_648 = 0;
  uStack_640 = 0;
  FUN_140002c00(&local_658,local_270);
  local_278 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_270 != local_258) {
    puVar16 = local_270;
    if ((0xfff < local_260) &&
       (puVar16 = *(undefined1 **)(local_270 + -8),
       (undefined1 *)0x1f < local_270 + (-8 - (longlong)puVar16))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar16);
  }
  puVar8 = (undefined8 *)FUN_14029cb50(&local_658,0,&DAT_140a4aefc,1);
  local_4d8 = (void *)*puVar8;
  uStack_4d0 = puVar8[1];
  local_4c8 = puVar8[2];
  uStack_4c0 = puVar8[3];
  puVar8[3] = 0xf;
  *(undefined1 *)puVar8 = 0;
  puVar8[2] = 0;
  lVar9 = FUN_1402d82e0("info_mode_tp",&DAT_140a8e968);
  lVar21 = -1;
  do {
    lVar21 = lVar21 + 1;
  } while (*(char *)(lVar9 + lVar21) != '\0');
  plVar10 = (longlong *)FUN_14029cb50(&local_4d8,0);
  local_608 = (undefined8 ****)*plVar10;
  uStack_600 = plVar10[1];
  local_5f8 = plVar10[2];
  uStack_5f0 = plVar10[3];
  *(undefined1 *)plVar10 = 0;
  plVar10[2] = 0;
  plVar10[3] = 0xf;
  pppppuVar13 = &local_608;
  if (0xf < uStack_5f0) {
    pppppuVar13 = (undefined8 *****)local_608;
  }
  (**(code **)(*param_4 + 0xc0))(param_4,pppppuVar13);
  if (0xf < uStack_5f0) {
    pppppuVar13 = (undefined8 *****)local_608;
    if ((0xfff < uStack_5f0 + 1) &&
       (pppppuVar13 = (undefined8 *****)local_608[-1],
       0x1f < (ulonglong)((longlong)local_608 + (-8 - (longlong)pppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppuVar13);
  }
  uVar7 = _UNK_140aac918;
  uVar6 = _DAT_140aac910;
  local_5f8 = _DAT_140aac910;
  uStack_5f0 = _UNK_140aac918;
  local_608 = (undefined8 ****)((ulonglong)local_608 & 0xffffffffffffff00);
  if (0xf < uStack_4c0) {
    pvVar15 = local_4d8;
    if ((0xfff < uStack_4c0 + 1) &&
       (pvVar15 = *(void **)((longlong)local_4d8 + -8),
       0x1f < (ulonglong)((longlong)local_4d8 + (-8 - (longlong)pvVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar15);
  }
  local_4c8 = uVar6;
  uStack_4c0 = uVar7;
  local_4d8 = (void *)((ulonglong)local_4d8 & 0xffffffffffffff00);
  if (0xf < uStack_640) {
    ppppcVar14 = local_658;
    if ((0xfff < uStack_640 + 1) &&
       (ppppcVar14 = (char ****)local_658[-1],
       0x1f < (ulonglong)((longlong)local_658 + (-8 - (longlong)ppppcVar14)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppcVar14);
  }
  local_648 = uVar6;
  uStack_640 = uVar7;
  local_658 = (char ****)((ulonglong)local_658 & 0xffffffffffffff00);
  if (0xf < local_3e0) {
    pvVar15 = local_3f8;
    if ((0xfff < local_3e0 + 1) &&
       (pvVar15 = *(void **)((longlong)local_3f8 + -8),
       0x1f < (ulonglong)((longlong)local_3f8 + (-8 - (longlong)pvVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar15);
  }
  *(undefined1 *)(param_4 + 5) = 1;
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  uVar19 = 0;
  if (bVar3) {
    *(undefined4 *)((longlong)param_4 + 0x2c) = uVar25;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    *(undefined1 *)((longlong)param_4 + 0x4c) = 1;
    *(int *)(param_4 + 10) = local_620;
    local_668 = *(code **)(*param_4 + 0x98);
    if ((char)local_res18 == '\0') {
      uVar20 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 0) {
        uVar20 = 2;
      }
    }
    else {
      uVar20 = 1;
    }
    local_638 = (code *)FUN_1402d82e0("track_editor","Tracks");
    local_618 = (undefined8 *)*param_2;
    uStack_624 = uStack_624 & 0xffffff00;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5c0[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5c0);
    uVar6 = FUN_14081dff0(local_618,local_5e8,&local_658,CONCAT44(uStack_624,local_628));
    uVar7 = CONCAT44(uVar27,0x12);
    cVar5 = (*local_668)(param_4,uVar6,local_638,uVar20,uVar7);
    uVar27 = (undefined4)((ulonglong)uVar7 >> 0x20);
    uVar6 = extraout_XMM0_Qa_05;
    if (0xf < uStack_640) {
      uVar6 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 0;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar6,local_5e8,"mode_tracks",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(&local_658,&local_638,&local_668);
    pppppcVar17 = &local_658;
    if (0xf < uStack_640) {
      pppppcVar17 = (char *****)local_658;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,pppppcVar17);
    FUN_140002d30(&local_658);
    FUN_140002d30(local_5e8);
    *(undefined4 *)((longlong)param_4 + 0x2c) = uVar25;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    *(undefined1 *)((longlong)param_4 + 0x4c) = 1;
    *(int *)(param_4 + 10) = local_620;
    pcVar2 = *(code **)(*param_4 + 0x98);
    if ((char)local_res18 == '\0') {
      if (*(int *)(param_1 + 0xa0) == 1) {
        uVar11 = 2;
      }
    }
    else {
      uVar11 = 1;
    }
    uVar6 = FUN_1402d82e0("control_main_mode","Control");
    local_668._0_5_ = (uint5)(uint)local_668;
    local_5f8 = _DAT_140aaca00;
    uStack_5f0 = _UNK_140aaca08;
    local_608 = (undefined8 ****)s_icon_signal_svg_140a909b8._0_8_;
    uStack_600._0_6_ =
         CONCAT24(s_icon_signal_svg_140a909b8._12_2_,s_icon_signal_svg_140a909b8._8_4_);
    uStack_600._0_7_ = CONCAT16(s_icon_signal_svg_140a909b8[0xe],(undefined6)uStack_600);
    uStack_600 = (ulonglong)(uint7)uStack_600;
    uVar7 = FUN_14081dff0(*param_2,local_5e8,&local_608,local_668);
    uVar26 = CONCAT44(uVar27,0x12);
    cVar5 = (*pcVar2)(param_4,uVar7,uVar6,uVar11,uVar26);
    uVar27 = (undefined4)((ulonglong)uVar26 >> 0x20);
    uVar11 = extraout_XMM0_Qa_06;
    if (0xf < uStack_5f0) {
      uVar11 = FUN_140003040(&local_608,local_608);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 1;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_5e8,"mode_control",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(&local_658,&local_638,&local_668);
    pppppcVar17 = &local_658;
    if (0xf < uStack_640) {
      pppppcVar17 = (char *****)local_658;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,pppppcVar17);
    FUN_140002d30(&local_658);
    FUN_140002d30(local_5e8);
    *(undefined4 *)((longlong)param_4 + 0x2c) = uVar25;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    *(undefined1 *)((longlong)param_4 + 0x4c) = 1;
    *(int *)(param_4 + 10) = local_620;
    local_638 = *(code **)(*param_4 + 0x98);
    if ((char)local_res18 == '\0') {
      uVar20 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 2) {
        uVar20 = 2;
      }
    }
    else {
      uVar20 = 1;
    }
    uVar6 = FUN_1402d82e0("stations","Stations");
    uVar11 = *param_2;
    local_668._0_5_ = (uint5)(uint)local_668;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5b8[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5b8);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,local_668);
    uVar7 = CONCAT44(uVar27,0x12);
    cVar5 = (*local_638)(param_4,uVar11,uVar6,uVar20,uVar7);
    uVar27 = (undefined4)((ulonglong)uVar7 >> 0x20);
    uVar11 = extraout_XMM0_Qa_07;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 2;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_5e8,"mode_stations",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(&local_658,&local_638,&local_668);
    pppppcVar17 = &local_658;
    if (0xf < uStack_640) {
      pppppcVar17 = (char *****)local_658;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,pppppcVar17);
    FUN_140002d30(&local_658);
    FUN_140002d30(local_5e8);
    *(undefined4 *)((longlong)param_4 + 0x2c) = uVar25;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    *(undefined1 *)((longlong)param_4 + 0x4c) = 1;
    *(int *)(param_4 + 10) = local_620;
    local_638 = *(code **)(*param_4 + 0x98);
    if ((char)local_res18 == '\0') {
      uVar20 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 4) {
        uVar20 = 2;
      }
    }
    else {
      uVar20 = 1;
    }
    uVar6 = FUN_1402d82e0("lines","Lines");
    uVar11 = *param_2;
    local_668._0_5_ = (uint5)(uint)local_668;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5b0[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5b0);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,local_668);
    uVar7 = CONCAT44(uVar27,0x12);
    cVar5 = (*local_638)(param_4,uVar11,uVar6,uVar20,uVar7);
    uVar27 = (undefined4)((ulonglong)uVar7 >> 0x20);
    uVar11 = extraout_XMM0_Qa_08;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 4;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_5e8,"mode_lines",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(&local_658,&local_638,&local_668);
    pppppcVar17 = &local_658;
    if (0xf < uStack_640) {
      pppppcVar17 = (char *****)local_658;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,pppppcVar17);
    FUN_140002d30(&local_658);
    FUN_140002d30(local_5e8);
    uVar20 = DAT_140aac334;
    lVar9 = FUN_14055a6c0(param_4 + 3,uVar25,DAT_140aac334);
    *(undefined1 *)(lVar9 + 0x20) = 1;
    *(undefined8 *)(lVar9 + 0x24) = 0x40800000;
    *(undefined8 *)(lVar9 + 0x2c) = 0;
    *(undefined1 *)(lVar9 + 0x34) = 1;
    *(int *)(lVar9 + 0x38) = local_620;
    local_638 = *(code **)(*param_4 + 0x98);
    if ((char)local_res18 == '\0') {
      if (*(int *)(param_1 + 0xa0) == 3) {
        uVar19 = 2;
      }
    }
    else {
      uVar19 = 1;
    }
    uVar6 = FUN_1402d82e0("trains","Trains");
    uVar11 = *param_2;
    local_668._0_5_ = (uint5)(uint)local_668;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5a8[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5a8);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,local_668);
    uVar7 = CONCAT44(uVar27,0x12);
    cVar5 = (*local_638)(param_4,uVar11,uVar6,uVar19,uVar7);
    uVar27 = (undefined4)((ulonglong)uVar7 >> 0x20);
    uVar11 = extraout_XMM0_Qa_09;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 3;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_5e8,"mode_trains",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(&local_658,&local_638,&local_668);
    pppppcVar17 = &local_658;
    if (0xf < uStack_640) {
      pppppcVar17 = (char *****)local_658;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,pppppcVar17);
    FUN_140002d30(&local_658);
    FUN_140002d30(local_5e8);
    lVar9 = FUN_14055a6c0(param_4 + 3,uVar25,uVar20);
    *(undefined1 *)(lVar9 + 0x20) = 1;
    *(undefined8 *)(lVar9 + 0x24) = 0x40800000;
    *(undefined8 *)(lVar9 + 0x2c) = 0;
    *(undefined1 *)(lVar9 + 0x34) = 1;
    *(int *)(lVar9 + 0x38) = local_620;
    pcVar2 = *(code **)(*param_4 + 0x98);
    if ((char)local_res18 == '\0') {
      if (*(int *)(param_1 + 0xa0) == 5) {
        uVar18 = 2;
      }
    }
    else {
      uVar18 = 1;
    }
    uVar6 = FUN_1402d82e0("schedules","Schedules");
    uVar11 = *param_2;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5a0[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5a0);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,
                           CONCAT44(uStackX_1c,local_res18) & 0xffffff00ffffffff);
    cVar5 = (*pcVar2)(param_4,uVar11,uVar6,uVar18,CONCAT44(uVar27,0x12));
    uVar11 = extraout_XMM0_Qa_10;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 5;
    }
    uVar11 = FUN_140726b70(uVar11,local_4b8,"mode_schedules",1);
    local_668 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    do {
      ppuVar23 = (undefined8 **)((longlong)ppuVar23 + 1);
    } while (local_668[(longlong)ppuVar23] != (code)0x0);
    ppuStack_660 = ppuVar23;
    puVar8 = (undefined8 *)FUN_1402a2f70(local_5e8,&local_668,uVar11);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_5e8);
    puVar16 = local_4b8;
  }
  else {
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    pcVar2 = *(code **)(*param_4 + 0x80);
    if ((char)local_res18 == '\0') {
      if (*(int *)(param_1 + 0xa0) == 0) {
        uVar18 = 2;
      }
    }
    else {
      uVar18 = 1;
    }
    uVar11 = *param_2;
    uStack_624 = uStack_624 & 0xffffff00;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5c0[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5c0);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,CONCAT44(uStack_624,local_628));
    cVar5 = (*pcVar2)(param_4,uVar11,uVar18);
    uVar11 = extraout_XMM0_Qa;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 0;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_298,"mode_tracks",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(local_3d8,&local_638,&local_668);
    puVar12 = (ulonglong *)FUN_14029cb50(local_3d8,0,&DAT_140a4aefc,1);
    local_598 = *puVar12;
    uStack_590 = puVar12[1];
    local_588 = puVar12[2];
    uStack_580 = puVar12[3];
    *(undefined1 *)puVar12 = 0;
    puVar12[2] = 0;
    puVar12[3] = 0xf;
    uVar11 = FUN_1402d82e0("track_editor","Tracks");
    puVar8 = (undefined8 *)FUN_14029a380(local_2b8,uVar11,&local_598);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_2b8);
    if (0xf < uStack_580) {
      FUN_140003040(&local_598,local_598);
    }
    local_588 = uVar6;
    uStack_580 = uVar7;
    local_598 = local_598 & 0xffffffffffffff00;
    FUN_140002d30(local_3d8);
    FUN_140002d30(local_298);
    uVar4 = uStack_624;
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    pcVar2 = *(code **)(*param_4 + 0x80);
    if ((char)local_res18 == '\0') {
      uVar18 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 1) {
        uVar18 = 2;
      }
    }
    else {
      uVar18 = 1;
    }
    uStack_624 = uStack_624 & 0xffffff00;
    local_5f8 = _DAT_140aaca00;
    uStack_5f0 = _UNK_140aaca08;
    local_608 = (undefined8 ****)s_icon_signal_svg_140a909b8._0_8_;
    uStack_600._0_6_ =
         CONCAT24(s_icon_signal_svg_140a909b8._12_2_,s_icon_signal_svg_140a909b8._8_4_);
    uStack_600._0_7_ = CONCAT16(s_icon_signal_svg_140a909b8[0xe],(undefined6)uStack_600);
    uStack_600 = (ulonglong)(uint7)uStack_600;
    uVar11 = FUN_14081dff0(*param_2,local_5e8,&local_608,
                           CONCAT44(uVar4,local_628) & 0xffffff00ffffffff);
    cVar5 = (*pcVar2)(param_4,uVar11,uVar18);
    uVar11 = extraout_XMM0_Qa_00;
    if (0xf < uStack_5f0) {
      uVar11 = FUN_140003040(&local_608,local_608);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 1;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_398,"mode_control",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(local_498,&local_638,&local_668);
    puVar12 = (ulonglong *)FUN_14029cb50(local_498,0,&DAT_140a4aefc,1);
    local_578 = *puVar12;
    uStack_570 = puVar12[1];
    local_568 = puVar12[2];
    uStack_560 = puVar12[3];
    *(undefined1 *)puVar12 = 0;
    puVar12[2] = 0;
    puVar12[3] = 0xf;
    uVar11 = FUN_1402d82e0("control_main_mode","Control");
    puVar8 = (undefined8 *)FUN_14029a380(local_3b8,uVar11,&local_578);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_3b8);
    if (0xf < uStack_560) {
      FUN_140003040(&local_578,local_578);
    }
    local_568 = uVar6;
    uStack_560 = uVar7;
    local_578 = local_578 & 0xffffffffffffff00;
    FUN_140002d30(local_498);
    FUN_140002d30(local_398);
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    pcVar2 = *(code **)(*param_4 + 0x80);
    if ((char)local_res18 == '\0') {
      uVar18 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 2) {
        uVar18 = 2;
      }
    }
    else {
      uVar18 = 1;
    }
    uVar11 = *param_2;
    uStack_624 = uStack_624 & 0xffffff00;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5b8[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5b8);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,CONCAT44(uStack_624,local_628));
    cVar5 = (*pcVar2)(param_4,uVar11,uVar18);
    uVar11 = extraout_XMM0_Qa_01;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 2;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_358,"mode_stations",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(local_478,&local_638,&local_668);
    puVar12 = (ulonglong *)FUN_14029cb50(local_478,0,&DAT_140a4aefc,1);
    local_558 = *puVar12;
    uStack_550 = puVar12[1];
    local_548 = puVar12[2];
    uStack_540 = puVar12[3];
    *(undefined1 *)puVar12 = 0;
    puVar12[2] = 0;
    puVar12[3] = 0xf;
    uVar11 = FUN_1402d82e0("stations","Stations");
    puVar8 = (undefined8 *)FUN_14029a380(local_378,uVar11,&local_558);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_378);
    if (0xf < uStack_540) {
      FUN_140003040(&local_558,local_558);
    }
    local_548 = uVar6;
    uStack_540 = uVar7;
    local_558 = local_558 & 0xffffffffffffff00;
    FUN_140002d30(local_478);
    FUN_140002d30(local_358);
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    pcVar2 = *(code **)(*param_4 + 0x80);
    if ((char)local_res18 == '\0') {
      uVar18 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 4) {
        uVar18 = 2;
      }
    }
    else {
      uVar18 = 1;
    }
    uVar11 = *param_2;
    uStack_624 = uStack_624 & 0xffffff00;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5b0[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5b0);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,CONCAT44(uStack_624,local_628));
    cVar5 = (*pcVar2)(param_4,uVar11,uVar18);
    uVar11 = extraout_XMM0_Qa_02;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 4;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_318,"mode_lines",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(local_458,&local_638,&local_668);
    puVar12 = (ulonglong *)FUN_14029cb50(local_458,0,&DAT_140a4aefc,1);
    local_538 = *puVar12;
    uStack_530 = puVar12[1];
    local_528 = puVar12[2];
    uStack_520 = puVar12[3];
    *(undefined1 *)puVar12 = 0;
    puVar12[2] = 0;
    puVar12[3] = 0xf;
    uVar11 = FUN_1402d82e0("lines","Lines");
    puVar8 = (undefined8 *)FUN_14029a380(local_338,uVar11,&local_538);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_338);
    if (0xf < uStack_520) {
      FUN_140003040(&local_538,local_538);
    }
    local_528 = uVar6;
    uStack_520 = uVar7;
    local_538 = local_538 & 0xffffffffffffff00;
    FUN_140002d30(local_458);
    FUN_140002d30(local_318);
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    pcVar2 = *(code **)(*param_4 + 0x80);
    if ((char)local_res18 == '\0') {
      uVar18 = uVar19;
      if (*(int *)(param_1 + 0xa0) == 3) {
        uVar18 = 2;
      }
    }
    else {
      uVar18 = 1;
    }
    uVar11 = *param_2;
    uStack_624 = uStack_624 & 0xffffff00;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5a8[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5a8);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,CONCAT44(uStack_624,local_628));
    cVar5 = (*pcVar2)(param_4,uVar11,uVar18);
    uVar11 = extraout_XMM0_Qa_03;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 3;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_2d8,"mode_trains",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    ppuStack_630 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_630 = (undefined8 **)((longlong)ppuStack_630 + 1);
    } while (local_638[(longlong)ppuStack_630] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    FUN_140021e90(local_438,&local_638,&local_668);
    puVar12 = (ulonglong *)FUN_14029cb50(local_438,0,&DAT_140a4aefc,1);
    local_518 = *puVar12;
    uStack_510 = puVar12[1];
    local_508 = puVar12[2];
    uStack_500 = puVar12[3];
    *(undefined1 *)puVar12 = 0;
    puVar12[2] = 0;
    puVar12[3] = 0xf;
    uVar11 = FUN_1402d82e0("trains","Trains");
    puVar8 = (undefined8 *)FUN_14029a380(local_2f8,uVar11,&local_518);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_2f8);
    if (0xf < uStack_500) {
      FUN_140003040(&local_518,local_518);
    }
    local_508 = uVar6;
    uStack_500 = uVar7;
    local_518 = local_518 & 0xffffffffffffff00;
    FUN_140002d30(local_438);
    FUN_140002d30(local_2d8);
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x42100000;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_4 + 0x44) = 0;
    pcVar2 = *(code **)(*param_4 + 0x80);
    if ((char)local_res18 == '\0') {
      if (*(int *)(param_1 + 0xa0) == 5) {
        uVar19 = 2;
      }
    }
    else {
      uVar19 = 1;
    }
    uVar11 = *param_2;
    local_658 = (char ****)0x0;
    uStack_650 = 0;
    local_648 = 0;
    uStack_640 = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (local_5a0[lVar9] != '\0');
    FUN_140002c00(&local_658,local_5a0);
    uVar11 = FUN_14081dff0(uVar11,local_5e8,&local_658,
                           CONCAT44(uStackX_1c,local_res18) & 0xffffff00ffffffff);
    cVar5 = (*pcVar2)(param_4,uVar11,uVar19);
    uVar11 = extraout_XMM0_Qa_04;
    if (0xf < uStack_640) {
      uVar11 = FUN_140003040(&local_658,local_658);
    }
    if (cVar5 != '\0') {
      *(undefined4 *)(param_1 + 0xa0) = 5;
    }
    puVar8 = (undefined8 *)FUN_140726b70(uVar11,local_5e8,"mode_schedules",1);
    local_638 = (code *)FUN_1402d82e0("key_info2","[Key: {}]");
    do {
      ppuVar23 = (undefined8 **)((longlong)ppuVar23 + 1);
    } while (local_638[(longlong)ppuVar23] != (code)0x0);
    local_618 = puVar8;
    if (0xf < (ulonglong)puVar8[3]) {
      local_618 = (undefined8 *)*puVar8;
    }
    uStack_610 = puVar8[2];
    local_668 = (code *)0xd;
    ppuStack_660 = &local_618;
    ppuStack_630 = ppuVar23;
    FUN_140021e90(local_418,&local_638,&local_668);
    puVar12 = (ulonglong *)FUN_14029cb50(local_418,0,&DAT_140a4aefc,1);
    local_4f8 = *puVar12;
    uStack_4f0 = puVar12[1];
    local_4e8 = puVar12[2];
    uStack_4e0 = puVar12[3];
    *(undefined1 *)puVar12 = 0;
    puVar12[2] = 0;
    puVar12[3] = 0xf;
    uVar11 = FUN_1402d82e0("schedules","Schedules");
    puVar8 = (undefined8 *)FUN_14029a380(local_4b8,uVar11,&local_4f8);
    if (0xf < (ulonglong)puVar8[3]) {
      puVar8 = (undefined8 *)*puVar8;
    }
    (**(code **)(*param_4 + 0xc0))(param_4,puVar8);
    FUN_140002d30(local_4b8);
    if (0xf < uStack_4e0) {
      FUN_140003040(&local_4f8,local_4f8);
    }
    local_4e8 = uVar6;
    uStack_4e0 = uVar7;
    local_4f8 = local_4f8 & 0xffffffffffffff00;
    FUN_140002d30(local_418);
    puVar16 = local_5e8;
  }
  FUN_140002d30(puVar16);
                    /* WARNING: Could not recover jumptable at 0x000140675ae1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_4 + 0x18))(param_4);
  return;
}


// Incoming references
// 0xc32954 DATA caller none
// 0x6865d9 UNCONDITIONAL_CALL caller 140686590
