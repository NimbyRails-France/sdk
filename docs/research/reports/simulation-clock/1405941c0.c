// Candidate VA 1405941c0; RVA 0x5941c0
// Ghidra inferred prototype: undefined FUN_1405941c0()

void FUN_1405941c0(int *param_1,undefined8 *param_2,char *param_3,longlong param_4,longlong param_5,
                  undefined4 param_6,longlong *param_7)

{
  int *piVar1;
  code *pcVar2;
  ulonglong *puVar3;
  undefined4 *puVar4;
  void *pvVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  char cVar9;
  int iVar10;
  longlong *plVar11;
  void *pvVar12;
  longlong *plVar13;
  undefined8 uVar14;
  undefined8 *puVar15;
  undefined8 *puVar16;
  undefined8 *puVar17;
  ulonglong uVar18;
  undefined1 *_Memory;
  ulonglong uVar19;
  ushort *puVar20;
  longlong lVar21;
  int *piVar22;
  undefined8 *puVar23;
  undefined4 *puVar24;
  longlong lVar25;
  longlong *plVar26;
  char *pcVar27;
  ulonglong *puVar28;
  ulonglong uVar29;
  ushort *puVar30;
  bool bVar31;
  undefined8 local_388;
  undefined8 uStack_380;
  undefined8 local_378;
  ulonglong uStack_370;
  int *local_368;
  int *local_360;
  int local_358;
  longlong *local_350;
  longlong *local_348;
  longlong *local_340;
  undefined8 *local_338;
  undefined8 local_330;
  longlong local_328;
  longlong lStack_320;
  undefined8 *local_318;
  undefined8 *puStack_310;
  undefined8 *local_308;
  undefined8 *local_2f8;
  undefined8 uStack_2f0;
  void *local_2e8;
  undefined8 uStack_2e0;
  undefined8 local_2d8;
  ulonglong local_2d0;
  undefined1 local_2c8;
  undefined7 uStack_2c7;
  undefined8 local_2b8;
  ulonglong local_2b0;
  undefined1 local_2a0;
  undefined7 uStack_29f;
  undefined8 local_290;
  ulonglong local_288;
  undefined **local_278;
  undefined1 *local_270;
  undefined8 local_268;
  ulonglong local_260;
  undefined1 local_258 [536];

  plVar26 = (longlong *)0x0;
  local_358 = 0;
  *(char *)(param_1 + 2) = *param_3;
  if ((*(longlong *)(param_4 + 600) == 0) ||
     (lVar25 = *(longlong *)(param_4 + 0x260), local_328 = lVar25, lVar25 == 0)) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  local_2f8 = *(undefined8 **)(*(longlong *)(param_4 + 600) + 0x408);
  cVar9 = (**(code **)(*param_7 + 0x170))(param_7);
  if (cVar9 != '\0') goto LAB_140594354;
  puVar17 = *(undefined8 **)(param_4 + 0x88);
  if (puVar17 < *(undefined8 **)(param_4 + 0x90)) {
    *puVar17 = 0;
    puVar17[1] = 5;
    *(longlong *)(param_4 + 0x88) = *(longlong *)(param_4 + 0x88) + 0x10;
    plVar13 = *(longlong **)(param_4 + 0x88);
  }
  else {
    lVar25 = (longlong)puVar17 - *(longlong *)(param_4 + 0x80) >> 4;
    if (lVar25 == 0) {
      lVar21 = 1;
LAB_1405942ad:
      lVar25 = lVar21 << 4;
      plVar11 = (longlong *)thunk_FUN_140983da8(lVar25);
    }
    else {
      lVar21 = lVar25 * 2;
      if (lVar21 != 0) goto LAB_1405942ad;
      lVar25 = lVar25 << 5;
      plVar11 = plVar26;
    }
    pvVar12 = *(void **)(param_4 + 0x80);
    plVar13 = plVar11;
    if (pvVar12 != *(void **)(param_4 + 0x88)) {
      uVar18 = (longlong)*(void **)(param_4 + 0x88) - (longlong)pvVar12;
      pvVar12 = memmove(plVar11,pvVar12,uVar18);
      plVar13 = (longlong *)((longlong)pvVar12 + (uVar18 & 0xfffffffffffffff0));
    }
    *plVar13 = 0;
    plVar13[1] = 5;
    plVar13 = plVar13 + 2;
    if (*(void **)(param_4 + 0x80) != (void *)0x0) {
      free(*(void **)(param_4 + 0x80));
    }
    *(longlong **)(param_4 + 0x80) = plVar11;
    *(longlong **)(param_4 + 0x88) = plVar13;
    *(longlong *)(param_4 + 0x90) = (longlong)plVar11 + lVar25;
    lVar25 = local_328;
  }
  plVar13[-2] = param_5;
  *(int *)(plVar13 + -1) = *param_1;
  *(int *)((longlong)plVar13 + -4) = param_1[1];
LAB_140594354:
  plVar11 = *(longlong **)(*(longlong *)(lVar25 + 0x2200) + 1000);
  plVar13 = *(longlong **)(*(longlong *)(lVar25 + 0x2200) + 0x3e0);
  if (plVar13 != plVar11) {
    do {
      if (((*plVar13 == param_5) && ((int)plVar13[1] == *param_1)) &&
         (*(int *)((longlong)plVar13 + 0xc) == param_1[1])) {
        plVar26 = plVar13;
      }
      plVar13 = plVar13 + 0xb;
    } while (plVar13 != plVar11);
    if ((plVar26 != (longlong *)0x0) && (plVar26[2] != plVar26[3])) {
      piVar22 = (int *)plVar26[6];
      local_368 = (int *)plVar26[5];
      if (local_368 != piVar22) {
        do {
          if (*local_368 == param_1[1]) break;
          local_368 = local_368 + 1;
        } while (local_368 != piVar22);
        if (local_368 == piVar22) {
          param_1[1] = piVar22[-1];
          local_368 = (int *)(plVar26[6] + -4);
        }
        local_318 = (undefined8 *)0x0;
        puStack_310 = (undefined8 *)0x0;
        local_308 = (undefined8 *)0x0;
        local_330 = (ulonglong)local_330._4_4_ << 0x20;
        piVar1 = (int *)plVar26[3];
        local_350 = plVar26;
        for (piVar22 = (int *)plVar26[2]; piVar22 != piVar1; piVar22 = piVar22 + 1) {
          if (*piVar22 == *param_1) {
            local_330 = (longlong)puStack_310 - (longlong)local_318 >> 5;
          }
          uStack_380 = (undefined8 **)0x0;
          local_378 = 0;
          uStack_370 = 0xf;
          local_388 = (void *)0x0;
          local_358 = 8;
          switch(*piVar22) {
          case 0:
            lVar25 = FUN_1402d82e0("accounting_period_lifetime","Lifetime");
            lVar21 = -1;
            do {
              lVar21 = lVar21 + 1;
            } while (*(char *)(lVar25 + lVar21) != '\0');
            break;
          case 1:
            lVar25 = FUN_1402d82e0("accounting_period_year",&DAT_140a84ab8);
            lVar21 = -1;
            do {
              lVar21 = lVar21 + 1;
            } while (*(char *)(lVar25 + lVar21) != '\0');
            break;
          case 2:
            lVar25 = FUN_1402d82e0("accounting_period_quarter","Quarter");
            lVar21 = -1;
            do {
              lVar21 = lVar21 + 1;
            } while (*(char *)(lVar25 + lVar21) != '\0');
            break;
          case 3:
            lVar25 = FUN_1402d82e0("accounting_period_month","Month");
            lVar21 = -1;
            do {
              lVar21 = lVar21 + 1;
            } while (*(char *)(lVar25 + lVar21) != '\0');
            break;
          case 4:
            lVar25 = FUN_1402d82e0("accounting_period_week",&DAT_140a84b50);
            lVar21 = -1;
            do {
              lVar21 = lVar21 + 1;
            } while (*(char *)(lVar25 + lVar21) != '\0');
            break;
          case 5:
            lVar25 = FUN_1402d82e0("accounting_period_day",&DAT_140a84b10);
            lVar21 = -1;
            do {
              lVar21 = lVar21 + 1;
            } while (*(char *)(lVar25 + lVar21) != '\0');
            break;
          default:
            goto switchD_14059446f_default;
          }
          FUN_140030630(&local_388,lVar25);
switchD_14059446f_default:
          if (puStack_310 < local_308) {
            *puStack_310 = 0;
            puStack_310[1] = 0;
            puStack_310[2] = 0;
            puStack_310[3] = 0;
            *(undefined4 *)puStack_310 = (undefined4)local_388;
            *(undefined4 *)((longlong)puStack_310 + 4) = local_388._4_4_;
            *(undefined4 *)(puStack_310 + 1) = (undefined4)uStack_380;
            *(undefined4 *)((longlong)puStack_310 + 0xc) = uStack_380._4_4_;
            puStack_310[2] = local_378;
            puStack_310[3] = uStack_370;
            local_378 = 0;
            uStack_370 = 0xf;
            local_388 = (int *)((ulonglong)local_388 & 0xffffffffffffff00);
            puStack_310 = puStack_310 + 4;
          }
          else {
            FUN_1404ba220(&local_318,&local_388);
          }
          if (0xf < uStack_370) {
            pvVar12 = local_388;
            if ((0xfff < uStack_370 + 1) &&
               (pvVar12 = *(void **)((longlong)local_388 + -8),
               0x1f < (ulonglong)((longlong)local_388 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(pvVar12);
          }
        }
        local_348 = param_7 + 3;
        *(undefined1 *)(param_7 + 4) = 1;
        *(undefined4 *)((longlong)param_7 + 0x24) = 2;
        (**(code **)(*param_7 + 8))(param_7);
        *(undefined4 *)((longlong)param_7 + 0x2c) = 0x42280000;
        *(undefined1 *)(param_7 + 5) = 1;
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x41e00000;
        *(undefined1 *)(param_7 + 6) = 1;
        pcVar2 = *(code **)(*param_7 + 0x80);
        cVar9 = *param_3;
        local_360._0_5_ = (uint5)(uint)local_360;
        local_378 = 0xd;
        uStack_370 = 0xf;
        local_388 = (int *)s_icon_plot_svg_140a85050._0_8_;
        uStack_380 = (undefined8 **)
                     (ulonglong)
                     CONCAT14(s_icon_plot_svg_140a85050[0xc],s_icon_plot_svg_140a85050._8_4_);
        uVar14 = FUN_14081dff0(*param_2,&local_2c8,&local_388);
        cVar9 = (*pcVar2)(param_7,uVar14,-(cVar9 != '\0') & 2);
        if (0xf < uStack_370) {
          pvVar12 = local_388;
          if ((0xfff < uStack_370 + 1) &&
             (pvVar12 = *(void **)((longlong)local_388 + -8),
             0x1f < (ulonglong)((longlong)local_388 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        if (cVar9 != '\0') {
          param_3[0x58] = '\x01';
          local_348 = param_7 + 3;
        }
        pcVar2 = *(code **)(*param_7 + 0xc0);
        uVar14 = FUN_1402d82e0("plots_button_tp","Open charts window");
        (*pcVar2)(param_7,uVar14);
        *(undefined4 *)((longlong)param_7 + 0x2c) = 0x41e00000;
        *(undefined1 *)(param_7 + 5) = 1;
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x41e00000;
        *(undefined1 *)(param_7 + 6) = 1;
        *(undefined1 *)(param_7 + 7) = 1;
        *(undefined8 *)((longlong)param_7 + 0x3c) = 0x40800000;
        *(undefined8 *)((longlong)param_7 + 0x44) = 0;
        pcVar2 = *(code **)(*param_7 + 0x80);
        bVar31 = local_368 == (int *)local_350[5];
        uVar14 = *param_2;
        local_360._0_5_ = (uint5)(uint)local_360;
        local_388 = (int *)0x0;
        uStack_380 = (undefined8 **)0x0;
        local_378 = 0;
        uStack_370 = 0;
        local_388 = (int *)FUN_140003270(0x20);
        uVar8 = s_icon_tri_left_svg_140a84ce8._12_4_;
        uVar7 = s_icon_tri_left_svg_140a84ce8._8_4_;
        uVar6 = s_icon_tri_left_svg_140a84ce8._4_4_;
        local_378 = 0x11;
        uStack_370 = 0x1f;
        *local_388 = s_icon_tri_left_svg_140a84ce8._0_4_;
        *(undefined4 *)((longlong)local_388 + 4) = uVar6;
        *(undefined4 *)((longlong)local_388 + 8) = uVar7;
        *(undefined4 *)((longlong)local_388 + 0xc) = uVar8;
        *(char *)((longlong)local_388 + 0x10) = s_icon_tri_left_svg_140a84ce8[0x10];
        *(char *)((longlong)local_388 + 0x11) = '\0';
        uVar14 = FUN_14081dff0(uVar14,&local_2c8,&local_388);
        cVar9 = (*pcVar2)(param_7,uVar14,bVar31);
        if (0xf < uStack_370) {
          pcVar27 = (char *)local_388;
          if ((0xfff < uStack_370 + 1) &&
             (pcVar27 = *(char **)((longlong)local_388 + -8),
             (char *)0x1f < (char *)((longlong)local_388 + (-8 - (longlong)pcVar27)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar27);
        }
        if (cVar9 != '\0') {
          local_368 = local_368 + -1;
          local_348 = param_7 + 3;
        }
        *(undefined1 *)(param_7 + 3) = 1;
        *(undefined4 *)((longlong)param_7 + 0x1c) = 0x1e0;
        *(undefined1 *)(param_7 + 7) = 1;
        *(undefined8 *)((longlong)param_7 + 0x3c) = 0x40800000;
        *(undefined8 *)((longlong)param_7 + 0x44) = 0x40800000;
        local_358 = (**(code **)(*param_7 + 0x120))(param_7,&local_318,local_330 & 0xffffffff);
        *(undefined4 *)((longlong)param_7 + 0x2c) = 0x41e00000;
        *(undefined1 *)(param_7 + 5) = 1;
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x41e00000;
        *(undefined1 *)(param_7 + 6) = 1;
        pcVar2 = *(code **)(*param_7 + 0x80);
        bVar31 = local_368 == (int *)(local_350[6] + -4);
        uVar14 = *param_2;
        local_360._0_5_ = (uint5)(uint)local_360;
        local_388 = (int *)0x0;
        uStack_380 = (undefined8 **)0x0;
        local_378 = 0;
        uStack_370 = 0;
        local_388 = (int *)FUN_140003270(0x20);
        uVar8 = s_icon_tri_right_svg_140a83c00._12_4_;
        uVar7 = s_icon_tri_right_svg_140a83c00._8_4_;
        uVar6 = s_icon_tri_right_svg_140a83c00._4_4_;
        local_378 = 0x12;
        uStack_370 = 0x1f;
        *local_388 = s_icon_tri_right_svg_140a83c00._0_4_;
        *(undefined4 *)((longlong)local_388 + 4) = uVar6;
        *(undefined4 *)((longlong)local_388 + 8) = uVar7;
        *(undefined4 *)((longlong)local_388 + 0xc) = uVar8;
        *(undefined2 *)((longlong)local_388 + 0x10) = s_icon_tri_right_svg_140a83c00._16_2_;
        *(char *)((longlong)local_388 + 0x12) = '\0';
        uVar14 = FUN_14081dff0(uVar14,&local_2c8,&local_388);
        cVar9 = (*pcVar2)(param_7,uVar14,bVar31);
        if (0xf < uStack_370) {
          pcVar27 = (char *)local_388;
          if ((0xfff < uStack_370 + 1) &&
             (pcVar27 = *(char **)((longlong)local_388 + -8),
             (char *)0x1f < (char *)((longlong)local_388 + (-8 - (longlong)pcVar27)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar27);
        }
        if (cVar9 != '\0') {
          local_368 = local_368 + 1;
          local_348 = param_7 + 3;
        }
        (**(code **)(*param_7 + 0x18))(param_7);
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x40800000;
        *(undefined1 *)(param_7 + 6) = 1;
        (**(code **)(*param_7 + 200))(param_7);
        FUN_140499b00(&local_2a0,*param_1,param_1[1],param_6);
        *(undefined1 *)((longlong)param_7 + 0x4c) = 1;
        *(undefined4 *)(param_7 + 10) = 1;
        (**(code **)(*param_7 + 0xa0))(param_7,&local_2a0,0x12);
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x40800000;
        *(undefined1 *)(param_7 + 6) = 1;
        (**(code **)(*param_7 + 200))(param_7);
        local_388 = (int *)local_2f8[0x8e];
        local_360 = (int *)local_2f8[0x8d];
        if (local_360 != local_388) {
          puVar17 = local_2f8 + 0x93;
          plVar26 = local_350 + 8;
          local_340 = plVar26;
          local_338 = puVar17;
          do {
            lVar25 = 0;
            if ((undefined8 *)puVar17[2] == (undefined8 *)0x0) {
LAB_140594b02:
              puVar23 = puVar17;
            }
            else {
              puVar15 = (undefined8 *)puVar17[2];
              puVar23 = puVar17;
              do {
                if (*(int *)(puVar15 + 4) < *local_360) {
                  puVar16 = (undefined8 *)*puVar15;
                }
                else {
                  puVar16 = (undefined8 *)puVar15[1];
                  puVar23 = puVar15;
                }
                puVar15 = puVar16;
              } while (puVar16 != (undefined8 *)0x0);
              if ((puVar23 == puVar17) || (*local_360 < *(int *)(puVar23 + 4))) goto LAB_140594b02;
            }
            puVar20 = (ushort *)puVar23[6];
            puVar30 = (ushort *)puVar23[5];
            if (puVar30 != puVar20) {
              iVar10 = param_1[1];
              puVar3 = (ulonglong *)*plVar26;
              if (iVar10 < 0) {
                iVar10 = 0;
              }
              do {
                uVar29 = (ulonglong)*puVar30 |
                         ((longlong)iVar10 | (longlong)*param_1 << 0x20) << 0x10;
                uVar18 = plVar26[1] - (longlong)puVar3 >> 4;
                puVar28 = puVar3;
                if (0 < (longlong)uVar18) {
                  do {
                    uVar19 = uVar18 >> 1;
                    if (puVar28[uVar19 * 2] < uVar29) {
                      puVar28 = puVar28 + uVar19 * 2 + 2;
                      uVar19 = uVar18 + (-1 - uVar19);
                    }
                    uVar18 = uVar19;
                  } while (0 < (longlong)uVar19);
                  puVar20 = (ushort *)puVar23[6];
                }
                if ((((puVar28 != (ulonglong *)plVar26[1]) && (*puVar28 <= uVar29)) &&
                    (puVar28 != puVar28 + 2)) && ((double)puVar28[1] != 0.0)) {
                  lVar25 = lVar25 + (longlong)(double)puVar28[1];
                }
                puVar30 = puVar30 + 2;
              } while (puVar30 != puVar20);
              plVar26 = local_340;
              puVar17 = local_338;
              if (lVar25 != 0) {
                if ((char)local_360[10] != '\0') {
                  *(undefined1 *)((longlong)local_348 + 0x34) = 1;
                  *(undefined4 *)(local_348 + 7) = 1;
                  (**(code **)(*param_7 + 0xa0))(param_7,local_360 + 2,0x11);
                }
                plVar26 = local_340;
                puVar4 = (undefined4 *)puVar23[6];
                puVar17 = local_338;
                for (puVar24 = (undefined4 *)puVar23[5]; local_338 = puVar17, puVar24 != puVar4;
                    puVar24 = puVar24 + 1) {
                  FUN_1405937c0(param_1,param_2,param_3,local_2f8,param_7,plVar26,*puVar24,param_5);
                  puVar17 = local_338;
                }
              }
            }
            local_360 = local_360 + 0xc;
          } while (local_360 != local_388);
        }
        lVar25 = -1;
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x40800000;
        *(undefined1 *)(param_7 + 6) = 1;
        (**(code **)(*param_7 + 200))(param_7);
        if (*(longlong *)(param_4 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
          abort();
        }
        puVar17 = (undefined8 *)
                  FUN_140572850(&local_2c8,
                                (((longlong)
                                  *(int *)(*(longlong *)(*(longlong *)(param_4 + 600) + 0x410) +
                                          0x6c) + *(longlong *)(local_328 + 0x20)) * 100 +
                                *(longlong *)(local_328 + 0x28)) * 10000);
        local_328 = FUN_1402d82e0("accounting_utc_warn","Accounting time: {}");
        do {
          lVar25 = lVar25 + 1;
        } while (*(char *)(local_328 + lVar25) != '\0');
        local_2f8 = puVar17;
        if (0xf < (ulonglong)puVar17[3]) {
          local_2f8 = (undefined8 *)*puVar17;
        }
        uStack_2f0 = puVar17[2];
        uStack_380 = &local_2f8;
        local_268 = 0;
        local_278 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_270 = local_258;
        local_260 = 500;
        local_388 = (int *)0xd;
        lStack_320 = lVar25;
        FUN_140022830(&local_278,&local_328,&local_388);
        local_2e8 = (void *)0x0;
        uStack_2e0 = 0;
        local_2d8 = 0;
        local_2d0 = 0;
        FUN_140002c00(&local_2e8,local_270);
        local_278 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_270 != local_258) {
          _Memory = local_270;
          if ((0xfff < local_260) &&
             (_Memory = *(undefined1 **)(local_270 + -8),
             (undefined1 *)0x1f < local_270 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(_Memory);
        }
        if (0xf < local_2b0) {
          pvVar5 = (void *)CONCAT71(uStack_2c7,local_2c8);
          pvVar12 = pvVar5;
          if ((0xfff < local_2b0 + 1) &&
             (pvVar12 = *(void **)((longlong)pvVar5 + -8),
             0x1f < (ulonglong)((longlong)pvVar5 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        local_2b8 = 0;
        local_2b0 = 0xf;
        local_2c8 = 0;
        (**(code **)(*param_7 + 0xa0))(param_7,&local_2e8);
        *(undefined4 *)((longlong)param_7 + 0x34) = 0x40800000;
        *(undefined1 *)(param_7 + 6) = 1;
        (**(code **)(*param_7 + 200))(param_7);
        if ((local_358 != (int)local_330) && (local_358 < (int)(local_350[3] - local_350[2] >> 2)))
        {
          *param_1 = *(int *)(local_350[2] + (longlong)local_358 * 4);
        }
        param_1[1] = *local_368;
        if (0xf < local_2d0) {
          pvVar12 = local_2e8;
          if ((0xfff < local_2d0 + 1) &&
             (pvVar12 = *(void **)((longlong)local_2e8 + -8),
             0x1f < (ulonglong)((longlong)local_2e8 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        local_2d8 = 0;
        local_2d0 = 0xf;
        local_2e8 = (void *)((ulonglong)local_2e8 & 0xffffffffffffff00);
        if (0xf < local_288) {
          pvVar5 = (void *)CONCAT71(uStack_29f,local_2a0);
          pvVar12 = pvVar5;
          if ((0xfff < local_288 + 1) &&
             (pvVar12 = *(void **)((longlong)pvVar5 + -8),
             0x1f < (ulonglong)((longlong)pvVar5 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        puVar15 = puStack_310;
        local_290 = 0;
        local_288 = 0xf;
        local_2a0 = 0;
        for (puVar17 = local_318; puVar17 != puVar15; puVar17 = puVar17 + 4) {
          FUN_140002d30(puVar17);
        }
        if (local_318 != (undefined8 *)0x0) {
          free(local_318);
        }
      }
    }
  }
  return;
}


// Incoming references
// 0xc2ebf8 DATA caller none
// 0x5b9e1d UNCONDITIONAL_CALL caller 1405b9bd0
// 0x6574fe UNCONDITIONAL_CALL caller 1406574c0
// 0x7ff56d UNCONDITIONAL_CALL caller 1407fe960
// 0x6bc3ae UNCONDITIONAL_CALL caller 1406bc1e0
// 0x6c1447 UNCONDITIONAL_CALL caller 1406c1270
