// Candidate VA 140805710; RVA 0x805710
// Ghidra inferred prototype: undefined FUN_140805710()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140805710(longlong *param_1,char param_2,undefined8 *param_3,undefined8 *param_4,
                  undefined8 *param_5,int param_6,longlong *param_7)

{
  longlong lVar1;
  code *pcVar2;
  undefined8 *puVar3;
  longlong *plVar4;
  undefined *puVar5;
  undefined8 uVar6;
  char cVar7;
  undefined8 uVar8;
  longlong *plVar9;
  longlong *plVar10;
  longlong *plVar11;
  undefined1 *puVar12;
  char *pcVar13;
  undefined8 ****ppppuVar14;
  longlong lVar15;
  longlong lVar16;
  undefined *local_748;
  longlong lStack_740;
  undefined8 local_738;
  char *local_730;
  undefined8 uStack_728;
  undefined8 local_720;
  ulonglong uStack_718;
  undefined8 ***local_708;
  longlong **pplStack_700;
  undefined8 local_6f8;
  ulonglong uStack_6f0;
  longlong *local_6e8;
  longlong lStack_6e0;
  undefined8 **local_6d8;
  longlong **pplStack_6d0;
  undefined1 local_6c8 [48];
  undefined **local_698;
  undefined1 *local_690;
  undefined8 local_688;
  ulonglong local_680;
  undefined1 local_678 [512];
  undefined **local_478;
  undefined1 *local_470;
  undefined8 local_468;
  ulonglong local_460;
  undefined1 local_458 [512];
  undefined **local_258;
  undefined1 *local_250;
  undefined8 local_248;
  ulonglong local_240;
  undefined1 local_238 [512];

  plVar11 = (longlong *)0x0;
  lVar16 = -1;
  if (param_2 == '\0') {
LAB_1408057b0:
    if (param_5 == (undefined8 *)0x0) goto LAB_140805dff;
LAB_1408057c0:
    lVar15 = *param_1;
    *(undefined4 *)(lVar15 + 0x2c) = 0x41c00000;
    *(undefined1 *)(lVar15 + 0x28) = 1;
    *(undefined4 *)(lVar15 + 0x34) = 0x41c00000;
    *(undefined1 *)(lVar15 + 0x30) = 1;
    *(undefined1 *)(lVar15 + 0x38) = 1;
    *(undefined8 *)(lVar15 + 0x3c) = 0;
    *(undefined8 *)(lVar15 + 0x44) = 0x40000000;
    plVar9 = (longlong *)*param_1;
    pcVar2 = *(code **)(*plVar9 + 0x80);
    uVar8 = *(undefined8 *)param_1[1];
    local_738._0_5_ = CONCAT14(1,*(undefined4 *)(param_5 + 0x17));
    pcVar13 = "icon_line_depot.svg";
    if (*(int *)((longlong)param_5 + 0xfc) != 1) {
      pcVar13 = "icon_line2.svg";
    }
    local_730 = (char *)0x0;
    uStack_728 = 0;
    local_720 = 0;
    uStack_718 = 0;
    lVar15 = -1;
    do {
      lVar15 = lVar15 + 1;
    } while (pcVar13[lVar15] != '\0');
    FUN_140002c00(&local_730);
    uVar8 = FUN_14081dff0(uVar8,local_6c8,&local_730);
    cVar7 = (*pcVar2)(plVar9,uVar8,0);
    if (0xf < uStack_718) {
      pcVar13 = local_730;
      if ((0xfff < uStack_718 + 1) &&
         (pcVar13 = *(char **)(local_730 + -8), (char *)0x1f < local_730 + (-8 - (longlong)pcVar13))
         ) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pcVar13);
    }
    if (cVar7 != '\0') {
      DAT_140b81980 = *param_5;
      DAT_140b7b368 = param_6;
    }
    lVar15 = *param_1;
    *(undefined4 *)(lVar15 + 0x2c) = 0xbf800000;
    *(undefined1 *)(lVar15 + 0x28) = 1;
    plVar9 = (longlong *)*param_1;
    pcVar2 = *(code **)(*plVar9 + 0xa0);
    local_748 = (undefined *)FUN_1402d82e0("line_to3","{0} [{1}] at");
    do {
      lVar16 = lVar16 + 1;
    } while (local_748[lVar16] != '\0');
    local_6e8 = param_5 + 0xf;
    if (0xf < (ulonglong)param_5[0x12]) {
      local_6e8 = (longlong *)param_5[0xf];
    }
    local_708 = (undefined8 ***)CONCAT44(local_708._4_4_,param_6 + 1);
    lStack_6e0 = param_5[0x11];
    local_6d8 = local_708;
    pplStack_6d0 = pplStack_700;
    pplStack_700 = &local_6e8;
    local_468 = 0;
    local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_470 = local_458;
    local_460 = 500;
    local_708 = (undefined8 ***)0x1d;
    lStack_740 = lVar16;
    FUN_140022830(&local_478,&local_748,&local_708);
    local_730 = (char *)0x0;
    uStack_728 = 0;
    local_720 = 0;
    uStack_718 = 0;
    FUN_140002c00(&local_730,local_470);
    local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_470 != local_458) {
      puVar12 = local_470;
      if ((0xfff < local_460) &&
         (puVar12 = *(undefined1 **)(local_470 + -8),
         (undefined1 *)0x1f < local_470 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar12);
    }
    (*pcVar2)(plVar9,&local_730);
    if (uStack_718 < 0x10) goto LAB_140805dff;
    pcVar13 = local_730;
    if ((0xfff < uStack_718 + 1) &&
       (pcVar13 = *(char **)(local_730 + -8), (char *)0x1f < local_730 + (-8 - (longlong)pcVar13)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar13);
  }
  else {
    if ((param_3 != (undefined8 *)0x0) && (param_4 != (undefined8 *)0x0)) {
      lVar15 = param_3[0xf];
      lVar1 = *param_1;
      *(undefined1 *)(lVar1 + 0x28) = 1;
      if (lVar15 == 0) {
        *(undefined4 *)(lVar1 + 0x2c) = 0x41c00000;
        *(undefined4 *)(lVar1 + 0x34) = 0x41c00000;
        *(undefined1 *)(lVar1 + 0x30) = 1;
        *(undefined1 *)(lVar1 + 0x38) = 1;
        *(undefined8 *)(lVar1 + 0x3c) = 0;
        *(undefined8 *)(lVar1 + 0x44) = 0x40000000;
        plVar9 = (longlong *)*param_1;
        pcVar2 = *(code **)(*plVar9 + 0x80);
        uVar8 = *(undefined8 *)param_1[1];
        local_738._0_5_ = CONCAT14(1,*(undefined4 *)(param_3 + 10));
        local_730 = (char *)0x0;
        uStack_728 = 0;
        local_720 = 0;
        uStack_718 = 0;
        FUN_140002c00(&local_730,"icon_schedule_w.svg",0x13);
        uVar8 = FUN_14081dff0(uVar8,local_6c8,&local_730);
        cVar7 = (*pcVar2)(plVar9,uVar8,0);
        if (0xf < uStack_718) {
          pcVar13 = local_730;
          if ((0xfff < uStack_718 + 1) &&
             (pcVar13 = *(char **)(local_730 + -8),
             (char *)0x1f < local_730 + (-8 - (longlong)pcVar13))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar13);
        }
        if (cVar7 != '\0') {
          DAT_140b81968 = *param_3;
          DAT_140b81970 = *param_4;
        }
        lVar15 = *param_1;
        *(undefined4 *)(lVar15 + 0x2c) = 0xbf800000;
        *(undefined1 *)(lVar15 + 0x28) = 1;
        *(undefined1 *)(lVar15 + 0x38) = 1;
        *(undefined4 *)(lVar15 + 0x44) = 0x40000000;
        plVar9 = (longlong *)*param_1;
        pcVar2 = *(code **)(*plVar9 + 0xa0);
        local_748 = (undefined *)FUN_1402d82e0("sched_info_brief","{0} [{1}]");
        lStack_740 = -1;
        do {
          lStack_740 = lStack_740 + 1;
        } while (local_748[lStack_740] != '\0');
        local_6e8 = param_3 + 2;
        if (0xf < (ulonglong)param_3[5]) {
          local_6e8 = (longlong *)param_3[2];
        }
        lStack_6e0 = param_3[4];
        local_6d8 = (undefined8 **)(param_4 + 2);
        if (0xf < (ulonglong)param_4[5]) {
          local_6d8 = (undefined8 **)param_4[2];
        }
        pplStack_6d0 = (longlong **)param_4[4];
        pplStack_700 = &local_6e8;
        local_688 = 0;
        local_698 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_680 = 500;
        local_708 = (undefined8 ***)0xdd;
        local_690 = local_678;
        FUN_140022830(&local_698,&local_748,&local_708);
        local_730 = (char *)0x0;
        uStack_728 = 0;
        local_720 = 0;
        uStack_718 = 0;
        FUN_140002c00(&local_730,local_690);
        local_698 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_690 != local_678) {
          puVar12 = local_690;
          if ((0xfff < local_680) &&
             (puVar12 = *(undefined1 **)(local_690 + -8),
             (undefined1 *)0x1f < local_690 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar12);
        }
        (*pcVar2)(plVar9,&local_730);
        if (0xf < uStack_718) {
          pcVar13 = local_730;
          if ((0xfff < uStack_718 + 1) &&
             (pcVar13 = *(char **)(local_730 + -8),
             (char *)0x1f < local_730 + (-8 - (longlong)pcVar13))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar13);
        }
      }
      else {
        *(undefined4 *)(lVar1 + 0x2c) = 0xbf800000;
        *(undefined1 *)(lVar1 + 0x38) = 1;
        *(undefined4 *)(lVar1 + 0x44) = 0x40000000;
        plVar9 = (longlong *)*param_1;
        pcVar2 = *(code **)(*plVar9 + 0xa8);
        uVar8 = FUN_1402d82e0("autorun_dispatch_info_brief","Auto run");
        (*pcVar2)(plVar9,uVar8,0x11);
      }
      goto LAB_1408057b0;
    }
    if (param_5 != (undefined8 *)0x0) {
      lVar15 = *param_1;
      *(undefined4 *)(lVar15 + 0x2c) = 0xbf800000;
      *(undefined1 *)(lVar15 + 0x28) = 1;
      *(undefined1 *)(lVar15 + 0x38) = 1;
      *(undefined4 *)(lVar15 + 0x44) = 0x40000000;
      plVar9 = (longlong *)*param_1;
      pcVar2 = *(code **)(*plVar9 + 0xa8);
      uVar8 = FUN_1402d82e0("run_dispatch_info_brief","Single run");
      (*pcVar2)(plVar9,uVar8,0x11);
      goto LAB_1408057c0;
    }
LAB_140805dff:
    local_738 = (undefined8 ****)0x0;
    if (param_5 == (undefined8 *)0x0) goto LAB_140805e4b;
  }
  local_738 = (undefined8 ****)0x0;
  if ((-1 < param_6) &&
     (param_6 < (int)((longlong)(param_5[0x24] - param_5[0x23]) >> 3) * 0x2fa0be83)) {
    local_738 = *(undefined8 *****)((longlong)param_6 * 0x158 + 0x78 + param_5[0x23]);
  }
LAB_140805e4b:
  ppppuVar14 = local_738;
  puVar3 = (undefined8 *)param_1[2];
  local_708 = (undefined8 ***)*param_7;
  plVar9 = plVar11;
  if ((undefined8 ****)local_708 != (undefined8 ****)0x0) {
    plVar9 = (longlong *)FUN_14032c420(*puVar3,local_708);
  }
  plVar10 = plVar11;
  if (ppppuVar14 != (undefined8 ****)0x0) {
    plVar10 = (longlong *)FUN_14032c420(*puVar3,ppppuVar14);
  }
  if ((plVar9 != (longlong *)0x0) || (plVar10 != (longlong *)0x0)) {
    lVar16 = *param_1;
    *(undefined4 *)(lVar16 + 0x2c) = 0x41c00000;
    *(undefined1 *)(lVar16 + 0x28) = 1;
    *(undefined4 *)(lVar16 + 0x34) = 0x41c00000;
    *(undefined1 *)(lVar16 + 0x30) = 1;
    *(undefined1 *)(lVar16 + 0x38) = 1;
    *(undefined8 *)(lVar16 + 0x3c) = 0x40800000;
    *(undefined8 *)(lVar16 + 0x44) = 0x40000000;
    plVar4 = (longlong *)*param_1;
    pcVar2 = *(code **)(*plVar4 + 0x80);
    uVar8 = *(undefined8 *)param_1[1];
    local_748._0_5_ = (uint5)(uint)local_748;
    local_730 = (char *)0x0;
    uStack_728 = 0;
    local_720 = 0;
    uStack_718 = 0;
    local_6e8 = plVar10;
    local_730 = (char *)FUN_140003270(0x20);
    uVar6 = s_icon_pin_outline_svg_140a83c18._8_8_;
    local_720 = _DAT_140aaca60;
    uStack_718 = _UNK_140aaca68;
    *(undefined8 *)local_730 = s_icon_pin_outline_svg_140a83c18._0_8_;
    *(undefined8 *)(local_730 + 8) = uVar6;
    *(undefined4 *)(local_730 + 0x10) = s_icon_pin_outline_svg_140a83c18._16_4_;
    local_730[0x14] = '\0';
    uVar8 = FUN_14081dff0(uVar8,local_6c8,&local_730);
    cVar7 = (*pcVar2)(plVar4,uVar8,0);
    if (0xf < uStack_718) {
      pcVar13 = local_730;
      if ((0xfff < uStack_718 + 1) &&
         (pcVar13 = *(char **)(local_730 + -8), (char *)0x1f < local_730 + (-8 - (longlong)pcVar13))
         ) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pcVar13);
    }
    if (cVar7 != '\0') {
      plVar10 = local_6e8;
      if (plVar9 != (longlong *)0x0) {
        plVar10 = plVar9;
      }
      puVar5 = (undefined *)*plVar10;
      if (puVar5 != (undefined *)0x0) {
        lStack_740 = CONCAT71(lStack_740._1_7_,*(char *)(param_1[1] + 0x79) == '\0');
        DAT_140b81908 = lStack_740;
        DAT_140b81900 = puVar5;
        local_748 = puVar5;
        if (DAT_140b81920 != '\x05') {
          DAT_140b81920 = '\x05';
        }
      }
    }
    if (plVar9 != (longlong *)0x0) {
      lVar16 = *param_1;
      *(undefined4 *)(lVar16 + 0x2c) = 0xbf800000;
      *(undefined1 *)(lVar16 + 0x28) = 1;
      if (((param_5 != (undefined8 *)0x0) && (-1 < param_6)) &&
         (param_6 < (int)((longlong)(param_5[0x24] - param_5[0x23]) >> 3) * 0x2fa0be83)) {
        FUN_14036d0a0(param_1[3],&local_730,(longlong)param_6 * 0x158 + param_5[0x23]);
        if (((plVar9[0x1a] != 0) && (local_738 != (undefined8 ****)0x0)) &&
           (((undefined8 ****)local_708 != (undefined8 ****)0x0 &&
            ((undefined8 ****)local_708 != local_738)))) {
          puVar3 = (undefined8 *)param_1[2];
          plVar10 = plVar11;
          if (plVar9[2] != 0) {
            plVar10 = (longlong *)FUN_14032c420(*puVar3);
          }
          if (plVar9[1] != 0) {
            plVar11 = (longlong *)FUN_14032c420(*puVar3);
          }
          plVar11 = (longlong *)FUN_14038af60(plVar9,local_6c8,plVar11,plVar10);
          local_6e8 = plVar11;
          if (0xf < (ulonglong)plVar11[3]) {
            local_6e8 = (longlong *)*plVar11;
          }
          lStack_6e0 = plVar11[2];
          pplStack_700 = &local_6e8;
          local_248 = 0;
          local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          local_250 = local_238;
          local_240 = 500;
          local_708 = (undefined8 ***)0xd;
          local_748 = &DAT_140a83c84;
          lStack_740 = 5;
          FUN_140022830(&local_258,&local_748,&local_708);
          local_708 = (undefined8 ****)0x0;
          pplStack_700 = (longlong **)0x0;
          local_6f8 = 0;
          uStack_6f0 = 0;
          FUN_140002c00(&local_708,local_250);
          local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          if (local_250 != local_238) {
            puVar12 = local_250;
            if ((0xfff < local_240) &&
               (puVar12 = *(undefined1 **)(local_250 + -8),
               (undefined1 *)0x1f < local_250 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(puVar12);
          }
          ppppuVar14 = &local_708;
          if (0xf < uStack_6f0) {
            ppppuVar14 = (undefined8 ****)local_708;
          }
          FUN_1400254f0(&local_730,ppppuVar14,local_6f8);
          if (0xf < uStack_6f0) {
            FUN_140003040(&local_708,local_708);
          }
          FUN_140002d30(local_6c8);
        }
        (**(code **)(*(longlong *)*param_1 + 0xa0))((longlong *)*param_1,&local_730);
        if (0xf < uStack_718) {
          pcVar13 = local_730;
          if ((0xfff < uStack_718 + 1) &&
             (pcVar13 = *(char **)(local_730 + -8),
             (char *)0x1f < local_730 + (-8 - (longlong)pcVar13))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar13);
        }
      }
    }
  }
  return;
}


// Incoming references
// 0xc3ad48 DATA caller none
// 0x8047cf UNCONDITIONAL_CALL caller 140804000
// 0x804834 UNCONDITIONAL_CALL caller 140804000
// 0x804ee7 UNCONDITIONAL_CALL caller 140804000
