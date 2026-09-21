// Candidate VA 14079f4d0; RVA 0x79f4d0
// Ghidra inferred prototype: undefined FUN_14079f4d0()

longlong FUN_14079f4d0(longlong param_1,longlong param_2,longlong param_3,ulonglong param_4,
                      undefined8 param_5,undefined8 *param_6)

{
  ulonglong *puVar1;
  ulonglong uVar2;
  undefined8 *puVar3;
  void *pvVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int *piVar8;
  int *piVar9;
  void *pvVar10;
  int *piVar11;
  int *piVar12;
  longlong lVar13;
  void *pvVar14;
  ulonglong *puVar15;
  undefined8 *puVar16;
  int *piVar17;
  char *pcVar18;
  int iVar19;
  longlong *plVar20;
  longlong *plVar21;
  longlong *plVar22;
  ulonglong uVar23;
  undefined8 *puVar24;
  void *pvVar25;
  void *pvVar26;
  longlong lVar27;
  ulonglong *puVar28;
  ulonglong uVar29;
  longlong lVar30;
  longlong lVar31;
  undefined8 *puVar32;
  uint uVar33;
  ulonglong local_res20;
  char local_468;
  undefined1 local_467;
  undefined1 local_466 [6];
  void *local_460;
  void *pvStack_458;
  void *local_450;
  char *local_448;
  undefined8 uStack_440;
  undefined8 *local_438;
  ulonglong local_430;
  undefined8 local_428;
  undefined4 uStack_420;
  undefined4 uStack_41c;
  undefined4 uStack_418;
  undefined4 uStack_414;
  undefined8 local_410;
  int *local_408;
  void **ppvStack_400;
  uint local_3f8;
  uint local_3f4;
  int *local_3f0;
  longlong *local_3e8;
  longlong *plStack_3e0;
  undefined8 local_3d8;
  int *local_3d0;
  int *piStack_3c8;
  int *local_3c0;
  undefined4 local_3b8 [2];
  longlong local_3b0;
  void *local_3a8;
  void *pvStack_3a0;
  undefined8 local_398;
  void *local_390;
  void *pvStack_388;
  void *local_380;
  void *local_378;
  void *pvStack_370;
  void *local_368;
  void *local_360;
  undefined8 uStack_358;
  undefined8 local_350;
  ulonglong local_348;
  undefined8 local_340;
  longlong local_338;
  longlong local_330;
  undefined8 local_328;
  undefined8 uStack_320;
  undefined8 local_318;
  undefined8 local_310;
  undefined8 local_308;
  undefined8 uStack_300;
  undefined8 local_2f8;
  undefined8 local_2f0;
  void *local_2e8;
  void *local_2e0;
  void *local_2d0;
  void *pvStack_2c8;
  void *local_2c0;
  longlong local_2b0 [2];
  undefined1 *local_2a0;
  longlong lStack_298;
  undefined1 *local_290;
  longlong lStack_288;
  uint *local_280;
  longlong lStack_278;
  longlong local_270;
  undefined8 *puStack_268;
  void **local_260;
  char *pcStack_258;
  undefined1 *local_250;
  int **ppiStack_248;
  void **local_240;
  void **ppvStack_238;
  undefined8 *local_230;
  void **ppvStack_228;
  void **local_220;
  void **ppvStack_218;
  undefined1 *local_210;
  longlong **pplStack_208;
  undefined8 local_200;
  undefined8 uStack_1f8;
  undefined4 *local_1f0;
  undefined1 local_1e8 [8];
  char local_1e0;
  undefined1 local_1d8 [104];
  longlong *local_170;
  longlong *local_168;
  longlong *local_160;
  void *local_158;
  void *local_150;
  undefined1 *local_118;
  longlong lStack_110;
  undefined1 *local_108;
  longlong lStack_100;
  uint *local_f8;
  longlong lStack_f0;
  longlong local_e8;
  undefined8 *puStack_e0;
  void **local_d8;
  char *pcStack_d0;
  undefined1 *local_c8;
  int **ppiStack_c0;
  void **local_b8;
  void **ppvStack_b0;
  undefined8 *local_a8;
  void **ppvStack_a0;
  void **local_98;
  void **ppvStack_90;
  undefined1 *local_88;
  longlong **pplStack_80;
  undefined4 local_78;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined4 uStack_6c;
  undefined4 *local_68;

  puVar32 = param_6;
  local_res20 = param_4;
  FUN_1402f7860(local_1e8,param_6);
  local_468 = '\0';
  local_467 = DAT_140be285d;
  local_338 = *(longlong *)(param_4 + 600);
  if ((local_338 == 0) || (local_330 = local_338 + 0x428, *(longlong *)(param_4 + 0x260) == 0)) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  local_3b0 = *(longlong *)(local_338 + 0x408);
  local_2b0[0] = (longlong)*(int *)(puVar32 + 6) * 0x38 + *(longlong *)(local_3b0 + 0x120);
  local_460 = (void *)0x0;
  pvStack_458 = (void *)0x0;
  pvVar26 = (void *)0x0;
  local_450 = (void *)0x0;
  local_3d0 = (int *)0x0;
  piStack_3c8 = (int *)0x0;
  local_3c0 = (int *)0x0;
  local_3f4 = 0xffffffff;
  local_3f8 = 0;
  piVar17 = *(int **)(local_3b0 + 0x120);
  piVar11 = *(int **)(local_3b0 + 0x128);
  local_408 = piVar11;
  if (piVar17 != piVar11) {
    piVar12 = piVar17 + 2;
    pvVar14 = pvVar26;
    do {
      pvVar25 = pvStack_458;
      local_3f0 = piVar12;
      if (*(char *)((longlong)piVar12 + 0x2e) != '\0') {
        if (pvStack_458 < pvVar26) {
          pvStack_458 = (void *)((longlong)pvStack_458 + 0x20);
          FUN_140019ca0(pvVar25,piVar12);
          uVar33 = (uint)pvVar14;
        }
        else {
          lVar27 = (longlong)pvStack_458 - (longlong)local_460 >> 5;
          if (lVar27 == 0) {
            lVar13 = 1;
LAB_14079f628:
            lVar27 = lVar13 << 5;
            pvVar26 = (void *)thunk_FUN_140983da8(lVar27);
          }
          else {
            lVar13 = lVar27 * 2;
            if (lVar13 != 0) goto LAB_14079f628;
            pvVar26 = (void *)0x0;
            lVar27 = lVar27 << 6;
          }
          lVar31 = FUN_14033afd0(local_460,pvStack_458,pvVar26);
          FUN_140019ca0(lVar31,piVar12);
          pvVar14 = pvStack_458;
          lVar30 = lVar27;
          pvVar25 = local_460;
          if (local_460 != pvStack_458) {
            do {
              FUN_140002d30(pvVar25);
              pvVar25 = (void *)((longlong)pvVar25 + 0x20);
            } while (pvVar25 != pvVar14);
            lVar30 = lVar13 << 5;
          }
          if (local_460 != (void *)0x0) {
            free(local_460);
            lVar30 = lVar27;
          }
          local_450 = (void *)(lVar30 + (longlong)pvVar26);
          uVar33 = local_3f8;
          local_460 = pvVar26;
          pvStack_458 = (void *)(lVar31 + 0x20);
        }
        iVar19 = *piVar17;
        if (piStack_3c8 < local_3c0) {
          *piStack_3c8 = iVar19;
          piVar12 = piStack_3c8;
        }
        else {
          lVar27 = (longlong)piStack_3c8 - (longlong)local_3d0 >> 2;
          if (lVar27 == 0) {
            lVar13 = 1;
LAB_14079f6f7:
            lVar27 = lVar13 * 4;
            piVar11 = (int *)thunk_FUN_140983da8(lVar27);
          }
          else {
            lVar13 = lVar27 * 2;
            if (lVar13 != 0) goto LAB_14079f6f7;
            piVar11 = (int *)0x0;
            lVar27 = lVar27 << 3;
          }
          piVar9 = piStack_3c8;
          piVar8 = local_3d0;
          piVar12 = piVar11;
          if (local_3d0 != piStack_3c8) {
            pvVar26 = memmove(piVar11,local_3d0,(longlong)piStack_3c8 - (longlong)local_3d0);
            piVar12 = (int *)((longlong)pvVar26 + ((longlong)piVar9 - (longlong)piVar8 >> 2) * 4);
          }
          *piVar12 = iVar19;
          if (local_3d0 != (int *)0x0) {
            free(local_3d0);
          }
          local_3c0 = (int *)(lVar27 + (longlong)piVar11);
          local_3d0 = piVar11;
        }
        piStack_3c8 = piVar12 + 1;
        if (*(int *)(param_6 + 6) == *piVar17) {
          local_3f4 = uVar33;
        }
        local_3f8 = uVar33 + 1;
        pvVar14 = (void *)(ulonglong)local_3f8;
        pvVar26 = local_450;
        piVar11 = local_408;
      }
      piVar17 = piVar17 + 0xe;
      piVar12 = local_3f0 + 0xe;
      param_4 = local_res20;
      local_3f0 = piVar12;
    } while (piVar17 != piVar11);
  }
  lVar27 = local_3b0;
  lVar13 = FUN_1402d82e0("signal_limits_block_always","Always");
  local_448 = (char *)0x0;
  uStack_440 = 0;
  local_438 = (undefined8 *)0x0;
  local_430 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_448,lVar13);
  lVar13 = FUN_1402d82e0("signal_limits_block_same","Only for same direction");
  local_428 = 0;
  uStack_420 = 0;
  uStack_41c = 0;
  uStack_418 = 0;
  uStack_414 = 0;
  local_410 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_428,lVar13);
  local_378 = (void *)0x0;
  pvStack_370 = (void *)0x0;
  local_368 = (void *)0x0;
  local_378 = (void *)thunk_FUN_140983da8(0x40);
  pvStack_370 = (void *)((longlong)local_378 + 0x40);
  local_368 = pvStack_370;
  FUN_14041eae0(&local_448,&local_408,local_378);
  _eh_vector_destructor_iterator_(&local_448,0x20,2,thunk_FUN_140002d30);
  lVar13 = FUN_1402d82e0("signal_stop_check_signals_stops","Signals and train stop");
  local_328 = 0;
  uStack_320 = 0;
  local_318 = 0;
  local_310 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_328,lVar13);
  lVar13 = FUN_1402d82e0("signal_stop_check_only_signals","Only signals");
  local_308 = 0;
  uStack_300 = 0;
  local_2f8 = 0;
  local_2f0 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_308,lVar13);
  local_2d0 = (void *)0x0;
  pvStack_2c8 = (void *)0x0;
  local_2c0 = (void *)0x0;
  pvVar14 = (void *)thunk_FUN_140983da8(0x40);
  pvVar26 = (void *)((longlong)pvVar14 + 0x40);
  local_2d0 = pvVar14;
  pvStack_2c8 = pvVar26;
  local_2c0 = pvVar26;
  FUN_14041eae0(&local_328,&local_2e8,pvVar14);
  _eh_vector_destructor_iterator_(&local_328,0x20,2,thunk_FUN_140002d30);
  lVar13 = FUN_1402d82e0("signal_filter_applies","Applies to all trains");
  local_448 = (char *)0x0;
  uStack_440 = 0;
  local_438 = (undefined8 *)0x0;
  local_430 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_448,lVar13);
  lVar13 = FUN_1402d82e0("signal_filter_ignored","Ignored by all trains");
  local_428 = 0;
  uStack_420 = 0;
  uStack_41c = 0;
  uStack_418 = 0;
  uStack_414 = 0;
  local_410 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_428,lVar13);
  local_390 = (void *)0x0;
  pvStack_388 = (void *)0x0;
  local_380 = (void *)0x0;
  local_390 = (void *)thunk_FUN_140983da8(0x40);
  pvStack_388 = (void *)((longlong)local_390 + 0x40);
  local_380 = pvStack_388;
  FUN_14041eae0(&local_448,&local_408,local_390);
  _eh_vector_destructor_iterator_(&local_448,0x20,2,thunk_FUN_140002d30);
  lVar13 = FUN_1402d82e0("signal_alert_opt_default","Same wait time as global option");
  local_328 = 0;
  uStack_320 = 0;
  local_318 = 0;
  local_310 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_328,lVar13);
  lVar13 = FUN_1402d82e0("signal_alert_opt_custom","Custom wait time");
  local_308 = 0;
  uStack_300 = 0;
  local_2f8 = 0;
  local_2f0 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar13 + lVar30) != '\0');
  FUN_140002c00(&local_308,lVar13);
  local_408 = (int *)&local_328;
  ppvStack_400 = &local_2e8;
  FUN_1405bde30(&local_2e8,&local_408);
  _eh_vector_destructor_iterator_(&local_328,0x20,2,thunk_FUN_140002d30);
  puVar32 = param_6;
  if (*(longlong *)(param_4 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  FUN_14057c820(param_1 + 0x21b8,*(longlong *)(param_4 + 600),*param_6,0,param_6 + 0x12);
  local_3b8[0] = FUN_140762660(lVar27,local_330,param_3 + 0x118);
  local_448 = (char *)0x0;
  uStack_440 = 0;
  local_448 = (char *)FUN_140003270(0x20);
  uVar7 = s___signal_filter_tags_140a9fb50._12_4_;
  uVar6 = s___signal_filter_tags_140a9fb50._8_4_;
  uVar5 = s___signal_filter_tags_140a9fb50._4_4_;
  local_438 = (undefined8 *)0x14;
  local_430 = 0x1f;
  *(undefined4 *)local_448 = s___signal_filter_tags_140a9fb50._0_4_;
  *(undefined4 *)(local_448 + 4) = uVar5;
  *(undefined4 *)(local_448 + 8) = uVar6;
  *(undefined4 *)(local_448 + 0xc) = uVar7;
  *(undefined4 *)(local_448 + 0x10) = s___signal_filter_tags_140a9fb50._16_4_;
  local_448[0x14] = '\0';
  FUN_1405752e0(param_1 + 0x2088);
  FUN_140002d30(&local_448);
  local_3e8 = (longlong *)0x0;
  plStack_3e0 = (longlong *)0x0;
  local_3d8 = 0;
  plVar21 = (longlong *)puVar32[0xf];
  plVar22 = (longlong *)0x0;
  local_466[0] = DAT_140b5ecac;
  for (lVar27 = puVar32[0x10] - (longlong)plVar21 >> 3; DAT_140b5ecac = local_466[0], 0 < lVar27;
      lVar27 = lVar27 + -1) {
    if (((plVar22 == plStack_3e0) || (*plVar21 < *plVar22)) &&
       ((plVar22 == local_3e8 || (plVar22[-1] < *plVar21)))) {
      piVar17 = (int *)FUN_140364880(&local_3e8,plVar22,plVar21);
    }
    else {
      FUN_1403f3710(&local_3e8,&local_408,plVar21);
      piVar17 = local_408;
    }
    plVar22 = (longlong *)(piVar17 + 2);
    plVar21 = plVar21 + 1;
    local_466[0] = DAT_140b5ecac;
  }
  local_448 = (char *)*puVar32;
  uStack_420 = 0x3f800000;
  uStack_41c = 0x40000000;
  uStack_418 = 0;
  local_430 = 1;
  local_438 = &DAT_140b5abc0;
  local_428 = 0;
  if (*(char *)(param_1 + 0xd8) == '\0') {
    FUN_1407cb8c0(param_1 + 0xa0);
    *(undefined1 *)(param_1 + 0xd8) = 1;
  }
  else {
    *(char **)(param_1 + 0xa0) = local_448;
    FUN_140334440(param_1 + 0xa8);
  }
  FUN_1402531c0(&uStack_440);
  if (1 < local_430) {
    free(local_438);
  }
  local_3f0 = (int *)puVar32[0x12];
  piVar17 = (int *)puVar32[0x13];
  local_408 = piVar17;
  if (local_3f0 != piVar17) {
    do {
      plVar21 = *(longlong **)(local_3f0 + 6);
      puVar32 = param_6;
      pvVar14 = local_2d0;
      pvVar26 = local_2c0;
      for (plVar22 = *(longlong **)(local_3f0 + 4); param_6 = puVar32, local_2d0 = pvVar14,
          local_2c0 = pvVar26, plVar22 != plVar21; plVar22 = plVar22 + 2) {
        if ((plVar22 == (longlong *)0x0) || (plVar20 = plVar22, (char)plVar22[1] != '\v')) {
          plVar20 = (longlong *)0x0;
        }
        if (plVar20 == (longlong *)0x0) {
          if ((plVar22 != (longlong *)0x0) && ((char)plVar22[1] == '\x11')) {
            puVar28 = *(ulonglong **)*plVar22;
            puVar1 = (ulonglong *)((longlong *)*plVar22)[1];
            if (puVar28 != puVar1) {
              do {
                uVar2 = *puVar28;
                for (puVar15 = *(ulonglong **)
                                (*(longlong *)(param_1 + 0xb0) +
                                (uVar2 % (ulonglong)*(uint *)(param_1 + 0xb8)) * 8);
                    puVar15 != (ulonglong *)0x0; puVar15 = (ulonglong *)puVar15[1]) {
                  if (uVar2 == *puVar15) goto LAB_14079fe0c;
                }
                puVar15 = (ulonglong *)thunk_FUN_140983da8(0x10);
                *puVar15 = uVar2;
                puVar15[1] = 0;
                FUN_1403414c0(param_1 + 0xa8);
LAB_14079fe0c:
                puVar28 = puVar28 + 1;
              } while (puVar28 != puVar1);
            }
          }
        }
        else {
          FUN_140362640(param_1 + 0xa8);
        }
        piVar17 = local_408;
        puVar32 = param_6;
        pvVar14 = local_2d0;
        pvVar26 = local_2c0;
      }
      local_3f0 = local_3f0 + 0x14;
    } while (local_3f0 != piVar17);
  }
  local_3a8 = (void *)0x0;
  pvStack_3a0 = (void *)0x0;
  local_398 = 0;
  plVar22 = *(longlong **)(local_3b0 + 0x140);
  puVar24 = (undefined8 *)*plVar22;
  if (puVar24 == (undefined8 *)0x0) {
    plVar22 = plVar22 + 1;
    puVar24 = (undefined8 *)*plVar22;
    while (puVar24 == (undefined8 *)0x0) {
      plVar22 = plVar22 + 1;
      puVar24 = (undefined8 *)*plVar22;
    }
  }
  puVar3 = *(undefined8 **)(*(longlong *)(local_3b0 + 0x140) + *(longlong *)(local_3b0 + 0x148) * 8)
  ;
  pvVar25 = local_3a8;
  pvVar4 = pvStack_3a0;
  while (local_3a8 = pvVar25, pvStack_3a0 = pvVar4, puVar24 != puVar3) {
    puVar16 = (undefined8 *)FUN_1407bdc30(&local_3a8);
    *puVar16 = *puVar24;
    plVar21 = puVar24 + 8;
    if (0xf < (ulonglong)puVar24[0xb]) {
      plVar21 = (longlong *)*plVar21;
    }
    plVar20 = puVar24 + 0xc;
    if (0xf < (ulonglong)puVar24[0xf]) {
      plVar20 = (longlong *)*plVar20;
    }
    lVar27 = FUN_1402d82e0(plVar20,plVar21);
    lVar13 = -1;
    do {
      lVar13 = lVar13 + 1;
    } while (*(char *)(lVar27 + lVar13) != '\0');
    FUN_140030630(puVar16 + 1,lVar27);
    puVar24 = (undefined8 *)puVar24[0x13];
    while (pvVar25 = local_3a8, pvVar4 = pvStack_3a0, puVar24 == (undefined8 *)0x0) {
      plVar22 = plVar22 + 1;
      puVar24 = (undefined8 *)*plVar22;
    }
  }
  if (pvVar25 != pvVar4) {
    iVar19 = 0;
    for (lVar27 = ((longlong)pvVar4 - (longlong)pvVar25 >> 3) * -0x3333333333333333; lVar27 != 0;
        lVar27 = lVar27 >> 1) {
      iVar19 = iVar19 + 1;
    }
    FUN_1407c9a70(pvVar25,pvVar4,(longlong)(iVar19 + -1) * 2,local_res20 & 0xff);
    if ((longlong)pvVar4 - (longlong)pvVar25 < 0x488) {
      FUN_1407c4bc0(pvVar25,pvVar4);
    }
    else {
      FUN_1407c4bc0(pvVar25,(longlong)pvVar25 + 0x460);
      FUN_1407c4e20((longlong)pvVar25 + 0x460,pvVar4);
    }
  }
  lVar27 = FUN_1402d82e0("pick_placeholder","< Pick >");
  local_360 = (void *)0x0;
  uStack_358 = 0;
  local_350 = 0;
  local_348 = 0;
  lVar13 = -1;
  do {
    lVar13 = lVar13 + 1;
  } while (*(char *)(lVar27 + lVar13) != '\0');
  FUN_140002c00(&local_360,lVar27);
  lVar27 = local_3b0;
  local_340 = 0;
  lVar13 = FUN_1404148d0(local_3b0,*(undefined4 *)(puVar32 + 6),puVar32[7]);
  if (lVar13 != 0) {
    plVar22 = (longlong *)(lVar13 + 0x38);
    if (0xf < *(ulonglong *)(lVar13 + 0x50)) {
      plVar22 = (longlong *)*plVar22;
    }
    plVar21 = (longlong *)(lVar13 + 0x58);
    if (0xf < *(ulonglong *)(lVar13 + 0x70)) {
      plVar21 = (longlong *)*plVar21;
    }
    lVar30 = FUN_1402d82e0(plVar21,plVar22);
    lVar31 = -1;
    do {
      lVar31 = lVar31 + 1;
    } while (*(char *)(lVar30 + lVar31) != '\0');
    FUN_140030630(&local_360,lVar30);
    local_340 = *(undefined8 *)(lVar13 + 0x28);
  }
  if (*(char *)(param_1 + 0x2468) != '\0') {
    iVar19 = FUN_14079d9c0(param_1,param_3,local_338);
    if ((-1 < iVar19) &&
       (iVar19 < (int)(*(longlong *)(param_1 + 0xb28) - *(longlong *)(param_1 + 0xb20) >> 3) *
                 -0x3d70a3d7)) {
      piVar17 = (int *)FUN_1402f7860(&local_118,puVar32);
      local_408 = piVar17;
      FUN_140339140((longlong)iVar19 * 200 + *(longlong *)(param_1 + 0xb20),piVar17);
      FUN_1402f79c0(piVar17);
      *(undefined1 *)(param_1 + 0x2468) = 0;
    }
  }
  uVar2 = local_res20;
  local_2a0 = local_466;
  local_290 = (undefined1 *)&param_5;
  local_280 = &local_3f4;
  lStack_278 = local_2b0[0];
  local_270 = lVar27;
  local_260 = &local_460;
  pcStack_258 = &local_468;
  local_250 = local_1e8;
  ppiStack_248 = &local_3d0;
  local_240 = &local_360;
  ppvStack_238 = &local_3a8;
  local_230 = &local_340;
  ppvStack_228 = &local_378;
  local_220 = &local_2e8;
  ppvStack_218 = &local_390;
  local_210 = &local_467;
  local_200 = local_338;
  lVar13 = local_200;
  uStack_1f8 = local_res20;
  local_1f0 = local_3b8;
  local_438 = (undefined8 *)0xf;
  local_430 = 0xf;
  local_448 = (char *)s___signal_editor_140a9fb88._0_8_;
  uStack_440 = (ulonglong)
               CONCAT16(s___signal_editor_140a9fb88[0xe],
                        CONCAT24(s___signal_editor_140a9fb88._12_2_,
                                 s___signal_editor_140a9fb88._8_4_));
  lStack_f0 = local_2b0[0];
  local_e8 = lVar27;
  local_200._0_4_ = (undefined4)local_338;
  local_200._4_4_ = (undefined4)((ulonglong)local_338 >> 0x20);
  uStack_1f8._0_4_ = (undefined4)local_res20;
  uStack_1f8._4_4_ = (undefined4)(local_res20 >> 0x20);
  local_78 = (undefined4)local_200;
  uStack_74 = local_200._4_4_;
  uStack_70 = (undefined4)uStack_1f8;
  uStack_6c = uStack_1f8._4_4_;
  lStack_298 = param_3;
  lStack_288 = param_1;
  puStack_268 = puVar32;
  pplStack_208 = &local_3e8;
  local_200 = lVar13;
  uStack_1f8 = uVar2;
  local_118 = local_2a0;
  lStack_110 = param_3;
  local_108 = local_290;
  lStack_100 = param_1;
  local_f8 = local_280;
  puStack_e0 = puVar32;
  local_d8 = local_260;
  pcStack_d0 = pcStack_258;
  local_c8 = local_250;
  ppiStack_c0 = ppiStack_248;
  local_b8 = local_240;
  ppvStack_b0 = ppvStack_238;
  local_a8 = local_230;
  ppvStack_a0 = ppvStack_228;
  local_98 = local_220;
  ppvStack_90 = ppvStack_218;
  local_68 = local_1f0;
  local_88 = local_210;
  pplStack_80 = &local_3e8;
  FUN_1407c5020(param_3,&local_448);
  if (0xf < local_430) {
    pcVar18 = local_448;
    if ((0xfff < local_430 + 1) &&
       (pcVar18 = *(char **)(local_448 + -8), (char *)0x1f < local_448 + (-8 - (longlong)pcVar18)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar18);
  }
  puVar24 = (undefined8 *)(param_1 + 0xa00);
  uVar23 = *(ulonglong *)(param_1 + 0xa18);
  if (uVar23 < 0xf) {
    uVar29 = 0x7fffffffffffffff;
    if (uVar23 <= 0x7fffffffffffffff - (uVar23 >> 1)) {
      uVar23 = uVar23 + (uVar23 >> 1);
      uVar29 = 0xf;
      if (0xf < uVar23) {
        uVar29 = uVar23;
      }
    }
    pcVar18 = (char *)FUN_140003270(uVar29 + 1);
    *(undefined8 *)(param_1 + 0xa10) = 0xf;
    *(ulonglong *)(param_1 + 0xa18) = uVar29;
    *(undefined8 *)pcVar18 = s___signal_editor_140a9fb88._0_8_;
    *(undefined4 *)(pcVar18 + 8) = s___signal_editor_140a9fb88._8_4_;
    *(undefined2 *)(pcVar18 + 0xc) = s___signal_editor_140a9fb88._12_2_;
    pcVar18[0xe] = s___signal_editor_140a9fb88[0xe];
    pcVar18[0xf] = '\0';
    *puVar24 = pcVar18;
  }
  else {
    if (0xf < uVar23) {
      puVar24 = (undefined8 *)*puVar24;
    }
    *(undefined8 *)(param_1 + 0xa10) = 0xf;
    memmove(puVar24,"##signal_editor",0xf);
    *(undefined1 *)((longlong)puVar24 + 0xf) = 0;
  }
  FUN_140573630(param_1 + 0x2088);
  if (*(char *)(param_1 + 0x2168) != '\0') {
    local_res20 = *(ulonglong *)(param_1 + 0x2160);
    uVar23 = (longlong)plStack_3e0 - (longlong)local_3e8 >> 3;
    plVar22 = local_3e8;
    while (uVar29 = uVar23, 0 < (longlong)uVar29) {
      uVar23 = uVar29 >> 1;
      if (plVar22[uVar23] < (longlong)local_res20) {
        plVar22 = plVar22 + uVar23 + 1;
        uVar23 = uVar29 + (-1 - uVar23);
      }
    }
    if (((plVar22 == plStack_3e0) || ((longlong)local_res20 < *plVar22)) || (plVar22 == plVar22 + 1)
       ) {
      FUN_1403f2cb0(&local_3e8,local_2b0);
    }
    else {
      FUN_1403bbe50(&local_3e8,&local_res20);
    }
    local_168 = local_170;
    plVar22 = local_3e8;
    for (lVar27 = (longlong)plStack_3e0 - (longlong)local_3e8 >> 3; 0 < lVar27; lVar27 = lVar27 + -1
        ) {
      if (local_168 < local_160) {
        *local_168 = *plVar22;
        local_168 = local_168 + 1;
      }
      else {
        FUN_140253b10(&local_170,plVar22);
      }
      plVar22 = plVar22 + 1;
    }
    local_468 = '\x01';
  }
  DAT_140be285d = local_467;
  FUN_14057cd10(param_1 + 0x21b8,uVar2);
  if (local_468 == '\0') {
    DAT_140b5ecac = local_466[0];
    DAT_140be285c = *(char *)(puVar32 + 1) != local_1e0;
    *(undefined1 *)(param_2 + 200) = 0;
    if (0xf < local_348) {
      pvVar25 = local_360;
      if ((0xfff < local_348 + 1) &&
         (pvVar25 = *(void **)((longlong)local_360 + -8),
         0x1f < (ulonglong)((longlong)local_360 + (-8 - (longlong)pvVar25)))) goto LAB_1407a07b9;
      free(pvVar25);
    }
    pvVar4 = pvStack_3a0;
    local_350 = 0;
    local_348 = 0xf;
    local_360 = (void *)((ulonglong)local_360 & 0xffffffffffffff00);
    for (pvVar25 = local_3a8; pvVar25 != pvVar4; pvVar25 = (void *)((longlong)pvVar25 + 0x28)) {
      FUN_140002d30((longlong)pvVar25 + 8);
    }
    if (local_3a8 != (void *)0x0) {
      free(local_3a8);
    }
    pvVar25 = local_2e8;
    if (local_3e8 != (longlong *)0x0) {
      free(local_3e8);
      pvVar25 = local_2e8;
    }
    for (; pvVar25 != local_2e0; pvVar25 = (void *)((longlong)pvVar25 + 0x20)) {
      FUN_140002d30(pvVar25);
    }
    pvVar25 = local_390;
    pvVar4 = pvStack_388;
    if (local_2e8 != (void *)0x0) {
      free(local_2e8);
      pvVar25 = local_390;
      pvVar4 = pvStack_388;
    }
    for (; pvVar10 = pvStack_388, pvVar25 != pvStack_388;
        pvVar25 = (void *)((longlong)pvVar25 + 0x20)) {
      pvStack_388 = pvVar4;
      FUN_140002d30(pvVar25);
      pvVar4 = pvStack_388;
      pvStack_388 = pvVar10;
    }
    pvVar25 = pvVar14;
    pvStack_388 = pvVar4;
    if (local_390 != (void *)0x0) {
      free(local_390);
    }
    for (; pvVar25 != pvVar26; pvVar25 = (void *)((longlong)pvVar25 + 0x20)) {
      FUN_140002d30(pvVar25);
    }
    free(pvVar14);
    pvVar14 = pvStack_370;
    for (pvVar26 = local_378; pvVar26 != pvVar14; pvVar26 = (void *)((longlong)pvVar26 + 0x20)) {
      FUN_140002d30(pvVar26);
    }
    if (local_378 != (void *)0x0) {
      free(local_378);
    }
    pvVar26 = local_460;
    pvVar14 = pvStack_458;
    if (local_3d0 != (int *)0x0) {
      free(local_3d0);
      pvVar26 = local_460;
      pvVar14 = pvStack_458;
    }
    for (; pvVar25 = pvStack_458, pvVar26 != pvStack_458;
        pvVar26 = (void *)((longlong)pvVar26 + 0x20)) {
      pvStack_458 = pvVar14;
      FUN_140002d30(pvVar26);
      pvVar14 = pvStack_458;
      pvStack_458 = pvVar25;
    }
    pvStack_458 = pvVar14;
    pvVar26 = local_158;
    if (local_460 != (void *)0x0) {
      free(local_460);
      pvVar26 = local_158;
    }
    for (; pvVar26 != local_150; pvVar26 = (void *)((longlong)pvVar26 + 0x50)) {
      FUN_14030dbd0(pvVar26);
    }
  }
  else {
    FUN_1403a81f0(local_330,local_1e8);
    FUN_14033bf80(param_2,local_1e8);
    *(undefined1 *)(param_2 + 200) = 1;
    if (0xf < local_348) {
      pvVar25 = local_360;
      if ((0xfff < local_348 + 1) &&
         (pvVar25 = *(void **)((longlong)local_360 + -8),
         0x1f < (ulonglong)((longlong)local_360 + (-8 - (longlong)pvVar25)))) {
LAB_1407a07b9:
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar25);
    }
    pvVar4 = pvStack_3a0;
    local_350 = 0;
    local_348 = 0xf;
    local_360 = (void *)((ulonglong)local_360 & 0xffffffffffffff00);
    for (pvVar25 = local_3a8; pvVar25 != pvVar4; pvVar25 = (void *)((longlong)pvVar25 + 0x28)) {
      FUN_140002d30((longlong)pvVar25 + 8);
    }
    if (local_3a8 != (void *)0x0) {
      free(local_3a8);
    }
    pvVar25 = local_2e8;
    if (local_3e8 != (longlong *)0x0) {
      free(local_3e8);
      pvVar25 = local_2e8;
    }
    for (; pvVar25 != local_2e0; pvVar25 = (void *)((longlong)pvVar25 + 0x20)) {
      FUN_140002d30(pvVar25);
    }
    pvVar25 = local_390;
    pvVar4 = pvStack_388;
    if (local_2e8 != (void *)0x0) {
      free(local_2e8);
      pvVar25 = local_390;
      pvVar4 = pvStack_388;
    }
    for (; pvVar10 = pvStack_388, pvVar25 != pvStack_388;
        pvVar25 = (void *)((longlong)pvVar25 + 0x20)) {
      pvStack_388 = pvVar4;
      FUN_140002d30(pvVar25);
      pvVar4 = pvStack_388;
      pvStack_388 = pvVar10;
    }
    pvVar25 = pvVar14;
    pvStack_388 = pvVar4;
    if (local_390 != (void *)0x0) {
      free(local_390);
    }
    for (; pvVar25 != pvVar26; pvVar25 = (void *)((longlong)pvVar25 + 0x20)) {
      FUN_140002d30(pvVar25);
    }
    free(pvVar14);
    pvVar14 = pvStack_370;
    for (pvVar26 = local_378; pvVar26 != pvVar14; pvVar26 = (void *)((longlong)pvVar26 + 0x20)) {
      FUN_140002d30(pvVar26);
    }
    if (local_378 != (void *)0x0) {
      free(local_378);
    }
    pvVar26 = local_460;
    pvVar14 = pvStack_458;
    if (local_3d0 != (int *)0x0) {
      free(local_3d0);
      pvVar26 = local_460;
      pvVar14 = pvStack_458;
    }
    for (; pvVar25 = pvStack_458, pvVar26 != pvStack_458;
        pvVar26 = (void *)((longlong)pvVar26 + 0x20)) {
      pvStack_458 = pvVar14;
      FUN_140002d30(pvVar26);
      pvVar14 = pvStack_458;
      pvStack_458 = pvVar25;
    }
    pvStack_458 = pvVar14;
    pvVar26 = local_158;
    if (local_460 != (void *)0x0) {
      free(local_460);
      pvVar26 = local_158;
    }
    for (; pvVar26 != local_150; pvVar26 = (void *)((longlong)pvVar26 + 0x50)) {
      FUN_14030dbd0(pvVar26);
    }
  }
  if (local_158 != (void *)0x0) {
    free(local_158);
  }
  if (local_170 != (longlong *)0x0) {
    free(local_170);
  }
  FUN_140002d30(local_1d8);
  return param_2;
}


// Incoming references
// 0xc392a8 DATA caller none
// 0x7b2657 UNCONDITIONAL_CALL caller 1407afe40
