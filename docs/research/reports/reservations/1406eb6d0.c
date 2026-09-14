// Candidate VA 1406eb6d0; RVA 0x6eb6d0
// Ghidra inferred prototype: undefined FUN_1406eb6d0()

void FUN_1406eb6d0(longlong *param_1,longlong *param_2,undefined4 *param_3,longlong param_4)

{
  int *piVar1;
  longlong lVar2;
  double dVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  undefined8 uVar7;
  code *pcVar8;
  double dVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  longlong *plVar13;
  undefined1 uVar14;
  int iVar15;
  undefined8 uVar16;
  longlong *_Dst;
  longlong *plVar17;
  longlong *plVar18;
  void *pvVar19;
  longlong *plVar20;
  longlong *plVar21;
  undefined4 *puVar22;
  longlong lVar23;
  longlong **pplVar24;
  longlong *plVar25;
  longlong lVar26;
  undefined4 *puVar27;
  float fVar28;
  double dVar29;
  undefined1 local_res8 [8];
  longlong *local_res10;
  undefined4 *local_res18;
  longlong local_res20;
  undefined8 in_stack_fffffffffffffd08;
  undefined4 uVar30;
  void *local_298;
  undefined8 uStack_290;
  longlong local_288;
  ulonglong local_280;
  longlong *local_278;
  longlong *plStack_270;
  longlong *local_268;
  undefined8 uStack_260;
  undefined1 *local_258;
  longlong *local_248;
  longlong *plStack_240;
  undefined8 local_238;
  ulonglong local_230;
  longlong *local_228;
  longlong *plStack_220;
  longlong *local_218;
  undefined4 uStack_210;
  undefined4 uStack_20c;
  undefined1 *local_208;
  longlong *local_1f8;
  longlong local_1f0;
  undefined8 uStack_1e8;
  longlong *local_1e0;
  undefined8 *puStack_1d8;
  longlong **local_1d0;
  undefined4 *puStack_1c8;
  undefined8 local_1c0;
  undefined8 uStack_1b8;
  longlong **local_1b0;
  undefined8 *local_1a8;
  double local_1a0;
  longlong local_198;
  longlong local_190;
  longlong *local_188;
  undefined8 *puStack_180;
  longlong **local_178;
  longlong lStack_170;
  undefined4 local_168;
  undefined4 uStack_164;
  undefined4 uStack_160;
  undefined4 uStack_15c;
  longlong **local_158;
  undefined1 local_148 [264];
  
  *(undefined4 *)((longlong)param_1 + 4) = *(undefined4 *)(param_4 + 0x18);
  local_res8[0] = (undefined1)*param_1;
  local_res10 = param_2;
  local_res18 = param_3;
  uVar14 = FUN_14024fe00(*(undefined8 *)(*(longlong *)((longlong)param_3 + 0x250) + 0x10));
  *(undefined1 *)(param_1 + 0x82) = uVar14;
  *(undefined1 *)((longlong)param_1 + 0x411) = uVar14;
  uVar14 = FUN_14024ffb0(*(undefined8 *)(*(longlong *)((longlong)param_3 + 0x250) + 0x10));
  *(undefined1 *)((longlong)param_1 + 0x412) = uVar14;
  *(undefined1 *)((longlong)param_1 + 0x413) = uVar14;
  lVar26 = **(longlong **)(*(longlong *)((longlong)param_3 + 0x250) + 0x10);
  lVar23 = lVar26 + 0x110;
  local_res20 = lVar23;
  iVar15 = _Mtx_lock(lVar23);
  if (iVar15 != 0) {
LAB_1406ec368:
    std::_Throw_Cpp_error(5);
    pcVar8 = (code *)swi(3);
    (*pcVar8)();
    return;
  }
  if (*(int *)(lVar26 + 0x15c) == 0x7fffffff) {
    *(undefined4 *)(lVar26 + 0x15c) = 0x7ffffffe;
  }
  else {
    FUN_140251310(&local_298,lVar26 + 0x30);
    _Mtx_unlock(lVar23);
    local_278 = (longlong *)0x0;
    plStack_270 = (longlong *)0x0;
    local_278 = (longlong *)FUN_140003270(0x20);
    uVar11 = s___user_map_manager_140a95808._12_4_;
    uVar10 = s___user_map_manager_140a95808._8_4_;
    uVar30 = s___user_map_manager_140a95808._4_4_;
    local_268 = (longlong *)0x12;
    uStack_260 = 0x1f;
    *(undefined4 *)local_278 = s___user_map_manager_140a95808._0_4_;
    *(undefined4 *)((longlong)local_278 + 4) = uVar30;
    *(undefined4 *)(local_278 + 1) = uVar10;
    *(undefined4 *)((longlong)local_278 + 0xc) = uVar11;
    *(undefined2 *)(local_278 + 2) = s___user_map_manager_140a95808._16_2_;
    *(char *)((longlong)local_278 + 0x12) = '\0';
    FUN_1406e8500(param_1 + 1,&local_278);
    if (0xf < uStack_260) {
      plVar18 = local_278;
      if ((0xfff < uStack_260 + 1) &&
         (plVar18 = (longlong *)local_278[-1],
         (char *)0x1f < (char *)((longlong)local_278 + (-8 - (longlong)plVar18)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(plVar18);
    }
    FUN_1402387a0(local_298,uStack_290);
    if (local_298 != (void *)0x0) {
      free(local_298);
    }
    local_258 = local_res8;
    local_288 = 0xd;
    local_280 = 0xf;
    local_298 = (void *)s___map_options_140a95820._0_8_;
    uStack_290 = (ulonglong)CONCAT14(s___map_options_140a95820[0xc],s___map_options_140a95820._8_4_)
    ;
    uStack_260._0_4_ = (undefined4)param_4;
    uStack_260._4_4_ = (undefined4)((ulonglong)param_4 >> 0x20);
    uStack_210 = (undefined4)uStack_260;
    uStack_20c = uStack_260._4_4_;
    local_278 = param_1;
    plStack_270 = param_2;
    local_268 = (longlong *)param_3;
    uStack_260 = param_4;
    local_228 = param_1;
    plStack_220 = param_2;
    local_218 = (longlong *)param_3;
    local_208 = local_258;
    FUN_1406ed600(param_2,&local_298);
    if (0xf < local_280) {
      pvVar19 = local_298;
      if ((0xfff < local_280 + 1) &&
         (pvVar19 = *(void **)((longlong)local_298 + -8),
         0x1f < (ulonglong)((longlong)local_298 + (-8 - (longlong)pvVar19)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar19);
    }
    if (*(char *)((longlong)param_1 + 0x413) != *(char *)((longlong)param_1 + 0x412)) {
      FUN_1402505d0(*(undefined8 *)(*(longlong *)((longlong)param_3 + 0x250) + 0x10));
    }
    if (*(char *)((longlong)param_1 + 0x411) != (char)param_1[0x82]) {
      FUN_140250070(*(undefined8 *)(*(longlong *)((longlong)param_3 + 0x250) + 0x10));
    }
    lVar26 = **(longlong **)(*(longlong *)((longlong)param_3 + 0x250) + 0x10);
    lVar23 = lVar26 + 0x110;
    local_res20 = lVar23;
    iVar15 = _Mtx_lock(lVar23);
    if (iVar15 != 0) goto LAB_1406ec368;
    if (*(int *)(lVar26 + 0x15c) != 0x7fffffff) {
      FUN_140251310(&local_298,lVar26 + 0x30);
      _Mtx_unlock(lVar23);
      FUN_1406e8750(param_1 + 1,&local_298);
      pvVar19 = local_298;
      FUN_1402387a0(local_298,uStack_290);
      if (pvVar19 != (void *)0x0) {
        free(pvVar19);
      }
      if (param_1[0x42] != 0) {
        uVar7 = *(undefined8 *)(*(longlong *)((longlong)param_3 + 0x250) + 0x10);
        uVar16 = FUN_1402504e0(local_148);
        FUN_140250890(uVar7,uVar16);
      }
      if ((char)param_1[0x3c] != '\0') {
        local_248 = param_1 + 100;
        if (0xf < (ulonglong)param_1[0x67]) {
          local_248 = (longlong *)param_1[100];
        }
        plStack_240 = (longlong *)param_1[0x66];
        FUN_140250720(*(undefined8 *)(*(longlong *)((longlong)param_3 + 0x250) + 0x10),
                      param_1 + 0x36,&local_248);
      }
      lVar23 = param_1[0x3d];
      if (lVar23 != 0) {
        lVar26 = **(longlong **)(*(longlong *)((longlong)param_3 + 0x250) + 0x10);
        FUN_14024afa0(lVar26,&local_248,lVar23);
        if ((local_248 != (longlong *)0x0) && ((char)local_248[6] != '\0')) {
          lVar2 = lVar26 + 0x110;
          local_res20 = lVar2;
          iVar15 = _Mtx_lock(lVar2);
          if (iVar15 != 0) {
            std::_Throw_Cpp_error(5);
            pcVar8 = (code *)swi(3);
            (*pcVar8)();
            return;
          }
          if (*(int *)(lVar26 + 0x15c) == 0x7fffffff) {
            *(undefined4 *)(lVar26 + 0x15c) = 0x7ffffffe;
            std::_Throw_Cpp_error(6);
          }
          FUN_14024b6a0(lVar26,lVar23);
          _Mtx_unlock(lVar2);
        }
        plVar18 = plStack_240;
        if (plStack_240 != (longlong *)0x0) {
          LOCK();
          plVar21 = plStack_240 + 1;
          lVar23 = *plVar21;
          *(int *)plVar21 = (int)*plVar21 + -1;
          UNLOCK();
          if ((int)lVar23 == 1) {
            (**(code **)*plStack_240)(plStack_240);
            LOCK();
            piVar1 = (int *)((longlong)plVar18 + 0xc);
            iVar15 = *piVar1;
            *piVar1 = *piVar1 + -1;
            UNLOCK();
            if (iVar15 == 1) {
              (**(code **)(*plVar18 + 8))(plVar18);
            }
          }
        }
      }
      if ((char)param_1[0x40] != '\0') {
        lVar23 = param_1[0x3f];
        lVar26 = param_1[0x3e];
        lVar2 = **(longlong **)(*(longlong *)((longlong)param_3 + 0x250) + 0x10);
        iVar15 = _Mtx_lock(lVar2 + 0x110);
        if (iVar15 != 0) goto LAB_1406ec368;
        if (*(int *)(lVar2 + 0x15c) == 0x7fffffff) {
          *(undefined4 *)(lVar2 + 0x15c) = 0x7ffffffe;
          goto LAB_1406ebb9e;
        }
        if (lVar26 != lVar23) {
          plVar18 = *(longlong **)(lVar2 + 0x38);
          plVar21 = *(longlong **)(lVar2 + 0x30);
          plVar25 = plVar18;
          plVar20 = plVar18;
          if (plVar21 != plVar18) {
            do {
              if (*(longlong *)(*plVar21 + 8) == lVar26) {
                plVar25 = plVar21;
              }
              if (*(longlong *)(*plVar21 + 8) == lVar23) {
                plVar20 = plVar21;
              }
              plVar21 = plVar21 + 2;
            } while (plVar21 != plVar18);
            if (((plVar25 != plVar20) && (plVar25 != plVar18)) && (plVar20 != plVar18)) {
              lVar23 = *plVar25;
              *plVar25 = *plVar20;
              *plVar20 = lVar23;
              lVar23 = plVar25[1];
              plVar25[1] = plVar20[1];
              plVar20[1] = lVar23;
              *(longlong *)(lVar2 + 0x80) = *(longlong *)(lVar2 + 0x80) + 1;
            }
          }
          iVar15 = 1;
          plVar18 = *(longlong **)(lVar2 + 0x38);
          for (plVar21 = *(longlong **)(lVar2 + 0x30); plVar21 != plVar18; plVar21 = plVar21 + 2) {
            *(int *)(*plVar21 + 0x108) = iVar15;
            iVar15 = iVar15 + 1;
          }
        }
        _Mtx_unlock(lVar2 + 0x110);
      }
      if (*(int *)(param_4 + 0x20) != 4) {
        return;
      }
      lVar23 = *(longlong *)((longlong)param_3 + 600);
      if (lVar23 != 0) {
        local_1a8 = (undefined8 *)(lVar23 + 0x428);
        local_198 = lVar23 + 0x890;
        local_248 = (longlong *)(lVar23 + 0x780);
        if (*(longlong *)((longlong)param_3 + 0x260) != 0) {
          local_190 = *(longlong *)((longlong)param_3 + 0x260) + 0xa0;
          FUN_140647a30(&local_298,param_2,local_1a8);
          local_278 = (longlong *)0x0;
          plStack_270 = (longlong *)0x0;
          local_268 = (longlong *)0x0;
          local_228 = (longlong *)0x0;
          plStack_220 = (longlong *)0x0;
          local_218 = (longlong *)0x0;
          if (local_288 != 0) {
            local_1f0 = local_288;
            uStack_1e8 = 0;
            plVar18 = *(longlong **)((longlong)param_3 + 0xb8);
            if (plVar18 == *(longlong **)((longlong)param_3 + 0xc0)) {
              FUN_140017c10((longlong)param_3 + 0xb0,plVar18,&local_1f0);
            }
            else {
              *plVar18 = local_288;
              plVar18[1] = 0;
              *(longlong *)((longlong)param_3 + 0xb8) =
                   *(longlong *)((longlong)param_3 + 0xb8) + 0x10;
            }
          }
          if (*(longlong *)((longlong)param_3 + 0x260) != 0) {
            lVar23 = *(longlong *)(*(longlong *)((longlong)param_3 + 0x260) + 0x2200);
            local_res20 = *(longlong *)(lVar23 + 0x410);
            local_1f0 = *(longlong *)(lVar23 + 0x418);
            if (local_res20 != local_1f0) {
              local_1f8 = (longlong *)(local_res20 + 0x18);
              plVar18 = plStack_270;
              do {
                plVar25 = (longlong *)*local_1f8;
                plVar20 = (longlong *)*plVar25;
                plVar21 = plVar25;
                if (plVar20 == (longlong *)0x0) {
                  plVar21 = plVar25 + 1;
                  plVar20 = (longlong *)*plVar21;
                  while (plVar20 == (longlong *)0x0) {
                    plVar21 = plVar21 + 1;
                    plVar20 = (longlong *)*plVar21;
                  }
                }
                plVar25 = (longlong *)plVar25[local_1f8[1]];
                param_3 = local_res18;
                while (local_res18 = param_3, plVar20 != plVar25) {
                  plVar17 = plVar18;
                  if (*plVar20 == local_288) {
                    if (plVar18 < local_268) {
                      plVar17 = plVar18 + 4;
                      lVar23 = plVar20[2];
                      *plVar18 = plVar20[1];
                      plVar18[1] = lVar23;
                      lVar23 = plVar20[4];
                      plVar18[2] = plVar20[3];
                      plVar18[3] = lVar23;
                      plStack_270 = plVar17;
                    }
                    else {
                      lVar23 = (longlong)plVar18 - (longlong)local_278 >> 5;
                      if (lVar23 == 0) {
                        lVar26 = 1;
LAB_1406ebdf7:
                        lVar23 = lVar26 << 5;
                        _Dst = (longlong *)thunk_FUN_140983da8(lVar23);
                        plVar18 = plStack_270;
                      }
                      else {
                        lVar26 = lVar23 * 2;
                        if (lVar26 != 0) goto LAB_1406ebdf7;
                        _Dst = (longlong *)0x0;
                        lVar23 = lVar23 << 6;
                      }
                      plVar13 = local_278;
                      plVar17 = _Dst;
                      if (local_278 != plVar18) {
                        pvVar19 = memmove(_Dst,local_278,(longlong)plVar18 - (longlong)local_278);
                        plVar17 = (longlong *)
                                  ((longlong)pvVar19 +
                                  ((longlong)plVar18 - (longlong)plVar13 & 0xffffffffffffffe0U));
                      }
                      lVar26 = plVar20[2];
                      *plVar17 = plVar20[1];
                      plVar17[1] = lVar26;
                      lVar26 = plVar20[4];
                      plVar17[2] = plVar20[3];
                      plVar17[3] = lVar26;
                      plVar17 = plVar17 + 4;
                      if (local_278 != (longlong *)0x0) {
                        free(local_278);
                      }
                      local_268 = (longlong *)((longlong)_Dst + lVar23);
                      local_278 = _Dst;
                      plStack_270 = plVar17;
                    }
                  }
                  plVar20 = (longlong *)plVar20[5];
                  while (plVar18 = plVar17, param_3 = local_res18, plVar20 == (longlong *)0x0) {
                    plVar21 = plVar21 + 1;
                    plVar20 = (longlong *)*plVar21;
                  }
                }
                local_res20 = local_res20 + 0x40;
                local_1f8 = local_1f8 + 8;
              } while (local_res20 != local_1f0);
            }
            if (*(longlong *)((longlong)param_3 + 0x260) != 0) {
              FUN_140459ea0(*(longlong *)((longlong)param_3 + 0x260) + 0x2c8,&local_1e0,local_288);
              local_res18 = puStack_1c8;
              pplVar24 = local_1d0;
LAB_1406ebeeb:
              uVar30 = (undefined4)((ulonglong)in_stack_fffffffffffffd08 >> 0x20);
              puVar22 = local_res18;
              do {
                if ((local_1e0 == (longlong *)0x0) ||
                   (((*local_1e0 == local_1e0[1] ||
                     (*(longlong *)(*local_1e0 + (longlong)pplVar24 * 8) == 0)) &&
                    ((puVar22 == (undefined4 *)0x0 || (puVar22 == local_1c0)))))) {
LAB_1406ec092:
                  plVar18 = local_res10;
                  if ((local_288 != 0) &&
                     (lVar23 = FUN_14032c420(*local_1a8), plVar18 = local_res10, lVar23 != 0)) {
                    lVar26 = *local_res10;
                    plVar21 = *(longlong **)(lVar26 + 0x1580);
                    FUN_1403898d0(lVar23,&local_1a8,uStack_290,0);
                    dVar9 = (double)local_1a8 * (double)plVar18[0x2c];
                    dVar3 = (double)plVar18[0x2f];
                    dVar4 = (double)plVar18[0x32];
                    dVar5 = (double)plVar18[0x2e];
                    dVar29 = (double)local_1a8 * (double)plVar18[0x2b];
                    dVar6 = (double)plVar18[0x31];
                    fVar28 = *(float *)(lVar26 + 0x1550);
                    if (fVar28 < 0.0) {
                      fVar28 = DAT_140aabae4;
                    }
                    FUN_1401b2200(plVar21,0xc,(short)plVar18[0x14],
                                  *(undefined2 *)((longlong)plVar18 + 0xa4),
                                  CONCAT44(uVar30,fVar28 * DAT_140aabb98));
                    (**(code **)*plVar21)(plVar21);
                    (**(code **)(*plVar21 + 0x50))
                              (plVar21,(float)(local_1a0 * dVar5 + dVar29 + dVar6),
                               (float)(dVar9 + local_1a0 * dVar3 + dVar4),DAT_140aac19c);
                    (**(code **)(*plVar21 + 0x88))(plVar21,0xaa0077aa,DAT_140aac0a0);
                    (**(code **)(*plVar21 + 0x88))(plVar21,0xff00ccff,DAT_140aabe40);
                    FUN_1401b2400(plVar21);
                  }
                  local_1e0 = &local_288;
                  puStack_1d8 = &uStack_290;
                  local_1d0 = &local_278;
                  puStack_1c8 = (undefined4 *)local_198;
                  local_1c0 = (undefined4 *)local_190;
                  uStack_1b8 = local_248;
                  local_1b0 = &local_228;
                  local_248 = (longlong *)0x0;
                  plStack_240 = (longlong *)0x0;
                  local_248 = (longlong *)FUN_140003270(0x20);
                  uVar7 = s___reservation_inspector_140a95830._8_8_;
                  local_238 = 0x17;
                  local_230 = 0x1f;
                  *local_248 = s___reservation_inspector_140a95830._0_8_;
                  local_248[1] = uVar7;
                  *(undefined4 *)(local_248 + 2) = s___reservation_inspector_140a95830._16_4_;
                  *(undefined2 *)((longlong)local_248 + 0x14) =
                       s___reservation_inspector_140a95830._20_2_;
                  *(char *)((longlong)local_248 + 0x16) = s___reservation_inspector_140a95830[0x16];
                  *(char *)((longlong)local_248 + 0x17) = '\0';
                  local_188 = local_1e0;
                  puStack_180 = puStack_1d8;
                  local_178 = local_1d0;
                  lStack_170 = (longlong)puStack_1c8;
                  local_168 = (undefined4)local_1c0;
                  uStack_164 = local_1c0._4_4_;
                  uStack_160 = (undefined4)uStack_1b8;
                  uStack_15c = uStack_1b8._4_4_;
                  local_158 = local_1b0;
                  FUN_1406eda10(plVar18,&local_248);
                  if (0xf < local_230) {
                    plVar18 = local_248;
                    if ((0xfff < local_230 + 1) &&
                       (plVar18 = (longlong *)local_248[-1],
                       (char *)0x1f < (char *)((longlong)local_248 + (-8 - (longlong)plVar18)))) {
                    /* WARNING: Subroutine does not return */
                      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
                    }
                    free(plVar18);
                  }
                  if (local_228 != (longlong *)0x0) {
                    free(local_228);
                  }
                  if (local_278 == (longlong *)0x0) {
                    return;
                  }
                  free(local_278);
                  return;
                }
                puVar27 = puVar22;
                if ((*local_1e0 != local_1e0[1]) &&
                   (*(longlong *)(*local_1e0 + (longlong)pplVar24 * 8) != 0)) {
                  puVar27 = (undefined4 *)((longlong)pplVar24 * 0x20 + local_1e0[3]);
                }
                lVar23 = *local_1e0;
                if (local_1e0[1] - lVar23 >> 3 != 0) {
                  while (puVar22 = local_res18, *(longlong *)(lVar23 + (longlong)pplVar24 * 8) != 0)
                  {
                    pplVar24 = (longlong **)
                               ((local_1e0[1] - *local_1e0 >> 3) - 1U & (longlong)pplVar24 + 1U);
                    lVar23 = *(longlong *)(*local_1e0 + (longlong)pplVar24 * 8);
                    if ((((undefined8 *)lVar23 == puStack_1d8) || (lVar23 == 0)) ||
                       (lVar23 = *local_1e0, local_1e0[1] == lVar23)) break;
                  }
                }
                if (((*local_1e0 == local_1e0[1]) ||
                    (*(longlong *)(*local_1e0 + (longlong)pplVar24 * 8) == 0)) &&
                   (puVar22 < local_1c0)) {
                  puVar22 = puVar22 + 8;
                  local_res18 = puVar22;
                }
                if (puVar27 == (undefined4 *)0x0) goto LAB_1406ec092;
                if (local_218 <= plStack_220) goto LAB_1406ebff5;
                uVar10 = puVar27[1];
                uVar11 = puVar27[2];
                uVar12 = puVar27[3];
                *(undefined4 *)plStack_220 = *puVar27;
                *(undefined4 *)((longlong)plStack_220 + 4) = uVar10;
                *(undefined4 *)(plStack_220 + 1) = uVar11;
                *(undefined4 *)((longlong)plStack_220 + 0xc) = uVar12;
                lVar23 = *(longlong *)(puVar27 + 6);
                plStack_220[2] = *(longlong *)(puVar27 + 4);
                plStack_220[3] = lVar23;
                plStack_220 = plStack_220 + 4;
              } while( true );
            }
          }
                    /* WARNING: Subroutine does not return */
          abort();
        }
      }
                    /* WARNING: Subroutine does not return */
      abort();
    }
    *(undefined4 *)(lVar26 + 0x15c) = 0x7ffffffe;
  }
LAB_1406ebb9e:
  std::_Throw_Cpp_error(6);
  pcVar8 = (code *)swi(3);
  (*pcVar8)();
  return;
LAB_1406ebff5:
  lVar23 = (longlong)plStack_220 - (longlong)local_228 >> 5;
  if (lVar23 == 0) {
    lVar23 = 1;
  }
  else {
    lVar23 = lVar23 * 2;
    if (lVar23 == 0) {
      plVar18 = (longlong *)0x0;
      goto LAB_1406ec024;
    }
  }
  plVar18 = (longlong *)thunk_FUN_140983da8(lVar23 << 5);
LAB_1406ec024:
  plVar25 = plStack_220;
  plVar21 = local_228;
  plVar20 = plVar18;
  if (local_228 != plStack_220) {
    pvVar19 = memmove(plVar18,local_228,(longlong)plStack_220 - (longlong)local_228);
    plVar20 = (longlong *)
              ((longlong)pvVar19 + ((longlong)plVar25 - (longlong)plVar21 & 0xffffffffffffffe0U));
  }
  uVar30 = puVar27[1];
  uVar10 = puVar27[2];
  uVar11 = puVar27[3];
  *(undefined4 *)plVar20 = *puVar27;
  *(undefined4 *)((longlong)plVar20 + 4) = uVar30;
  *(undefined4 *)(plVar20 + 1) = uVar10;
  *(undefined4 *)((longlong)plVar20 + 0xc) = uVar11;
  lVar26 = *(longlong *)(puVar27 + 6);
  plVar20[2] = *(longlong *)(puVar27 + 4);
  plVar20[3] = lVar26;
  if (local_228 != (longlong *)0x0) {
    free(local_228);
  }
  local_218 = plVar18 + lVar23 * 4;
  local_228 = plVar18;
  plStack_220 = plVar20 + 4;
  goto LAB_1406ebeeb;
}


// Incoming references
// 0xc35534 DATA caller none
// 0x683dd9 UNCONDITIONAL_CALL caller 1406830d0
