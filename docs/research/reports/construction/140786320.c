
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140786320(longlong param_1,longlong *param_2,LARGE_INTEGER param_3,longlong *param_4,
                  longlong param_5)

{
  undefined2 uVar1;
  ulonglong uVar2;
  int *piVar3;
  longlong lVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  int *piVar8;
  LARGE_INTEGER LVar9;
  char *pcVar10;
  char *pcVar11;
  char *pcVar12;
  undefined1 uVar13;
  char cVar14;
  int iVar15;
  uint uVar16;
  uint uVar17;
  undefined8 *puVar18;
  longlong *plVar19;
  char *pcVar20;
  longlong lVar21;
  longlong lVar22;
  double *pdVar23;
  undefined2 *puVar24;
  uint uVar25;
  longlong *plVar26;
  longlong *plVar27;
  ulonglong uVar28;
  size_t sVar29;
  undefined4 uVar30;
  undefined4 uVar31;
  undefined4 uVar32;
  int *piVar33;
  longlong *plVar34;
  byte *pbVar35;
  float fVar36;
  double dVar37;
  undefined8 uVar38;
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
  undefined8 extraout_XMM0_Qa_11;
  undefined8 extraout_XMM0_Qa_12;
  undefined8 extraout_XMM0_Qa_13;
  undefined8 extraout_XMM0_Qa_14;
  undefined8 extraout_XMM0_Qa_15;
  undefined8 extraout_XMM0_Qa_16;
  undefined8 extraout_XMM0_Qa_17;
  undefined8 extraout_XMM0_Qa_18;
  undefined8 extraout_XMM0_Qa_19;
  undefined8 extraout_XMM0_Qa_20;
  undefined8 extraout_XMM0_Qa_21;
  undefined8 extraout_XMM0_Qa_22;
  undefined8 extraout_XMM0_Qa_23;
  undefined8 extraout_XMM0_Qa_24;
  undefined8 extraout_XMM0_Qa_25;
  undefined8 extraout_XMM0_Qa_26;
  undefined8 extraout_XMM0_Qa_27;
  double dVar39;
  LARGE_INTEGER local_res18;
  longlong *local_res20;
  undefined8 in_stack_fffffffffffff708;
  char *local_8c0;
  char *local_8b8;
  char *pcStack_8b0;
  char *local_8a8;
  undefined8 uStack_8a0;
  char *local_898;
  undefined1 local_888 [8];
  int *local_880;
  longlong *plStack_878;
  uint local_870;
  longlong *local_868;
  longlong local_860;
  char *local_858;
  char *pcStack_850;
  char *local_848;
  undefined8 uStack_840;
  char *local_838;
  double local_828;
  double dStack_820;
  double local_818;
  longlong *local_810;
  longlong local_808;
  undefined1 local_7f8 [8];
  longlong local_7f0;
  longlong local_7e8;
  undefined8 local_7d0;
  undefined1 local_7a7;
  undefined4 local_7a4;
  char *local_7a0;
  undefined4 local_798;
  undefined4 local_794;
  char local_790;
  double local_788;
  undefined8 uStack_780;
  undefined1 local_778 [48];
  int *local_748;
  int *local_740;
  undefined1 local_6f8 [8];
  undefined1 local_6f0 [16];
  double local_6e0;
  double local_6d8;
  undefined1 local_6c8 [48];
  undefined8 local_698;
  undefined8 local_690;
  undefined8 *local_480;
  undefined1 local_478 [88];
  ulonglong local_420;
  void *local_418;
  void *local_410;
  undefined1 local_400 [40];
  undefined1 local_3d8 [848];
  undefined1 local_88;
  
  lVar22 = *(longlong *)(param_3.QuadPart + 600);
  local_res18 = param_3;
  local_res20 = param_4;
  local_860 = lVar22;
  if ((lVar22 == 0) || (*(longlong *)(param_3.QuadPart + 0x260) == 0)) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  plVar27 = (longlong *)(lVar22 + 0x428);
  local_808 = *(longlong *)(lVar22 + 0x408);
  local_810 = *(longlong **)(*(longlong *)(param_3.QuadPart + 0x250) + 0x10);
  lVar21 = *local_810;
  if (*(longlong *)(lVar21 + 0x50) != 0) {
    LOCK();
    piVar33 = (int *)(*(longlong *)(lVar21 + 0x50) + 8);
    *piVar33 = *piVar33 + 1;
    UNLOCK();
  }
  local_880 = *(int **)(lVar21 + 0x48);
  plVar19 = *(longlong **)(lVar21 + 0x50);
  plStack_878 = plVar19;
  local_868 = plVar27;
  if ((local_880 != (int *)0x0) &&
     (*(longlong *)(local_880 + 0x28) != *(longlong *)(param_1 + 0xd60))) {
    *(longlong *)(param_1 + 0xd60) = *(longlong *)(local_880 + 0x28);
    FUN_14061d1f0(*(undefined8 *)(param_1 + 0xd68),local_810,local_810);
  }
  local_8c0 = (char *)0xffffffffffffffff;
  if (plVar19 != (longlong *)0x0) {
    LOCK();
    plVar34 = plVar19 + 1;
    lVar21 = *plVar34;
    *(int *)plVar34 = (int)*plVar34 + -1;
    UNLOCK();
    if ((int)lVar21 == 1) {
      (**(code **)*plVar19)(plVar19);
      LOCK();
      piVar33 = (int *)((longlong)plVar19 + 0xc);
      iVar15 = *piVar33;
      *piVar33 = *piVar33 + -1;
      UNLOCK();
      if (iVar15 == 1) {
        (**(code **)(*plVar19 + 8))(plVar19);
      }
    }
  }
  puVar18 = (undefined8 *)FUN_140783840(param_1,&local_828,plVar27);
  uVar38 = puVar18[1];
  *(undefined8 *)(param_1 + 0x90) = *puVar18;
  *(undefined8 *)(param_1 + 0x98) = uVar38;
  FUN_140784ad0(param_1,lVar22);
  FUN_1407255f0();
  if (DAT_140b819d0 == '\0') {
                    /* WARNING: Subroutine does not return */
    FUN_1402f2020();
  }
  plVar19 = (longlong *)FUN_1402ed700(&DAT_140b819a0);
  pcStack_850 = (char *)0x0;
  pcVar20 = (char *)FUN_140003270(0x20);
  uVar38 = s_trackeditor_hide_controls_140a89608._8_8_;
  local_848 = (char *)0x19;
  uStack_840 = 0x1f;
  *(undefined8 *)pcVar20 = s_trackeditor_hide_controls_140a89608._0_8_;
  *(undefined8 *)(pcVar20 + 8) = uVar38;
  *(undefined8 *)(pcVar20 + 0x10) = s_trackeditor_hide_controls_140a89608._16_8_;
  pcVar20[0x18] = s_trackeditor_hide_controls_140a89608[0x18];
  pcVar20[0x19] = '\0';
  plVar27 = (longlong *)plVar19[2];
  local_858 = pcVar20;
  if (plVar27 == (longlong *)0x0) {
LAB_1407865a3:
    free(pcVar20);
LAB_1407865b0:
    uVar13 = 0;
  }
  else {
    plVar34 = plVar19;
    do {
      plVar26 = plVar27 + 4;
      uVar2 = plVar27[6];
      if (0xf < (ulonglong)plVar27[7]) {
        plVar26 = (longlong *)*plVar26;
      }
      uVar28 = uVar2;
      if (0x19 < uVar2) {
        uVar28 = 0x19;
      }
      iVar15 = memcmp(plVar26,pcVar20,uVar28);
      param_4 = local_res20;
      plVar26 = plVar27;
      if (iVar15 == 0) {
        if (0x18 < uVar2) goto LAB_140786511;
      }
      else if (-1 < iVar15) {
LAB_140786511:
        plVar26 = plVar27 + 1;
        plVar34 = plVar27;
      }
      plVar27 = (longlong *)*plVar26;
    } while (plVar27 != (longlong *)0x0);
    if (plVar34 == plVar19) goto LAB_1407865a3;
    plVar27 = plVar34 + 4;
    uVar2 = plVar34[6];
    if (0xf < (ulonglong)plVar34[7]) {
      plVar27 = (longlong *)*plVar27;
    }
    sVar29 = 0x19;
    if (uVar2 < 0x19) {
      sVar29 = uVar2;
    }
    iVar15 = memcmp(pcVar20,plVar27,sVar29);
    if (iVar15 == 0) {
      if (0x19 < uVar2) goto LAB_1407865a3;
    }
    else if (iVar15 < 0) goto LAB_1407865a3;
    free(pcVar20);
    pbVar35 = (byte *)(plVar34 + 8);
    if (0xf < (ulonglong)plVar34[0xb]) {
      pbVar35 = *(byte **)pbVar35;
    }
    if (plVar34[10] != 1) goto LAB_1407865b0;
    if (*pbVar35 == 0x31) {
      uVar16 = 0;
    }
    else {
      uVar16 = -(uint)(*pbVar35 < 0x31) | 1;
    }
    if (uVar16 != 0) goto LAB_1407865b0;
    uVar13 = 1;
  }
  *(undefined1 *)(param_1 + 0xe0) = uVar13;
  pcStack_850 = (char *)0x0;
  pcVar20 = (char *)FUN_140003270(0x20);
  uVar38 = s_trackeditor_hold_keybinds_140a89720._8_8_;
  local_848 = (char *)0x19;
  uStack_840 = 0x1f;
  *(undefined8 *)pcVar20 = s_trackeditor_hold_keybinds_140a89720._0_8_;
  *(undefined8 *)(pcVar20 + 8) = uVar38;
  *(undefined8 *)(pcVar20 + 0x10) = s_trackeditor_hold_keybinds_140a89720._16_8_;
  pcVar20[0x18] = s_trackeditor_hold_keybinds_140a89720[0x18];
  pcVar20[0x19] = '\0';
  plVar27 = (longlong *)plVar19[2];
  plVar34 = plVar19;
  local_858 = pcVar20;
  if (plVar27 == (longlong *)0x0) {
LAB_1407866a1:
    free(pcVar20);
LAB_1407866ae:
    uVar13 = 0;
  }
  else {
    do {
      plVar26 = plVar27 + 4;
      uVar2 = plVar27[6];
      if (0xf < (ulonglong)plVar27[7]) {
        plVar26 = (longlong *)*plVar26;
      }
      uVar28 = uVar2;
      if (0x19 < uVar2) {
        uVar28 = 0x19;
      }
      iVar15 = memcmp(plVar26,pcVar20,uVar28);
      param_4 = local_res20;
      plVar26 = plVar27;
      if (iVar15 == 0) {
        if (0x18 < uVar2) goto LAB_140786651;
      }
      else if (-1 < iVar15) {
LAB_140786651:
        plVar26 = plVar27 + 1;
        plVar34 = plVar27;
      }
      plVar27 = (longlong *)*plVar26;
    } while (plVar27 != (longlong *)0x0);
    if (plVar34 == plVar19) goto LAB_1407866a1;
    plVar27 = plVar34 + 4;
    uVar2 = plVar34[6];
    if (0xf < (ulonglong)plVar34[7]) {
      plVar27 = (longlong *)*plVar27;
    }
    sVar29 = 0x19;
    if (uVar2 < 0x19) {
      sVar29 = uVar2;
    }
    iVar15 = memcmp(pcVar20,plVar27,sVar29);
    if (iVar15 == 0) {
      if (0x19 < uVar2) goto LAB_1407866a1;
    }
    else if (iVar15 < 0) goto LAB_1407866a1;
    free(pcVar20);
    pbVar35 = (byte *)(plVar34 + 8);
    if (0xf < (ulonglong)plVar34[0xb]) {
      pbVar35 = *(byte **)pbVar35;
    }
    if (plVar34[10] != 1) goto LAB_1407866ae;
    if (*pbVar35 == 0x31) {
      uVar16 = 0;
    }
    else {
      uVar16 = -(uint)(*pbVar35 < 0x31) | 1;
    }
    if (uVar16 != 0) goto LAB_1407866ae;
    uVar13 = 1;
  }
  *(undefined1 *)(param_1 + 0xc60) = uVar13;
  pcStack_850 = (char *)0x0;
  pcVar20 = (char *)FUN_140003270(0x20);
  uVar38 = s_trackeditor_clickable_tracks_140a895d0._8_8_;
  uVar32 = (undefined4)((ulonglong)in_stack_fffffffffffff708 >> 0x20);
  local_848 = (char *)0x1c;
  uStack_840 = 0x1f;
  *(undefined8 *)pcVar20 = s_trackeditor_clickable_tracks_140a895d0._0_8_;
  *(undefined8 *)(pcVar20 + 8) = uVar38;
  *(undefined8 *)(pcVar20 + 0x10) = s_trackeditor_clickable_tracks_140a895d0._16_8_;
  *(undefined4 *)(pcVar20 + 0x18) = s_trackeditor_clickable_tracks_140a895d0._24_4_;
  pcVar20[0x1c] = '\0';
  plVar27 = (longlong *)plVar19[2];
  local_858 = pcVar20;
  if (plVar27 == (longlong *)0x0) {
LAB_140786825:
    FUN_140003040(&local_858,pcVar20,0x1f);
LAB_140786837:
    uVar13 = 0;
  }
  else {
    plVar34 = plVar19;
    do {
      plVar26 = plVar27 + 4;
      uVar2 = plVar27[6];
      if (0xf < (ulonglong)plVar27[7]) {
        plVar26 = (longlong *)*plVar26;
      }
      uVar28 = uVar2;
      if (0x1c < uVar2) {
        uVar28 = 0x1c;
      }
      iVar15 = memcmp(plVar26,pcVar20,uVar28);
      param_4 = local_res20;
      uVar32 = (undefined4)((ulonglong)in_stack_fffffffffffff708 >> 0x20);
      plVar26 = plVar27;
      if (iVar15 == 0) {
        if (0x1b < uVar2) goto LAB_1407867a2;
      }
      else if (-1 < iVar15) {
LAB_1407867a2:
        plVar26 = plVar27 + 1;
        plVar34 = plVar27;
      }
      plVar27 = (longlong *)*plVar26;
    } while (plVar27 != (longlong *)0x0);
    if (plVar34 == plVar19) goto LAB_140786825;
    plVar27 = plVar34 + 4;
    if (0xf < (ulonglong)plVar34[7]) {
      plVar27 = (longlong *)*plVar27;
    }
    iVar15 = FUN_140238820(pcVar20,0x1c,plVar27);
    if (iVar15 < 0) goto LAB_140786825;
    FUN_140003040(&local_858,pcVar20,0x1f);
    pbVar35 = (byte *)(plVar34 + 8);
    if (0xf < (ulonglong)plVar34[0xb]) {
      pbVar35 = *(byte **)pbVar35;
    }
    if (plVar34[10] != 1) goto LAB_140786837;
    if (*pbVar35 == 0x31) {
      uVar16 = 0;
    }
    else {
      uVar16 = -(uint)(*pbVar35 < 0x31) | 1;
    }
    if (uVar16 != 0) goto LAB_140786837;
    uVar13 = 1;
  }
  *(undefined1 *)(param_1 + 0xc61) = uVar13;
  *(undefined1 *)(param_1 + 0x6d8) = 0;
  if (*(int *)(param_1 + 0x7f0) == 0) {
    *(undefined4 *)(param_1 + 0x7f0) = 1;
  }
  uVar13 = FUN_14073ec90(local_res18.QuadPart,3);
  *(undefined1 *)(param_1 + 0x6d8) = uVar13;
  uVar16 = (**(code **)(**(longlong **)(param_1 + 0xd88) + 8))();
  bVar5 = (double)param_2[0x25] < DAT_140aabe98;
  if ((uVar16 == 1) || (uVar16 == 9)) {
    bVar6 = true;
  }
  else {
    bVar6 = false;
  }
  local_870 = uVar16;
  if (DAT_140aabe78 <= (double)param_2[0x25]) {
    FUN_14024ac30(param_2 + 0x23,local_6f8);
    local_880 = (int *)param_2[0x23];
    plStack_878 = (longlong *)param_2[0x24];
    dVar37 = (double)FUN_140249120(DAT_140aac0c8,&local_880);
    if ((0.0 < local_6e0) && (0.0 < local_6d8)) {
      dVar39 = (double)local_6f0._8_8_ - dVar37;
      local_6f0._8_4_ = SUB84(dVar39,0);
      local_6f0._0_8_ = (double)local_6f0._0_8_ - dVar37;
      local_6f0._12_4_ = (int)((ulonglong)dVar39 >> 0x20);
      local_6e0 = local_6e0 + dVar37 + dVar37;
      local_6d8 = local_6d8 + dVar37 + dVar37;
    }
    uVar17 = (uint)(*(char *)(param_5 + 0x1c) != '\0');
    uVar25 = uVar17 | 2;
    if (*(char *)(param_5 + 0x1d) == '\0') {
      uVar25 = uVar17;
    }
    if (uVar16 == 1) {
      uVar38 = *(undefined8 *)(param_1 + 0x6d0);
    }
    else {
      uVar38 = 0;
    }
    FUN_140626510(param_2,local_868,local_6f0,uVar38,CONCAT44(uVar32,uVar25));
  }
  uVar38 = FUN_1402d3a10(local_778);
  if (*(longlong *)(*(longlong *)(*param_4 + 0x260) + 0x400) != 0) {
    FUN_1402d3970(local_778,local_6f8);
    FUN_1407289e0(local_778);
    uVar38 = FUN_1402d67f0(local_6c8);
  }
  piVar33 = local_740;
  lVar22 = *(longlong *)(*(longlong *)(*param_4 + 0x260) + 0x408);
  if ((bVar6) && (lVar22 != 0)) {
    *(longlong *)(param_1 + 0x6d0) = lVar22;
  }
  *(undefined8 *)(param_1 + 0xc98) = 0;
  *(undefined4 *)(param_1 + 0xca4) = 0;
  *(undefined2 *)(param_1 + 0xca8) = 0;
  *(undefined1 *)(param_1 + 0xcaa) = 0;
  dVar37 = DAT_140aabd08;
  piVar3 = local_748;
  if (bVar5) {
    for (; piVar8 = local_748, piVar3 != piVar33; piVar3 = piVar3 + 0x1c) {
      if (*piVar3 == 8) {
        cVar14 = FUN_140726750(uVar38,"track_layer_up",piVar3);
        if (cVar14 == '\0') {
          cVar14 = FUN_140726750(extraout_XMM0_Qa,"track_layer_down",piVar3);
          uVar38 = extraout_XMM0_Qa_00;
          if (cVar14 != '\0') {
            *(undefined1 *)(param_1 + 0xca8) = 1;
          }
        }
        else {
          *(undefined1 *)(param_1 + 0xca7) = 1;
          uVar38 = extraout_XMM0_Qa;
        }
      }
    }
    goto joined_r0x000140787bac;
  }
  if ((uVar16 < 0x1a) && ((0x27d5402U >> (uVar16 & 0x1f) & 1) != 0)) {
    bVar5 = true;
    if ((((((uVar16 != 1) && (uVar16 != 10)) && (uVar16 != 0xe)) &&
         ((uVar16 != 0x10 && (uVar16 != 0x12)))) && (uVar16 != 0x13)) &&
       ((uVar16 != 0x14 && (bVar6 = true, uVar16 != 0x15)))) goto LAB_140786b00;
    bVar6 = true;
    bVar7 = true;
  }
  else {
    bVar5 = false;
    bVar6 = false;
LAB_140786b00:
    bVar7 = false;
  }
  local_8b8 = _DAT_140aad760;
  pcStack_8b0 = _UNK_140aad768;
  local_8a8 = (char *)0xffffffffffffffff;
  uVar32 = 0;
  uStack_8a0 = 0;
  uVar31 = 0;
  local_898 = (char *)0xffffffffffffffff;
  local_880 = local_740;
  pcVar20 = _DAT_140aad760;
  piVar33 = local_748;
  uVar30 = 0;
  if (local_748 == local_740) {
    local_8c0 = (char *)0xffffffffffffffff;
  }
  else {
    do {
      uVar32 = uVar30;
      LVar9 = local_res18;
      lVar22 = *(longlong *)(local_res18.QuadPart + 0x250);
      for (piVar3 = *(int **)(*(longlong *)(lVar22 + 0x4f0) +
                             SUB168((ZEXT816(0) << 0x40 | ZEXT816(3)) %
                                    ZEXT416(*(uint *)(lVar22 + 0x4f8)),0) * 8); piVar3 != (int *)0x0
          ; piVar3 = *(int **)(piVar3 + 6)) {
        if (*piVar3 == 3) {
          if ((piVar3 != *(int **)(*(longlong *)(lVar22 + 0x4f0) + *(longlong *)(lVar22 + 0x4f8) * 8
                                  )) && (*(ulonglong *)(piVar3 + 4) < *(ulonglong *)(piVar3 + 2)))
          goto LAB_14078759d;
          break;
        }
      }
      if (*(char *)(param_1 + 0x6e0) != '\0') goto LAB_14078759d;
      cVar14 = (**(code **)(**(longlong **)(local_res18.QuadPart + 0x200) + 0x38))();
      if (cVar14 != '\0') break;
      FUN_140786200(param_1,local_7f8,local_810,param_2,LVar9.QuadPart,piVar33);
      lVar22 = local_860;
      iVar15 = *piVar33;
      if ((iVar15 == 8) && (0 < piVar33[0xf])) {
        cVar14 = FUN_140786290();
        if (cVar14 != '\0') {
          (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x40))();
        }
LAB_140786d2a:
        uVar31 = uStack_8a0._4_4_;
      }
      else {
        if ((bVar5) &&
           (((*(char *)((longlong)param_2 + 0xfa) != '\0' && (iVar15 == 8)) &&
            (piVar33[0xe] == 0x7a)))) {
          uVar16 = local_870;
          if (*(longlong *)(param_1 + 0xd80) != 0) {
            lVar21 = FUN_1407b57c0(local_7d0);
            lVar22 = *(longlong *)(param_1 + 0xd78);
            FUN_140324a00(lVar21 + 0x20,lVar22 + 0x10);
            FUN_140324ab0(lVar21 + 0xe0,lVar22 + 0xd0);
            FUN_140324b60(lVar21 + 0x1a0,lVar22 + 400);
            FUN_140324c10(lVar21 + 0x260,lVar22 + 0x250);
            FUN_140324ca0(lVar21 + 800,lVar22 + 0x310);
            FUN_140324d30(lVar21 + 0x3e0,lVar22 + 0x3d0);
            FUN_140324dc0(lVar21 + 0x4a0,lVar22 + 0x490);
            FUN_140324e50(lVar21 + 0x560,lVar22 + 0x550);
            FUN_1407c16d0(param_1 + 0xd70);
            uVar16 = local_870;
          }
          goto LAB_140786d2a;
        }
        local_7a7 = *(undefined1 *)(param_1 + 0xe0);
        local_7a4 = *(undefined4 *)(*(longlong *)(local_860 + 0x410) + 0x5c);
        if (iVar15 == 0) {
          (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x20))();
        }
        else if (iVar15 == 1) {
          (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x28))();
        }
        else if (iVar15 == 6) {
          (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x18))();
        }
        else if (iVar15 == 8) {
          if (*(int *)(local_7e8 + 0x38) == 0x1b) {
            uVar13 = (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x48))();
            *(undefined1 *)(param_1 + 0x6d8) = uVar13;
          }
          else {
            (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x30))();
            cVar14 = FUN_140786290();
            if (cVar14 != '\0') {
              (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x40))();
            }
          }
        }
        else if (iVar15 == 9) {
          (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x38))();
        }
        if (*piVar33 == 8) {
          if (*(char *)(local_7f0 + 0xfa) == '\0') {
            cVar14 = FUN_140726750(local_7f0,"track_move",piVar33);
            if (cVar14 == '\0') {
              cVar14 = FUN_140726750(extraout_XMM0_Qa_01,"track_new",piVar33);
              if (cVar14 == '\0') {
                cVar14 = FUN_140726750(extraout_XMM0_Qa_02,"track_branch",piVar33);
                if (cVar14 == '\0') {
                  cVar14 = FUN_140726750(extraout_XMM0_Qa_03,"track_sat_split",piVar33);
                  if (cVar14 == '\0') {
                    cVar14 = FUN_140726750(extraout_XMM0_Qa_04,"track_sat_track_tape",piVar33);
                    if (cVar14 == '\0') {
                      cVar14 = FUN_140726750(extraout_XMM0_Qa_05,"track_sat_building_tape",piVar33);
                      if (cVar14 == '\0') {
                        cVar14 = FUN_140726750(extraout_XMM0_Qa_06,"track_sat_parallel_tape",piVar33
                                              );
                        if (cVar14 == '\0') {
                          cVar14 = FUN_140726750(extraout_XMM0_Qa_07,"track_station",piVar33);
                          if (cVar14 == '\0') {
                            cVar14 = FUN_140726750(extraout_XMM0_Qa_08,"track_building",piVar33);
                            if (cVar14 == '\0') {
                              cVar14 = FUN_140726750(extraout_XMM0_Qa_09,"track_poi",piVar33);
                              if (cVar14 == '\0') {
                                cVar14 = FUN_140726750(extraout_XMM0_Qa_10,"track_build_selection",
                                                       piVar33);
                                if (cVar14 == '\0') {
                                  cVar14 = FUN_140726750(extraout_XMM0_Qa_11,"track_sel_signal",
                                                         piVar33);
                                  if (cVar14 == '\0') {
                                    cVar14 = FUN_140726750(extraout_XMM0_Qa_12,"track_new_signal",
                                                           piVar33);
                                    if (cVar14 == '\0') {
                                      cVar14 = FUN_140726750(extraout_XMM0_Qa_13,"track_2x",piVar33)
                                      ;
                                      if (cVar14 == '\0') {
                                        cVar14 = FUN_140726750(extraout_XMM0_Qa_14,
                                                               "track_flip_signal",piVar33);
                                        if (cVar14 == '\0') {
                                          cVar14 = FUN_140726750(extraout_XMM0_Qa_15,
                                                                 "track_plat_promote",piVar33);
                                          if (cVar14 == '\0') {
                                            cVar14 = FUN_140726750(extraout_XMM0_Qa_16,
                                                                   "track_layer_up",piVar33);
                                            if (cVar14 == '\0') {
                                              cVar14 = FUN_140726750(extraout_XMM0_Qa_17,
                                                                     "track_layer_down",piVar33);
                                              if (cVar14 == '\0') {
                                                cVar14 = FUN_140726750(extraout_XMM0_Qa_18,
                                                                       "track_pick_parent",piVar33);
                                                if ((cVar14 != '\0') && (uVar16 == 1)) {
                                                  if (*(longlong *)(param_1 + 0x18) == 1) {
                                                    if (*(longlong *)(param_1 + 0x48) == 0) {
                                                      plVar27 = (longlong *)
                                                                **(longlong **)(param_1 + 8);
                                                      if (plVar27 == (longlong *)0x0) {
                                                        plVar19 = *(longlong **)(param_1 + 8) + 1;
                                                        plVar27 = (longlong *)*plVar19;
                                                        while (plVar27 == (longlong *)0x0) {
                                                          plVar19 = plVar19 + 1;
                                                          plVar27 = (longlong *)*plVar19;
                                                        }
                                                      }
                                                      if (((*plVar27 != 0) &&
                                                          (lVar22 = FUN_14032c420(*local_868),
                                                          lVar22 != 0)) &&
                                                         ((*(char *)(lVar22 + 0x20) != '\0' &&
                                                          (*(longlong *)(lVar22 + 0x3f0) == 0)))) {
                                                        *(undefined4 *)(param_1 + 0xc98) = 8;
                                                      }
                                                    }
                                                  }
                                                  else if ((*(longlong *)(param_1 + 0x18) == 0) &&
                                                          (*(longlong *)(param_1 + 0x48) == 1)) {
                                                    plVar27 = (longlong *)
                                                              **(longlong **)(param_1 + 0x38);
                                                    if (plVar27 == (longlong *)0x0) {
                                                      plVar19 = *(longlong **)(param_1 + 0x38) + 1;
                                                      plVar27 = (longlong *)*plVar19;
                                                      while (plVar27 == (longlong *)0x0) {
                                                        plVar19 = plVar19 + 1;
                                                        plVar27 = (longlong *)*plVar19;
                                                      }
                                                    }
                                                    if ((((*plVar27 != 0) &&
                                                         (lVar22 = FUN_14033f860(*local_868 + 0x100)
                                                         , lVar22 != 0)) &&
                                                        (*(char *)(lVar22 + 0x24) != '\0')) &&
                                                       (*(char *)((longlong)*(int *)(lVar22 + 8) *
                                                                  0x128 + 0x88 +
                                                                 *(longlong *)(local_808 + 0x108))
                                                        != '\0')) {
                                                      *(undefined4 *)(param_1 + 0xc98) = 0x11;
                                                    }
                                                  }
                                                }
                                              }
                                              else {
                                                *(undefined1 *)(param_1 + 0xca8) = 1;
                                              }
                                            }
                                            else {
                                              *(undefined1 *)(param_1 + 0xca7) = 1;
                                            }
                                          }
                                          else {
                                            *(undefined1 *)(param_1 + 0xcaa) = 1;
                                          }
                                        }
                                        else {
                                          *(undefined1 *)(param_1 + 0xca6) = 1;
                                        }
                                      }
                                      else {
                                        *(undefined1 *)(param_1 + 0xca4) = 1;
                                      }
                                    }
                                    else {
                                      *(undefined4 *)(param_1 + 0xc98) = 0x19;
                                    }
                                  }
                                  else {
                                    *(undefined4 *)(param_1 + 0xc98) = 0x16;
                                  }
                                }
                                else {
                                  *(undefined1 *)(param_1 + 0xca9) = 1;
                                }
                              }
                              else {
                                *(undefined4 *)(param_1 + 0xc98) = 0x10;
                              }
                            }
                            else {
                              *(undefined4 *)(param_1 + 0xc98) = 0xe;
                            }
                          }
                          else {
                            *(undefined4 *)(param_1 + 0xc98) = 0xc;
                          }
                        }
                        else {
                          *(undefined4 *)(param_1 + 0xc98) = 0x15;
                        }
                      }
                      else {
                        *(undefined4 *)(param_1 + 0xc98) = 0x14;
                      }
                    }
                    else {
                      *(undefined4 *)(param_1 + 0xc98) = 0x13;
                    }
                  }
                  else {
                    *(undefined4 *)(param_1 + 0xc98) = 0x12;
                  }
                }
                else {
                  *(undefined1 *)(param_1 + 0xca5) = 1;
                }
              }
              else {
                *(undefined4 *)(param_1 + 0xc98) = 10;
              }
            }
            else {
              *(undefined4 *)(param_1 + 0xc98) = 1;
            }
          }
          else if (uVar16 == 1) {
            if (*(int *)(local_7e8 + 0x38) == 99) {
              uVar38 = FUN_1403aac20(lVar22 + 0x428,local_6f8,param_1);
              FUN_140786110(param_1,lVar22,uVar38);
            }
            else {
              if (*(int *)(local_7e8 + 0x38) != 0x78) goto LAB_140786ecb;
              uVar38 = FUN_1403aac20(lVar22 + 0x428,local_6f8,param_1);
              FUN_140786110(param_1,lVar22,uVar38);
              FUN_140776ef0(local_888,local_7f8,0);
            }
          }
          else {
LAB_140786ecb:
            if ((bVar7) && (*(int *)(local_7e8 + 0x38) == 0x76)) {
              lVar22 = FUN_1407bcc40(local_7d0);
              FUN_1407b4de0(lVar22 + 0x20,param_1 + 0xe8);
              if ((*(char *)(local_7f0 + 0xf9) == '\0') ||
                 ((*(double *)(param_1 + 0x148) == 0.0 && (*(double *)(param_1 + 0x150) == 0.0)))) {
                local_828 = (double)*(float *)(local_7e8 + 0x18);
                dStack_820 = (double)*(float *)(local_7e8 + 0x1c);
                local_818 = dVar37;
                pdVar23 = (double *)FUN_140250b00(&local_858,local_7f0 + 0x1a0,&local_828);
                local_828 = *pdVar23;
                dStack_820 = pdVar23[1];
                pdVar23 = &local_828;
              }
              else {
                local_788 = *(double *)(param_1 + 0x148);
                uStack_780 = *(undefined8 *)(param_1 + 0x150);
                pdVar23 = &local_788;
              }
              dVar39 = pdVar23[1];
              *(double *)(lVar22 + 0x90) = *pdVar23;
              *(double *)(lVar22 + 0x98) = dVar39;
              *(longlong *)(lVar22 + 0xa0) = param_2[1];
            }
            else if ((uVar16 == 1) &&
                    ((*(int *)(local_7e8 + 0x38) == 0x62 && (*(char *)(local_7f0 + 0xfa) != '\0'))))
            {
              FUN_1403aac20(local_868,local_6f8,param_1);
              lVar22 = FUN_1407bcc40(local_7d0);
              FUN_1407b4de0(lVar22 + 0x20);
              *(undefined8 *)(lVar22 + 0x90) = local_698;
              *(undefined8 *)(lVar22 + 0x98) = local_690;
              *(longlong *)(lVar22 + 0xa0) = param_2[1];
              FUN_1402ff120(local_6f8);
            }
          }
        }
        if (*piVar33 == 9) {
          cVar14 = FUN_140726750();
          if (cVar14 == '\0') {
            cVar14 = FUN_140726750(extraout_XMM0_Qa_19,"track_new",piVar33);
            if (cVar14 == '\0') {
              cVar14 = FUN_140726750(extraout_XMM0_Qa_20,"track_sat_split",piVar33);
              if (cVar14 == '\0') {
                cVar14 = FUN_140726750(extraout_XMM0_Qa_21,"track_sat_track_tape",piVar33);
                if (cVar14 == '\0') {
                  cVar14 = FUN_140726750(extraout_XMM0_Qa_22,"track_sat_building_tape",piVar33);
                  if (cVar14 == '\0') {
                    cVar14 = FUN_140726750(extraout_XMM0_Qa_23,"track_sat_parallel_tape",piVar33);
                    if (cVar14 == '\0') {
                      cVar14 = FUN_140726750(extraout_XMM0_Qa_24,"track_station",piVar33);
                      if (cVar14 == '\0') {
                        cVar14 = FUN_140726750(extraout_XMM0_Qa_25,"track_building",piVar33);
                        if (cVar14 == '\0') {
                          cVar14 = FUN_140726750(extraout_XMM0_Qa_26,"track_sel_signal",piVar33);
                          if (cVar14 == '\0') {
                            cVar14 = FUN_140726750(extraout_XMM0_Qa_27,"track_new_signal",piVar33);
                            if (cVar14 != '\0') {
                              *(undefined4 *)(param_1 + 0xc9c) = 0x19;
                            }
                          }
                          else {
                            *(undefined4 *)(param_1 + 0xc9c) = 0x16;
                          }
                        }
                        else {
                          *(undefined4 *)(param_1 + 0xc9c) = 0xe;
                        }
                      }
                      else {
                        *(undefined4 *)(param_1 + 0xc9c) = 0xc;
                      }
                    }
                    else {
                      *(undefined4 *)(param_1 + 0xc9c) = 0x15;
                    }
                  }
                  else {
                    *(undefined4 *)(param_1 + 0xc9c) = 0x14;
                  }
                }
                else {
                  *(undefined4 *)(param_1 + 0xc9c) = 0x13;
                }
              }
              else {
                *(undefined4 *)(param_1 + 0xc9c) = 0x12;
              }
            }
            else {
              *(undefined4 *)(param_1 + 0xc9c) = 10;
            }
          }
          else {
            *(undefined4 *)(param_1 + 0xc9c) = 1;
          }
        }
        pcVar20 = (char *)0x0;
        local_8b8 = (char *)0x0;
        local_8c0 = (char *)0x0;
        local_898 = (char *)0x0;
        pcStack_8b0 = (char *)0x0;
        local_8a8 = (char *)0x0;
        uVar32 = 0;
        uStack_8a0 = 0;
        uVar30 = 0;
        uVar31 = 0;
        if (local_790 == '\x01') {
          local_8b8 = local_7a0;
          pcVar20 = local_7a0;
          uVar31 = uVar30;
        }
        else if (local_790 == '\x02') {
          local_8c0 = local_7a0;
          local_898 = local_8c0;
          uVar31 = uVar30;
        }
        else if (local_790 == '\x03') {
          pcStack_8b0 = local_7a0;
          uVar31 = uVar30;
        }
        else {
          uVar32 = 0;
          if (local_790 == '\x04') {
            local_8a8 = local_7a0;
            uStack_8a0 = CONCAT44(local_794,local_798);
            pcVar20 = (char *)0x0;
            uVar32 = local_798;
            uVar31 = local_794;
          }
        }
      }
      piVar33 = piVar33 + 0x1c;
      uVar30 = uVar32;
      bVar5 = bVar6;
    } while (piVar33 != local_880);
    uVar31 = uStack_8a0._4_4_;
  }
