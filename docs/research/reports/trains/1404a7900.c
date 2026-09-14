// Candidate VA 1404a7900; RVA 0x4a7900
// Ghidra inferred prototype: undefined FUN_1404a7900()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1404a7900(longlong *param_1,longlong param_2)

{
  ulonglong uVar1;
  int iVar2;
  undefined8 *****pppppuVar3;
  undefined8 *****pppppuVar4;
  char ******ppppppcVar5;
  longlong lVar6;
  undefined8 ******ppppppuVar7;
  ushort *puVar8;
  ushort *puVar9;
  undefined8 uVar10;
  ushort *puVar11;
  undefined1 *puVar12;
  undefined8 *puVar13;
  ulonglong uVar14;
  int iVar15;
  ulonglong *puVar16;
  longlong lVar17;
  ulonglong uVar18;
  ushort *puVar19;
  int iVar20;
  ushort *puVar21;
  double dVar22;
  undefined4 local_res18;
  undefined8 in_stack_fffffffffffff908;
  undefined4 uVar23;
  char *****local_6e8;
  undefined8 uStack_6e0;
  undefined8 local_6d8;
  ulonglong local_6d0;
  int local_6c8;
  double local_6c0;
  int local_6b8;
  undefined8 *****local_6b0;
  undefined8 ****ppppuStack_6a8;
  undefined8 ****local_6a0;
  undefined8 ****ppppuStack_698;
  undefined8 *****local_690;
  undefined8 uStack_688;
  undefined8 local_680;
  ulonglong uStack_678;
  int local_670;
  longlong local_668;
  undefined8 *****local_660;
  undefined8 uStack_658;
  undefined8 ****local_650;
  undefined8 ****ppppuStack_648;
  undefined8 *****local_640;
  undefined8 uStack_638;
  undefined8 ****local_630;
  undefined8 ****ppppuStack_628;
  undefined8 *****local_620 [2];
  undefined8 local_610;
  ulonglong local_608;
  int *local_600;
  undefined8 *****local_5f8;
  undefined8 uStack_5f0;
  undefined8 ****local_5e8;
  undefined8 ****ppppuStack_5e0;
  undefined8 *****local_5d8;
  undefined8 uStack_5d0;
  undefined8 ****local_5c8;
  undefined8 ****ppppuStack_5c0;
  ushort *local_5b8;
  longlong *plStack_5b0;
  int *local_5a8;
  ulonglong *local_5a0;
  ulonglong *local_598;
  undefined8 local_588;
  longlong *plStack_580;
  undefined *local_578;
  undefined8 uStack_570;
  undefined4 local_568;
  undefined4 uStack_564;
  undefined4 uStack_560;
  undefined4 uStack_55c;
  int *local_558;
  int *local_550;
  longlong local_540 [2];
  char *****local_530;
  undefined8 local_528;
  undefined8 *****local_520;
  undefined8 ****local_518;
  undefined8 *****local_510;
  undefined8 ****local_508;
  undefined8 *local_500;
  undefined8 local_4f8;
  undefined8 *****local_4f0;
  undefined8 local_4e8;
  undefined8 *****local_4e0;
  undefined8 ****local_4d8;
  undefined8 *****local_4d0;
  undefined8 ****local_4c8;
  undefined8 *****local_4c0;
  undefined8 ****local_4b8;
  undefined1 local_4b0 [8];
  undefined **local_4a8;
  undefined1 *local_4a0;
  undefined8 local_498;
  ulonglong local_490;
  undefined1 local_488 [512];
  undefined **local_288;
  undefined1 *local_280;
  undefined8 local_278;
  ulonglong local_270;
  undefined1 local_268 [552];
  
  pppppuVar4 = _UNK_140aac918;
  pppppuVar3 = _DAT_140aac910;
  iVar15 = 0;
  uStack_658 = 0;
  local_650 = _DAT_140aac910;
  ppppuStack_648 = _UNK_140aac918;
  local_660 = (undefined8 ******)0x0;
  uStack_638 = 0;
  local_630 = _DAT_140aac910;
  ppppuStack_628 = _UNK_140aac918;
  local_640 = (undefined8 ******)0x0;
  uStack_6e0 = 0;
  local_6d8 = 0;
  local_6d0 = 0xf;
  local_6e8 = (char *****)0x0;
  uStack_5d0 = 0;
  local_5c8 = _DAT_140aac910;
  ppppuStack_5c0 = _UNK_140aac918;
  local_5d8 = (undefined8 ******)0x0;
  uStack_5f0 = 0;
  local_5e8 = _DAT_140aac910;
  ppppuStack_5e0 = _UNK_140aac918;
  local_5f8 = (undefined8 ******)0x0;
  ppppuStack_6a8 = (undefined8 *****)0x0;
  local_6a0 = _DAT_140aac910;
  ppppuStack_698 = _UNK_140aac918;
  local_6b0 = (undefined8 ******)0x0;
  local_res18 = 0;
  lVar17 = *param_1;
  if (param_2 == 0) {
    puVar13 = (undefined8 *)(*(longlong *)(lVar17 + 0x410) + 0x20);
    if (0xf < *(ulonglong *)(*(longlong *)(lVar17 + 0x410) + 0x38)) {
      puVar13 = (undefined8 *)*puVar13;
    }
    lVar17 = -1;
    do {
      lVar17 = lVar17 + 1;
    } while (*(char *)((longlong)puVar13 + lVar17) != '\0');
    FUN_140030630(&local_660);
    uVar14 = local_6d0;
    if (local_6d0 < 7) {
      uVar18 = 0x7fffffffffffffff;
      if (local_6d0 <= 0x7fffffffffffffff - (local_6d0 >> 1)) {
        uVar1 = (local_6d0 >> 1) + local_6d0;
        uVar18 = 0xf;
        if (0xf < uVar1) {
          uVar18 = uVar1;
        }
      }
      ppppppcVar5 = (char ******)FUN_140003270(uVar18 + 1);
      local_6d8 = 7;
      *(undefined4 *)ppppppcVar5 = s_company_140a75cf8._0_4_;
      *(undefined2 *)((longlong)ppppppcVar5 + 4) = s_company_140a75cf8._4_2_;
      *(char *)((longlong)ppppppcVar5 + 6) = s_company_140a75cf8[6];
      *(char *)((longlong)ppppppcVar5 + 7) = '\0';
      local_6d0 = uVar18;
      if (0xf < uVar14) {
        FUN_140003040(&local_6e8,local_6e8);
      }
    }
    else {
      ppppppcVar5 = &local_6e8;
      if (0xf < local_6d0) {
        ppppppcVar5 = (char ******)local_6e8;
      }
      local_6d8 = 7;
      memmove(ppppppcVar5,"company",7);
      *(char *)((longlong)ppppppcVar5 + 7) = '\0';
      ppppppcVar5 = (char ******)local_6e8;
    }
    local_6e8 = (char *****)ppppppcVar5;
    local_res18 = *(undefined4 *)(*(longlong *)(*param_1 + 0x410) + 0x40);
    iVar15 = 0;
  }
  else {
    lVar6 = FUN_1403ca140(lVar17 + 0x430);
    if (lVar6 == 0) {
      lVar6 = FUN_14033f7f0(*(longlong *)(lVar17 + 0x780) + 0x180);
      if (lVar6 == 0) {
        lVar17 = FUN_14033f780(*(longlong *)(lVar17 + 0x890) + 0x200,param_2);
        if (lVar17 == 0) goto LAB_1404a86a2;
        ppppppuVar7 = (undefined8 ******)(lVar17 + 0x10);
        if (&local_660 != ppppppuVar7) {
          if (0xf < *(ulonglong *)(lVar17 + 0x28)) {
            ppppppuVar7 = (undefined8 ******)*ppppppuVar7;
          }
          FUN_140030630(&local_660,ppppppuVar7,*(undefined8 *)(lVar17 + 0x20));
        }
        ppppppuVar7 = (undefined8 ******)(lVar17 + 0x30);
        if (&local_640 != ppppppuVar7) {
          if (0xf < *(ulonglong *)(lVar17 + 0x48)) {
            ppppppuVar7 = (undefined8 ******)*ppppppuVar7;
          }
          FUN_140030630(&local_640,ppppppuVar7,*(undefined8 *)(lVar17 + 0x40));
        }
        uVar14 = local_6d0;
        if (local_6d0 < 5) {
          uVar18 = 0x7fffffffffffffff;
          if (local_6d0 <= 0x7fffffffffffffff - (local_6d0 >> 1)) {
            uVar1 = local_6d0 + (local_6d0 >> 1);
            uVar18 = 0xf;
            if (0xf < uVar1) {
              uVar18 = uVar1;
            }
          }
          ppppppcVar5 = (char ******)FUN_140003270(uVar18 + 1);
          local_6d8 = 5;
          *(undefined4 *)ppppppcVar5 = s_train_140a7386c._0_4_;
          *(char *)((longlong)ppppppcVar5 + 4) = s_train_140a7386c[4];
          *(char *)((longlong)ppppppcVar5 + 5) = '\0';
          local_6d0 = uVar18;
          if (0xf < uVar14) {
            FUN_140003040(&local_6e8,local_6e8,uVar14);
          }
        }
        else {
          ppppppcVar5 = &local_6e8;
          if (0xf < local_6d0) {
            ppppppcVar5 = (char ******)local_6e8;
          }
          local_6d8 = 5;
          memmove(ppppppcVar5,"train",5);
          *(char *)((longlong)ppppppcVar5 + 5) = '\0';
          ppppppcVar5 = (char ******)local_6e8;
        }
        local_6e8 = (char *****)ppppppcVar5;
        local_res18 = *(undefined4 *)(lVar17 + 8);
        iVar15 = 2;
        puVar13 = (undefined8 *)FUN_14044ffe0(param_1[1] + 0xa0,*param_1 + 0x400);
        if (puVar13 != (undefined8 *)0x0) {
          uVar10 = FUN_14028fc90(local_620,*puVar13);
          FUN_140025630(&local_5d8,uVar10);
          iVar15 = 2;
          goto LAB_1404a7fc2;
        }
      }
      else {
        ppppppuVar7 = (undefined8 ******)(lVar6 + 0x58);
        if (&local_660 != ppppppuVar7) {
          if (0xf < *(ulonglong *)(lVar6 + 0x70)) {
            ppppppuVar7 = (undefined8 ******)*ppppppuVar7;
          }
          FUN_140030630(&local_660,ppppppuVar7);
        }
        ppppppuVar7 = (undefined8 ******)(lVar6 + 0x78);
        if (&local_640 != ppppppuVar7) {
          if (0xf < *(ulonglong *)(lVar6 + 0x90)) {
            ppppppuVar7 = (undefined8 ******)*ppppppuVar7;
          }
          FUN_140030630(&local_640,ppppppuVar7);
        }
        uVar14 = local_6d0;
        if (local_6d0 < 4) {
          uVar18 = 0x7fffffffffffffff;
          if (local_6d0 <= 0x7fffffffffffffff - (local_6d0 >> 1)) {
            uVar1 = (local_6d0 >> 1) + local_6d0;
            uVar18 = 0xf;
            if (0xf < uVar1) {
              uVar18 = uVar1;
            }
          }
          ppppppcVar5 = (char ******)FUN_140003270(uVar18 + 1);
          local_6d8 = 4;
          *(undefined4 *)ppppppcVar5 = 0x656e696c;
          *(char *)((longlong)ppppppcVar5 + 4) = '\0';
          local_6d0 = uVar18;
          if (0xf < uVar14) {
            FUN_140003040(&local_6e8,local_6e8);
          }
          local_res18 = *(undefined4 *)(lVar6 + 8);
          iVar15 = 3;
          local_6e8 = (char *****)ppppppcVar5;
        }
        else {
          ppppppcVar5 = &local_6e8;
          if (0xf < local_6d0) {
            ppppppcVar5 = (char ******)local_6e8;
          }
          local_6d8 = 4;
          *(undefined4 *)ppppppcVar5 = 0x656e696c;
          *(char *)((longlong)ppppppcVar5 + 4) = '\0';
          local_res18 = *(undefined4 *)(lVar6 + 8);
          iVar15 = 3;
        }
      }
    }
    else {
      lVar17 = FUN_14032c3b0(*(longlong *)(lVar17 + 0x428) + 0x80);
      if (lVar17 != 0) {
        ppppppuVar7 = (undefined8 ******)(lVar6 + 8);
        if (*(char *)(lVar17 + 0x40) == '\0') {
          ppppppuVar7 = (undefined8 ******)(lVar17 + 0x20);
        }
        if (&local_660 != ppppppuVar7) {
          if ((undefined8 *****)0xf < ppppppuVar7[3]) {
            ppppppuVar7 = (undefined8 ******)*ppppppuVar7;
          }
          FUN_140030630(&local_660,ppppppuVar7);
        }
        uVar14 = local_6d0;
        if (local_6d0 < 7) {
          uVar18 = 0x7fffffffffffffff;
          if (local_6d0 <= 0x7fffffffffffffff - (local_6d0 >> 1)) {
            uVar1 = local_6d0 + (local_6d0 >> 1);
            uVar18 = 0xf;
            if (0xf < uVar1) {
              uVar18 = uVar1;
            }
          }
          ppppppcVar5 = (char ******)FUN_140003270(uVar18 + 1);
          local_6d8 = 7;
          *(undefined4 *)ppppppcVar5 = s_station_140a75cf0._0_4_;
          *(undefined2 *)((longlong)ppppppcVar5 + 4) = s_station_140a75cf0._4_2_;
          *(char *)((longlong)ppppppcVar5 + 6) = s_station_140a75cf0[6];
          *(char *)((longlong)ppppppcVar5 + 7) = '\0';
          local_6d0 = uVar18;
          if (0xf < uVar14) {
            FUN_140003040(&local_6e8,local_6e8);
          }
        }
        else {
          ppppppcVar5 = &local_6e8;
          if (0xf < local_6d0) {
            ppppppcVar5 = (char ******)local_6e8;
          }
          local_6d8 = 7;
          memmove(ppppppcVar5,"station",7);
          *(char *)((longlong)ppppppcVar5 + 7) = '\0';
          ppppppcVar5 = (char ******)local_6e8;
        }
        local_6e8 = (char *****)ppppppcVar5;
        local_res18 = *(undefined4 *)(lVar17 + 8);
        dVar22 = exp(*(double *)(lVar17 + 0x18) * DAT_140aab8f0);
        dVar22 = atan(dVar22);
        dVar22 = dVar22 * DAT_140aac2c0 - DAT_140aac298;
        local_6c0 = (double)(int)(*(double *)(lVar17 + 0x10) * DAT_140aabf88) * DAT_140aab8d8;
        uVar10 = FUN_1404b71e0(local_620,&local_6c0);
        FUN_140025630(&local_5f8,uVar10);
        FUN_140002d30(local_620);
        local_6c0 = (double)(int)dVar22 * DAT_140aab8d8;
        ppppppuVar7 = (undefined8 ******)FUN_1404b71e0(local_620,&local_6c0);
        iVar15 = 1;
        if (&local_6b0 != ppppppuVar7) {
          if ((undefined8 *****)0xf < ppppuStack_698) {
            FUN_140003040(&local_6b0,local_6b0);
          }
          local_6b0 = *ppppppuVar7;
          ppppuStack_6a8 = ppppppuVar7[1];
          local_6a0 = ppppppuVar7[2];
          ppppuStack_698 = ppppppuVar7[3];
          ppppppuVar7[2] = (undefined8 *****)0x0;
          ppppppuVar7[3] = (undefined8 *****)0xf;
          *(undefined1 *)ppppppuVar7 = 0;
        }
LAB_1404a7fc2:
        FUN_140002d30(local_620);
      }
    }
  }
  local_6b8 = 0;
  lVar17 = *(longlong *)param_1[2];
  if (0 < (int)(((longlong *)param_1[2])[1] - lVar17 >> 2)) {
    local_668 = 0;
    local_6c0 = 0.0;
    do {
      lVar6 = local_668;
      iVar20 = *(int *)((longlong)local_6c0 + lVar17);
      local_6c8 = iVar20;
      FUN_14047e140(param_1[1],&local_558,*(undefined8 *)(*param_1 + 0x410),iVar20);
      local_600 = local_558;
      local_5a8 = local_550;
      if (local_558 != local_550) {
        do {
          uVar23 = (undefined4)((ulonglong)in_stack_fffffffffffff908 >> 0x20);
          iVar2 = *local_600;
          local_670 = iVar2;
          FUN_14049a4c0(local_620,iVar20,iVar2,local_res18);
          in_stack_fffffffffffff908 = CONCAT44(uVar23,iVar2);
          FUN_14047e440(param_1[1],&local_5a0,param_2,iVar20,in_stack_fffffffffffff908);
          if (local_5a0 == local_598) {
            if (local_5a0 != (ulonglong *)0x0) {
              free(local_5a0);
            }
            if (0xf < local_608) {
              ppppppuVar7 = (undefined8 ******)local_620[0];
              if ((0xfff < local_608 + 1) &&
                 (ppppppuVar7 = (undefined8 ******)local_620[0][-1],
                 0x1f < (ulonglong)((longlong)local_620[0] + (-8 - (longlong)ppppppuVar7)))) {
LAB_1404a8764:
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(ppppppuVar7);
            }
          }
          else {
            lVar17 = param_1[3];
            puVar13 = (undefined8 *)(local_668 + *(longlong *)param_1[4]);
            local_530 = (char *****)&local_6e8;
            if (0xf < local_6d0) {
              local_530 = local_6e8;
            }
            local_520 = &local_660;
            if ((undefined8 *****)0xf < ppppuStack_648) {
              local_520 = local_660;
            }
            local_510 = &local_640;
            if ((undefined8 *****)0xf < ppppuStack_628) {
              local_510 = local_640;
            }
            local_500 = puVar13;
            if (0xf < (ulonglong)puVar13[3]) {
              local_500 = (undefined8 *)*puVar13;
            }
            local_4f8 = puVar13[2];
            local_4f0 = local_620;
            if (0xf < local_608) {
              local_4f0 = local_620[0];
            }
            local_4e0 = &local_5d8;
            if ((undefined8 *****)0xf < ppppuStack_5c0) {
              local_4e0 = local_5d8;
            }
            local_4d0 = &local_5f8;
            if ((undefined8 *****)0xf < ppppuStack_5e0) {
              local_4d0 = local_5f8;
            }
            local_4c0 = &local_6b0;
            if ((undefined8 *****)0xf < ppppuStack_698) {
              local_4c0 = local_6b0;
            }
            local_528 = local_6d8;
            local_518 = local_650;
            local_508 = local_630;
            local_4e8 = local_610;
            local_4d8 = local_5c8;
            local_4c8 = local_5e8;
            local_4b8 = local_6a0;
            local_588 = 0xdddddddd3;
            plStack_5b0 = local_540;
            local_578 = &DAT_140a75d08;
            uStack_570 = 0x1a;
            local_498 = 0;
            local_4a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            local_4a0 = local_488;
            local_490 = 500;
            local_5b8 = (ushort *)0xdddddddd3;
            local_568 = 0x40a75d08;
            uStack_564 = 1;
            uStack_560 = 0x1a;
            uStack_55c = 0;
            plStack_580 = plStack_5b0;
            local_540[0] = param_2;
            FUN_140022830(&local_4a8,&local_568,&local_5b8);
            local_690 = (undefined8 ******)0x0;
            uStack_688 = 0;
            local_680 = 0;
            uStack_678 = 0;
            FUN_140002c00(&local_690,local_4a0);
            local_4a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            if (local_4a0 != local_488) {
              puVar12 = local_4a0;
              if ((0xfff < local_490) &&
                 (puVar12 = *(undefined1 **)(local_4a0 + -8),
                 (undefined1 *)0x1f < local_4a0 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(puVar12);
            }
            ppppppuVar7 = &local_690;
            if (0xf < uStack_678) {
              ppppppuVar7 = (undefined8 ******)local_690;
            }
            FUN_1402e46d0(lVar17,ppppppuVar7);
            if (0xf < uStack_678) {
              ppppppuVar7 = (undefined8 ******)local_690;
              if ((0xfff < uStack_678 + 1) &&
                 (ppppppuVar7 = (undefined8 ******)local_690[-1],
                 0x1f < (ulonglong)((longlong)local_690 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(ppppppuVar7);
            }
            puVar19 = *(ushort **)(param_1[5] + 0x480);
            local_5b8 = *(ushort **)(param_1[5] + 0x488);
            if (puVar19 != local_5b8) {
              puVar21 = puVar19 + 0x28;
              do {
                puVar8 = *(ushort **)(puVar21 + 8);
                puVar11 = puVar21;
                if (puVar8 == (ushort *)0x0) {
LAB_1404a86f7:
                  lVar17 = param_1[3];
LAB_1404a8702:
                  FUN_1402ccc90(lVar17,&DAT_140a75d00);
                }
                else {
                  do {
                    puVar9 = puVar8;
                    if (iVar15 <= *(int *)(puVar8 + 0x10)) {
                      puVar9 = puVar8 + 4;
                      puVar11 = puVar8;
                    }
                    puVar8 = *(ushort **)puVar9;
                  } while (puVar8 != (ushort *)0x0);
                  if ((puVar11 == puVar21) || (iVar15 < *(int *)(puVar11 + 0x10)))
                  goto LAB_1404a86f7;
                  iVar20 = local_670;
                  if (local_670 < 0) {
                    iVar20 = 0;
                  }
                  uVar18 = ((longlong)iVar20 | (longlong)local_6c8 << 0x20) << 0x10 |
                           (ulonglong)*puVar19;
                  uVar14 = (longlong)local_598 - (longlong)local_5a0 >> 4;
                  puVar16 = local_5a0;
                  while (uVar1 = uVar14, 0 < (longlong)uVar1) {
                    uVar14 = uVar1 >> 1;
                    if (puVar16[uVar14 * 2] < uVar18) {
                      puVar16 = puVar16 + uVar14 * 2 + 2;
                      uVar14 = uVar1 + (-1 - uVar14);
                    }
                  }
                  if (((puVar16 == local_598) || (uVar18 < *puVar16)) || (puVar16 == puVar16 + 2)) {
                    puVar16 = local_598;
                  }
                  lVar17 = param_1[3];
                  if (puVar16 == local_598) goto LAB_1404a8702;
                  uVar10 = FUN_1402ccc90(lVar17,&DAT_140a75d00);
                  local_278 = 0;
                  local_288 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable
                  ;
                  local_280 = local_268;
                  local_270 = 500;
                  FUN_140026a80(local_4b0,&local_288,puVar16[1]);
                  local_690 = (undefined8 ******)0x0;
                  uStack_688 = 0;
                  local_680 = 0;
                  uStack_678 = 0;
                  FUN_140002c00(&local_690,local_280);
                  local_288 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable
                  ;
                  if (local_280 != local_268) {
                    puVar12 = local_280;
                    if ((0xfff < local_270) &&
                       (puVar12 = *(undefined1 **)(local_280 + -8),
                       (undefined1 *)0x1f < local_280 + (-8 - (longlong)puVar12))) {
                    /* WARNING: Subroutine does not return */
                      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
                    }
                    free(puVar12);
                  }
                  ppppppuVar7 = &local_690;
                  if (0xf < uStack_678) {
                    ppppppuVar7 = (undefined8 ******)local_690;
                  }
                  FUN_1402e46d0(uVar10,ppppppuVar7);
                  if (0xf < uStack_678) {
                    ppppppuVar7 = (undefined8 ******)local_690;
                    if ((0xfff < uStack_678 + 1) &&
                       (ppppppuVar7 = (undefined8 ******)local_690[-1],
                       0x1f < (ulonglong)((longlong)local_690 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
                      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
                    }
                    free(ppppppuVar7);
                  }
                }
                puVar19 = puVar19 + 0x44;
                puVar21 = puVar21 + 0x44;
              } while (puVar19 != local_5b8);
            }
            std::basic_ostream<char,std::char_traits<char>_>::operator<<
                      ((basic_ostream<char,std::char_traits<char>_> *)param_1[3],FUN_1402cce70);
            if (local_5a0 != (ulonglong *)0x0) {
              free(local_5a0);
            }
            iVar20 = local_6c8;
            if (0xf < local_608) {
              ppppppuVar7 = (undefined8 ******)local_620[0];
              if ((0xfff < local_608 + 1) &&
                 (ppppppuVar7 = (undefined8 ******)local_620[0][-1],
                 0x1f < (ulonglong)((longlong)local_620[0] + (-8 - (longlong)ppppppuVar7))))
              goto LAB_1404a8764;
              free(ppppppuVar7);
              iVar20 = local_6c8;
            }
          }
          local_600 = local_600 + 1;
          lVar6 = local_668;
        } while (local_600 != local_5a8);
      }
      if (local_558 != (int *)0x0) {
        free(local_558);
      }
      local_6b8 = local_6b8 + 1;
      local_6c0 = (double)((longlong)local_6c0 + 4);
      local_668 = lVar6 + 0x20;
      lVar17 = *(longlong *)param_1[2];
    } while (local_6b8 < (int)(((longlong *)param_1[2])[1] - lVar17 >> 2));
  }
LAB_1404a86a2:
  if ((undefined8 *****)0xf < ppppuStack_698) {
    ppppppuVar7 = (undefined8 ******)local_6b0;
    if ((0xfff < (longlong)ppppuStack_698 + 1U) &&
       (ppppppuVar7 = (undefined8 ******)local_6b0[-1],
       0x1f < (ulonglong)((longlong)local_6b0 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppuVar7);
  }
  local_6a0 = pppppuVar3;
  ppppuStack_698 = pppppuVar4;
  local_6b0 = (undefined8 *****)((ulonglong)local_6b0 & 0xffffffffffffff00);
  if ((undefined8 *****)0xf < ppppuStack_5e0) {
    ppppppuVar7 = (undefined8 ******)local_5f8;
    if ((0xfff < (longlong)ppppuStack_5e0 + 1U) &&
       (ppppppuVar7 = (undefined8 ******)local_5f8[-1],
       0x1f < (ulonglong)((longlong)local_5f8 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppuVar7);
  }
  local_5e8 = pppppuVar3;
  ppppuStack_5e0 = pppppuVar4;
  local_5f8 = (undefined8 *****)((ulonglong)local_5f8 & 0xffffffffffffff00);
  if ((undefined8 *****)0xf < ppppuStack_5c0) {
    ppppppuVar7 = (undefined8 ******)local_5d8;
    if ((0xfff < (longlong)ppppuStack_5c0 + 1U) &&
       (ppppppuVar7 = (undefined8 ******)local_5d8[-1],
       0x1f < (ulonglong)((longlong)local_5d8 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppuVar7);
  }
  local_5c8 = pppppuVar3;
  ppppuStack_5c0 = pppppuVar4;
  local_5d8 = (undefined8 *****)((ulonglong)local_5d8 & 0xffffffffffffff00);
  if (0xf < local_6d0) {
    ppppppcVar5 = (char ******)local_6e8;
    if ((0xfff < local_6d0 + 1) &&
       (ppppppcVar5 = (char ******)local_6e8[-1],
       (char *)0x1f < (char *)((longlong)local_6e8 + (-8 - (longlong)ppppppcVar5)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppcVar5);
  }
  local_6d8 = 0;
  local_6d0 = 0xf;
  local_6e8 = (char *****)((ulonglong)local_6e8 & 0xffffffffffffff00);
  if ((undefined8 *****)0xf < ppppuStack_628) {
    ppppppuVar7 = (undefined8 ******)local_640;
    if ((0xfff < (longlong)ppppuStack_628 + 1U) &&
       (ppppppuVar7 = (undefined8 ******)local_640[-1],
       0x1f < (ulonglong)((longlong)local_640 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppuVar7);
  }
  local_630 = pppppuVar3;
  ppppuStack_628 = pppppuVar4;
  local_640 = (undefined8 *****)((ulonglong)local_640 & 0xffffffffffffff00);
  if ((undefined8 *****)0xf < ppppuStack_648) {
    ppppppuVar7 = (undefined8 ******)local_660;
    if ((0xfff < (longlong)ppppuStack_648 + 1U) &&
       (ppppppuVar7 = (undefined8 ******)local_660[-1],
       0x1f < (ulonglong)((longlong)local_660 + (-8 - (longlong)ppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppuVar7);
  }
  return;
}


// Incoming references
// 0xc268a0 DATA caller none
// 0x4a74ef UNCONDITIONAL_CALL caller 1404a6240
// 0x4a752a UNCONDITIONAL_CALL caller 1404a6240
// 0x4a757a UNCONDITIONAL_CALL caller 1404a6240
// 0x4a75ca UNCONDITIONAL_CALL caller 1404a6240
