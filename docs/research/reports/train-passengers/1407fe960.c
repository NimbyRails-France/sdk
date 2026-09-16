// Candidate VA 1407fe960; RVA 0x7fe960
// Ghidra inferred prototype: undefined FUN_1407fe960()

void FUN_1407fe960(undefined8 *param_1,longlong *param_2)

{
  code *pcVar1;
  ulonglong *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  char cVar5;
  longlong lVar6;
  undefined8 *_Memory;
  undefined8 *_Memory_00;
  undefined8 *puVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 *puVar10;
  char *pcVar11;
  undefined8 *puVar12;
  longlong lVar13;
  longlong lVar14;
  void *pvVar15;
  ulonglong *puVar16;
  undefined4 uVar17;
  undefined8 in_stack_fffffffffffffe38;
  undefined4 uVar18;
  undefined8 in_stack_fffffffffffffe40;
  undefined4 uVar19;
  undefined8 local_158;
  undefined8 uStack_150;
  undefined8 local_148;
  ulonglong uStack_140;
  uint uStack_134;
  undefined8 *local_130;
  undefined8 *puStack_128;
  undefined8 *local_120;
  undefined8 local_118;
  undefined8 uStack_110;
  undefined8 local_108;
  undefined8 local_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  undefined8 local_e0;
  undefined1 local_d8 [40];
  undefined1 local_b0 [8];
  undefined1 local_a8 [32];
  undefined1 local_88 [80];

  lVar6 = FUN_1402d82e0("train_editor_basics","Basics");
  local_118 = 0;
  uStack_110 = 0;
  local_108 = 0;
  local_100 = 0;
  lVar13 = -1;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar6 + lVar14) != '\0');
  FUN_140002c00(&local_118,lVar6);
  lVar6 = FUN_1402d82e0("train_editor_orders","Orders");
  local_f8 = 0;
  uStack_f0 = 0;
  local_e8 = 0;
  local_e0 = 0;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar6 + lVar14) != '\0');
  FUN_140002c00(&local_f8,lVar6);
  local_130 = (undefined8 *)0x0;
  puStack_128 = (undefined8 *)0x0;
  local_120 = (undefined8 *)0x0;
  _Memory = (undefined8 *)thunk_FUN_140983da8(0x40);
  puVar10 = _Memory + 8;
  local_130 = _Memory;
  puStack_128 = puVar10;
  local_120 = puVar10;
  FUN_14041eae0(&local_118,local_d8,_Memory);
  _eh_vector_destructor_iterator_(&local_118,0x20,2,thunk_FUN_140002d30);
  lVar6 = FUN_1402d82e0("accounting","Accounting");
  local_158 = (void *)0x0;
  uStack_150 = 0;
  local_148 = 0;
  uStack_140 = 0;
  lVar14 = -1;
  do {
    lVar14 = lVar14 + 1;
  } while (*(char *)(lVar6 + lVar14) != '\0');
  FUN_140002c00(&local_158,lVar6);
  _Memory_00 = (undefined8 *)thunk_FUN_140983da8(0x80);
  uVar18 = (undefined4)((ulonglong)in_stack_fffffffffffffe38 >> 0x20);
  puVar7 = _Memory_00;
  if (_Memory != puVar10) {
    lVar6 = (longlong)_Memory - (longlong)_Memory_00;
    do {
      *puVar7 = 0;
      puVar7[1] = 0;
      puVar7[2] = 0;
      puVar7[3] = 0;
      uVar8 = ((undefined8 *)((longlong)puVar7 + lVar6))[1];
      *puVar7 = *(undefined8 *)((longlong)puVar7 + lVar6);
      puVar7[1] = uVar8;
      puVar12 = (undefined8 *)((longlong)puVar7 + lVar6 + 0x10);
      uVar8 = puVar12[1];
      puVar7[2] = *puVar12;
      puVar7[3] = uVar8;
      *(undefined8 *)((longlong)puVar7 + lVar6 + 0x10) = 0;
      *(undefined8 *)((longlong)puVar7 + lVar6 + 0x18) = 0xf;
      *(undefined1 *)((longlong)puVar7 + lVar6) = 0;
      puVar7 = puVar7 + 4;
    } while ((undefined8 *)((longlong)puVar7 + lVar6) != puVar10);
  }
  *(undefined4 *)puVar7 = (undefined4)local_158;
  *(undefined4 *)((longlong)puVar7 + 4) = local_158._4_4_;
  *(undefined4 *)(puVar7 + 1) = (undefined4)uStack_150;
  *(undefined4 *)((longlong)puVar7 + 0xc) = uStack_150._4_4_;
  puVar7[2] = local_148;
  puVar7[3] = uStack_140;
  for (puVar12 = _Memory; uVar19 = (undefined4)((ulonglong)in_stack_fffffffffffffe40 >> 0x20),
      puVar12 != puVar10; puVar12 = puVar12 + 4) {
    FUN_140002d30(puVar12);
    uVar18 = (undefined4)((ulonglong)in_stack_fffffffffffffe38 >> 0x20);
  }
  free(_Memory);
  local_120 = _Memory_00 + 0x10;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  uVar17 = DAT_140aac038;
  if ((*(longlong *)*param_1 == 0) && (*(char *)(param_1[1] + 0x2491) == '\0')) {
    uVar17 = 0;
  }
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
  *(undefined4 *)((longlong)param_2 + 0x44) = 0;
  *(undefined4 *)(param_2 + 9) = uVar17;
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  local_130 = _Memory_00;
  puStack_128 = puVar7 + 4;
  (**(code **)(*param_2 + 8))(param_2);
  lVar6 = *(longlong *)*param_1;
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41c00000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41c00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0x40800000;
  if (lVar6 == 0) {
    pcVar1 = *(code **)(*param_2 + 0x80);
    uVar8 = *(undefined8 *)param_1[2];
    uStack_134 = uStack_134 & 0xffffff00;
    pcVar11 = "icon_tri_right.svg";
    if (*(char *)(param_1[1] + 0x2491) != '\0') {
      pcVar11 = "icon_tri_down.svg";
    }
    local_158 = (void *)0x0;
    uStack_150 = 0;
    local_148 = 0;
    uStack_140 = 0;
    do {
      lVar13 = lVar13 + 1;
    } while (pcVar11[lVar13] != '\0');
    FUN_140002c00(&local_158,pcVar11,lVar13);
    uVar8 = FUN_14081dff0(uVar8,local_d8,&local_158);
    cVar5 = (*pcVar1)(param_2,uVar8,0);
    if (0xf < uStack_140) {
      pvVar15 = local_158;
      if ((0xfff < uStack_140 + 1) &&
         (pvVar15 = *(void **)((longlong)local_158 + -8),
         0x1f < (ulonglong)((longlong)local_158 + (-8 - (longlong)pvVar15)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar15);
    }
    if (cVar5 != '\0') {
      *(bool *)param_1[3] = *(char *)(param_1[1] + 0x2491) == '\0';
    }
  }
  else {
    (**(code **)(*param_2 + 200))(param_2);
  }
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
  (**(code **)(*param_2 + 200))();
  FUN_140572a80(param_1[1] + 0x590,&local_130,param_2);
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
  (**(code **)(*param_2 + 200))(param_2);
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x42f00000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  pcVar1 = *(code **)(*param_2 + 0x98);
  uVar9 = FUN_1402d82e0("intervention","Intervention");
  uVar8 = *(undefined8 *)param_1[2];
  uStack_134 = uStack_134 & 0xffffff00;
  local_158 = (char *)0x0;
  uStack_150 = 0;
  local_148 = 0;
  uStack_140 = 0;
  local_158 = (char *)FUN_140003270(0x20);
  uVar4 = s_icon_alert_outline_svg_140a83cd8._12_4_;
  uVar3 = s_icon_alert_outline_svg_140a83cd8._8_4_;
  uVar17 = s_icon_alert_outline_svg_140a83cd8._4_4_;
  local_148 = 0x16;
  uStack_140 = 0x1f;
  *(undefined4 *)local_158 = s_icon_alert_outline_svg_140a83cd8._0_4_;
  *(undefined4 *)(local_158 + 4) = uVar17;
  *(undefined4 *)(local_158 + 8) = uVar3;
  *(undefined4 *)(local_158 + 0xc) = uVar4;
  *(undefined4 *)(local_158 + 0x10) = s_icon_alert_outline_svg_140a83cd8._16_4_;
  *(undefined2 *)(local_158 + 0x14) = s_icon_alert_outline_svg_140a83cd8._20_2_;
  local_158[0x16] = '\0';
  uVar8 = FUN_14081dff0(uVar8,local_d8,&local_158);
  cVar5 = (*pcVar1)(param_2,uVar8,uVar9,0,CONCAT44(uVar18,0x12));
  if (0xf < uStack_140) {
    pcVar11 = local_158;
    if ((0xfff < uStack_140 + 1) &&
       (pcVar11 = *(char **)(local_158 + -8), (char *)0x1f < local_158 + (-8 - (longlong)pcVar11)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar11);
  }
  if (cVar5 != '\0') {
    uVar8 = *(undefined8 *)param_1[2];
    uVar9 = FUN_1402d82e0("train_intervention_dialog_title","Train intervention");
    FUN_14081d6d0(uVar8,uVar9,0,1);
  }
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x40800000;
  *(undefined1 *)(param_2 + 5) = 1;
  (**(code **)(*param_2 + 200))(param_2);
  lVar6 = *(longlong *)param_1[4];
  if (*(char *)(lVar6 + 0x5f0) != '\0') {
    if (*(longlong *)(lVar6 + 0x5d8) == 0) {
      puVar10 = (undefined8 *)0x0;
    }
    else {
      puVar10 = (undefined8 *)FUN_14033f710(*(longlong *)param_1[5] + 0x280);
    }
    if (puVar10 != (undefined8 *)0x0) {
      for (puVar2 = *(ulonglong **)
                     (puVar10[0x37] +
                     (*(ulonglong *)(lVar6 + 0x5e0) % (ulonglong)*(uint *)(puVar10 + 0x38)) * 8);
          puVar2 != (ulonglong *)0x0; puVar2 = (ulonglong *)puVar2[0x3b]) {
        if (*(ulonglong *)(lVar6 + 0x5e0) == *puVar2) {
          if ((puVar2 != (ulonglong *)0x0) &&
             (puVar16 = puVar2 + 1, puVar2 != *(ulonglong **)(puVar10[0x37] + puVar10[0x38] * 8)))
          goto LAB_1407fef63;
          break;
        }
      }
      puVar16 = (ulonglong *)0x0;
LAB_1407fef63:
      if (puVar16 != (ulonglong *)0x0) {
        *(undefined4 *)((longlong)param_2 + 0x2c) = 0x42700000;
        *(undefined1 *)(param_2 + 5) = 1;
        *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
        *(undefined1 *)(param_2 + 6) = 1;
        *(undefined1 *)(param_2 + 7) = 1;
        *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
        *(undefined8 *)((longlong)param_2 + 0x44) = 0;
        pcVar1 = *(code **)(*param_2 + 0x80);
        uVar8 = *(undefined8 *)param_1[2];
        uStack_134 = uStack_134 & 0xffffff00;
        local_158 = (char *)0x0;
        uStack_150 = 0;
        local_148 = 0;
        uStack_140 = 0;
        local_158 = (char *)FUN_140003270(0x20);
        uVar9 = s_icon_sched_trains_info_svg_140a93cc0._8_8_;
        local_148 = 0x1a;
        uStack_140 = 0x1f;
        *(undefined8 *)local_158 = s_icon_sched_trains_info_svg_140a93cc0._0_8_;
        *(undefined8 *)(local_158 + 8) = uVar9;
        *(undefined8 *)(local_158 + 0x10) = s_icon_sched_trains_info_svg_140a93cc0._16_8_;
        *(undefined2 *)(local_158 + 0x18) = s_icon_sched_trains_info_svg_140a93cc0._24_2_;
        local_158[0x1a] = '\0';
        uVar8 = FUN_14081dff0(uVar8,local_d8,&local_158);
        cVar5 = (*pcVar1)(param_2,uVar8,0);
        if (0xf < uStack_140) {
          pcVar11 = local_158;
          if ((0xfff < uStack_140 + 1) &&
             (pcVar11 = *(char **)(local_158 + -8),
             (char *)0x1f < local_158 + (-8 - (longlong)pcVar11))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar11);
        }
        if (((cVar5 != '\0') && (lVar6 = *(longlong *)param_1[4], lVar6 != 0)) &&
           (*(char *)(lVar6 + 0x5f0) != '\0')) {
          DAT_140b81968 = *puVar10;
          DAT_140b81970 = *puVar16;
          DAT_140b5cce8 = *(undefined4 *)(lVar6 + 0x5e8);
        }
        pcVar1 = *(code **)(*param_2 + 0xc0);
        uVar8 = FUN_1402d82e0("train_info_open_editor_sched","Open shift in schedule editor");
        (*pcVar1)(param_2,uVar8);
      }
    }
  }
  if (*(char *)(*(longlong *)param_1[4] + 0x1d0) != '\0') {
    *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
    *(undefined1 *)(param_2 + 5) = 1;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
    *(undefined1 *)(param_2 + 6) = 1;
    *(undefined1 *)(param_2 + 7) = 1;
    *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_2 + 0x44) = 0;
    pcVar1 = *(code **)(*param_2 + 0x80);
    lVar6 = *(longlong *)(*(longlong *)param_1[4] + 0xb8);
    uStack_134 = uStack_134 & 0xffffff00;
    local_148 = 0xf;
    uStack_140 = 0xf;
    local_158 = (char *)s_icon_camera_svg_140a93b88._0_8_;
    uStack_150 = (ulonglong)
                 CONCAT16(s_icon_camera_svg_140a93b88[0xe],
                          CONCAT24(s_icon_camera_svg_140a93b88._12_2_,
                                   s_icon_camera_svg_140a93b88._8_4_));
    uVar8 = FUN_14081dff0(*(undefined8 *)param_1[2],local_d8,&local_158);
    cVar5 = (*pcVar1)(param_2,uVar8,lVar6 == 0);
    if (0xf < uStack_140) {
      pcVar11 = local_158;
      if ((0xfff < uStack_140 + 1) &&
         (pcVar11 = *(char **)(local_158 + -8), (char *)0x1f < local_158 + (-8 - (longlong)pcVar11))
         ) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pcVar11);
    }
    if (cVar5 != '\0') {
      DAT_140b81950 = **(undefined8 **)param_1[6];
      DAT_140b77aaa = 1;
    }
  }
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0;
  FUN_1406a9ef0(param_1[2],param_2,*(undefined8 *)param_1[4]);
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x41f00000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0;
  pcVar1 = *(code **)(*param_2 + 0x80);
  uStack_134 = uStack_134 & 0xffffff00;
  local_148 = 0xd;
  uStack_140 = 0xf;
  local_158 = (char *)s_icon_info_svg_140a875c0._0_8_;
  uStack_150 = (ulonglong)CONCAT14(s_icon_info_svg_140a875c0[0xc],s_icon_info_svg_140a875c0._8_4_);
  uVar8 = FUN_14081dff0(*(undefined8 *)param_1[2],local_d8,&local_158);
  cVar5 = (*pcVar1)(param_2,uVar8,0);
  if (0xf < uStack_140) {
    pvVar15 = local_158;
    if ((0xfff < uStack_140 + 1) &&
       (pvVar15 = *(void **)((longlong)local_158 + -8),
       0x1f < (ulonglong)((longlong)local_158 + (-8 - (longlong)pvVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar15);
  }
  if (cVar5 != '\0') {
    DAT_140b81978 = **(undefined8 **)param_1[6];
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  lVar6 = *(longlong *)*param_1;
  puVar10 = _Memory_00;
  if (lVar6 == 0) {
    if (*(char *)(param_1[1] + 0x2491) != '\0') {
      uStack_150 = 0;
      local_148 = 0;
      uStack_140 = 0xf;
      local_158 = (void *)0x0;
      uVar8 = FUN_1407f3600(param_1[1],local_b0,param_1[2],param_1[8],param_1[9],param_2,param_1[10]
                            ,param_1[0xb],param_1[0xc],param_1[0xd],param_1[0xe],param_1[5],
                            *(undefined8 *)param_1[6],*(undefined8 *)param_1[4],param_1[0xf],0,
                            &local_158);
      FUN_1407fab20(param_1[7],uVar8);
      FUN_140002d30(local_88);
      FUN_140002d30(local_a8);
    }
  }
  else {
    if (lVar6 == 1) {
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      *(undefined1 *)(param_2 + 4) = 1;
      *(undefined4 *)((longlong)param_2 + 0x24) = 2;
      (**(code **)(*param_2 + 8))(param_2);
      *(undefined4 *)((longlong)param_2 + 0x2c) = 0x43960000;
      *(undefined1 *)(param_2 + 5) = 1;
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x140;
      *(undefined1 *)(param_2 + 7) = 1;
      *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
      *(undefined8 *)((longlong)param_2 + 0x44) = 0x40800000;
      FUN_14055b8e0(param_2,&DAT_140b8e660);
      (**(code **)(*param_2 + 8))(param_2);
      FUN_1407fcd20(param_1[1] + 0x29e8,param_1[2],param_1[8]);
      (**(code **)(*param_2 + 0x18))(param_2);
    }
    else {
      if (lVar6 != 2) goto joined_r0x0001407ff5a9;
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      *(undefined1 *)(param_2 + 4) = 1;
      *(undefined4 *)((longlong)param_2 + 0x24) = 3;
      FUN_14055b8e0(param_2,&DAT_140b8e660);
      (**(code **)(*param_2 + 8))(param_2);
      FUN_1405941c0(param_1[1] + 0x1c,param_1[2],param_1[0x10],param_1[8],
                    **(undefined8 **)param_1[6],
                    CONCAT44(uVar19,*(undefined4 *)(*(undefined8 **)param_1[6] + 1)),param_2);
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      (**(code **)(*param_2 + 200))(param_2);
    }
    (**(code **)(*param_2 + 0x18))(param_2);
  }
joined_r0x0001407ff5a9:
  for (; puVar10 != puVar7 + 4; puVar10 = puVar10 + 4) {
    FUN_140002d30(puVar10);
  }
  if (_Memory_00 != (undefined8 *)0x0) {
    free(_Memory_00);
  }
  return;
}


// Incoming references
// 0xc3ac64 DATA caller none
// 0x814f67 UNCONDITIONAL_CALL caller 140814bd0
// 0x8155af UNCONDITIONAL_CALL caller 140814bd0
