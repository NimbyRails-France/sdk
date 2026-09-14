// Candidate VA 140804000; RVA 0x804000
// Ghidra inferred prototype: undefined FUN_140804000()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140804000(undefined8 *param_1,longlong param_2,longlong *param_3,ulonglong param_4,
                  int param_5)

{
  code *pcVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  char cVar5;
  undefined4 uVar6;
  int iVar7;
  ulonglong ******ppppppuVar8;
  ulonglong *******pppppppuVar9;
  ulonglong *puVar10;
  undefined8 uVar11;
  longlong lVar12;
  ulonglong *******pppppppuVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  ulonglong *******pppppppuVar16;
  char *pcVar17;
  void *_Memory;
  undefined1 *puVar18;
  longlong *plVar19;
  char *pcVar20;
  int iVar21;
  ulonglong uVar22;
  longlong lVar23;
  ulonglong *******pppppppuVar24;
  undefined8 in_stack_fffffffffffffa10;
  undefined4 uVar25;
  ulonglong ******local_5d8;
  ulonglong ******ppppppuStack_5d0;
  ulonglong *****local_5c8;
  ulonglong ******local_5b8;
  undefined8 uStack_5b0;
  ulonglong ******local_5a8;
  ulonglong uStack_5a0;
  ulonglong ******local_598;
  undefined8 uStack_590;
  undefined2 local_588;
  undefined2 uStack_586;
  undefined2 uStack_584;
  undefined2 uStack_582;
  ulonglong ******local_578;
  undefined8 local_570;
  longlong local_568;
  longlong *local_560;
  undefined8 *puStack_558;
  ulonglong ******local_550;
  longlong *local_548;
  longlong *local_540;
  ulonglong ******local_538;
  ulonglong ******ppppppuStack_530;
  void *local_518;
  undefined8 uStack_510;
  longlong local_508;
  ulonglong local_500;
  ulonglong ******local_4f8;
  ulonglong ****ppppuStack_4f0;
  longlong local_4e8;
  ulonglong ******local_4d8;
  ulonglong ******ppppppuStack_4d0;
  ulonglong ******local_4c8;
  ulonglong uStack_4c0;
  ulonglong ******local_4b8;
  ulonglong uStack_4b0;
  char local_4a0;
  undefined4 local_49c;
  undefined4 local_498;
  undefined4 local_488;
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
  
  uVar25 = (undefined4)((ulonglong)in_stack_fffffffffffffa10 >> 0x20);
  uVar15 = 0;
  lVar12 = *(longlong *)(param_2 + 600);
  if ((lVar12 == 0) || (local_4e8 = *(longlong *)(param_2 + 0x260), local_4e8 == 0)) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  local_5d8 = (ulonglong ******)(lVar12 + 0x428);
  lVar23 = local_4e8 + 0xa0;
  local_540 = (longlong *)(lVar12 + 0x780);
  if (param_4 == 0) {
    return;
  }
  ppppppuVar8 = (ulonglong ******)FUN_14033f780(*(longlong *)(lVar12 + 0x890) + 0x200,param_4);
  if (ppppppuVar8 == (ulonglong ******)0x0) {
    return;
  }
  if (*ppppppuVar8 == (ulonglong *****)0x0) {
    return;
  }
  local_4f8 = ppppppuVar8;
  pppppppuVar9 = (ulonglong *******)FUN_14045f300(lVar23);
  if (pppppppuVar9 == (ulonglong *******)0x0) {
    return;
  }
  pppppppuVar24 = pppppppuVar9;
  local_4d8 = (ulonglong ******)pppppppuVar9;
  uVar6 = FUN_14043e130();
  local_578 = (ulonglong ******)CONCAT44(local_578._4_4_,uVar6);
  if ((*(char *)(pppppppuVar24 + 0xbe) == '\0') || (pppppppuVar24[0xbb] == (ulonglong ******)0x0)) {
    local_568 = 0;
LAB_14080411f:
    local_570 = 0;
  }
  else {
    local_568 = FUN_14033f710(*(longlong *)(lVar12 + 0x890) + 0x280);
    if (local_568 == 0) goto LAB_14080411f;
    local_570 = FUN_1403e44d0(local_568);
  }
  if (*(char *)(pppppppuVar9 + 0xba) == '\0') {
    uVar6 = 0;
  }
  else {
    uVar6 = *(undefined4 *)(pppppppuVar9 + 0xb9);
  }
  local_538 = (ulonglong ******)(param_4 % (ulonglong)*(uint *)(param_2 + 0x48));
  for (puVar10 = *(ulonglong **)(*(longlong *)(param_2 + 0x40) + (longlong)local_538 * 8);
      puVar10 != (ulonglong *)0x0; puVar10 = (ulonglong *)puVar10[1]) {
    local_4f8 = ppppppuVar8;
    if (param_4 == *puVar10) goto LAB_1408041ac;
  }
  puVar10 = (ulonglong *)thunk_FUN_140983da8(0x10);
  *puVar10 = param_4;
  puVar10[1] = 0;
  FUN_1403414c0(param_2 + 0x38);
  uVar25 = (undefined4)((ulonglong)puVar10 >> 0x20);
LAB_1408041ac:
  lVar12 = *(longlong *)(local_4e8 + 0x2200);
  puVar10 = *(ulonglong **)
             (*(longlong *)(lVar12 + 0x350) + (param_4 % (ulonglong)*(uint *)(lVar12 + 0x358)) * 8);
  while( true ) {
    if (puVar10 == (ulonglong *)0x0) {
      return;
    }
    if (param_4 == *puVar10) break;
    puVar10 = (ulonglong *)puVar10[9];
  }
  if (puVar10 == (ulonglong *)0x0) {
    return;
  }
  if (puVar10 == *(ulonglong **)(*(longlong *)(lVar12 + 0x350) + *(longlong *)(lVar12 + 0x358) * 8))
  {
    return;
  }
  local_598 = (ulonglong ******)(puVar10 + 1);
  if ((ulonglong *******)local_598 == (ulonglong *******)0x0) {
    return;
  }
  local_550 = local_5d8;
  local_548 = local_540;
  local_560 = param_3;
  puStack_558 = param_1;
  FUN_14055b8e0(param_3,&DAT_140b8dfe0);
  *(undefined4 *)((longlong)param_3 + 0x34) = 0x41c00000;
  *(undefined1 *)(param_3 + 6) = 1;
  (**(code **)(*param_3 + 8))(param_3);
  pppppppuVar24 = (ulonglong *******)0xffffffffffffffff;
  if (*(int *)(local_4f8 + 0x17) == 2) {
    *(undefined1 *)(param_3 + 3) = 1;
    *(undefined4 *)((longlong)param_3 + 0x1c) = 0x1e0;
    pcVar1 = *(code **)(*param_3 + 0xa8);
    uVar11 = FUN_1402d82e0("train_status_mothballed","Mothballed");
    (*pcVar1)(param_3,uVar11,0x11);
    local_4f8 = local_4d8 + 0x43;
    pppppppuVar16 = (ulonglong *******)local_4d8;
    pppppppuVar13 = (ulonglong *******)(local_4d8 + 0x3a);
    goto LAB_140804cb2;
  }
  uStack_510 = 0;
  local_508 = 0;
  local_500 = 0xf;
  local_518 = (void *)0x0;
  iVar7 = (int)local_578;
  if ((int)local_578 != 0) {
    switch((int)local_578) {
    case 1:
      pcVar20 = "Line is closed";
      pcVar17 = "train_info_status_line_closed";
      break;
    default:
      goto switchD_14080430c_caseD_2;
    case 3:
      pcVar20 = "No path to destination";
      pcVar17 = "train_info_status_no_path_to_destination";
      break;
    case 4:
      pcVar20 = "Lost or bad orders";
      pcVar17 = "train_info_status_lost_or_bad_orders";
      break;
    case 5:
      pcVar20 = "Collision";
      pcVar17 = "train_info_status_collision";
      break;
    case 6:
      pcVar20 = "Waiting at signal";
      pcVar17 = "train_info_status_signal_wait";
      break;
    case 7:
      pcVar20 = "Schedule is closed";
      pcVar17 = "train_info_status_sched_closed";
      break;
    case 8:
      pcVar20 = "Cannot dispatch: tracks are busy";
      pcVar17 = "train_alert_dispatch_busy_tracks";
      break;
    case 9:
      pcVar20 = "Cannot dispatch: not enabled in active schedules";
      pcVar17 = "train_alert_dispatch_no_shifts";
      break;
    case 10:
      pcVar20 = "Cannot dispatch: enabled shifts are already assigned";
      pcVar17 = "train_alert_dispatch_busy_shifts";
    }
    lVar12 = FUN_1402d82e0(pcVar17,pcVar20);
    lVar23 = -1;
    do {
      lVar23 = lVar23 + 1;
    } while (*(char *)(lVar12 + lVar23) != '\0');
    FUN_140030630(&local_518,lVar12);
    if (local_508 != 0) {
      lVar12 = FUN_14055a690(param_3 + 3);
      *(undefined1 *)(lVar12 + 0x20) = 1;
      *(undefined8 *)(lVar12 + 0x24) = 0;
      *(undefined8 *)(lVar12 + 0x2c) = 0x40000000;
      pcVar1 = *(code **)(*param_3 + 0x100);
      local_5d8._0_5_ = (uint5)(uint)local_5d8;
      local_5a8 = (ulonglong ******)0xe;
      uStack_5a0 = 0xf;
      local_5b8 = (ulonglong ******)s_icon_alert_svg_140a83c48._0_8_;
      uStack_5b0 = (ulonglong)
                   CONCAT24(s_icon_alert_svg_140a83c48._12_2_,s_icon_alert_svg_140a83c48._8_4_);
      uVar11 = FUN_14081dff0(*param_1,&local_4c8,&local_5b8);
      (*pcVar1)(param_3,uVar11,0xffffffff);
      FUN_140002d30(&local_5b8);
      if (((iVar7 == 3) && (*(char *)(pppppppuVar9 + 0x16) != '\0')) &&
         (0.0 < *(float *)((longlong)pppppppuVar9 + 0xac))) {
        iVar7 = lroundf(*(float *)((longlong)pppppppuVar9 + 0xac) * DAT_140aab678);
        ppppppuVar8 = (ulonglong ******)FUN_140572550(&local_4c8,(longlong)iVar7);
        local_4f8 = ppppppuVar8;
        if ((ulonglong *****)0xf < ppppppuVar8[3]) {
          local_4f8 = (ulonglong ******)*ppppppuVar8;
        }
        ppppuStack_4f0 = (ulonglong ****)ppppppuVar8[2];
        ppppppuStack_5d0 = (ulonglong ******)&local_4f8;
        local_5d8 = (ulonglong ******)0xd;
        local_538 = (ulonglong ******)&DAT_140a88b80;
        ppppppuStack_530 = (ulonglong ******)0xa;
        FUN_140021e90(&local_5b8,&local_538,&local_5d8);
        pppppppuVar16 = &local_5b8;
        if (0xf < uStack_5a0) {
          pppppppuVar16 = (ulonglong *******)local_5b8;
        }
        FUN_1400254f0(&local_518,pppppppuVar16,local_5a8);
        FUN_140002d30(&local_5b8);
        FUN_140002d30(&local_4c8);
      }
      *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
      *(undefined1 *)(param_3 + 5) = 1;
      *(undefined1 *)(param_3 + 7) = 1;
      *(undefined8 *)((longlong)param_3 + 0x3c) = 0;
      *(undefined8 *)((longlong)param_3 + 0x44) = 0x40800000;
      (**(code **)(*param_3 + 0xa0))(param_3,&local_518);
    }
  }
switchD_14080430c_caseD_2:
  cVar5 = FUN_14043e1f0(pppppppuVar9,local_570);
  if (cVar5 != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0x41c00000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined4 *)((longlong)param_3 + 0x34) = 0x41c00000;
    *(undefined1 *)(param_3 + 6) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined8 *)((longlong)param_3 + 0x3c) = 0;
    *(undefined8 *)((longlong)param_3 + 0x44) = 0x40000000;
    pcVar1 = *(code **)(*param_3 + 0x100);
    uVar11 = *param_1;
    local_5d8._0_5_ = (uint5)(uint)local_5d8;
    local_5b8 = (ulonglong ******)0x0;
    uStack_5b0 = 0;
    local_5a8 = (ulonglong ******)0x0;
    uStack_5a0 = 0;
    local_5b8 = (ulonglong ******)FUN_140003270(0x20);
    uVar4 = s_icon_alert_outline_svg_140a83cd8._12_4_;
    uVar3 = s_icon_alert_outline_svg_140a83cd8._8_4_;
    uVar2 = s_icon_alert_outline_svg_140a83cd8._4_4_;
    local_5a8 = (ulonglong ******)0x16;
    uStack_5a0 = 0x1f;
    *(undefined4 *)local_5b8 = s_icon_alert_outline_svg_140a83cd8._0_4_;
    *(undefined4 *)((longlong)local_5b8 + 4) = uVar2;
    *(undefined4 *)(local_5b8 + 1) = uVar3;
    *(undefined4 *)((longlong)local_5b8 + 0xc) = uVar4;
    *(undefined4 *)(local_5b8 + 2) = s_icon_alert_outline_svg_140a83cd8._16_4_;
    *(undefined2 *)((longlong)local_5b8 + 0x14) = s_icon_alert_outline_svg_140a83cd8._20_2_;
    *(undefined1 *)((longlong)local_5b8 + 0x16) = 0;
    uVar11 = FUN_14081dff0(uVar11,&local_4c8,&local_5b8);
    (*pcVar1)(param_3,uVar11);
    if (0xf < uStack_5a0) {
      pppppppuVar16 = (ulonglong *******)local_5b8;
      if ((0xfff < uStack_5a0 + 1) &&
         (pppppppuVar16 = (ulonglong *******)local_5b8[-1],
         (undefined1 *)0x1f < (undefined1 *)((longlong)local_5b8 + (-8 - (longlong)pppppppuVar16))))
      {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppppuVar16);
    }
    pcVar1 = *(code **)(*param_3 + 0xc0);
    uVar11 = FUN_1402d82e0("stale_run_tooltip",
                           "The orders for this train have changed. It will automatically respawn on when resuming the simulation.\nPax will be automatically relocated to their next stops."
                          );
    (*pcVar1)(param_3,uVar11);
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined8 *)((longlong)param_3 + 0x3c) = 0;
    *(undefined8 *)((longlong)param_3 + 0x44) = 0x40800000;
    pcVar1 = *(code **)(*param_3 + 0xa8);
    uVar11 = FUN_1402d82e0("stale_run","Stale run");
    (*pcVar1)(param_3,uVar11);
    pcVar1 = *(code **)(*param_3 + 0xc0);
    uVar11 = FUN_1402d82e0("stale_run_tooltip",
                           "The orders for this train have changed. It will automatically respawn on when resuming the simulation.\nPax will be automatically relocated to their next stops."
                          );
    (*pcVar1)(param_3,uVar11);
  }
  pppppppuVar16 = (ulonglong *******)local_4d8;
  uVar11 = uVar15;
  if ((*(char *)(local_4d8 + 0xba) != '\0') && (pppppppuVar9[0xb5] != (ulonglong ******)0x0)) {
    uVar11 = FUN_14033f7f0(*local_540 + 0x180);
  }
  local_4f8 = (ulonglong ******)(pppppppuVar9 + 0x43);
  if (*(char *)local_4f8 == '\0') {
    pppppppuVar13 = pppppppuVar9 + 0x3a;
    local_578 = (ulonglong ******)pppppppuVar13;
    if (*(char *)pppppppuVar13 != '\0') {
      local_5d8 = pppppppuVar9[0x17];
      ppppppuStack_5d0 = pppppppuVar9[0x18];
      local_5c8 = (ulonglong *****)pppppppuVar9[0x19];
      uVar14 = CONCAT44(uVar25,uVar6);
      FUN_140805710(&local_560,1,local_568,local_570,uVar11,uVar14,&local_5d8);
      uVar25 = (undefined4)((ulonglong)uVar14 >> 0x20);
      local_578 = (ulonglong ******)pppppppuVar13;
    }
  }
  else {
    local_5d8 = pppppppuVar9[0x3f];
    ppppppuStack_5d0 = pppppppuVar9[0x40];
    local_5c8 = (ulonglong *****)pppppppuVar9[0x41];
    uVar14 = CONCAT44(uVar25,uVar6);
    FUN_140805710(&local_560,1,local_568,local_570,uVar11,uVar14,&local_5d8);
    uVar25 = (undefined4)((ulonglong)uVar14 >> 0x20);
    local_578 = (ulonglong ******)(pppppppuVar16 + 0x3a);
  }
  if (*(char *)(pppppppuVar9 + 0xb3) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined8 *)((longlong)param_3 + 0x3c) = 0;
    *(undefined8 *)((longlong)param_3 + 0x44) = 0x40800000;
    iVar7 = (int)((longlong)(pppppppuVar9[0xac] + *(longlong *)(local_4e8 + 0x28) * -0x4e2) /
                 1000000);
    iVar21 = 0;
    if (0 < iVar7) {
      iVar21 = iVar7;
    }
    local_538 = (ulonglong ******)
                FUN_1402d82e0("train_info_status_dispatch_cooldown","Attempting dispatch in {}s");
    ppppppuStack_530 = (ulonglong ******)0xffffffffffffffff;
    do {
      ppppppuStack_530 = (ulonglong ******)((longlong)ppppppuStack_530 + 1);
    } while (*(char *)((longlong)local_538 + (longlong)ppppppuStack_530) != '\0');
    local_5d8 = (ulonglong ******)CONCAT44(local_5d8._4_4_,iVar21);
    local_4d8 = local_5d8;
    ppppppuStack_4d0 = ppppppuStack_5d0;
    ppppppuStack_5d0 = (ulonglong ******)&local_4d8;
    local_468 = 0;
    local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_470 = local_458;
    local_460 = 500;
    local_5d8 = (ulonglong ******)0x1;
    FUN_140022830(&local_478,&local_538,&local_5d8);
    local_4c8 = (ulonglong ******)0x0;
    uStack_4c0 = 0;
    local_4b8 = (ulonglong ******)0x0;
    uStack_4b0 = 0;
    FUN_140002c00(&local_4c8,local_470);
    local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_470 != local_458) {
      puVar18 = local_470;
      if ((0xfff < local_460) &&
         (puVar18 = *(undefined1 **)(local_470 + -8),
         (undefined1 *)0x1f < local_470 + (-8 - (longlong)puVar18))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar18);
    }
    if (local_508 != 0) {
      local_538 = local_4b8;
      if (0x7fffffffffffffffU - (longlong)local_4b8 < 3) {
                    /* WARNING: Subroutine does not return */
        FUN_140001c70();
      }
      local_4d8 = (ulonglong ******)&local_4c8;
      if (0xf < uStack_4b0) {
        local_4d8 = local_4c8;
      }
      local_5b8 = (ulonglong ******)0x0;
      uStack_5b0 = 0;
      local_5d8 = (ulonglong ******)((longlong)local_4b8 + 3);
      uVar22 = 0xf;
      pppppppuVar13 = &local_5b8;
      if ((ulonglong *******)0xf < local_5d8) {
        uVar22 = (ulonglong)local_5d8 | 0xf;
        if (uVar22 < 0x8000000000000000) {
          if (uVar22 < 0x16) {
            uVar22 = 0x16;
          }
        }
        else {
          uVar22 = 0x7fffffffffffffff;
        }
        pppppppuVar13 = (ulonglong *******)FUN_140003270(uVar22 + 1);
        local_5b8 = (ulonglong ******)pppppppuVar13;
      }
      local_5a8 = local_5d8;
      uStack_5a0 = uVar22;
      *(short *)pppppppuVar13 = (short)DAT_140a85fe4;
      *(char *)((longlong)pppppppuVar13 + 2) = (char)((uint)DAT_140a85fe4 >> 0x10);
      ppppppuVar8 = local_538;
      memcpy((void *)((longlong)pppppppuVar13 + 3),local_4d8,(size_t)local_538);
      *(undefined1 *)((longlong)pppppppuVar13 + 3 + (longlong)ppppppuVar8) = 0;
      if (0xf < uStack_4b0) {
        FUN_140003040(&local_4c8,local_4c8);
      }
      local_4c8 = local_5b8;
      uStack_4c0 = uStack_5b0;
      local_4b8 = local_5a8;
      uStack_4b0 = uStack_5a0;
      local_5a8 = (ulonglong ******)0x0;
      uStack_5a0 = 0xf;
      local_5b8 = (ulonglong ******)((ulonglong)local_5b8 & 0xffffffffffffff00);
      FUN_140002d30(&local_5b8);
    }
    (**(code **)(*param_3 + 0xa0))(param_3,&local_4c8);
    if (0xf < uStack_4b0) {
      pppppppuVar13 = (ulonglong *******)local_4c8;
      if ((0xfff < uStack_4b0 + 1) &&
         (pppppppuVar13 = (ulonglong *******)local_4c8[-1],
         0x1f < (ulonglong)((longlong)local_4c8 + (-8 - (longlong)pppppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppppuVar13);
    }
  }
  *(undefined1 *)(param_3 + 3) = 1;
  *(undefined4 *)((longlong)param_3 + 0x1c) = 0x1e0;
  (**(code **)(*param_3 + 200))(param_3);
  if ((param_5 == 0) && (*(char *)(pppppppuVar9 + 0x3a) != '\0')) {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined8 *)((longlong)param_3 + 0x3c) = 0x41200000;
    *(undefined8 *)((longlong)param_3 + 0x44) = 0;
    pcVar1 = *(code **)(*param_3 + 0xa0);
    FUN_140572550(&local_538,(longlong)*(int *)(pppppppuVar9 + 9));
    local_598 = (ulonglong ******)FUN_140572550(&local_4c8,local_598[7]);
    local_5d8 = (ulonglong ******)FUN_1402d82e0("info_train_pax","Pax: {0} / {1}");
    ppppppuStack_5d0 = (ulonglong ******)0xffffffffffffffff;
    do {
      ppppppuStack_5d0 = (ulonglong ******)((longlong)ppppppuStack_5d0 + 1);
    } while (*(char *)((longlong)local_5d8 + (longlong)ppppppuStack_5d0) != '\0');
    uVar11 = FUN_140438d40(&local_5b8,&local_5d8,local_598);
    (*pcVar1)(param_3,uVar11);
    FUN_140002d30(&local_5b8);
    FUN_140002d30(&local_4c8);
    FUN_140002d30(&local_538);
  }
  pppppppuVar13 = (ulonglong *******)local_578;
  if (0xf < local_500) {
    _Memory = local_518;
    if ((0xfff < local_500 + 1) &&
       (_Memory = *(void **)((longlong)local_518 + -8),
       0x1f < (ulonglong)((longlong)local_518 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
    local_4f8 = (ulonglong ******)(pppppppuVar9 + 0x43);
    pppppppuVar13 = (ulonglong *******)local_578;
  }
LAB_140804cb2:
  (**(code **)(*param_3 + 0x18))(param_3);
  if ((*(char *)pppppppuVar13 != '\0') || (*(char *)local_4f8 != '\0')) {
    FUN_14055b8e0(param_3,&DAT_140b8dfe0);
    *(undefined4 *)((longlong)param_3 + 0x34) = 0x41c00000;
    *(undefined1 *)(param_3 + 6) = 1;
    (**(code **)(*param_3 + 8))(param_3);
    FUN_14058c400(&local_5b8);
    if (local_5a8 != (ulonglong ******)0x0) {
      *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
      *(undefined1 *)(param_3 + 5) = 1;
      (**(code **)(*param_3 + 0xa0))(param_3,&local_5b8);
    }
    if (0xf < uStack_5a0) {
      pppppppuVar13 = (ulonglong *******)local_5b8;
      if ((0xfff < uStack_5a0 + 1) &&
         (pppppppuVar13 = (ulonglong *******)local_5b8[-1],
         (undefined1 *)0x1f < (undefined1 *)((longlong)local_5b8 + (-8 - (longlong)pppppppuVar13))))
      {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppppuVar13);
    }
    uVar11 = local_570;
    if ((*(char *)(pppppppuVar9 + 0x9a) != '\0') && (*(char *)(pppppppuVar16 + 0xba) != '\0')) {
      FUN_14043e9a0(&local_4c8,local_570,pppppppuVar9);
      FUN_14043ef00(&local_4c8,local_540);
      if (local_4a0 == '\0') {
        if (((ulonglong *******)local_4c8 == (ulonglong *******)0x0) ||
           (local_4c8[10] != local_4c8[0xb])) {
          cVar5 = '\0';
          if ((ulonglong *******)local_4b8 != (ulonglong *******)0x0) {
            local_598 = local_4b8;
            uStack_590 = (ulonglong *******)CONCAT44(local_498,local_49c);
            local_588 = (undefined2)local_488;
            uStack_586 = (undefined2)((uint)local_488 >> 0x10);
            local_5b8 = local_4b8;
            uStack_5b0 = CONCAT44(local_498,local_49c);
            local_5a8 = (ulonglong ******)CONCAT26(uStack_582,CONCAT24(uStack_584,local_488));
            cVar5 = '\x01';
          }
        }
        else {
          cVar5 = '\0';
        }
      }
      else {
        cVar5 = '\0';
      }
      uStack_5a0 = CONCAT71(uStack_5a0._1_7_,cVar5);
      if (cVar5 == '\x01') {
        *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
        *(undefined1 *)(param_3 + 5) = 1;
        *(undefined1 *)(param_3 + 7) = 1;
        *(undefined8 *)((longlong)param_3 + 0x3c) = 0x40800000;
        *(undefined8 *)((longlong)param_3 + 0x44) = 0x40800000;
        pcVar1 = *(code **)(*param_3 + 0xa8);
        uVar14 = FUN_1402d82e0("train_departing_to_connect",&DAT_140a4d88c);
        (*pcVar1)(param_3,uVar14,0x11);
        uStack_586 = 0;
        uStack_584 = 0;
        uStack_582 = 0;
        local_598 = (ulonglong ******)0x0;
        uStack_590 = (ulonglong *******)0x0;
        local_588 = 0x101;
        uVar6 = uStack_5b0._4_4_;
        if ((ulonglong *******)local_5b8 != (ulonglong *******)0x0) {
          uVar15 = FUN_14033f7f0(*local_540 + 0x180);
        }
        local_5d8 = local_598;
        ppppppuStack_5d0 = (ulonglong ******)uStack_590;
        local_5c8 = (ulonglong *****)
                    CONCAT26(uStack_582,CONCAT24(uStack_584,CONCAT22(uStack_586,local_588)));
        FUN_140805710(&local_560,0,local_568,uVar11,uVar15,CONCAT44(uVar25,uVar6),&local_5d8);
      }
    }
    *(undefined1 *)(param_3 + 3) = 1;
    *(undefined4 *)((longlong)param_3 + 0x1c) = 0x1e0;
    (**(code **)(*param_3 + 200))(param_3);
    if ((param_5 == 0) && (*(char *)(pppppppuVar9 + 0x96) != '\0')) {
      *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
      *(undefined1 *)(param_3 + 5) = 1;
      pcVar1 = *(code **)(*param_3 + 0xa0);
      iVar7 = lround(SUB84((double)pppppppuVar9[0x79] * _DAT_140aabdf0,0));
      pppppppuVar16 = (ulonglong *******)FUN_140572550(&local_5b8,(longlong)iVar7);
      local_5d8 = (ulonglong ******)FUN_1402d82e0("info_train_speed","Speed: {} km/h");
      do {
        pppppppuVar24 = (ulonglong *******)((longlong)pppppppuVar24 + 1);
      } while (*(char *)((longlong)local_5d8 + (longlong)pppppppuVar24) != '\0');
      local_538 = (ulonglong ******)pppppppuVar16;
      if ((ulonglong ******)0xf < pppppppuVar16[3]) {
        local_538 = *pppppppuVar16;
      }
      ppppppuStack_530 = pppppppuVar16[2];
      uStack_590 = &local_538;
      local_598 = (ulonglong ******)0xd;
      ppppppuStack_5d0 = (ulonglong ******)pppppppuVar24;
      FUN_140021e90(&local_560,&local_5d8,&local_598);
      (*pcVar1)(param_3,&local_560);
      if ((longlong *)0xf < local_548) {
        plVar19 = local_560;
        if ((0xfff < (longlong)local_548 + 1U) &&
           (plVar19 = (longlong *)local_560[-1],
           0x1f < (ulonglong)((longlong)local_560 + (-8 - (longlong)plVar19)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(plVar19);
      }
      local_550 = (ulonglong ******)0x0;
      local_548 = (longlong *)0xf;
      local_560 = (longlong *)((ulonglong)local_560 & 0xffffffffffffff00);
      FUN_140002d30(&local_5b8);
    }
    (**(code **)(*param_3 + 0x18))(param_3);
  }
  *(undefined4 *)((longlong)param_3 + 0x34) = 0x41900000;
  *(undefined1 *)(param_3 + 6) = 1;
  *(undefined1 *)(param_3 + 3) = 1;
  *(undefined4 *)((longlong)param_3 + 0x1c) = 0xa0;
  *(undefined1 *)(param_3 + 4) = 1;
  *(undefined4 *)((longlong)param_3 + 0x24) = 2;
  (**(code **)(*param_3 + 8))(param_3);
  if (*(char *)(pppppppuVar9 + 0x16) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"fatal",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x3e) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"hitch",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x3a) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"presence",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x43) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"blackhole",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x51) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"new_destination",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x96) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"drive",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x9a) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"timed_stop",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0x9e) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"schedule_stop",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0xab) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"station_stop",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0xa2) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"run_stop",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0xb3) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"dispatch_cooldown",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0xba) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"run_dispatch",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0xbe) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"schedule_dispatch",0x11);
  }
  if (*(char *)(pppppppuVar9 + 0xc1) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"ended_drive",0x11);
  }
  if (*(char *)((longlong)pppppppuVar9 + 0x611) != '\0') {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    (**(code **)(*param_3 + 0xa8))(param_3,"ended_timed_stop",0x11);
  }
  if (pppppppuVar9[0xc4] != pppppppuVar9[0xc3]) {
    *(undefined4 *)((longlong)param_3 + 0x2c) = 0xbf800000;
    *(undefined1 *)(param_3 + 5) = 1;
    *(undefined1 *)(param_3 + 7) = 1;
    *(undefined4 *)((longlong)param_3 + 0x44) = 0x41000000;
    *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
    *(undefined4 *)(param_3 + 10) = 6;
    pcVar1 = *(code **)(*param_3 + 0xa0);
    local_538 = (ulonglong ******)((longlong)pppppppuVar9[0xc4] - (longlong)pppppppuVar9[0xc3] >> 5)
    ;
    ppppppuStack_530 = (ulonglong ******)uStack_590;
    uStack_590 = &local_538;
    local_248 = 0;
    local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_250 = local_238;
    local_240 = 500;
    local_598 = (ulonglong ******)&DAT_00000004;
    local_5d8 = (ulonglong ******)0x140aa4df0;
    ppppppuStack_5d0 = (ulonglong ******)&DAT_00000008;
    FUN_140022830(&local_258,&local_5d8,&local_598);
    local_560 = (longlong *)0x0;
    puStack_558 = (undefined8 *)0x0;
    local_550 = (ulonglong ******)0x0;
    local_548 = (longlong *)0x0;
    FUN_140002c00(&local_560,local_250);
    local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_250 != local_238) {
      puVar18 = local_250;
      if ((0xfff < local_240) &&
         (puVar18 = *(undefined1 **)(local_250 + -8),
         (undefined1 *)0x1f < local_250 + (-8 - (longlong)puVar18))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar18);
    }
    (*pcVar1)(param_3,&local_560);
    if ((longlong *)0xf < local_548) {
      plVar19 = local_560;
      if ((0xfff < (longlong)local_548 + 1U) &&
         (plVar19 = (longlong *)local_560[-1],
         0x1f < (ulonglong)((longlong)local_560 + (-8 - (longlong)plVar19)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(plVar19);
    }
  }
  *(undefined1 *)(param_3 + 3) = 1;
  *(undefined4 *)((longlong)param_3 + 0x1c) = 0x1e0;
  (**(code **)(*param_3 + 200))(param_3);
  (**(code **)(*param_3 + 0x18))(param_3);
  return;
}


// Incoming references
// 0xc3ad3c DATA caller none
// 0x7f6cf4 UNCONDITIONAL_CALL caller 1407f3600
// 0x6b9cbd UNCONDITIONAL_CALL caller 1406b9c40
