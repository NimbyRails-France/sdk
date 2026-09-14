// Candidate VA 1406e9920; RVA 0x6e9920
// Ghidra inferred prototype: undefined FUN_1406e9920()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1406e9920(longlong param_1,undefined8 *param_2,undefined8 param_3,longlong *param_4,
                  longlong param_5)

{
  int iVar1;
  code *pcVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  char cVar5;
  undefined8 uVar6;
  undefined8 *puVar7;
  longlong lVar8;
  longlong *plVar9;
  undefined8 uVar10;
  ulonglong *puVar11;
  undefined1 *puVar12;
  undefined8 *****pppppuVar13;
  char *pcVar14;
  void *pvVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  longlong lVar18;
  undefined8 extraout_XMM0_Qa;
  undefined8 extraout_XMM0_Qa_00;
  undefined8 local_res18;
  longlong *local_6d8;
  undefined8 **ppuStack_6d0;
  longlong local_6c8;
  undefined8 **ppuStack_6c0;
  char *local_6b8;
  undefined8 uStack_6b0;
  undefined8 local_6a8;
  ulonglong uStack_6a0;
  undefined8 *local_698;
  undefined8 uStack_690;
  undefined8 ****local_688;
  longlong lStack_680;
  longlong local_678;
  ulonglong uStack_670;
  undefined8 ****local_668;
  longlong lStack_660;
  longlong local_658;
  ulonglong uStack_650;
  ulonglong local_648;
  ulonglong uStack_640;
  ulonglong local_638;
  ulonglong uStack_630;
  ulonglong local_628;
  ulonglong uStack_620;
  ulonglong local_618;
  ulonglong uStack_610;
  ulonglong local_608;
  ulonglong uStack_600;
  ulonglong local_5f8;
  ulonglong uStack_5f0;
  void *local_5e8;
  undefined8 uStack_5e0;
  undefined8 local_5d8;
  ulonglong uStack_5d0;
  ulonglong local_5c8 [5];
  longlong *local_5a0;
  undefined1 local_598 [40];
  void *local_570;
  ulonglong local_558;
  undefined1 local_550 [32];
  undefined1 local_530 [32];
  undefined1 local_510 [32];
  undefined1 local_4f0 [32];
  undefined1 local_4d0 [32];
  undefined1 local_4b0 [40];
  undefined **local_488;
  undefined1 *local_480;
  undefined8 local_478;
  ulonglong local_470;
  undefined1 local_468 [512];
  undefined **local_268;
  undefined1 *local_260;
  undefined8 local_258;
  ulonglong local_250;
  undefined1 local_248 [528];
  
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 4) = 1;
  *(undefined4 *)((longlong)param_4 + 0x24) = 2;
  local_res18 = param_3;
  (**(code **)(*param_4 + 8))(param_4);
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  pcVar2 = *(code **)(*param_4 + 0x80);
  uVar16 = 0;
  if (*(int *)(param_5 + 0x20) == 0) {
    uVar16 = 2;
  }
  uVar6 = *param_2;
  local_res18._0_5_ = (uint5)(uint)local_res18;
  local_6b8 = (char *)0x0;
  uStack_6b0 = 0;
  local_6a8 = 0;
  uStack_6a0 = 0;
  local_6b8 = (char *)FUN_140003270(0x20);
  uVar4 = s_icon_track_default_svg_140a953e0._8_8_;
  local_6a8 = _DAT_140aaca80;
  uStack_6a0 = _UNK_140aaca88;
  *(undefined8 *)local_6b8 = s_icon_track_default_svg_140a953e0._0_8_;
  *(undefined8 *)(local_6b8 + 8) = uVar4;
  *(undefined4 *)(local_6b8 + 0x10) = s_icon_track_default_svg_140a953e0._16_4_;
  *(undefined2 *)(local_6b8 + 0x14) = s_icon_track_default_svg_140a953e0._20_2_;
  local_6b8[0x16] = '\0';
  uVar6 = FUN_14081dff0(uVar6,local_598,&local_6b8);
  cVar5 = (*pcVar2)(param_4,uVar6,uVar16);
  if (0xf < uStack_6a0) {
    pcVar14 = local_6b8;
    if ((0xfff < uStack_6a0 + 1) &&
       (pcVar14 = *(char **)(local_6b8 + -8), (char *)0x1f < local_6b8 + (-8 - (longlong)pcVar14)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar14);
  }
  if (cVar5 != '\0') {
    *(undefined4 *)(param_5 + 0x20) = 0;
  }
  puVar7 = (undefined8 *)FUN_140726b70();
  local_6c8 = FUN_1402d82e0("key_info2","[Key: {}]");
  ppuStack_6c0 = (undefined8 **)0xffffffffffffffff;
  do {
    ppuStack_6c0 = (undefined8 **)((longlong)ppuStack_6c0 + 1);
  } while (*(char *)(local_6c8 + (longlong)ppuStack_6c0) != '\0');
  local_698 = puVar7;
  if (0xf < (ulonglong)puVar7[3]) {
    local_698 = (undefined8 *)*puVar7;
  }
  uStack_690 = puVar7[2];
  ppuStack_6d0 = &local_698;
  local_478 = 0;
  local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_480 = local_468;
  local_470 = 500;
  local_6d8 = (longlong *)0xd;
  FUN_140022830(&local_488,&local_6c8,&local_6d8);
  local_6b8 = (char *)0x0;
  uStack_6b0 = 0;
  local_6a8 = 0;
  uStack_6a0 = 0;
  FUN_140002c00(&local_6b8,local_480);
  local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_480 != local_468) {
    puVar12 = local_480;
    if ((0xfff < local_470) &&
       (puVar12 = *(undefined1 **)(local_480 + -8),
       (undefined1 *)0x1f < local_480 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar12);
  }
  puVar7 = (undefined8 *)FUN_14029cb50(&local_6b8,0,&DAT_140a4aefc,1);
  local_5e8 = (void *)*puVar7;
  uStack_5e0 = puVar7[1];
  local_5d8 = puVar7[2];
  uStack_5d0 = puVar7[3];
  puVar7[3] = 0xf;
  *(undefined1 *)puVar7 = 0;
  puVar7[2] = 0;
  lVar8 = FUN_1402d82e0("mode_map_tracks","Plain tracks");
  lVar18 = -1;
  do {
    lVar18 = lVar18 + 1;
  } while (*(char *)(lVar8 + lVar18) != '\0');
  plVar9 = (longlong *)FUN_14029cb50(&local_5e8,0);
  local_668 = (undefined8 ****)*plVar9;
  lStack_660 = plVar9[1];
  local_658 = plVar9[2];
  uStack_650 = plVar9[3];
  *(undefined1 *)plVar9 = 0;
  plVar9[2] = 0;
  plVar9[3] = 0xf;
  pppppuVar13 = &local_668;
  if (0xf < uStack_650) {
    pppppuVar13 = (undefined8 *****)local_668;
  }
  (**(code **)(*param_4 + 0xc0))(param_4,pppppuVar13);
  if (0xf < uStack_650) {
    pppppuVar13 = (undefined8 *****)local_668;
    if ((0xfff < uStack_650 + 1) &&
       (pppppuVar13 = (undefined8 *****)local_668[-1],
       0x1f < (ulonglong)((longlong)local_668 + (-8 - (longlong)pppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppuVar13);
  }
  uVar4 = _UNK_140aac918;
  uVar6 = _DAT_140aac910;
  local_658 = _DAT_140aac910;
  uStack_650 = _UNK_140aac918;
  local_668 = (undefined8 ****)((ulonglong)local_668 & 0xffffffffffffff00);
  if (0xf < uStack_5d0) {
    pvVar15 = local_5e8;
    if ((0xfff < uStack_5d0 + 1) &&
       (pvVar15 = *(void **)((longlong)local_5e8 + -8),
       0x1f < (ulonglong)((longlong)local_5e8 + (-8 - (longlong)pvVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar15);
  }
  local_5d8 = uVar6;
  uStack_5d0 = uVar4;
  local_5e8 = (void *)((ulonglong)local_5e8 & 0xffffffffffffff00);
  if (0xf < uStack_6a0) {
    pcVar14 = local_6b8;
    if ((0xfff < uStack_6a0 + 1) &&
       (pcVar14 = *(char **)(local_6b8 + -8), (char *)0x1f < local_6b8 + (-8 - (longlong)pcVar14)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar14);
  }
  local_6a8 = uVar6;
  uStack_6a0 = uVar4;
  local_6b8 = (char *)((ulonglong)local_6b8 & 0xffffffffffffff00);
  if (0xf < local_558) {
    pvVar15 = local_570;
    if ((0xfff < local_558 + 1) &&
       (pvVar15 = *(void **)((longlong)local_570 + -8),
       0x1f < (ulonglong)((longlong)local_570 + (-8 - (longlong)pvVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar15);
  }
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_4 + 0x44) = 0;
  pcVar2 = *(code **)(*param_4 + 0x80);
  uVar17 = 0;
  uVar16 = uVar17;
  if (*(int *)(param_5 + 0x20) == 1) {
    uVar16 = 2;
  }
  uVar10 = *param_2;
  local_res18._0_5_ = (uint5)(uint)local_res18;
  local_6b8 = (char *)0x0;
  uStack_6b0 = 0;
  local_6a8 = 0;
  uStack_6a0 = 0;
  local_6b8 = (char *)FUN_140003270(0x20);
  uVar3 = s_icon_track_lines_svg_140a95418._8_8_;
  local_6a8 = _DAT_140aaca60;
  uStack_6a0 = _UNK_140aaca68;
  *(undefined8 *)local_6b8 = s_icon_track_lines_svg_140a95418._0_8_;
  *(undefined8 *)(local_6b8 + 8) = uVar3;
  *(undefined4 *)(local_6b8 + 0x10) = s_icon_track_lines_svg_140a95418._16_4_;
  local_6b8[0x14] = '\0';
  uVar10 = FUN_14081dff0(uVar10,local_598,&local_6b8);
  cVar5 = (*pcVar2)(param_4,uVar10,uVar16);
  if (0xf < uStack_6a0) {
    pcVar14 = local_6b8;
    if ((0xfff < uStack_6a0 + 1) &&
       (pcVar14 = *(char **)(local_6b8 + -8), (char *)0x1f < local_6b8 + (-8 - (longlong)pcVar14)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar14);
  }
  if (cVar5 != '\0') {
    *(undefined4 *)(param_5 + 0x20) = 1;
  }
  puVar7 = (undefined8 *)FUN_140726b70();
  local_6d8 = (longlong *)FUN_1402d82e0("key_info2","[Key: {}]");
  ppuStack_6d0 = (undefined8 **)0xffffffffffffffff;
  do {
    ppuStack_6d0 = (undefined8 **)((longlong)ppuStack_6d0 + 1);
  } while (*(char *)((longlong)local_6d8 + (longlong)ppuStack_6d0) != '\0');
  local_698 = puVar7;
  if (0xf < (ulonglong)puVar7[3]) {
    local_698 = (undefined8 *)*puVar7;
  }
  uStack_690 = puVar7[2];
  ppuStack_6c0 = &local_698;
  local_258 = 0;
  local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_260 = local_248;
  local_250 = 500;
  local_6c8 = 0xd;
  FUN_140022830(&local_268,&local_6d8,&local_6c8);
  local_5c8[0] = 0;
  local_5c8[1] = 0;
  local_5c8[2] = 0;
  local_5c8[3] = 0;
  FUN_140002c00(local_5c8,local_260);
  local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_260 != local_248) {
    puVar12 = local_260;
    if ((0xfff < local_250) &&
       (puVar12 = *(undefined1 **)(local_260 + -8),
       (undefined1 *)0x1f < local_260 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar12);
  }
  puVar11 = (ulonglong *)FUN_14029cb50(local_5c8,0,&DAT_140a4aefc,1);
  local_648 = *puVar11;
  uStack_640 = puVar11[1];
  local_638 = puVar11[2];
  uStack_630 = puVar11[3];
  puVar11[3] = 0xf;
  *(undefined1 *)puVar11 = 0;
  puVar11[2] = 0;
  lVar8 = FUN_1402d82e0("mode_map_tracks_lines","Lines overlay");
  lVar18 = -1;
  do {
    lVar18 = lVar18 + 1;
  } while (*(char *)(lVar8 + lVar18) != '\0');
  plVar9 = (longlong *)FUN_14029cb50(&local_648,0,lVar8);
  local_688 = (undefined8 ****)*plVar9;
  lStack_680 = plVar9[1];
  local_678 = plVar9[2];
  uStack_670 = plVar9[3];
  *(undefined1 *)plVar9 = 0;
  plVar9[2] = 0;
  plVar9[3] = 0xf;
  pppppuVar13 = &local_688;
  if (0xf < uStack_670) {
    pppppuVar13 = (undefined8 *****)local_688;
  }
  (**(code **)(*param_4 + 0xc0))(param_4,pppppuVar13);
  if (0xf < uStack_670) {
    FUN_140003040(&local_688,local_688);
  }
  local_678 = uVar6;
  uStack_670 = uVar4;
  local_688 = (undefined8 ****)((ulonglong)local_688 & 0xffffffffffffff00);
  if (0xf < uStack_630) {
    FUN_140003040(&local_648,local_648);
  }
  local_638 = uVar6;
  uStack_630 = uVar4;
  local_648 = local_648 & 0xffffffffffffff00;
  if (0xf < local_5c8[3]) {
    FUN_140003040(local_5c8,local_5c8[0]);
  }
  local_5c8[2] = uVar6;
  local_5c8[3] = uVar4;
  local_5c8[0] = local_5c8[0] & 0xffffffffffffff00;
  FUN_140002d30(local_510);
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_4 + 0x44) = 0;
  pcVar2 = *(code **)(*param_4 + 0x80);
  uVar16 = uVar17;
  if (*(int *)(param_5 + 0x20) == 2) {
    uVar16 = 2;
  }
  uVar10 = *param_2;
  local_res18._0_5_ = (uint5)(uint)local_res18;
  local_6b8 = (char *)0x0;
  uStack_6b0 = 0;
  local_6a8 = 0;
  uStack_6a0 = 0;
  local_6b8 = (char *)FUN_140003270(0x20);
  uVar3 = s_icon_track_speed_svg_140a953b8._8_8_;
  local_6a8 = _DAT_140aaca60;
  uStack_6a0 = _UNK_140aaca68;
  *(undefined8 *)local_6b8 = s_icon_track_speed_svg_140a953b8._0_8_;
  *(undefined8 *)(local_6b8 + 8) = uVar3;
  *(undefined4 *)(local_6b8 + 0x10) = s_icon_track_speed_svg_140a953b8._16_4_;
  local_6b8[0x14] = '\0';
  uVar10 = FUN_14081dff0(uVar10,local_598,&local_6b8,local_res18);
  cVar5 = (*pcVar2)(param_4,uVar10,uVar16);
  uVar10 = extraout_XMM0_Qa;
  if (0xf < uStack_6a0) {
    uVar10 = FUN_140003040(&local_6b8,local_6b8);
  }
  if (cVar5 != '\0') {
    *(undefined4 *)(param_5 + 0x20) = 2;
  }
  puVar7 = (undefined8 *)FUN_140726b70(uVar10,local_4d0,"map_toggle_tracks",1);
  local_6d8 = (longlong *)FUN_1402d82e0("key_info2","[Key: {}]");
  ppuStack_6d0 = (undefined8 **)0xffffffffffffffff;
  do {
    ppuStack_6d0 = (undefined8 **)((longlong)ppuStack_6d0 + 1);
  } while (*(char *)((longlong)local_6d8 + (longlong)ppuStack_6d0) != '\0');
  local_698 = puVar7;
  if (0xf < (ulonglong)puVar7[3]) {
    local_698 = (undefined8 *)*puVar7;
  }
  uStack_690 = puVar7[2];
  local_6c8 = 0xd;
  ppuStack_6c0 = &local_698;
  FUN_140021e90(local_550,&local_6d8,&local_6c8);
  puVar11 = (ulonglong *)FUN_14029cb50(local_550,0,&DAT_140a4aefc,1);
  local_628 = *puVar11;
  uStack_620 = puVar11[1];
  local_618 = puVar11[2];
  uStack_610 = puVar11[3];
  *(undefined1 *)puVar11 = 0;
  puVar11[2] = 0;
  puVar11[3] = 0xf;
  uVar10 = FUN_1402d82e0("mode_map_tracks_speed","Speed overlay");
  puVar7 = (undefined8 *)FUN_14029a380(local_4f0,uVar10,&local_628);
  if (0xf < (ulonglong)puVar7[3]) {
    puVar7 = (undefined8 *)*puVar7;
  }
  (**(code **)(*param_4 + 0xc0))(param_4,puVar7);
  FUN_140002d30(local_4f0);
  if (0xf < uStack_610) {
    FUN_140003040(&local_628,local_628);
  }
  local_618 = uVar6;
  uStack_610 = uVar4;
  local_628 = local_628 & 0xffffffffffffff00;
  FUN_140002d30(local_550);
  FUN_140002d30(local_4d0);
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x42100000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined8 *)((longlong)param_4 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_4 + 0x44) = 0;
  pcVar2 = *(code **)(*param_4 + 0x80);
  if (*(int *)(param_5 + 0x20) == 4) {
    uVar17 = 2;
  }
  uVar10 = *param_2;
  local_res18._0_5_ = (uint5)(uint)local_res18;
  local_6b8 = (char *)0x0;
  uStack_6b0 = 0;
  local_6a8 = 0;
  uStack_6a0 = 0;
  local_6b8 = (char *)FUN_140003270(0x20);
  uVar3 = s_icon_track_signals_svg_140a95348._8_8_;
  local_6a8 = _DAT_140aaca80;
  uStack_6a0 = _UNK_140aaca88;
  *(undefined8 *)local_6b8 = s_icon_track_signals_svg_140a95348._0_8_;
  *(undefined8 *)(local_6b8 + 8) = uVar3;
  *(undefined4 *)(local_6b8 + 0x10) = s_icon_track_signals_svg_140a95348._16_4_;
  *(undefined2 *)(local_6b8 + 0x14) = s_icon_track_signals_svg_140a95348._20_2_;
  local_6b8[0x16] = '\0';
  uVar10 = FUN_14081dff0(uVar10,local_598,&local_6b8,local_res18);
  cVar5 = (*pcVar2)(param_4,uVar10,uVar17);
  uVar10 = extraout_XMM0_Qa_00;
  if (0xf < uStack_6a0) {
    uVar10 = FUN_140003040(&local_6b8,local_6b8);
  }
  if (cVar5 != '\0') {
    *(undefined4 *)(param_5 + 0x20) = 4;
  }
  puVar7 = (undefined8 *)FUN_140726b70(uVar10,local_598,"map_toggle_tracks",1);
  local_6d8 = (longlong *)FUN_1402d82e0("key_info2","[Key: {}]");
  ppuStack_6d0 = (undefined8 **)0xffffffffffffffff;
  do {
    ppuStack_6d0 = (undefined8 **)((longlong)ppuStack_6d0 + 1);
  } while (*(char *)((longlong)local_6d8 + (longlong)ppuStack_6d0) != '\0');
  local_698 = puVar7;
  if (0xf < (ulonglong)puVar7[3]) {
    local_698 = (undefined8 *)*puVar7;
  }
  uStack_690 = puVar7[2];
  local_6c8 = 0xd;
  ppuStack_6c0 = &local_698;
  FUN_140021e90(local_530,&local_6d8,&local_6c8);
  puVar11 = (ulonglong *)FUN_14029cb50(local_530,0,&DAT_140a4aefc,1);
  local_608 = *puVar11;
  uStack_600 = puVar11[1];
  local_5f8 = puVar11[2];
  uStack_5f0 = puVar11[3];
  *(undefined1 *)puVar11 = 0;
  puVar11[2] = 0;
  puVar11[3] = 0xf;
  uVar10 = FUN_1402d82e0("mode_map_tracks_signals","Track reservations");
  puVar7 = (undefined8 *)FUN_14029a380(local_4b0,uVar10,&local_608);
  if (0xf < (ulonglong)puVar7[3]) {
    puVar7 = (undefined8 *)*puVar7;
  }
  (**(code **)(*param_4 + 0xc0))(param_4,puVar7);
  FUN_140002d30(local_4b0);
  if (0xf < uStack_5f0) {
    FUN_140003040(&local_608,local_608);
  }
  local_5f8 = uVar6;
  uStack_5f0 = uVar4;
  local_608 = local_608 & 0xffffffffffffff00;
  FUN_140002d30(local_530);
  FUN_140002d30(local_598);
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x40800000;
  *(undefined1 *)(param_4 + 6) = 1;
  (**(code **)(*param_4 + 200))(param_4);
  (**(code **)(*param_4 + 0x18))(param_4);
  cVar5 = *(char *)(param_5 + 0x2b);
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  local_res18._0_4_ = (uint)(cVar5 != '\0');
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_map_region_names","Map labels");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x2b) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x2c) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_labels","User labels");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x2c) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x2d) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_poi_labels","User POI labels");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x2d) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x31) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_mod_poi_labels","Mod POI labels");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x31) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x2e) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_signals","Signals");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x2e) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x26) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_station_event","Station events");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x26) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x29) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_station_icons","Station icons");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x29) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x28) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_station_names","Station names");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x28) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x30) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_platform_names","Platform names");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x30) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x25) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_train_counters","Train counters");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x25) = (uint)local_res18 != 0;
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x27) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_user_train_icons","Train icons");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x27) = (uint)local_res18 != 0;
  iVar1 = *(int *)(param_1 + 4);
  if (iVar1 == 0) {
    local_res18._0_4_ = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 3) = 1;
    *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
    param_4[8] = 0x40800000;
    *(undefined4 *)(param_4 + 9) = 0;
    FUN_14055b8e0(param_4,&DAT_140b8e0e0);
    pcVar2 = *(code **)(*param_4 + 0xf0);
    uVar6 = FUN_1402d82e0("mvo_user_track_depth","Map over track layers");
    (*pcVar2)(param_4,uVar6,&local_res18);
    if ((uint)local_res18 == 1) goto LAB_1406eae84;
  }
  else {
    if (iVar1 == 1) {
      local_res18 = (ulonglong)local_res18._4_4_ << 0x20;
      *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
      *(undefined1 *)(param_4 + 6) = 1;
      *(undefined1 *)(param_4 + 3) = 1;
      *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
      *(undefined1 *)(param_4 + 7) = 1;
      *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
      param_4[8] = 0x40800000;
      *(undefined4 *)(param_4 + 9) = 0;
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xf0);
      uVar6 = FUN_1402d82e0("mvo_user_track_depth","Map over track layers");
      (*pcVar2)(param_4,uVar6,&local_res18);
      if ((uint)local_res18 != 0) {
        *(undefined4 *)(param_5 + 0x18) = 0;
      }
      local_res18._0_4_ = 1;
      *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
      *(undefined1 *)(param_4 + 6) = 1;
      *(undefined1 *)(param_4 + 3) = 1;
      *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
      *(undefined1 *)(param_4 + 7) = 1;
      *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
      param_4[8] = 0x40800000;
      *(undefined4 *)(param_4 + 9) = 0;
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xf0);
      uVar6 = FUN_1402d82e0("mvo_user_track_depth_bridge","Map bridges over track layers");
      (*pcVar2)(param_4,uVar6,&local_res18);
      if ((uint)local_res18 != 1) {
        *(undefined4 *)(param_5 + 0x18) = 2;
      }
      goto LAB_1406eae84;
    }
    if (iVar1 != 2) goto LAB_1406eae84;
    local_res18._0_4_ = 0;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
    *(undefined1 *)(param_4 + 6) = 1;
    *(undefined1 *)(param_4 + 3) = 1;
    *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
    *(undefined1 *)(param_4 + 7) = 1;
    *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
    param_4[8] = 0x40800000;
    *(undefined4 *)(param_4 + 9) = 0;
    FUN_14055b8e0(param_4,&DAT_140b8e0e0);
    pcVar2 = *(code **)(*param_4 + 0xf0);
    uVar6 = FUN_1402d82e0("mvo_user_track_depth_bridge","Map bridges over track layers");
    (*pcVar2)(param_4,uVar6,&local_res18);
    if ((uint)local_res18 == 0) goto LAB_1406eae84;
  }
  *(undefined4 *)(param_5 + 0x18) = 1;
