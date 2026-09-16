
void FUN_1406678f0(void *param_1,undefined8 *param_2,longlong param_3,ulonglong param_4)

{
  undefined8 uVar1;
  double dVar2;
  char cVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  longlong lVar7;
  longlong *plVar8;
  undefined8 uVar9;
  longlong lVar10;
  int iVar11;
  void *_Memory;
  char *******pppppppcVar12;
  undefined1 *puVar13;
  longlong *plVar14;
  ulonglong uVar15;
  longlong *plVar16;
  undefined4 uVar17;
  longlong local_528;
  longlong **pplStack_520;
  longlong *local_518;
  longlong **pplStack_510;
  longlong *local_508;
  longlong **pplStack_500;
  longlong **local_4f8;
  char ******local_4e8;
  undefined8 uStack_4e0;
  undefined8 local_4d8;
  ulonglong local_4d0;
  undefined8 local_4c8;
  undefined8 uStack_4c0;
  undefined8 local_4b8;
  undefined8 uStack_4b0;
  undefined4 local_4a8;
  undefined4 uStack_4a4;
  undefined4 uStack_4a0;
  undefined4 uStack_49c;
  undefined4 local_498;
  undefined4 uStack_494;
  undefined4 uStack_490;
  undefined4 uStack_48c;
  undefined **local_488;
  undefined1 *local_480;
  undefined8 local_478;
  ulonglong local_470;
  undefined1 local_468 [512];
  undefined **local_268;
  undefined1 *local_260;
  undefined8 local_258;
  ulonglong local_250;
  undefined1 local_248 [520];

  *(undefined4 *)((longlong)param_1 + 0x48) = *(undefined4 *)((longlong)param_1 + 0x20);
  plVar8 = (longlong *)((longlong)param_1 + 0x28);
  plVar16 = (longlong *)((longlong)param_1 + 0x50);
  if (plVar16 != plVar8) {
    plVar14 = plVar8;
    if (0xf < *(ulonglong *)((longlong)param_1 + 0x40)) {
      plVar14 = (longlong *)*plVar8;
    }
    FUN_140030630(plVar16,plVar14);
  }
  *(undefined1 *)((longlong)param_1 + 0x819) = *(undefined1 *)((longlong)param_1 + 0x818);
  local_4e8 = (char ******)0x0;
  uStack_4e0 = 0;
  local_4e8 = (char ******)FUN_140003270(0x20);
  uVar1 = s___company_dialog_140a90008._8_8_;
  local_4d8 = 0x10;
  local_4d0 = 0x1f;
  *local_4e8 = (char *****)s___company_dialog_140a90008._0_8_;
  local_4e8[1] = (char *****)uVar1;
  *(char *)(local_4e8 + 2) = '\0';
  local_4c8._0_4_ = SUB84(param_1,0);
  local_4c8._4_4_ = (undefined4)((ulonglong)param_1 >> 0x20);
  uStack_4c0._0_4_ = SUB84(param_2,0);
  uStack_4c0._4_4_ = (undefined4)((ulonglong)param_2 >> 0x20);
  local_4a8 = (undefined4)local_4c8;
  uStack_4a4 = local_4c8._4_4_;
  uStack_4a0 = (undefined4)uStack_4c0;
  uStack_49c = uStack_4c0._4_4_;
  local_4b8._0_4_ = (int)param_3;
  local_4b8._4_4_ = (undefined4)((ulonglong)param_3 >> 0x20);
  uStack_4b0._0_4_ = (undefined4)param_4;
  uStack_4b0._4_4_ = (undefined4)(param_4 >> 0x20);
  local_498 = (int)local_4b8;
  uStack_494 = local_4b8._4_4_;
  uStack_490 = (undefined4)uStack_4b0;
  uStack_48c = uStack_4b0._4_4_;
  local_4c8 = param_1;
  uStack_4c0 = param_2;
  local_4b8 = param_3;
  uStack_4b0 = param_4;
  FUN_140697ac0(param_2,&local_4e8);
  if (0xf < local_4d0) {
    pppppppcVar12 = (char *******)local_4e8;
    if ((0xfff < local_4d0 + 1) &&
       (pppppppcVar12 = (char *******)local_4e8[-1],
       (char *)0x1f < (char *)((longlong)local_4e8 + (-8 - (longlong)pppppppcVar12)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppppcVar12);
  }
  *(undefined4 *)((longlong)param_1 + 0x20) = *(undefined4 *)((longlong)param_1 + 0x48);
  if (plVar8 != plVar16) {
    if (0xf < *(ulonglong *)((longlong)param_1 + 0x68)) {
      plVar16 = (longlong *)*plVar16;
    }
    FUN_140030630(plVar8,plVar16,*(undefined8 *)((longlong)param_1 + 0x60));
  }
  *(undefined1 *)((longlong)param_1 + 0x818) = *(undefined1 *)((longlong)param_1 + 0x819);
  if (*(char *)((longlong)param_1 + 0x820) != '\0') {
    uVar1 = *param_2;
    uVar5 = FUN_1402d82e0("option_company_time_zones_change","Change map time zones mode");
    FUN_14081d6d0(uVar1,uVar5,0,1);
  }
  if (*(char *)((longlong)param_1 + 0x828) != '\0') {
    uVar1 = *param_2;
    uVar5 = FUN_1402d82e0("option_company_acc_utc0_offset_s_change",
                          "Change accounting day shift time");
    FUN_14081d6d0(uVar1,uVar5,0,1);
  }
  uVar1 = *param_2;
  local_528 = FUN_1402d82e0("cancel","Cancel");
  pplStack_520 = (longlong **)FUN_1402d82e0("switch_no_cash","Switch to unlimited");
  local_508 = (longlong *)0x0;
  pplStack_500 = (longlong **)0x0;
  local_4f8 = (longlong **)0x0;
  local_508 = (longlong *)thunk_FUN_140983da8(0x10);
  pplStack_500 = (longlong **)(local_508 + 2);
  local_4f8 = pplStack_500;
  memmove(local_508,&local_528,0x10);
  uVar5 = FUN_1402d82e0("confirm_set_no_cash_desc",
                        "Switching to unlimited money cannot be undone. Are you sure?");
  uVar6 = FUN_1402d82e0("confirm_set_no_cash","Confirm switching to unlimited money");
  iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar6,uVar5,&local_508,0);
  if (iVar4 == 1) {
    lVar7 = FUN_1407486d0(param_3 + 0x228);
    *(undefined1 *)(lVar7 + 0x21) = 1;
  }
  uVar1 = *param_2;
  local_528 = FUN_1402d82e0("cancel","Cancel");
  pplStack_520 = (longlong **)FUN_1402d82e0("switch_no_couplers","Switch to unrestricted");
  local_508 = (longlong *)0x0;
  pplStack_500 = (longlong **)0x0;
  local_4f8 = (longlong **)0x0;
  local_508 = (longlong *)thunk_FUN_140983da8(0x10);
  pplStack_500 = (longlong **)(local_508 + 2);
  local_4f8 = pplStack_500;
  memmove(local_508,&local_528,0x10);
  uVar5 = FUN_1402d82e0("confirm_set_no_couplers_desc",
                        "Switching to unrestricted coupling cannot be undone. Are you sure?");
  uVar6 = FUN_1402d82e0("confirm_set_no_couplers","Confirm switching to unrestricted coupling");
  iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar6,uVar5,&local_508,0);
  if (iVar4 == 1) {
    lVar7 = FUN_1407486d0(param_3 + 0x228);
    *(undefined1 *)(lVar7 + 0x20) = 1;
  }
  if (*(char *)((longlong)param_1 + 0x568) != '\0') {
    FUN_140247b90(&local_4c8,(longlong)param_1 + 0x548);
    plVar8 = (longlong *)FUN_140498e70(&local_4a8,&local_4c8);
    local_518 = (longlong *)FUN_1402d82e0("file_export_desc","Data has been exported to {}");
    pplStack_510 = (longlong **)0xffffffffffffffff;
    do {
      pplStack_510 = (longlong **)((longlong)pplStack_510 + 1);
    } while (*(char *)((longlong)local_518 + (longlong)pplStack_510) != '\0');
    local_508 = plVar8;
    if (0xf < (ulonglong)plVar8[3]) {
      local_508 = (longlong *)*plVar8;
    }
    pplStack_500 = (longlong **)plVar8[2];
    pplStack_520 = &local_508;
    local_528 = 0xd;
    FUN_140021e90(&local_4e8,&local_518,&local_528);
    FUN_140002d30(&local_4a8);
    FUN_140025470(&local_4c8);
    uVar1 = *param_2;
    pppppppcVar12 = &local_4e8;
    if (0xf < local_4d0) {
      pppppppcVar12 = (char *******)local_4e8;
    }
    uVar5 = FUN_1402d82e0("file_export_title","Exported to file");
    cVar3 = FUN_14072f2e0(uVar1,param_2 + 0x13,uVar5,pppppppcVar12);
    if ((cVar3 != '\0') && (*(char *)((longlong)param_1 + 0x568) != '\0')) {
      FUN_140025470((longlong)param_1 + 0x548);
      *(undefined1 *)((longlong)param_1 + 0x568) = 0;
    }
    FUN_140002d30(&local_4e8);
  }
  uVar1 = *param_2;
  local_518 = (longlong *)FUN_1402d82e0("cancel","Cancel");
  pplStack_510 = (longlong **)FUN_1402d82e0("full_simulation_reset_yes","Reset simulation");
  local_508 = (longlong *)0x0;
  pplStack_500 = (longlong **)0x0;
  local_4f8 = (longlong **)0x0;
  local_508 = (longlong *)thunk_FUN_140983da8(0x10);
  pplStack_500 = (longlong **)(local_508 + 2);
  local_4f8 = pplStack_500;
  memmove(local_508,&local_518,0x10);
  uVar5 = FUN_1402d82e0("full_simulation_reset_desc",
                        "All pax will be removed from stations and trains, all trains will be intervened, line and orders timing will be refreshed. Are you sure?"
                       );
  uVar6 = FUN_1402d82e0("full_simulation_reset","Full simulation reset");
  iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar6,uVar5,&local_508,0);
  if (iVar4 == 1) {
    lVar7 = FUN_140687af0(param_3);
    *(undefined1 *)(lVar7 + 0x20) = 1;
  }
  if (*(char *)((longlong)param_1 + 0x820) != '\0') {
    uVar1 = *param_2;
    local_528 = FUN_1402d82e0("cancel","Cancel");
    pplStack_520 = (longlong **)FUN_1402d82e0("full_simulation_reset_yes","Reset simulation");
    local_518 = &local_528;
    pplStack_510 = &local_518;
    local_508 = local_518;
    pplStack_500 = pplStack_510;
    uVar5 = FUN_1405bdf60(&local_4a8,&local_508);
    uVar6 = FUN_1402d82e0("full_simulation_reset_desc",
                          "All pax will be removed from stations and trains, all trains will be intervened, line and orders timing will be refreshed. Are you sure?"
                         );
    uVar9 = FUN_1402d82e0("option_company_time_zones_change","Change map time zones mode");
    uVar17 = 0;
    iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar9,uVar6,uVar5,0);
    if (iVar4 == 1) {
      if (*(longlong *)(param_3 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
        abort();
      }
      lVar7 = *(longlong *)(*(longlong *)(param_3 + 600) + 0x410);
      FUN_14073f9b0(param_3,*(undefined4 *)(lVar7 + 0x44),*(undefined4 *)(lVar7 + 0x48),
                    *(undefined4 *)(lVar7 + 0x4c),*(undefined4 *)(lVar7 + 0x50),
                    CONCAT44(uVar17,*(undefined4 *)(lVar7 + 0x54)),*(undefined1 *)(lVar7 + 0x58),
                    *(undefined4 *)(lVar7 + 0x5c),*(undefined4 *)(lVar7 + 100),
                    *(undefined4 *)((longlong)param_1 + 0x81c),*(undefined4 *)(lVar7 + 0x6c),
                    *(undefined4 *)(lVar7 + 0x60),*(undefined8 *)(lVar7 + 0xd0));
    }
    else if (iVar4 == -1) goto LAB_140667fff;
    *(undefined1 *)((longlong)param_1 + 0x820) = 0;
  }
