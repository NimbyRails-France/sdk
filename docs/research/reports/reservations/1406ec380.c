// Candidate VA 1406ec380; RVA 0x6ec380
// Ghidra inferred prototype: undefined FUN_1406ec380()

void FUN_1406ec380(undefined8 *param_1,longlong *param_2)

{
  ulonglong uVar1;
  double dVar2;
  code *pcVar3;
  size_t sVar4;
  undefined8 uVar5;
  undefined8 *puVar6;
  char ****ppppcVar7;
  longlong *plVar8;
  double *pdVar9;
  longlong lVar10;
  longlong lVar11;
  longlong lVar12;
  undefined8 ****ppppuVar13;
  double *pdVar14;
  undefined1 *puVar15;
  undefined8 ****ppppuVar16;
  longlong lVar17;
  longlong *plVar18;
  double *pdVar19;
  ulonglong uVar20;
  undefined1 local_598 [12];
  undefined4 uStack_58c;
  longlong *local_588;
  undefined8 uStack_580;
  char ***local_578;
  double *pdStack_570;
  ulonglong local_568;
  ulonglong uStack_560;
  undefined8 ***local_558;
  undefined8 uStack_550;
  ulonglong local_548;
  ulonglong uStack_540;
  undefined8 ***local_538;
  longlong lStack_530;
  longlong local_528;
  ulonglong uStack_520;
  undefined8 ***local_518;
  longlong lStack_510;
  longlong local_508;
  ulonglong uStack_500;
  double local_4f8;
  undefined8 local_4f0;
  undefined1 local_4e8 [16];
  undefined1 local_4d8 [16];
  undefined **local_4c8;
  undefined1 *local_4c0;
  undefined8 local_4b8;
  ulonglong local_4b0;
  undefined1 local_4a8 [512];
  undefined **local_2a8;
  undefined1 *local_2a0;
  undefined8 local_298;
  ulonglong local_290;
  undefined1 local_288 [584];
  
  lVar17 = 0;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 3;
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x43700000;
  *(undefined1 *)(param_2 + 5) = 1;
  (**(code **)(*param_2 + 8))(param_2);
  lVar11 = *(longlong *)*param_1;
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
  *(undefined1 *)(param_2 + 6) = 1;
  if (lVar11 == 0) {
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    (**(code **)(*param_2 + 200))(param_2);
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 0xa8))(param_2,"no nearby track",0x11);
  }
  else {
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
    pcVar3 = *(code **)(*param_2 + 0xa0);
    puVar6 = (undefined8 *)FUN_1404875b0(&local_558,*(undefined8 *)*param_1);
    local_598._0_8_ = puVar6;
    if (0xf < (ulonglong)puVar6[3]) {
      local_598._0_8_ = *puVar6;
    }
    uStack_580._0_4_ = (undefined4)puVar6[2];
    uStack_580._4_4_ = (undefined4)((ulonglong)puVar6[2] >> 0x20);
    local_598._8_4_ = (undefined4)uStack_580;
    uStack_58c = uStack_580._4_4_;
    local_588 = (longlong *)0xd;
    uStack_580 = local_598;
    local_578 = (char ***)0x140a957a0;
    pdStack_570 = (double *)0x8;
    FUN_140021e90(&local_4f8,&local_578,&local_588);
    (*pcVar3)(param_2,&local_4f8,0x11);
    FUN_140002d30(&local_4f8);
    FUN_140002d30(&local_558);
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
    *(undefined1 *)(param_2 + 6) = 1;
    pcVar3 = *(code **)(*param_2 + 0xa0);
    local_598._0_8_ = *(undefined8 *)param_1[1];
    local_578 = (char ***)0xa;
    pdStack_570 = (double *)local_598;
    local_588 = (longlong *)0x140a957b0;
    uStack_580 = (undefined1 *)0x8;
    FUN_140021e90(&local_4f8,&local_588,&local_578);
    (*pcVar3)(param_2,&local_4f8,0x11);
    FUN_140002d30(&local_4f8);
  }
  if (*(longlong *)param_1[2] == ((longlong *)param_1[2])[1]) {
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 0xa8))(param_2,"no reservations",0x11);
  }
  plVar18 = *(longlong **)param_1[2];
  local_588 = (longlong *)((longlong *)param_1[2])[1];
  if (plVar18 != local_588) {
    stack0xfffffffffffffa70 = &local_4f8;
    local_598._0_8_ = 0xaa;
    local_578 = (char ***)&DAT_140a95758;
    pdStack_570 = (double *)0x15;
    pdVar19 = (double *)(plVar18 + 3);
    uVar5 = local_4d8._8_8_;
    do {
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
      *(undefined1 *)(param_2 + 6) = 1;
      (**(code **)(*param_2 + 200))(param_2);
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
      *(undefined1 *)(param_2 + 6) = 1;
      pdVar14 = pdVar19 + -1;
      if (pdVar19[-1] < *pdVar19) {
        pdVar14 = pdVar19;
      }
      dVar2 = *(double *)param_1[1];
      pdVar9 = pdVar19 + -1;
      if (*pdVar19 < pdVar19[-1]) {
        pdVar9 = pdVar19;
      }
      if ((*pdVar9 <= dVar2) && (dVar2 < *pdVar14 || dVar2 == *pdVar14)) {
        *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
        *(undefined4 *)(param_2 + 10) = 1;
      }
      pcVar3 = *(code **)(*param_2 + 0xa0);
      local_4f8 = pdVar19[-1];
      local_4f0 = uVar5;
      local_598._0_8_ = *pdVar19;
      local_4e8._8_4_ = local_598._8_4_;
      local_4e8._0_8_ = *pdVar19;
      local_4e8._12_4_ = uStack_58c;
      local_4b8 = 0;
      local_4c8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_4c0 = local_4a8;
      local_4b0 = 500;
      local_578 = (char ***)0xaa;
      local_4d8._8_8_ = 0x15;
      local_4d8._0_8_ = &DAT_140a95758;
      pdStack_570 = &local_4f8;
      FUN_140022830(&local_4c8,local_4d8,&local_578);
      local_558 = (undefined8 ****)0x0;
      uStack_550 = 0;
      local_548 = 0;
      uStack_540 = 0;
      FUN_140002c00(&local_558,local_4c0);
      local_4c8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_4c0 != local_4a8) {
        puVar15 = local_4c0;
        if ((0xfff < local_4b0) &&
           (puVar15 = *(undefined1 **)(local_4c0 + -8),
           (undefined1 *)0x1f < local_4c0 + (-8 - (longlong)puVar15))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(puVar15);
      }
      (*pcVar3)(param_2,&local_558);
      if (0xf < uStack_540) {
        ppppuVar13 = (undefined8 ****)local_558;
        if ((0xfff < uStack_540 + 1) &&
           (ppppuVar13 = (undefined8 ****)local_558[-1],
           0x1f < (ulonglong)((longlong)local_558 + (-8 - (longlong)ppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppppuVar13);
      }
      lVar11 = *plVar18;
      lVar10 = lVar17;
      lVar12 = lVar17;
      if (lVar11 != 0) {
        lVar10 = FUN_14033f780(*(longlong *)param_1[3] + 0x200,lVar11);
        lVar11 = FUN_14045f300(param_1[4],lVar11);
        if (((lVar11 != 0) && (*(char *)(lVar11 + 0x5d0) != '\0')) &&
           (*(longlong *)(lVar11 + 0x5a8) != 0)) {
          lVar12 = FUN_14033f7f0(*(longlong *)param_1[5] + 0x180);
        }
      }
      lStack_530 = 0;
      local_528 = 0;
      uStack_520 = 0xf;
      local_538 = (undefined8 ****)0x0;
      if (lVar10 != 0) {
        ppppuVar13 = (undefined8 ****)(lVar10 + 0x30);
        if (&local_538 != ppppuVar13) {
          if (0xf < *(ulonglong *)(lVar10 + 0x48)) {
            ppppuVar13 = (undefined8 ****)*ppppuVar13;
          }
          FUN_140030630(&local_538,ppppuVar13);
        }
        if (lVar12 != 0) {
          puVar6 = (undefined8 *)(lVar12 + 0x78);
          sVar4 = *(size_t *)(lVar12 + 0x88);
          if (0x7fffffffffffffff - sVar4 < 3) {
                    /* WARNING: Subroutine does not return */
            FUN_140001c70();
          }
          if (0xf < *(ulonglong *)(lVar12 + 0x90)) {
            puVar6 = (undefined8 *)*puVar6;
          }
          local_578 = (char ***)0x0;
          pdStack_570 = (double *)0x0;
          local_568 = 0;
          uStack_560 = 0;
          uVar1 = sVar4 + 3;
          uVar20 = 0xf;
          ppppcVar7 = &local_578;
          if (0xf < uVar1) {
            uVar20 = uVar1 | 0xf;
            if (uVar20 < 0x8000000000000000) {
              if (uVar20 < 0x16) {
                uVar20 = 0x16;
              }
            }
            else {
              uVar20 = 0x7fffffffffffffff;
            }
            ppppcVar7 = (char ****)FUN_140003270(uVar20 + 1);
            local_578 = (char ***)ppppcVar7;
          }
          local_568 = uVar1;
          uStack_560 = uVar20;
          memcpy(ppppcVar7,puVar6,sVar4);
          *(short *)((longlong)ppppcVar7 + sVar4) = (short)DAT_140a6e030;
          *(char *)((longlong)ppppcVar7 + sVar4 + 2) = (char)((uint)DAT_140a6e030 >> 0x10);
          *(undefined1 *)((longlong)ppppcVar7 + uVar1) = 0;
          ppppuVar13 = &local_538;
          if (0xf < uStack_520) {
            ppppuVar13 = (undefined8 ****)local_538;
          }
          plVar8 = (longlong *)FUN_1400254f0(&local_578,ppppuVar13);
          ppppuVar13 = (undefined8 ****)*plVar8;
          lVar11 = plVar8[1];
          lVar10 = plVar8[2];
          uVar1 = plVar8[3];
          *(undefined1 *)plVar8 = 0;
          plVar8[2] = 0;
          plVar8[3] = 0xf;
          if (0xf < uStack_520) {
            ppppuVar16 = (undefined8 ****)local_538;
            if ((0xfff < uStack_520 + 1) &&
               (ppppuVar16 = (undefined8 ****)local_538[-1],
               0x1f < (ulonglong)((longlong)local_538 + (-8 - (longlong)ppppuVar16)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppuVar16);
          }
          local_538 = ppppuVar13;
          lStack_530 = lVar11;
          local_528 = lVar10;
          uStack_520 = uVar1;
          if (0xf < uStack_560) {
            ppppcVar7 = (char ****)local_578;
            if ((0xfff < uStack_560 + 1) &&
               (ppppcVar7 = (char ****)local_578[-1],
               (undefined *)0x1f < (undefined *)((longlong)local_578 + (-8 - (longlong)ppppcVar7))))
            {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppcVar7);
          }
        }
        if (local_528 != 0) {
          *(undefined1 *)(param_2 + 3) = 1;
          *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
          *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
          *(undefined1 *)(param_2 + 6) = 1;
          (**(code **)(*param_2 + 0xa0))(param_2,&local_538);
        }
      }
      if (0xf < uStack_520) {
        ppppuVar13 = (undefined8 ****)local_538;
        if ((0xfff < uStack_520 + 1) &&
           (ppppuVar13 = (undefined8 ****)local_538[-1],
           0x1f < (ulonglong)((longlong)local_538 + (-8 - (longlong)ppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppppuVar13);
      }
      plVar18 = plVar18 + 4;
      pdVar19 = pdVar19 + 4;
    } while (plVar18 != local_588);
  }
  if (*(longlong *)param_1[6] == ((longlong *)param_1[6])[1]) {
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 0xa8))(param_2,"no occupancy",0x11);
  }
  plVar18 = *(longlong **)param_1[6];
  local_588 = (longlong *)((longlong *)param_1[6])[1];
  if (plVar18 != local_588) {
    local_578 = (char ***)0xaa;
    stack0xfffffffffffffa70 = 0x14;
    local_598._0_8_ = &DAT_140a95780;
    pdVar19 = (double *)(plVar18 + 2);
    uVar5 = local_4d8._8_8_;
    pdStack_570 = &local_4f8;
    do {
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
      *(undefined1 *)(param_2 + 6) = 1;
      (**(code **)(*param_2 + 200))(param_2);
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
      *(undefined1 *)(param_2 + 6) = 1;
      pdVar14 = pdVar19 + -1;
      if (pdVar19[-1] < *pdVar19) {
        pdVar14 = pdVar19;
      }
      dVar2 = *(double *)param_1[1];
      pdVar9 = pdVar19 + -1;
      if (*pdVar19 < pdVar19[-1]) {
        pdVar9 = pdVar19;
      }
      if ((*pdVar9 <= dVar2) && (dVar2 < *pdVar14 || dVar2 == *pdVar14)) {
        *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
        *(undefined4 *)(param_2 + 10) = 1;
      }
      pcVar3 = *(code **)(*param_2 + 0xa0);
      local_4f8 = pdVar19[-1];
      local_4f0 = uVar5;
      local_4d8._0_8_ = *pdVar19;
      local_4e8._8_4_ = local_4d8._8_4_;
      local_4e8._0_8_ = *pdVar19;
      local_4e8._12_4_ = local_4d8._12_4_;
      local_298 = 0;
      local_2a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_2a0 = local_288;
      local_290 = 500;
      stack0xfffffffffffffa70 = &local_4f8;
      local_598._0_8_ = 0xaa;
      local_578 = (char ***)&DAT_140a95780;
      pdStack_570 = (double *)0x14;
      FUN_140022830(&local_2a8,&local_578,local_598);
      local_558 = (undefined8 ****)0x0;
      uStack_550 = 0;
      local_548 = 0;
      uStack_540 = 0;
      FUN_140002c00(&local_558,local_2a0);
      local_2a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_2a0 != local_288) {
        puVar15 = local_2a0;
        if ((0xfff < local_290) &&
           (puVar15 = *(undefined1 **)(local_2a0 + -8),
           (undefined1 *)0x1f < local_2a0 + (-8 - (longlong)puVar15))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(puVar15);
      }
      (*pcVar3)(param_2,&local_558);
      if (0xf < uStack_540) {
        ppppuVar13 = (undefined8 ****)local_558;
        if ((0xfff < uStack_540 + 1) &&
           (ppppuVar13 = (undefined8 ****)local_558[-1],
           0x1f < (ulonglong)((longlong)local_558 + (-8 - (longlong)ppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppppuVar13);
      }
      lVar11 = *plVar18;
      lVar10 = lVar17;
      lVar12 = lVar17;
      if (lVar11 != 0) {
        lVar10 = FUN_14033f780(*(longlong *)param_1[3] + 0x200,lVar11);
        lVar11 = FUN_14045f300(param_1[4],lVar11);
        if (((lVar11 != 0) && (*(char *)(lVar11 + 0x5d0) != '\0')) &&
           (*(longlong *)(lVar11 + 0x5a8) != 0)) {
          lVar12 = FUN_14033f7f0(*(longlong *)param_1[5] + 0x180);
        }
      }
      lStack_510 = 0;
      local_508 = 0;
      uStack_500 = 0xf;
      local_518 = (undefined8 ****)0x0;
      if (lVar10 != 0) {
        ppppuVar13 = (undefined8 ****)(lVar10 + 0x30);
        if (&local_518 != ppppuVar13) {
          if (0xf < *(ulonglong *)(lVar10 + 0x48)) {
            ppppuVar13 = (undefined8 ****)*ppppuVar13;
          }
          FUN_140030630(&local_518,ppppuVar13);
        }
        if (lVar12 != 0) {
          puVar6 = (undefined8 *)(lVar12 + 0x78);
          sVar4 = *(size_t *)(lVar12 + 0x88);
          if (0x7fffffffffffffff - sVar4 < 3) {
                    /* WARNING: Subroutine does not return */
            FUN_140001c70();
          }
          if (0xf < *(ulonglong *)(lVar12 + 0x90)) {
            puVar6 = (undefined8 *)*puVar6;
          }
          local_558 = (undefined8 ****)0x0;
          uStack_550 = 0;
          local_548 = 0;
          uStack_540 = 0;
          uVar1 = sVar4 + 3;
          uVar20 = 0xf;
          ppppuVar13 = &local_558;
          if (0xf < uVar1) {
            uVar20 = uVar1 | 0xf;
            if (uVar20 < 0x8000000000000000) {
              if (uVar20 < 0x16) {
                uVar20 = 0x16;
              }
            }
            else {
              uVar20 = 0x7fffffffffffffff;
            }
            ppppuVar13 = (undefined8 ****)FUN_140003270(uVar20 + 1);
            local_558 = ppppuVar13;
          }
          local_548 = uVar1;
          uStack_540 = uVar20;
          memcpy(ppppuVar13,puVar6,sVar4);
          *(short *)((longlong)ppppuVar13 + sVar4) = (short)DAT_140a6e030;
          *(char *)((longlong)ppppuVar13 + sVar4 + 2) = (char)((uint)DAT_140a6e030 >> 0x10);
          *(undefined1 *)((longlong)ppppuVar13 + uVar1) = 0;
          ppppuVar13 = &local_518;
          if (0xf < uStack_500) {
            ppppuVar13 = (undefined8 ****)local_518;
          }
          plVar8 = (longlong *)FUN_1400254f0(&local_558,ppppuVar13);
          ppppuVar13 = (undefined8 ****)*plVar8;
          lVar11 = plVar8[1];
          lVar10 = plVar8[2];
          uVar1 = plVar8[3];
          *(undefined1 *)plVar8 = 0;
          plVar8[2] = 0;
          plVar8[3] = 0xf;
          if (0xf < uStack_500) {
            ppppuVar16 = (undefined8 ****)local_518;
            if ((0xfff < uStack_500 + 1) &&
               (ppppuVar16 = (undefined8 ****)local_518[-1],
               0x1f < (ulonglong)((longlong)local_518 + (-8 - (longlong)ppppuVar16)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppuVar16);
          }
          local_518 = ppppuVar13;
          lStack_510 = lVar11;
          local_508 = lVar10;
          uStack_500 = uVar1;
          if (0xf < uStack_540) {
            ppppuVar13 = (undefined8 ****)local_558;
            if ((0xfff < uStack_540 + 1) &&
               (ppppuVar13 = (undefined8 ****)local_558[-1],
               0x1f < (ulonglong)((longlong)local_558 + (-8 - (longlong)ppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppuVar13);
          }
        }
        if (local_508 != 0) {
          *(undefined1 *)(param_2 + 3) = 1;
          *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
          *(undefined4 *)((longlong)param_2 + 0x34) = 0x41900000;
          *(undefined1 *)(param_2 + 6) = 1;
          (**(code **)(*param_2 + 0xa0))(param_2,&local_518);
        }
      }
      if (0xf < uStack_500) {
        ppppuVar13 = (undefined8 ****)local_518;
        if ((0xfff < uStack_500 + 1) &&
           (ppppuVar13 = (undefined8 ****)local_518[-1],
           0x1f < (ulonglong)((longlong)local_518 + (-8 - (longlong)ppppuVar13)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppppuVar13);
      }
      plVar18 = plVar18 + 4;
      pdVar19 = pdVar19 + 4;
    } while (plVar18 != local_588);
  }
                    /* WARNING: Could not recover jumptable at 0x0001406ed18d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x18))(param_2);
  return;
}


// Incoming references
// 0xc35540 DATA caller none
// 0x6ef4e7 UNCONDITIONAL_CALL caller 1406ef150
// 0x6efb2f UNCONDITIONAL_CALL caller 1406ef150