LAB_1406eae84:
  local_res18._0_4_ = (uint)(*(char *)(param_5 + 0x1c) != '\0');
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_track_editor_always_on_top","Track editor always on top");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x1c) = (uint)local_res18 != 0;
  local_res18 = CONCAT44(local_res18._4_4_,(uint)(*(char *)(param_5 + 0x1d) != '\0'));
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41b00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xf0);
  uVar6 = FUN_1402d82e0("mvo_track_editor_station_influence","Track editor station influence");
  (*pcVar2)(param_4,uVar6,&local_res18);
  *(bool *)(param_5 + 0x1d) = (uint)local_res18 != 0;
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41c00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  FUN_14055b8e0(param_4,&DAT_140b8e0e0);
  pcVar2 = *(code **)(*param_4 + 0xa8);
  uVar6 = FUN_1402d82e0("mvo_user_layer_toggles","Display layers:");
  (*pcVar2)(param_4,uVar6,0x11);
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x41c00000;
  *(undefined1 *)(param_4 + 6) = 1;
  *(undefined1 *)(param_4 + 3) = 1;
  *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0x40800000;
  *(undefined1 *)(param_4 + 4) = 1;
  *(undefined4 *)((longlong)param_4 + 0x24) = 2;
  puVar12 = (undefined1 *)FUN_14055a740(param_4 + 3);
  *(undefined4 *)(puVar12 + 0x1c) = 0x41c00000;
  puVar12[0x18] = 1;
  *puVar12 = 1;
  *(undefined4 *)(puVar12 + 4) = 0xa0;
  puVar12[0x20] = 1;
  *(undefined8 *)(puVar12 + 0x24) = 0;
  *(undefined8 *)(puVar12 + 0x2c) = 0;
  puVar12[0x34] = 1;
  *(undefined4 *)(puVar12 + 0x38) = 6;
  (**(code **)(*param_4 + 8))(param_4);
  ppuStack_6d0 = (undefined8 **)param_5;
  local_6d8 = param_4;
  FUN_1406eb1d0(&local_6d8,&DAT_140a95608,0xffffff9c);
  FUN_1406eb1d0(&local_6d8,&DAT_140a957ec,0xfffffffd);
  FUN_1406eb1d0(&local_6d8,&DAT_140a957f4,0xfffffffe);
  FUN_1406eb1d0(&local_6d8,&DAT_140a957fc,0xffffffff);
  uVar6 = FUN_1402d82e0("layer_ground_g",&DAT_140a95804);
  FUN_1406eb1d0(&local_6d8,uVar6,0);
  FUN_1406eb1d0(&local_6d8,&DAT_140a957e0,1);
  FUN_1406eb1d0(&local_6d8,&DAT_140a957e4,2);
  FUN_1406eb1d0(&local_6d8,&DAT_140a957e8,3);
  FUN_1406eb1d0(&local_6d8,&DAT_140a95798,100);
  (**(code **)(*param_4 + 0x18))(param_4);
  local_5c8[4] = param_5;
  ppuStack_6d0 = (undefined8 **)(local_5c8 + 4);
  local_6d8 = param_4;
  local_5a0 = param_4;
  FUN_1406eb590(&local_6d8,3);
  FUN_1406eb590(&local_6d8,1);
  FUN_1406eb590(&local_6d8,0);
  return;
}


// Incoming references
// 0xc354ec DATA caller none
// 0x6ed9d7 UNCONDITIONAL_CALL caller 1406ed860