LAB_140667fff:
  if (*(char *)((longlong)param_1 + 0x828) != '\0') {
    uVar1 = *param_2;
    local_528 = FUN_1402d82e0("cancel","Cancel");
    pplStack_520 = (longlong **)
                   FUN_1402d82e0("option_company_acc_utc0_offset_s_change_yes","Change time");
    local_518 = &local_528;
    pplStack_510 = &local_518;
    local_508 = local_518;
    pplStack_500 = pplStack_510;
    uVar5 = FUN_1405bdf60(&local_4a8,&local_508);
    uVar6 = FUN_1402d82e0("option_company_acc_utc0_offset_s_change_desc",
                          "Current day accounting will be lost and/or incorrect. Proper accounting won\'t be restablished until 24 hours of in-game time passes. Some historical accounting might be lost. No historical accounting will be time shifted. This setting only applies starting at the moment it is changed. Are you sure?"
                         );
    uVar9 = FUN_1402d82e0("option_company_acc_utc0_offset_s_change",
                          "Change accounting day shift time");
    uVar17 = 0;
    iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar9,uVar6,uVar5,0);
    if (iVar4 == 1) {
      if (*(longlong *)(param_3 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
        abort();
      }
      lVar7 = *(longlong *)(*(longlong *)(param_3 + 600) + 0x410);
      FUN_14073f9b0(param_3,*(undefined4 *)(lVar7 + 0x44),*(undefined4 *)(lVar7 + 0x48),
                    *(undefined4 *)(lVar7 + 0x4c),*(undefined4 *)(lVar7 + 0x50),
                    CONCAT44(uVar17,*(undefined4 *)(lVar7 + 0x54)),*(undefined1 *)(lVar7 + 0x58),
                    *(undefined4 *)(lVar7 + 0x5c),*(undefined4 *)(lVar7 + 100),
                    *(undefined4 *)(lVar7 + 0x68),*(undefined4 *)((longlong)param_1 + 0x824),
                    *(undefined4 *)(lVar7 + 0x60),*(undefined8 *)(lVar7 + 0xd0));
    }
    else if (iVar4 == -1) goto LAB_140668184;
    *(undefined1 *)((longlong)param_1 + 0x828) = 0;
  }
