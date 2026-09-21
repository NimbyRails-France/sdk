// Candidate VA 14049cb20; RVA 0x49cb20
// Ghidra inferred prototype: undefined FUN_14049cb20()

ulonglong * FUN_14049cb20(ulonglong *param_1,undefined8 param_2)

{
  int *piVar1;
  short sVar2;
  uint uVar3;
  short *psVar4;
  int iVar5;
  undefined4 uVar6;
  short *psVar7;
  short *psVar8;
  undefined8 *******pppppppuVar9;
  size_t sVar10;
  longlong lVar12;
  longlong lVar13;
  tm *_Tm;
  char *pcVar14;
  undefined8 *******pppppppuVar15;
  longlong *******ppppppplVar16;
  undefined8 uVar17;
  ulonglong uVar18;
  longlong *******ppppppplVar19;
  undefined8 ******ppppppuVar20;
  ulonglong uVar21;
  longlong *******ppppppplVar22;
  longlong *plVar23;
  ulonglong uVar24;
  longlong lVar25;
  short *psVar26;
  ulonglong uVar27;
  longlong lVar28;
  uint uVar29;
  ulonglong uVar30;
  longlong *plVar31;
  uint uVar32;
  uint uVar33;
  bool bVar34;
  undefined8 extraout_XMM0_Qa;
  undefined8 local_res18;
  undefined8 local_res20;
  ulonglong in_stack_fffffffffffffd38;
  undefined4 uVar35;
  undefined8 ******local_2b0;
  longlong lStack_2a8;
  ulonglong local_2a0;
  ulonglong local_298;
  undefined8 ******local_290;
  undefined8 uStack_288;
  ulonglong local_280;
  ulonglong local_278;
  longlong *local_268;
  longlong *plStack_260;
  undefined8 ******local_258;
  undefined8 uStack_250;
  longlong local_248;
  ulonglong uStack_240;
  longlong *local_238;
  longlong *plStack_230;
  longlong *local_228;
  longlong *plStack_220;
  longlong *local_218;
  longlong *local_210;
  undefined8 ******local_208 [2];
  longlong local_1f8;
  ulonglong local_1f0;
  longlong ******local_1e8;
  undefined8 uStack_1e0;
  ulonglong local_1d8;
  ulonglong local_1d0;
  undefined8 ******local_1c8;
  undefined8 uStack_1c0;
  undefined8 local_1b8;
  ulonglong local_1b0;
  undefined8 ******local_1a8;
  undefined8 uStack_1a0;
  longlong local_198;
  ulonglong uStack_190;
  longlong local_188;
  longlong local_180;
  undefined2 local_178;
  ulonglong local_118 [20];
  undefined2 local_78;
  undefined1 local_76;
  undefined8 local_75;
  undefined8 uStack_6d;
  undefined8 local_65;
  undefined8 uStack_5d;
  undefined1 local_48 [16];
  size_t sVar11;

  uVar30 = 0;
  uVar29 = 0;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  uVar33 = 1;
  local_228 = (longlong *)0x0;
  plStack_220 = (longlong *)0x0;
  iVar5 = FUN_1402e34e0(&local_228);
  if (iVar5 != 0) {
                    /* WARNING: Subroutine does not return */
    FUN_140246930("directory_iterator::directory_iterator",iVar5,param_2);
  }
  if (plStack_220 != (longlong *)0x0) {
    LOCK();
    *(int *)(plStack_220 + 1) = (int)plStack_220[1] + 1;
    UNLOCK();
  }
  local_268 = local_228;
  plStack_260 = plStack_220;
  if (plStack_220 != (longlong *)0x0) {
    LOCK();
    *(int *)(plStack_220 + 1) = (int)plStack_220[1] + 1;
    UNLOCK();
  }
  local_238 = local_228;
  plStack_230 = plStack_220;
  FUN_1402dca00(&local_218,&local_238);
  do {
    plVar23 = local_268;
    if (local_268 == local_218) {
      if (local_210 != (longlong *)0x0) {
        LOCK();
        plVar23 = local_210 + 1;
        lVar28 = *plVar23;
        *(int *)plVar23 = (int)*plVar23 + -1;
        UNLOCK();
        if ((int)lVar28 == 1) {
          (**(code **)*local_210)(local_210);
          LOCK();
          piVar1 = (int *)((longlong)local_210 + 0xc);
          iVar5 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar5 == 1) {
            (**(code **)(*local_210 + 8))(local_210);
          }
        }
      }
      plVar23 = plStack_260;
      if (plStack_260 != (longlong *)0x0) {
        LOCK();
        plVar31 = plStack_260 + 1;
        lVar28 = *plVar31;
        *(int *)plVar31 = (int)*plVar31 + -1;
        UNLOCK();
        if ((int)lVar28 == 1) {
          (**(code **)*plStack_260)(plStack_260);
          LOCK();
          piVar1 = (int *)((longlong)plVar23 + 0xc);
          iVar5 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar5 == 1) {
            (**(code **)(*plVar23 + 8))(plVar23);
          }
        }
      }
      plVar23 = plStack_220;
      if (plStack_220 != (longlong *)0x0) {
        LOCK();
        plVar31 = plStack_220 + 1;
        lVar28 = *plVar31;
        *(int *)plVar31 = (int)*plVar31 + -1;
        UNLOCK();
        if ((int)lVar28 == 1) {
          (**(code **)*plStack_220)(plStack_220);
          LOCK();
          piVar1 = (int *)((longlong)plVar23 + 0xc);
          iVar5 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar5 == 1) {
            (**(code **)(*plVar23 + 8))(plVar23);
          }
        }
      }
      uVar18 = param_1[1];
      uVar24 = *param_1;
      if (uVar24 != uVar18) {
        for (lVar28 = ((longlong)(uVar18 - uVar24) >> 3) * -0x505050505050505; lVar28 != 0;
            lVar28 = lVar28 >> 1) {
          uVar29 = (int)uVar30 + 1;
          uVar30 = (ulonglong)uVar29;
        }
        FUN_1404c4a10(uVar24,uVar18,(longlong)(int)(uVar29 - 1) * 2,(ulonglong)param_1 & 0xff);
        if ((longlong)(uVar18 - uVar24) < 0x2e38) {
          FUN_1404c34d0(uVar24,uVar18);
        }
        else {
          FUN_1404c34d0(uVar24,uVar24 + 0x2ca0);
          FUN_1404c3800(uVar24 + 0x2ca0,uVar18);
        }
      }
      return param_1;
    }
    FUN_1402ee5f0(local_268,&local_238,3);
    if (((int)plStack_230 != 0) && (((int)local_238 - 1U & 0xfffffff7) != 0)) {
                    /* WARNING: Subroutine does not return */
      FUN_140246930("directory_entry::status",(ulonglong)plStack_230 & 0xffffffff,plVar23 + 4);
    }
    if ((int)local_238 == 2) {
      ppppppplVar22 = (longlong *******)(plVar23 + 4);
      ppppppplVar19 = ppppppplVar22;
      if (7 < (ulonglong)plVar23[7]) {
        ppppppplVar19 = (longlong *******)*ppppppplVar22;
      }
      psVar8 = (short *)((longlong)ppppppplVar19 + plVar23[6] * 2);
      psVar7 = (short *)FUN_14026cac0(ppppppplVar19,psVar8);
      psVar26 = psVar8;
      if (psVar7 != psVar8) {
        do {
          psVar4 = psVar8;
          if ((*psVar7 != 0x5c) && (*psVar7 != 0x2f)) break;
          psVar7 = psVar7 + 1;
        } while (psVar7 != psVar8);
        do {
          psVar26 = psVar4;
          if (psVar7 == psVar26) break;
          sVar2 = psVar26[-1];
          if ((sVar2 == 0x5c) || (psVar4 = psVar26 + -1, sVar2 == 0x2f)) break;
        } while( true );
      }
      psVar8 = (short *)thunk_FUN_140981880(psVar26,psVar8,0x3a);
      if ((psVar26 == psVar8) || (psVar7 = psVar8 + -1, psVar26 == psVar7)) {
LAB_14049ccbf:
        psVar7 = psVar8;
      }
      else {
        if (*psVar7 != 0x2e) {
          for (psVar7 = psVar8 + -2; psVar26 != psVar7; psVar7 = psVar7 + -1) {
            if (*psVar7 == 0x2e) goto LAB_14049ccc2;
          }
          goto LAB_14049ccbf;
        }
        if ((psVar26 == psVar8 + -2) && (psVar8[-2] == 0x2e)) goto LAB_14049ccbf;
      }
LAB_14049ccc2:
      local_2b0 = (undefined8 *******)0x0;
      lStack_2a8 = 0;
      local_2a0 = 0;
      local_298 = 0;
      FUN_140025200(&local_2b0,psVar7,(longlong)psVar8 - (longlong)psVar7 >> 1);
      uVar18 = local_2a0;
      local_res18 = CONCAT44(local_res18._4_4_,uVar33) | 0xe;
      pppppppuVar15 = &local_2b0;
      if (7 < local_298) {
        pppppppuVar15 = (undefined8 *******)local_2b0;
      }
      uVar6 = FUN_1409831e8();
      uStack_288 = 0;
      local_280 = 0;
      local_278 = 0xf;
      local_290 = (undefined8 *******)0x0;
      local_res18 = local_res18 | 0x80;
      if (uVar18 != 0) {
        if (0x7fffffff < uVar18) {
                    /* WARNING: Subroutine does not return */
          FUN_140001f00(0x16);
        }
        in_stack_fffffffffffffd38 = in_stack_fffffffffffffd38 & 0xffffffff00000000;
        local_res20 = FUN_140983258(uVar6,pppppppuVar15,uVar18 & 0xffffffff,0,
                                    in_stack_fffffffffffffd38);
        uVar35 = (undefined4)(in_stack_fffffffffffffd38 >> 0x20);
        iVar5 = (int)((ulonglong)local_res20 >> 0x20);
        if (iVar5 != 0) {
                    /* WARNING: Subroutine does not return */
          FUN_140246210(iVar5);
        }
        iVar5 = (int)local_res20;
        uVar24 = (ulonglong)iVar5;
        if (local_280 < uVar24) {
          FUN_1402457c0(&local_290,uVar24 - local_280,0);
        }
        else {
          pppppppuVar9 = &local_290;
          if (0xf < local_278) {
            pppppppuVar9 = (undefined8 *******)local_290;
          }
          local_280 = uVar24;
          *(undefined1 *)((longlong)pppppppuVar9 + uVar24) = 0;
        }
        pppppppuVar9 = &local_290;
        if (0xf < local_278) {
          pppppppuVar9 = (undefined8 *******)local_290;
        }
        in_stack_fffffffffffffd38 = CONCAT44(uVar35,iVar5);
        local_res20 = FUN_140983258(uVar6,pppppppuVar15,uVar18 & 0xffffffff,pppppppuVar9,
                                    in_stack_fffffffffffffd38);
        iVar5 = (int)((ulonglong)local_res20 >> 0x20);
        if (iVar5 != 0) {
                    /* WARNING: Subroutine does not return */
          FUN_140246210(iVar5);
        }
      }
      uVar18 = local_278;
      ppppppuVar20 = local_290;
      uVar3 = (uint)local_res18;
      uVar32 = (uint)local_res18 & 0xffffff7f;
      uVar33 = uVar32 | 0x70;
      sVar11 = 0xffffffffffffffff;
      do {
        sVar10 = sVar11 + 1;
        lVar28 = sVar11 + 1;
        sVar11 = sVar10;
      } while (".nimbyrails5"[lVar28] != '\0');
      pppppppuVar15 = &local_290;
      if (0xf < local_278) {
        pppppppuVar15 = (undefined8 *******)local_290;
      }
      if (local_280 == sVar10) {
        if (local_280 == 0) {
          bVar34 = true;
        }
        else {
          iVar5 = memcmp(pppppppuVar15,".nimbyrails5",local_280);
          bVar34 = iVar5 == 0;
        }
      }
      else {
        bVar34 = false;
      }
      if (0xf < uVar18) {
        pppppppuVar15 = (undefined8 *******)ppppppuVar20;
        if ((0xfff < uVar18 + 1) &&
           (pppppppuVar15 = (undefined8 *******)ppppppuVar20[-1],
           0x1f < (ulonglong)((longlong)ppppppuVar20 + (-8 - (longlong)pppppppuVar15)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppuVar15);
      }
      local_280 = 0;
      local_278 = 0xf;
      local_290 = (undefined8 ******)((ulonglong)local_290 & 0xffffffffffffff00);
      FUN_140025470(&local_2b0);
      plVar31 = local_268;
      if (bVar34) {
        uStack_1e0 = 0;
        local_1d8 = 0;
        local_1d0 = 7;
        local_1e8 = (longlong ******)0x0;
        uStack_1c0 = 0;
        local_1b8 = 0;
        local_1b0 = 0xf;
        local_1c8 = (undefined8 *******)0x0;
        uStack_1a0 = 0;
        local_198 = 0;
        uStack_190 = 0xf;
        local_1a8 = (undefined8 ******)0x0;
        local_188 = 0;
        local_180 = 0;
        local_178 = 0;
        local_118[1] = 0;
        local_118[2] = 0;
        local_118[3] = 0xf;
        local_118[0] = 0;
        local_118[5] = 0;
        local_118[6] = 0;
        local_118[7] = 0xf;
        local_118[4] = 0;
        local_118[8] = 0;
        local_118[9] = 0;
        local_118[10] = 0;
        local_118[0xb] = 0;
        local_118[0xc] = 0;
        local_118[0xd] = 0;
        local_118[0xe] = 0;
        local_118[0xf] = 0;
        local_118[0x10] = 0;
        local_118[0x11] = 0;
        local_118[0x12] = 0;
        local_118[0x13] = 0;
        local_78 = 0;
        local_76 = 0;
        local_75 = 0;
        uStack_6d = 0;
        local_65 = 0;
        uStack_5d = 0;
        if ((*(uint *)((longlong)local_268 + 0x1c) >> 5 & 1) == 0) {
          if ((*(uint *)((longlong)local_268 + 0x1c) >> 1 & 1) == 0) {
            uVar6 = 0xffffffff;
          }
          else {
            uVar6 = (undefined4)local_268[2];
          }
          ppppppplVar19 = ppppppplVar22;
          if (7 < (ulonglong)plVar23[7]) {
            ppppppplVar19 = (longlong *******)*ppppppplVar22;
          }
          iVar5 = FUN_1409835a0(ppppppplVar19,&local_2b0,0x21,uVar6);
          pppppppuVar15 = (undefined8 *******)local_2b0;
          if (iVar5 != 0) {
                    /* WARNING: Subroutine does not return */
            FUN_140246930("directory_entry::last_write_time",iVar5,plVar31 + 4);
          }
        }
        else {
          pppppppuVar15 = (undefined8 *******)*local_268;
        }
        lVar12 = _Xtime_get_ticks();
        lVar28 = lVar12 + SUB168(SEXT816(-0x29406b2a1a85bd43) * SEXT816(lVar12),8);
        lVar28 = (lVar28 >> 0x17) - (lVar28 >> 0x3f);
        if ((lVar28 * 10000000 - lVar12 != 0) && (lVar12 <= lVar28 * 10000000)) {
          lVar28 = lVar28 + -1;
        }
        lVar12 = (longlong)pppppppuVar15 +
                 SUB168(SEXT816(-0x29406b2a1a85bd43) * SEXT816((longlong)pppppppuVar15),8);
        lVar12 = (lVar12 >> 0x17) - (lVar12 >> 0x3f);
        if ((lVar12 * 10000000 - (longlong)pppppppuVar15 != 0) &&
           ((longlong)pppppppuVar15 <= lVar12 * 10000000)) {
          lVar12 = lVar12 + -1;
        }
        lVar13 = _Xtime_get_ticks();
        lVar13 = lVar13 + 0x19db1ded53e8000;
        lVar25 = SUB168(SEXT816(-0x29406b2a1a85bd43) * SEXT816(lVar13),8) + lVar13;
        lVar25 = (lVar25 >> 0x17) - (lVar25 >> 0x3f);
        if ((lVar25 * 10000000 - lVar13 != 0) && (lVar13 <= lVar25 * 10000000)) {
          lVar25 = lVar25 + -1;
        }
        local_res18 = (lVar12 - lVar25) + lVar28;
        local_188 = local_res18;
        _Tm = _localtime64(&local_res18);
        uStack_250 = 0;
        local_248 = 0;
        uStack_240 = 0xf;
        local_258 = (undefined8 *******)0x0;
        if ((_Tm != (tm *)0x0) && (pcVar14 = asctime(_Tm), pcVar14 != (char *)0x0)) {
          lVar28 = -1;
          do {
            lVar28 = lVar28 + 1;
          } while (pcVar14[lVar28] != '\0');
          FUN_140030630(&local_258,pcVar14);
          if (local_248 != 0) {
            lVar28 = local_248 + -1;
            pppppppuVar15 = &local_258;
            if (0xf < uStack_240) {
              pppppppuVar15 = (undefined8 *******)local_258;
            }
            lVar12 = local_248 - (ulonglong)(local_248 != lVar28);
            memmove((void *)((longlong)pppppppuVar15 + lVar28),
                    (void *)((ulonglong)(local_248 != lVar28) + (longlong)pppppppuVar15 + lVar28),
                    (lVar12 - lVar28) + 1);
            local_248 = lVar12;
          }
        }
        if (0xf < uStack_190) {
          ppppppuVar20 = local_1a8;
          if ((0xfff < uStack_190 + 1) &&
             (ppppppuVar20 = (undefined8 ******)local_1a8[-1],
             0x1f < (ulonglong)((longlong)local_1a8 + (-8 - (longlong)ppppppuVar20)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(ppppppuVar20);
        }
        uVar18 = local_1d0;
        local_1a8 = local_258;
        uStack_1a0 = uStack_250;
        local_198 = local_248;
        uStack_190 = uStack_240;
        local_res18 = (CONCAT44(local_res18._4_4_,uVar3) & 0xffffffffffffff7f | 0x170) &
                      0xfffffffffffffeff;
        uVar33 = (uVar32 | 0x170) & 0xfffffeff;
        if (&local_1e8 != ppppppplVar22) {
          uVar24 = plVar23[6];
          ppppppplVar19 = ppppppplVar22;
          if (7 < (ulonglong)plVar23[7]) {
            ppppppplVar19 = (longlong *******)*ppppppplVar22;
          }
          if (local_1d0 < uVar24) {
            if (0x7ffffffffffffffe < uVar24) {
                    /* WARNING: Subroutine does not return */
              FUN_140001c70();
            }
            uVar27 = uVar24 | 7;
            if (uVar27 < 0x7fffffffffffffff) {
              if (0x7ffffffffffffffe - (local_1d0 >> 1) < local_1d0) {
                uVar27 = 0x7ffffffffffffffe;
                uVar21 = 0x7fffffffffffffff;
              }
              else {
                uVar21 = (local_1d0 >> 1) + local_1d0;
                if (uVar27 < uVar21) {
                  uVar27 = uVar21;
                }
                uVar21 = uVar27 + 1;
                if (0x7fffffffffffffff < uVar21) {
                    /* WARNING: Subroutine does not return */
                  FUN_140001b70();
                }
              }
            }
            else {
              uVar27 = 0x7ffffffffffffffe;
              uVar21 = 0x7fffffffffffffff;
            }
            ppppppplVar16 = (longlong *******)FUN_140003270(uVar21 * 2);
            local_1d8 = uVar24;
            local_1d0 = uVar27;
            memcpy(ppppppplVar16,ppppppplVar19,uVar24 * 2);
            *(undefined2 *)(uVar24 * 2 + (longlong)ppppppplVar16) = 0;
            if (7 < uVar18) {
              FUN_1400260a0();
            }
          }
          else {
            ppppppplVar16 = &local_1e8;
            if (7 < local_1d0) {
              ppppppplVar16 = (longlong *******)local_1e8;
            }
            local_1d8 = uVar24;
            memmove(ppppppplVar16,ppppppplVar19,uVar24 * 2);
            *(undefined2 *)(uVar24 * 2 + (longlong)ppppppplVar16) = 0;
            ppppppplVar16 = (longlong *******)local_1e8;
          }
          local_1e8 = (longlong ******)ppppppplVar16;
          plVar31 = local_268;
          uVar33 = (uint)local_res18;
        }
        local_1b8 = 0;
        pppppppuVar15 = &local_1c8;
        if (0xf < local_1b0) {
          pppppppuVar15 = (undefined8 *******)local_1c8;
        }
        *(undefined1 *)pppppppuVar15 = 0;
        if (7 < (ulonglong)plVar23[7]) {
          ppppppplVar22 = (longlong *******)*ppppppplVar22;
        }
        lVar28 = (longlong)ppppppplVar22 + plVar23[6] * 2;
        lVar12 = FUN_1402dc160(ppppppplVar22,lVar28);
        uVar17 = thunk_FUN_140981880(lVar12,lVar28,0x3a);
        lVar28 = FUN_140498870(lVar12,uVar17);
        local_2b0 = (undefined8 *******)0x0;
        lStack_2a8 = 0;
        local_2a0 = 0;
        local_298 = 0;
        FUN_140025200(&local_2b0,lVar12,lVar28 - lVar12 >> 1);
        uVar17 = FUN_140247b90(local_208,&local_2b0);
        uVar33 = uVar33 | 0x1e00;
        if (7 < local_298) {
          FUN_1400260a0(uVar17,local_2b0);
        }
        pppppppuVar15 = local_208;
        if (7 < local_1f0) {
          pppppppuVar15 = (undefined8 *******)local_208[0];
        }
        pppppppuVar9 = local_208;
        if (7 < local_1f0) {
          pppppppuVar9 = (undefined8 *******)local_208[0];
        }
        FUN_140247700(local_48,pppppppuVar9,(longlong)pppppppuVar15 + local_1f8 * 2);
        if ((*(uint *)((longlong)plVar31 + 0x1c) >> 3 & 1) == 0) {
          plVar23 = plVar31 + 4;
          if (7 < (ulonglong)plVar31[7]) {
            plVar23 = (longlong *)*plVar23;
          }
          iVar5 = FUN_1409835a0(plVar23,&local_2b0,9);
          lVar28 = lStack_2a8;
          if (iVar5 != 0) {
                    /* WARNING: Subroutine does not return */
            FUN_140246930("directory_entry::file_size",iVar5,plVar31 + 4);
          }
        }
        else {
          lVar28 = plVar31[1];
        }
        uVar18 = param_1[1];
        local_180 = lVar28;
        if (uVar18 < param_1[2]) {
          FUN_1404ad0e0(uVar18,&local_1e8);
          param_1[1] = param_1[1] + 0x198;
        }
        else {
          lVar28 = (longlong)(uVar18 - *param_1) >> 3;
          if (lVar28 * -0x505050505050505 == 0) {
            lVar12 = 1;
LAB_14049d489:
            lVar28 = lVar12 * 0x198;
            uVar18 = thunk_FUN_140983da8(lVar28);
          }
          else {
            lVar12 = lVar28 * -0xa0a0a0a0a0a0a0a;
            if (lVar12 != 0) goto LAB_14049d489;
            lVar28 = lVar28 * 0x10;
            uVar18 = uVar30;
          }
          lVar12 = FUN_1404c3390(*param_1,param_1[1],uVar18);
          FUN_1404ad0e0(lVar12,&local_1e8);
          FUN_1404ba350(*param_1,param_1[1]);
          if ((void *)*param_1 != (void *)0x0) {
            free((void *)*param_1);
          }
          *param_1 = uVar18;
          param_1[1] = lVar12 + 0x198;
          param_1[2] = uVar18 + lVar28;
        }
        uVar17 = FUN_140025470(local_208);
        if (0xf < local_118[7]) {
          uVar17 = FUN_140003040(local_118 + 4,local_118[4]);
        }
        local_118[6] = 0;
        local_118[7] = 0xf;
        local_118[4] = local_118[4] & 0xffffffffffffff00;
        if (0xf < local_118[3]) {
          uVar17 = FUN_140003040(local_118,local_118[0]);
        }
        local_118[2] = 0;
        local_118[3] = 0xf;
        local_118[0] = local_118[0] & 0xffffffffffffff00;
        if (0xf < uStack_190) {
          uVar17 = FUN_140003040(&local_1a8,local_1a8);
        }
        local_198 = 0;
        uStack_190 = 0xf;
        local_1a8 = (undefined8 ******)((ulonglong)local_1a8 & 0xffffffffffffff00);
        if (0xf < local_1b0) {
          uVar17 = FUN_140003040(&local_1c8,local_1c8);
        }
        local_1b8 = 0;
        local_1b0 = 0xf;
        local_1c8 = (undefined8 ******)((ulonglong)local_1c8 & 0xffffffffffffff00);
        if (7 < local_1d0) {
          FUN_1400260a0(uVar17,local_1e8);
        }
      }
    }
    iVar5 = FUN_1402dc560(&local_268);
    if (iVar5 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_1402dc500(extraout_XMM0_Qa,iVar5);
    }
  } while( true );
}


// Incoming references
// 0xc26720 DATA caller none
// 0x4abd95 UNCONDITIONAL_CALL caller 1404abd50
// 0x5de9ab UNCONDITIONAL_CALL caller 1405de980