LAB_14078759d:
  if (uVar16 != *(uint *)(param_1 + 0xc90)) {
    *(undefined8 *)(param_1 + 0xc68) = 0;
    *(undefined8 *)(param_1 + 0xc70) = 0;
    *(undefined8 *)(param_1 + 0xc78) = 0;
    *(undefined8 *)(param_1 + 0xc88) = 0;
  }
  if (pcVar20 == (char *)0xffffffffffffffff) {
    pcVar20 = *(char **)(param_1 + 0xc68);
    local_8b8 = pcVar20;
  }
  pcVar10 = local_8b8;
  *(char **)(param_1 + 0xc68) = pcVar20;
  if (pcStack_8b0 == (char *)0xffffffffffffffff) {
    pcStack_8b0 = *(char **)(param_1 + 0xc70);
  }
  pcVar20 = pcStack_8b0;
  *(char **)(param_1 + 0xc70) = pcStack_8b0;
  if (local_8a8 == (char *)0xffffffffffffffff) {
    local_8a8 = *(char **)(param_1 + 0xc78);
    uVar32 = *(undefined4 *)(param_1 + 0xc80);
    uVar31 = *(undefined4 *)(param_1 + 0xc84);
    uStack_8a0 = *(undefined8 *)(param_1 + 0xc80);
  }
  *(char **)(param_1 + 0xc78) = local_8a8;
  *(undefined4 *)(param_1 + 0xc80) = uVar32;
  *(undefined4 *)(param_1 + 0xc84) = uVar31;
  if (local_8c0 == (char *)0xffffffffffffffff) {
    local_8c0 = *(char **)(param_1 + 0xc88);
    local_898 = local_8c0;
  }
  *(char **)(param_1 + 0xc88) = local_8c0;
  *(uint *)(param_1 + 0xc90) = uVar16;
  if (uVar16 - 0x12 < 4) {
    *(uint *)(param_1 + 0xc94) = uVar16;
  }
  FUN_1407685f0(local_6f8,param_2,local_res18.QuadPart,param_1 + 0xd68,local_860,param_1 + 0x700,
                param_5);
  FUN_1407b4ea0(local_3d8);
  local_88 = *(undefined1 *)(param_1 + 0xe0);
  FUN_1407b5080(local_3d8,param_1 + 0x7f0);
  pcVar12 = local_898;
  uVar38 = uStack_8a0;
  pcVar11 = local_8a8;
  lVar22 = 0;
  lVar21 = 0;
  if ((*(longlong *)(param_1 + 0xd88) != 0) &&
     (lVar4 = *(longlong *)(*(longlong *)(param_1 + 0xd88) + 0x10), lVar22 = 0, lVar21 = 0,
     lVar4 != 0)) {
    lVar22 = lVar4 + 0x16f8;
    lVar21 = lVar4 + 0x1728;
  }
  local_848 = local_8a8;
  uStack_840 = uStack_8a0;
  local_838 = local_898;
  local_858 = pcVar10;
  pcStack_850 = pcVar20;
  FUN_140767a10(local_3d8,param_2,local_860,param_2 + 0x3d,lVar22,lVar21,0,0,param_1,&local_858);
  FUN_14076ca80(local_6f8,local_3d8);
  (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x50))
            (*(longlong **)(param_1 + 0xd88),local_6f8,local_3d8);
  FUN_14056e900(&local_698,param_2,*param_2 + 0x970);
  puVar24 = (undefined2 *)FUN_140519fd0(&DAT_140b5da20,local_888,*local_480);
  uVar1 = *puVar24;
  if (2 < local_420) {
    local_880 = (int *)local_480[0x29];
    plStack_878 = (longlong *)local_480[0x2a];
    FUN_140519370(&DAT_140b5da20,&local_880);
    FUN_140833f20(0,DAT_140b5daa6,uVar1,0xffffffff);
    FUN_14051a140(&DAT_140b5da20,local_400,local_478);
  }
  local_410 = local_418;
  local_420 = 0;
  FUN_1407b4fe0(local_3d8);
  if (local_418 != (void *)0x0) {
    free(local_418);
  }
  FUN_14056df60(&local_698);
  LVar9 = local_res18;
  lVar22 = *(longlong *)(*(longlong *)(param_1 + 0xd88) + 0x10);
  piVar8 = local_748;
  if (lVar22 != 0) {
    FUN_1407685f0(local_6f8,param_2,local_res18.QuadPart,param_1 + 0xd68,lVar22,param_1 + 0x700,
                  param_5);
    FUN_1407b4ea0(local_3d8);
    local_88 = *(undefined1 *)(param_1 + 0xe0);
    FUN_1407b5080(local_3d8,param_1 + 0x7f0);
    lVar22 = *(longlong *)(*(longlong *)(param_1 + 0xd88) + 0x10);
    local_8b8 = pcVar10;
    pcStack_8b0 = pcVar20;
    local_8a8 = pcVar11;
    uStack_8a0 = uVar38;
    local_898 = pcVar12;
    FUN_140767a10(local_3d8,param_2,lVar22,param_2 + 0x3d,0,0,lVar22 + 0x1690,lVar22 + 0x15a0,
                  param_1,&local_8b8);
    lVar22 = *(longlong *)(LVar9.QuadPart + 0x250);
    fVar36 = DAT_140aabae4;
    if (*(longlong *)(lVar22 + 0x28) != 0) {
      for (piVar33 = *(int **)(*(longlong *)(lVar22 + 0x4f0) +
                              SUB168((ZEXT816(0) << 0x40 | ZEXT816(3)) %
                                     ZEXT416(*(uint *)(lVar22 + 0x4f8)),0) * 8);
          fVar36 = DAT_140aab9dc, piVar33 != (int *)0x0; piVar33 = *(int **)(piVar33 + 6)) {
        if (*piVar33 == 3) {
          if ((piVar33 !=
               *(int **)(*(longlong *)(lVar22 + 0x4f0) + *(longlong *)(lVar22 + 0x4f8) * 8)) &&
             (*(ulonglong *)(piVar33 + 4) < *(ulonglong *)(piVar33 + 2))) {
            dVar39 = (double)FUN_140249580(param_1 + 0x6b8);
            if (DAT_140aac0f8 < dVar39) {
              local_res18.QuadPart = 0;
              QueryPerformanceCounter(&local_res18);
              ((LARGE_INTEGER *)(param_1 + 0x6b8))->QuadPart = (LONGLONG)local_res18;
            }
            dVar39 = (double)FUN_140249580(param_1 + 0x6b8);
            dVar39 = cos(dVar39 * DAT_140aabef0);
            fVar36 = (float)(((double)CONCAT44((uint)((ulonglong)dVar39 >> 0x20) & _UNK_140aad5e4,
                                               SUB84(dVar39,0) & _DAT_140aad5e0) + dVar37) *
                            DAT_140aabc48);
          }
          break;
        }
      }
    }
    *(float *)(*(longlong *)(*(longlong *)(param_1 + 0xd88) + 0x10) + 0x16c0) = fVar36;
    if (DAT_140b77b38 != '\0') {
      FUN_14076ca80(local_6f8,local_3d8);
    }
    FUN_140760850(*(undefined8 *)(*(longlong *)(param_1 + 0xd88) + 0x10),local_860);
    (**(code **)(**(longlong **)(param_1 + 0xd88) + 0x58))
              (*(longlong **)(param_1 + 0xd88),local_6f8,local_3d8);
    FUN_14056e900(&local_698,param_2,*param_2 + 0x970);
    puVar24 = (undefined2 *)FUN_140519fd0(&DAT_140b5da20,&local_res18,*local_480);
    uVar1 = *puVar24;
    if (2 < local_420) {
      local_880 = (int *)local_480[0x29];
      plStack_878 = (longlong *)local_480[0x2a];
      FUN_140519370(&DAT_140b5da20,&local_880);
      FUN_140833f20(0,DAT_140b5daa6,uVar1,0xffffffff);
      FUN_14051a140(&DAT_140b5da20,local_400,local_478);
    }
    local_410 = local_418;
    local_420 = 0;
    if (*(char *)(param_1 + 0xd8) != '\0') {
      FUN_140760e50(param_1 + 0xa0,param_2,LVar9.QuadPart);
    }
    FUN_1407b4fe0(local_3d8);
    if (local_418 != (void *)0x0) {
      free(local_418);
    }
    FUN_14056df60(&local_698);
    piVar8 = local_748;
  }
joined_r0x000140787bac:
  for (; piVar8 != local_740; piVar8 = piVar8 + 0x1c) {
    FUN_140002d30(piVar8 + 0x10);
  }
  if (local_748 != (int *)0x0) {
    free(local_748);
  }
  return;
}