LAB_140668184:
  uVar1 = *param_2;
  local_528 = FUN_1402d82e0("cancel","Cancel");
  pplStack_520 = (longlong **)FUN_1402d82e0("sell_all_trains_yes","Sell all");
  local_518 = &local_528;
  pplStack_510 = &local_518;
  local_508 = local_518;
  pplStack_500 = pplStack_510;
  uVar5 = FUN_1405bdf60(&local_4a8,&local_508);
  uVar6 = FUN_1402d82e0("mass_del_trains_desc","Do you want to sell all trains?");
  uVar9 = FUN_1402d82e0("mass_del_trains_title","Sell all trains");
  iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar9,uVar6,uVar5,0);
  if (iVar4 == 1) {
    lVar7 = FUN_1405bc200(param_3);
    *(undefined4 *)(lVar7 + 0x20) = 1;
  }
  uVar1 = *param_2;
  local_528 = FUN_1402d82e0("cancel","Cancel");
  pplStack_520 = (longlong **)FUN_1402d82e0("mass_del_lines_yes","Delete all");
  local_518 = &local_528;
  pplStack_510 = &local_518;
  local_508 = local_518;
  pplStack_500 = pplStack_510;
  uVar5 = FUN_1405bdf60(&local_4a8,&local_508);
  uVar6 = FUN_1402d82e0("mass_del_lines_desc","Do you want to delete all lines?");
  uVar9 = FUN_1402d82e0("mass_del_lines_title","Delete all lines");
  iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar9,uVar6,uVar5,0);
  if (iVar4 == 1) {
    lVar7 = FUN_140687d10(param_3);
    *(undefined1 *)(lVar7 + 0x20) = 1;
  }
  uVar1 = *param_2;
  local_528 = FUN_1402d82e0("cancel","Cancel");
  pplStack_520 = (longlong **)FUN_1402d82e0("mass_del_schedules_yes","Delete all");
  local_518 = &local_528;
  pplStack_510 = &local_518;
  local_508 = local_518;
  pplStack_500 = pplStack_510;
  uVar5 = FUN_1405bdf60(&local_4a8,&local_508);
  uVar6 = FUN_1402d82e0("mass_del_schedules_desc","Do you want to delete all schedules?");
  uVar9 = FUN_1402d82e0("mass_del_schedules_title","Delete all schedules");
  iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar9,uVar6,uVar5,0);
  if (iVar4 == 1) {
    lVar7 = FUN_140687d10(param_3);
    *(undefined1 *)(lVar7 + 0x21) = 1;
  }
  if (*(longlong *)(param_3 + 0x260) != 0) {
    if (*(longlong *)(param_3 + 600) != 0) {
      dVar2 = (double)FUN_14044ce00(*(longlong *)(param_3 + 0x260) + 0xa0,
                                    *(longlong *)(param_3 + 600),0);
      plVar8 = (longlong *)FUN_140572550(&local_4c8,(longlong)dVar2);
      local_528 = FUN_1402d82e0("mass_intervention_trains_desc",
                                "Do you want to issue an intervention to all trains for a price of ${}?"
                               );
      lVar7 = -1;
      pplStack_520 = (longlong **)0xffffffffffffffff;
      do {
        pplStack_520 = (longlong **)((longlong)pplStack_520 + 1);
      } while (*(char *)(local_528 + (longlong)pplStack_520) != '\0');
      local_508 = plVar8;
      if (0xf < (ulonglong)plVar8[3]) {
        local_508 = (longlong *)*plVar8;
      }
      pplStack_500 = (longlong **)plVar8[2];
      pplStack_510 = &local_508;
      local_478 = 0;
      local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_480 = local_468;
      local_470 = 500;
      local_518 = (longlong *)0xd;
      FUN_140022830(&local_488,&local_528,&local_518);
      local_4e8 = (char ******)0x0;
      uStack_4e0 = 0;
      local_4d8 = 0;
      local_4d0 = 0;
      FUN_140002c00(&local_4e8,local_480);
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
      if (0xf < uStack_4b0) {
        _Memory = local_4c8;
        if ((0xfff < uStack_4b0 + 1) &&
           (_Memory = *(void **)((longlong)local_4c8 + -8),
           0x1f < (ulonglong)((longlong)local_4c8 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory);
      }
      local_4b8 = 0;
      uStack_4b0 = 0xf;
      local_4c8 = (longlong *)((ulonglong)local_4c8 & 0xffffffffffffff00);
      uVar1 = *param_2;
      local_518 = (longlong *)FUN_1402d82e0("cancel","Cancel");
      pplStack_510 = (longlong **)FUN_1402d82e0("intervene_all_trains","Intervene all trains");
      local_508 = (longlong *)0x0;
      pplStack_500 = (longlong **)0x0;
      local_4f8 = (longlong **)0x0;
      local_508 = (longlong *)thunk_FUN_140983da8(0x10);
      pplStack_500 = (longlong **)(local_508 + 2);
      local_4f8 = pplStack_500;
      memmove(local_508,&local_518,0x10);
      pppppppcVar12 = &local_4e8;
      if (0xf < local_4d0) {
        pppppppcVar12 = (char *******)local_4e8;
      }
      uVar5 = FUN_1402d82e0("mass_intervention_trains_title","Confirm all trains intervention");
      iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar5,pppppppcVar12,&local_508,0);
      if (iVar4 == 1) {
        lVar10 = FUN_1405bc270(param_3);
        *(undefined4 *)(lVar10 + 0x20) = 3;
      }
      if (0xf < local_4d0) {
        pppppppcVar12 = (char *******)local_4e8;
        if ((0xfff < local_4d0 + 1) &&
           (pppppppcVar12 = (char *******)local_4e8[-1],
           0x1f < (ulonglong)((longlong)local_4e8 + (-8 - (longlong)pppppppcVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppcVar12);
      }
      if (*(longlong *)(param_3 + 0x260) != 0) {
        if (*(longlong *)(param_3 + 600) != 0) {
          dVar2 = (double)FUN_14044ce00(*(longlong *)(param_3 + 0x260) + 0xa0,
                                        *(longlong *)(param_3 + 600),1);
          FUN_14028fc90(&local_4c8,(longlong)dVar2);
          iVar11 = (int)local_4b8 + -3;
          iVar4 = (int)((longlong)dVar2 >> 0x3f);
          if (-iVar4 < iVar11) {
            lVar10 = (longlong)iVar11;
            uVar15 = (ulonglong)(((iVar11 + iVar4) - 1U) / 3 + 1);
            do {
              FUN_140587330(&local_4c8,lVar10);
              lVar10 = lVar10 + -3;
              uVar15 = uVar15 - 1;
            } while (uVar15 != 0);
          }
          local_528 = FUN_1402d82e0("mass_intervention_stale_trains_desc",
                                    "Do you want to issue an intervention to all stale trains for a price of ${}?"
                                   );
          do {
            lVar7 = lVar7 + 1;
          } while (*(char *)(local_528 + lVar7) != '\0');
          local_508 = &local_4c8;
          if (0xf < uStack_4b0) {
            local_508 = local_4c8;
          }
          pplStack_500 = (longlong **)local_4b8;
          pplStack_510 = &local_508;
          local_258 = 0;
          local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          local_260 = local_248;
          local_250 = 500;
          local_518 = (longlong *)0xd;
          pplStack_520 = (longlong **)lVar7;
          FUN_140022830(&local_268,&local_528,&local_518);
          local_4e8 = (char ******)0x0;
          uStack_4e0 = 0;
          local_4d8 = 0;
          local_4d0 = 0;
          FUN_140002c00(&local_4e8,local_260);
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
          FUN_140002d30(&local_4c8);
          uVar1 = *param_2;
          local_528 = FUN_1402d82e0("cancel","Cancel");
          pplStack_520 = (longlong **)
                         FUN_1402d82e0("intervene_all_stale_trains","Intervene all stale");
          local_518 = &local_528;
          pplStack_510 = &local_518;
          local_508 = local_518;
          pplStack_500 = pplStack_510;
          uVar5 = FUN_1405bdf60(&local_4a8,&local_508);
          pppppppcVar12 = &local_4e8;
          if (0xf < local_4d0) {
            pppppppcVar12 = (char *******)local_4e8;
          }
          uVar6 = FUN_1402d82e0("mass_intervention_stale_trains_title",
                                "Confirm all stale trains intervention");
          iVar4 = FUN_14072f090(uVar1,param_2 + 0x13,uVar6,pppppppcVar12,uVar5,0);
          if (iVar4 == 1) {
            lVar7 = FUN_1405bc270(param_3);
            *(undefined4 *)(lVar7 + 0x20) = 2;
          }
          if (0xf < local_4d0) {
            FUN_140003040(&local_4e8,local_4e8);
          }
          if ((((*(longlong *)(param_3 + 0x260) != 0) && (*(longlong *)(param_3 + 600) != 0)) &&
              (FUN_14065c6a0((longlong)param_1 + 0xc0,param_2,param_3,*(longlong *)(param_3 + 600),
                             *(longlong *)(param_3 + 0x260)), *(longlong *)(param_3 + 0x260) != 0))
             && (((*(longlong *)(param_3 + 600) != 0 &&
                  (FUN_14065f400((longlong)param_1 + 0x4c0,param_2,param_3),
                  *(longlong *)(param_3 + 0x260) != 0)) && (*(longlong *)(param_3 + 600) != 0)))) {
            FUN_14065fa70((longlong)param_1 + 0x498,param_2,param_3);
            return;
          }
                    /* WARNING: Subroutine does not return */
          abort();
        }
      }
                    /* WARNING: Subroutine does not return */
      abort();
    }
  }
                    /* WARNING: Subroutine does not return */
  abort();
}


// ref 140c32768 caller none
// ref 140683d9d caller 1406830d0
