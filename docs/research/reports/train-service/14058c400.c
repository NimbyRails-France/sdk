// Candidate VA 14058c400; RVA 0x58c400
// Ghidra inferred prototype: undefined FUN_14058c400()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 *******
FUN_14058c400(undefined8 *******param_1,undefined8 param_2,longlong param_3,longlong param_4)

{
  double dVar1;
  undefined1 auVar2 [12];
  undefined1 auVar3 [12];
  undefined1 auVar4 [16];
  undefined4 uVar5;
  int iVar6;
  longlong lVar7;
  undefined8 uVar8;
  undefined1 (*pauVar9) [16];
  longlong *plVar10;
  undefined8 *puVar11;
  longlong lVar12;
  void *_Memory;
  undefined8 *****_Memory_00;
  undefined1 *puVar13;
  undefined8 ******ppppppuVar14;
  undefined8 *******pppppppuVar15;
  int iVar16;
  undefined8 ******ppppppuVar17;
  undefined8 ******ppppppuVar18;
  undefined8 extraout_XMM0_Qb;
  undefined1 auVar19 [16];
  undefined8 extraout_XMM0_Qb_00;
  undefined8 extraout_XMM0_Qb_01;
  undefined1 auVar20 [16];
  float fVar21;
  float fVar22;
  undefined1 local_7d8 [4];
  undefined4 uStack_7d4;
  undefined8 uStack_7d0;
  undefined8 local_7c8;
  ulonglong uStack_7c0;
  undefined8 ******local_7b8;
  undefined8 **ppuStack_7b0;
  longlong local_7a8;
  ulonglong uStack_7a0;
  undefined8 *****local_798;
  undefined1 *puStack_790;
  ulonglong local_780;
  undefined8 ******local_778;
  undefined8 local_770;
  double local_768;
  undefined4 uStack_760;
  undefined4 uStack_75c;
  undefined8 *****local_758;
  undefined8 *****pppppuStack_750;
  undefined8 *****local_748;
  undefined8 *****pppppuStack_740;
  undefined8 *****local_738;
  undefined8 *****pppppuStack_730;
  undefined8 *****local_728;
  undefined8 *****pppppuStack_720;
  undefined8 ******local_718;
  undefined8 *****pppppuStack_710;
  undefined8 *****local_708;
  undefined8 *****pppppuStack_700;
  undefined8 *local_6f8;
  undefined8 local_6f0;
  undefined1 local_6e8 [16];
  float local_6d8;
  undefined4 uStack_6d4;
  undefined4 uStack_6d0;
  undefined4 uStack_6cc;
  undefined1 local_6c8 [32];
  undefined **local_6a8;
  undefined1 *local_6a0;
  undefined8 local_698;
  undefined8 local_690;
  undefined1 local_688 [512];
  undefined **local_488;
  undefined1 *local_480;
  undefined8 local_478;
  ulonglong local_470;
  undefined1 local_468 [512];
  undefined **local_268;
  undefined1 *local_260;
  undefined8 local_258;
  ulonglong local_250;
  undefined1 local_248 [560];

  *param_1 = (undefined8 ******)0x0;
  param_1[1] = (undefined8 ******)0x0;
  param_1[2] = (undefined8 ******)0x0;
  ppppppuVar17 = (undefined8 ******)0xf;
  param_1[3] = (undefined8 ******)0xf;
  *(undefined1 *)param_1 = 0;
  if (*(char *)(param_3 + 0x4d0) == '\0') {
    if (*(char *)(param_3 + 0x4b0) == '\0') {
      return param_1;
    }
    lVar7 = *(longlong *)(param_3 + 0x488);
    fVar22 = (float)(lVar7 + *(longlong *)(param_4 + 8) * -10000) * DAT_140aab608;
    lVar12 = FUN_14043e0c0(param_3 + 0x290,*(undefined8 *)(param_3 + 0x3a8));
    if (lVar7 < 1) {
      dVar1 = *(double *)(param_3 + 0x3d8) * DAT_140aaba78;
      pppppppuVar15 = (undefined8 *******)(*(double *)(param_3 + 0x3c0) * DAT_140aaba78);
      local_7b8 = (undefined8 ******)
                  FUN_1402d82e0("train_info_no_timing","{0:.1f} km of {1:.1f} km");
      ppuStack_7b0 = (undefined8 **)0xffffffffffffffff;
      do {
        ppuStack_7b0 = (undefined8 **)((longlong)ppuStack_7b0 + 1);
      } while (*(char *)((longlong)ppuStack_7b0 + (longlong)local_7b8) != '\0');
      local_770 = uStack_7d0;
      uStack_760 = (undefined4)uStack_7d0;
      uStack_75c = uStack_7d0._4_4_;
      local_258 = 0;
      local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_260 = local_248;
      local_250 = 500;
      uStack_7d0 = &local_778;
      _local_7d8 = (undefined8 *******)0xaa;
      local_798 = local_7b8;
      puStack_790 = (undefined1 *)ppuStack_7b0;
      local_778 = pppppppuVar15;
      local_768 = dVar1;
      FUN_140022830(&local_268,&local_7b8,local_7d8);
      local_718 = (undefined8 *******)0x0;
      pppppuStack_710 = (undefined8 ******)0x0;
      local_708 = (undefined8 ******)0x0;
      pppppuStack_700 = (undefined8 ******)0x0;
      FUN_140002c00(&local_718,local_260);
      local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_260 != local_248) {
        puVar13 = local_260;
        if ((0xfff < local_250) &&
           (puVar13 = *(undefined1 **)(local_260 + -8),
           (undefined1 *)0x1f < local_260 + (-8 - (longlong)puVar13))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(puVar13);
      }
      ppppppuVar18 = (undefined8 ******)pppppuStack_700;
      if (param_1 != &local_718) {
        if ((undefined8 ******)0xf < param_1[3]) {
          ppppppuVar18 = *param_1;
          ppppppuVar14 = ppppppuVar18;
          if ((0xfff < (longlong)param_1[3] + 1U) &&
             (ppppppuVar14 = (undefined8 ******)ppppppuVar18[-1],
             0x1f < (ulonglong)((longlong)ppppppuVar18 + (-8 - (longlong)ppppppuVar14))))
          goto LAB_14058ce84;
          free(ppppppuVar14);
        }
        *(undefined1 *)param_1 = 0;
        *param_1 = local_718;
        param_1[1] = (undefined8 ******)pppppuStack_710;
        param_1[2] = (undefined8 ******)local_708;
        param_1[3] = (undefined8 ******)pppppuStack_700;
        local_718 = (undefined8 ******)((ulonglong)local_718 & 0xffffffffffffff00);
        ppppppuVar18 = ppppppuVar17;
      }
      if (ppppppuVar18 < (undefined8 ******)0x10) {
        return param_1;
      }
      pppppppuVar15 = (undefined8 *******)local_718;
      if ((0xfff < (longlong)ppppppuVar18 + 1U) &&
         (pppppppuVar15 = (undefined8 *******)local_718[-1],
         0x1f < (ulonglong)((longlong)local_718 + (-8 - (longlong)pppppppuVar15)))) {
LAB_14058ce84:
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
    }
    else {
      local_6d8 = *(float *)(param_3 + 0x46c) * DAT_140aab678;
      fVar21 = *(float *)(param_3 + 0x474) * DAT_140aab678;
      if (11000000 < lVar12) {
        puVar11 = (undefined8 *)FUN_14058b630(local_6c8,lVar12 / 1000000,2,0);
        local_798 = (undefined8 *****)
                    FUN_1402d82e0("train_info_late_predicted2",
                                  "Est. late by {0}, {1:.1f} km of {2:.1f} km");
        puStack_790 = (undefined1 *)0xffffffffffffffff;
        do {
          puStack_790 = (undefined1 *)((longlong)puStack_790 + 1);
        } while (*(char *)((longlong)local_798 + (longlong)puStack_790) != '\0');
        local_6f8 = puVar11;
        if (0xf < (ulonglong)puVar11[3]) {
          local_6f8 = (undefined8 *)*puVar11;
        }
        local_6f0 = puVar11[2];
        auVar2._4_8_ = extraout_XMM0_Qb;
        auVar2._0_4_ = uStack_7d4;
        auVar19._0_8_ = auVar2._0_8_ << 0x20;
        auVar19._8_4_ = (undefined4)uStack_7d0;
        auVar19._12_4_ = uStack_7d0._4_4_;
        local_6e8._4_12_ = auVar19._4_12_;
        local_6e8._0_4_ = fVar21;
        uStack_6d4 = uStack_7d4;
        uStack_6d0 = (undefined4)uStack_7d0;
        uStack_6cc = uStack_7d0._4_4_;
        local_7b8 = (undefined8 ******)0x99d;
        ppuStack_7b0 = &local_6f8;
        _local_7d8 = local_6e8;
        FUN_140021e90(&local_778,&local_798,&local_7b8);
        FUN_140025630(param_1,&local_778);
        FUN_140002d30(&local_778);
        FUN_140002d30(local_6c8);
        return param_1;
      }
      iVar6 = (int)fVar22;
      iVar16 = -iVar6;
      if (iVar16 < 0) {
        iVar16 = iVar6;
      }
      puVar11 = (undefined8 *)FUN_14058b630(&local_778,iVar16,2,0);
      if (0.0 <= fVar22) {
        local_7b8 = (undefined8 ******)
                    FUN_1402d82e0("train_info_arrives","Arrives in {0}, {1:.1f} km of {2:.1f} km");
        ppuStack_7b0 = (undefined8 **)0xffffffffffffffff;
        do {
          ppuStack_7b0 = (undefined8 **)((longlong)ppuStack_7b0 + 1);
          uVar8 = extraout_XMM0_Qb_01;
        } while (*(char *)((longlong)ppuStack_7b0 + (longlong)local_7b8) != '\0');
      }
      else {
        local_7b8 = (undefined8 ******)
                    FUN_1402d82e0("train_info_late","Late by {0}, {1:.1f} km of {2:.1f} km");
        ppuStack_7b0 = (undefined8 **)0xffffffffffffffff;
        do {
          ppuStack_7b0 = (undefined8 **)((longlong)ppuStack_7b0 + 1);
          uVar8 = extraout_XMM0_Qb_00;
        } while (*(char *)((longlong)ppuStack_7b0 + (longlong)local_7b8) != '\0');
      }
      local_6f8 = puVar11;
      if (0xf < (ulonglong)puVar11[3]) {
        local_6f8 = (undefined8 *)*puVar11;
      }
      local_6f0 = puVar11[2];
      auVar3._4_8_ = uVar8;
      auVar3._0_4_ = uStack_7d4;
      auVar20._0_8_ = auVar3._0_8_ << 0x20;
      auVar20._8_4_ = (undefined4)uStack_7d0;
      auVar20._12_4_ = uStack_7d0._4_4_;
      local_6e8._4_12_ = auVar20._4_12_;
      local_6e8._0_4_ = fVar21;
      uStack_6d4 = uStack_7d4;
      uStack_6d0 = (undefined4)uStack_7d0;
      uStack_6cc = uStack_7d0._4_4_;
      local_698 = 0;
      local_6a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_6a0 = local_688;
      local_690 = 500;
      uStack_7d0 = &local_6f8;
      _local_7d8 = (undefined8 *******)0x99d;
      local_798 = local_7b8;
      puStack_790 = (undefined1 *)ppuStack_7b0;
      FUN_140022830(&local_6a8,&local_7b8,local_7d8);
      local_738 = (undefined8 ******)0x0;
      pppppuStack_730 = (undefined8 ******)0x0;
      local_728 = (undefined8 ******)0x0;
      pppppuStack_720 = (undefined8 ******)0x0;
      FUN_140002c00(&local_738,local_6a0);
      local_6a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_6a0 != local_688) {
        FUN_140003210(local_688);
      }
      ppppppuVar18 = (undefined8 ******)pppppuStack_720;
      if (param_1 != (undefined8 *******)&local_738) {
        if ((undefined8 ******)0xf < param_1[3]) {
          ppppppuVar18 = *param_1;
          ppppppuVar14 = ppppppuVar18;
          if ((0xfff < (longlong)param_1[3] + 1U) &&
             (ppppppuVar14 = (undefined8 ******)ppppppuVar18[-1],
             0x1f < (ulonglong)((longlong)ppppppuVar18 + (-8 - (longlong)ppppppuVar14))))
          goto LAB_14058cc28;
          free(ppppppuVar14);
        }
        *(undefined1 *)param_1 = 0;
        *param_1 = (undefined8 ******)local_738;
        param_1[1] = (undefined8 ******)pppppuStack_730;
        param_1[2] = (undefined8 ******)local_728;
        param_1[3] = (undefined8 ******)pppppuStack_720;
        local_738 = (undefined8 *****)((ulonglong)local_738 & 0xffffffffffffff00);
        ppppppuVar18 = ppppppuVar17;
      }
      if ((undefined8 ******)0xf < ppppppuVar18) {
        ppppppuVar17 = (undefined8 ******)local_738;
        if ((0xfff < (longlong)ppppppuVar18 + 1U) &&
           (ppppppuVar17 = (undefined8 ******)local_738[-1],
           0x1f < (ulonglong)((longlong)local_738 + (-8 - (longlong)ppppppuVar17)))) {
LAB_14058cc28:
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppppppuVar17);
      }
      if (CONCAT44(uStack_75c,uStack_760) < 0x10) {
        return param_1;
      }
      pppppppuVar15 = (undefined8 *******)local_778;
      if ((0xfff < CONCAT44(uStack_75c,uStack_760) + 1) &&
         (pppppppuVar15 = (undefined8 *******)local_778[-1],
         0x1f < (ulonglong)((longlong)local_778 + (-8 - (longlong)pppppppuVar15)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
    }
  }
  else {
    lVar12 = *(longlong *)(param_3 + 0x4c0) + *(longlong *)(param_4 + 8) * -10000;
    lVar7 = 0;
    if (0 < lVar12) {
      lVar7 = lVar12;
    }
    uVar5 = lround((double)lVar7 * DAT_140aab938);
    FUN_14058b630(&local_778,uVar5,2,0);
    if ((*(char *)(param_3 + 0x558) != '\0') && (0.0 < *(float *)(param_3 + 0x548))) {
      uVar8 = FUN_14028fde0(&local_798,(int)*(float *)(param_3 + 0x548));
      pauVar9 = (undefined1 (*) [16])FUN_14029cb50(uVar8,0,&DAT_140a84c88);
      _local_7d8 = *pauVar9;
      local_7c8 = *(undefined8 *)pauVar9[1];
      uStack_7c0 = *(ulonglong *)(pauVar9[1] + 8);
      *(undefined8 *)(pauVar9[1] + 8) = 0xf;
      (*pauVar9)[0] = 0;
      *(undefined8 *)pauVar9[1] = 0;
      plVar10 = (longlong *)FUN_1400254f0(local_7d8,&DAT_140a84c2c,2);
      local_7b8 = (undefined8 ******)*plVar10;
      ppuStack_7b0 = (undefined8 **)plVar10[1];
      local_7a8 = plVar10[2];
      uStack_7a0 = plVar10[3];
      *(undefined1 *)plVar10 = 0;
      plVar10[2] = 0;
      plVar10[3] = 0xf;
      pppppppuVar15 = &local_7b8;
      if (0xf < uStack_7a0) {
        pppppppuVar15 = (undefined8 *******)local_7b8;
      }
      FUN_1400254f0(&local_778,pppppppuVar15);
      if (0xf < uStack_7a0) {
        pppppppuVar15 = (undefined8 *******)local_7b8;
        if ((0xfff < uStack_7a0 + 1) &&
           (pppppppuVar15 = (undefined8 *******)local_7b8[-1],
           0x1f < (ulonglong)((longlong)local_7b8 + (-8 - (longlong)pppppppuVar15)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppuVar15);
      }
      if (0xf < uStack_7c0) {
        _Memory = _local_7d8;
        if ((0xfff < uStack_7c0 + 1) &&
           (_Memory = *(void **)((longlong)_local_7d8 + -8),
           0x1f < (ulonglong)((longlong)_local_7d8 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory);
      }
      local_7c8 = _DAT_140aac910;
      uStack_7c0 = _UNK_140aac918;
      auVar4[0xf] = 0;
      auVar4._0_15_ = stack0xfffffffffffff829;
      _local_7d8 = auVar4 << 8;
      if (0xf < local_780) {
        _Memory_00 = local_798;
        if ((0xfff < local_780 + 1) &&
           (_Memory_00 = (undefined8 *****)local_798[-1],
           0x1f < (ulonglong)((longlong)local_798 + (-8 - (longlong)_Memory_00)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory_00);
      }
    }
    local_7b8 = (undefined8 ******)FUN_1402d82e0("train_info_departs","Departs in {}");
    ppuStack_7b0 = (undefined8 **)0xffffffffffffffff;
    do {
      ppuStack_7b0 = (undefined8 **)((longlong)ppuStack_7b0 + 1);
    } while (*(char *)((longlong)ppuStack_7b0 + (longlong)local_7b8) != '\0');
    _local_7d8 = &local_778;
    if (0xf < CONCAT44(uStack_75c,uStack_760)) {
      _local_7d8 = (undefined8 *******)local_778;
    }
    uStack_7d0 = local_768;
    puStack_790 = local_7d8;
    local_478 = 0;
    local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_480 = local_468;
    local_470 = 500;
    local_798 = (undefined8 ******)0xd;
    FUN_140022830(&local_488,&local_7b8,&local_798);
    local_758 = (undefined8 ******)0x0;
    pppppuStack_750 = (undefined8 ******)0x0;
    local_748 = (undefined8 ******)0x0;
    pppppuStack_740 = (undefined8 ******)0x0;
    FUN_140002c00(&local_758,local_480);
    local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_480 != local_468) {
      puVar13 = local_480;
      if ((0xfff < local_470) &&
         (puVar13 = *(undefined1 **)(local_480 + -8),
         (undefined1 *)0x1f < local_480 + (-8 - (longlong)puVar13))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar13);
    }
    ppppppuVar18 = (undefined8 ******)pppppuStack_740;
    if (param_1 != (undefined8 *******)&local_758) {
      if ((undefined8 ******)0xf < param_1[3]) {
        ppppppuVar18 = *param_1;
        ppppppuVar14 = ppppppuVar18;
        if ((0xfff < (longlong)param_1[3] + 1U) &&
           (ppppppuVar14 = (undefined8 ******)ppppppuVar18[-1],
           0x1f < (ulonglong)((longlong)ppppppuVar18 + (-8 - (longlong)ppppppuVar14))))
        goto LAB_14058c865;
        free(ppppppuVar14);
      }
      *(undefined1 *)param_1 = 0;
      *param_1 = (undefined8 ******)local_758;
      param_1[1] = (undefined8 ******)pppppuStack_750;
      param_1[2] = (undefined8 ******)local_748;
      param_1[3] = (undefined8 ******)pppppuStack_740;
      local_758 = (undefined8 *****)((ulonglong)local_758 & 0xffffffffffffff00);
      ppppppuVar18 = ppppppuVar17;
    }
    if ((undefined8 ******)0xf < ppppppuVar18) {
      ppppppuVar17 = (undefined8 ******)local_758;
      if ((0xfff < (longlong)ppppppuVar18 + 1U) &&
         (ppppppuVar17 = (undefined8 ******)local_758[-1],
         0x1f < (ulonglong)((longlong)local_758 + (-8 - (longlong)ppppppuVar17)))) {
LAB_14058c865:
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(ppppppuVar17);
    }
    if (CONCAT44(uStack_75c,uStack_760) < 0x10) {
      return param_1;
    }
    pppppppuVar15 = (undefined8 *******)local_778;
    if ((0xfff < CONCAT44(uStack_75c,uStack_760) + 1) &&
       (pppppppuVar15 = (undefined8 *******)local_778[-1],
       0x1f < (ulonglong)((longlong)local_778 + (-8 - (longlong)local_778[-1])))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  free(pppppppuVar15);
  return param_1;
}


// Incoming references
// 0xc2eac0 DATA caller none
// 0x5a99a2 UNCONDITIONAL_CALL caller 1405a8c50
// 0x804d02 UNCONDITIONAL_CALL caller 140804000
