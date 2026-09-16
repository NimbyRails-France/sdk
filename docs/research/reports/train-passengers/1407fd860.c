// Candidate VA 1407fd860; RVA 0x7fd860
// Ghidra inferred prototype: undefined FUN_1407fd860()

void FUN_1407fd860(ulonglong *param_1,undefined8 *param_2,undefined8 param_3,undefined8 param_4,
                  undefined8 param_5,longlong param_6,ulonglong param_7,longlong param_8,
                  undefined1 param_9,undefined8 param_10,longlong *param_11,undefined8 param_12,
                  undefined8 param_13)

{
  ulonglong uVar1;
  double dVar2;
  char cVar3;
  int iVar4;
  longlong *plVar5;
  ulonglong uVar6;
  undefined8 uVar7;
  ulonglong *puVar8;
  ulonglong *puVar9;
  longlong *plVar10;
  longlong lVar11;
  ulonglong *puVar12;
  undefined8 uVar13;
  char ******_Memory;
  ulonglong *puVar14;
  undefined1 *_Memory_00;
  char *******pppppppcVar15;
  undefined8 *******pppppppuVar16;
  undefined4 uVar17;
  undefined4 extraout_XMM0_Da;
  longlong *local_4f8;
  char ******local_4f0;
  undefined8 uStack_4e8;
  undefined8 local_4e0;
  ulonglong local_4d8;
  ulonglong *local_4c8;
  undefined8 uStack_4c0;
  ulonglong *local_4b8;
  ulonglong uStack_4b0;
  undefined4 local_4a8;
  undefined4 uStack_4a4;
  undefined4 uStack_4a0;
  undefined4 uStack_49c;
  ulonglong *local_498;
  ulonglong **ppuStack_490;
  longlong local_488;
  ulonglong local_480;
  ulonglong local_478 [2];
  longlong local_468;
  longlong lStack_460;
  char local_458 [8];
  undefined8 ******local_450;
  undefined8 uStack_448;
  undefined8 local_440;
  ulonglong local_438;
  undefined8 ******local_430;
  undefined8 uStack_428;
  undefined8 local_420;
  ulonglong local_418;
  undefined4 local_410;
  undefined4 local_40c;
  undefined1 local_408;
  undefined1 local_407;
  undefined8 local_400;
  undefined4 local_3f8;
  char local_3f4;
  undefined4 local_3f0;
  char local_3ec;
  ulonglong *local_3e8;
  undefined8 *puStack_3e0;
  undefined8 local_3d8;
  ulonglong uStack_3d0;
  undefined8 local_3c8;
  undefined8 uStack_3c0;
  ulonglong *local_3b8;
  ulonglong *puStack_3b0;
  undefined8 *local_3a8;
  undefined1 *puStack_3a0;
  longlong *local_398;
  longlong *plStack_390;
  longlong **local_388;
  char *pcStack_380;
  longlong local_378;
  undefined8 uStack_370;
  undefined8 local_368;
  ulonglong uStack_360;
  longlong local_358;
  undefined8 uStack_350;
  undefined8 local_348;
  ulonglong *puStack_340;
  undefined8 local_338;
  ulonglong *local_328;
  ulonglong *puStack_320;
  undefined8 *local_318;
  undefined1 *puStack_310;
  longlong *local_308;
  longlong *plStack_300;
  longlong **local_2f8;
  char *pcStack_2f0;
  longlong local_2e8;
  undefined8 uStack_2e0;
  undefined8 local_2d8;
  ulonglong uStack_2d0;
  longlong local_2c8;
  undefined8 uStack_2c0;
  undefined4 local_2b8;
  undefined4 uStack_2b4;
  ulonglong *puStack_2b0;
  undefined8 local_2a8;
  undefined1 local_298 [48];
  undefined **local_268;
  undefined1 *local_260;
  undefined8 local_258;
  ulonglong local_250;
  undefined1 local_248 [520];

  plVar10 = param_11;
  uVar1 = *param_1;
  if (uVar1 == 0) {
    return;
  }
  plVar5 = (longlong *)FUN_14033f780(*param_11 + 0x200,uVar1);
  local_4f8 = plVar5;
  local_488 = FUN_14045f300(param_12);
  if (plVar5 == (longlong *)0x0) {
    return;
  }
  if (local_488 == 0) {
    return;
  }
  local_480 = 0;
  lVar11 = *(longlong *)(param_8 + 0x2208);
  for (puVar12 = *(ulonglong **)
                  (*(longlong *)(lVar11 + 0x90) + (uVar1 % (ulonglong)*(uint *)(lVar11 + 0x98)) * 8)
      ; puVar12 != (ulonglong *)0x0; puVar12 = (ulonglong *)puVar12[2]) {
    if (uVar1 == *puVar12) {
      if ((puVar12 != (ulonglong *)0x0) &&
         (puVar12 !=
          *(ulonglong **)(*(longlong *)(lVar11 + 0x90) + *(longlong *)(lVar11 + 0x98) * 8))) {
        local_480 = puVar12[1];
      }
      break;
    }
  }
  local_458[0] = '\0';
  uStack_448 = 0;
  local_440 = 0;
  local_438 = 0xf;
  local_450 = (undefined8 *******)0x0;
  uStack_428 = 0;
  local_420 = 0;
  local_418 = 0xf;
  local_430 = (undefined8 *******)0x0;
  local_400 = 0;
  local_3f8 = 0;
  local_3f4 = '\0';
  local_3ec = '\0';
  uVar6 = FUN_14033f780(*plVar10 + 0x200,uVar1);
  param_1[0x53d] = uVar6;
  param_1[0x53f] = plVar10[1];
  param_1[0x53e] = uVar1;
  local_4f0 = (char ******)FUN_1407fb920(param_1 + 0x540);
  local_4e0 = 0;
  local_4d8 = 0;
  local_4f0 = (char ******)FUN_140003270(0x20);
  uVar7 = s___train_editor_train_shifts_140aa43a0._8_8_;
  local_4e0 = 0x1b;
  local_4d8 = 0x1f;
  *local_4f0 = (char *****)s___train_editor_train_shifts_140aa43a0._0_8_;
  local_4f0[1] = (char *****)uVar7;
  local_4f0[2] = (char *****)s___train_editor_train_shifts_140aa43a0._16_8_;
  *(undefined2 *)(local_4f0 + 3) = s___train_editor_train_shifts_140aa43a0._24_2_;
  *(char *)((longlong)local_4f0 + 0x1a) = s___train_editor_train_shifts_140aa43a0[0x1a];
  *(char *)((longlong)local_4f0 + 0x1b) = '\0';
  FUN_1407fc990(param_1 + 0x547,local_4f0,plVar10,uVar1,&local_4f0);
  *(undefined1 *)((longlong)param_1 + 0x2bbc) = 0;
  *(undefined1 *)(param_1 + 0x579) = 0;
  *(undefined1 *)(param_1 + 0x57c) = 0;
  *(undefined1 *)(param_1 + 0x57f) = 0;
  FUN_14028fc90(&local_4c8,*local_4f8);
  local_4f0 = (char ******)0x0;
  uStack_4e8 = 0;
  local_4f0 = (char ******)FUN_140003270(0x20);
  uVar7 = s___train_editor_tag_picker_140aa4618._8_8_;
  local_4e0 = 0x19;
  local_4d8 = 0x1f;
  *local_4f0 = (char *****)s___train_editor_tag_picker_140aa4618._0_8_;
  local_4f0[1] = (char *****)uVar7;
  local_4f0[2] = (char *****)s___train_editor_tag_picker_140aa4618._16_8_;
  *(char *)(local_4f0 + 3) = s___train_editor_tag_picker_140aa4618[0x18];
  *(char *)((longlong)local_4f0 + 0x19) = '\0';
  FUN_14029c760(&local_3e8,param_9);
  FUN_1405752e0(param_1 + 0x151,&local_3e8);
  if (0xf < uStack_3d0) {
    puVar12 = local_3e8;
    if ((0xfff < uStack_3d0 + 1) &&
       (puVar12 = (ulonglong *)local_3e8[-1],
       0x1f < (ulonglong)((longlong)local_3e8 + (-8 - (longlong)puVar12)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar12);
  }
  if (0xf < local_4d8) {
    _Memory = local_4f0;
    if ((0xfff < local_4d8 + 1) &&
       (_Memory = (char ******)local_4f0[-1],
       (char *)0x1f < (char *)((longlong)local_4f0 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
  }
  if (0xf < uStack_4b0) {
    puVar12 = local_4c8;
    if ((0xfff < uStack_4b0 + 1) &&
       (puVar12 = (ulonglong *)local_4c8[-1],
       0x1f < (ulonglong)((longlong)local_4c8 + (-8 - (longlong)puVar12)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar12);
  }
  plVar10 = local_4f8 + 0x28;
  if (*(longlong *)(param_6 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  uVar17 = FUN_14057c820(param_1 + 0x177,*(longlong *)(param_6 + 600),*local_4f8,0,plVar10);
  plVar5 = local_4f8;
  puVar12 = param_1 + 0x123;
  if ((ulonglong *)param_1[0x125] == (ulonglong *)0x0) {
LAB_1407fdc4a:
    uVar7 = FUN_14033b7f0(puVar12,local_4f8);
    uVar17 = FUN_140339800(uVar7,plVar5);
  }
  else {
    puVar8 = (ulonglong *)param_1[0x125];
    puVar14 = puVar12;
    do {
      if ((longlong)puVar8[4] < *local_4f8) {
        puVar9 = (ulonglong *)*puVar8;
      }
      else {
        puVar9 = (ulonglong *)puVar8[1];
        puVar14 = puVar8;
      }
      puVar8 = puVar9;
    } while (puVar9 != (ulonglong *)0x0);
    if ((puVar14 == puVar12) || (*local_4f8 < (longlong)puVar14[4])) goto LAB_1407fdc4a;
  }
  if ((ulonglong *)param_1[0x125] != (ulonglong *)0x0) {
    puVar8 = (ulonglong *)param_1[0x125];
    puVar14 = puVar12;
    do {
      if ((longlong)puVar8[4] < *local_4f8) {
        puVar9 = (ulonglong *)*puVar8;
      }
      else {
        puVar9 = (ulonglong *)puVar8[1];
        puVar14 = puVar8;
      }
      puVar8 = puVar9;
    } while (puVar9 != (ulonglong *)0x0);
    if ((puVar14 != puVar12) && ((longlong)puVar14[4] <= *local_4f8)) {
      puVar12 = puVar14 + 5;
      if ((int)puVar14[0x20] != (int)local_4f8[0x1b]) {
        uVar17 = FUN_140339800(puVar12,local_4f8);
      }
      FUN_1407f18a0(uVar17,param_5,puVar12);
      plVar10 = local_4f8;
      FUN_1407ef700(param_1 + 0x1cd,param_5,*(undefined8 *)(param_7 + 0x410),puVar12,local_4f8,
                    local_488);
    }
  }
  uVar7 = param_13;
  local_478[0] = param_1[0xb2];
  param_9 = *(undefined1 *)((longlong)param_1 + 0x2491);
  puStack_3a0 = &param_9;
  local_398 = &local_488;
  plStack_390 = param_11;
  local_388 = &local_4f8;
  pcStack_380 = local_458;
  local_378 = param_6;
  local_368 = param_5;
  uStack_360 = param_7;
  local_358 = param_8;
  uStack_350 = param_10;
  local_348 = param_13;
  puStack_340 = &local_480;
  local_4b8 = (ulonglong *)0xf;
  uStack_4b0 = 0xf;
  local_4c8 = (ulonglong *)s___trains_editor_140aa4608._0_8_;
  uStack_4c0._0_6_ = CONCAT24(s___trains_editor_140aa4608._12_2_,s___trains_editor_140aa4608._8_4_);
  uStack_4c0._0_7_ = CONCAT16(s___trains_editor_140aa4608[0xe],(undefined6)uStack_4c0);
  uStack_4c0 = (ulonglong *)(ulonglong)(uint7)uStack_4c0;
  plStack_300 = param_11;
  local_2e8 = param_6;
  local_2d8 = param_5;
  uStack_2d0 = param_7;
  local_2c8 = param_8;
  uStack_2c0 = param_10;
  local_348._0_4_ = (undefined4)param_13;
  local_348._4_4_ = (undefined4)((ulonglong)param_13 >> 0x20);
  local_2b8 = (undefined4)local_348;
  uStack_2b4 = local_348._4_4_;
  local_3b8 = local_478;
  puStack_3b0 = param_1;
  local_3a8 = param_2;
  uStack_370 = param_4;
  local_348 = uVar7;
  local_338 = param_3;
  local_328 = local_478;
  puStack_320 = param_1;
  local_318 = param_2;
  puStack_310 = puStack_3a0;
  local_308 = local_398;
  local_2f8 = local_388;
  pcStack_2f0 = pcStack_380;
  uStack_2e0 = param_4;
  local_2a8 = param_3;
  puStack_2b0 = puStack_340;
  FUN_14080bea0(param_2,&local_4c8);
  if (0xf < uStack_4b0) {
    puVar12 = local_4c8;
    if ((0xfff < uStack_4b0 + 1) &&
       (puVar12 = (ulonglong *)local_4c8[-1],
       0x1f < (ulonglong)((longlong)local_4c8 + (-8 - (longlong)puVar12)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar12);
  }
  if (local_478[0] == 0) {
    local_3d8 = param_5;
    uStack_3d0 = param_7;
    local_3c8 = uVar7;
    uStack_3c0 = param_8;
    local_4f0 = (char ******)0x0;
    uStack_4e8 = 0;
    local_3e8 = param_1;
    puStack_3e0 = param_2;
    local_4f0 = (char ******)FUN_140003270(0x20);
    uVar13 = s___trains_editor_cars_140aa43c0._8_8_;
    local_4e0 = 0x14;
    local_4d8 = 0x1f;
    *local_4f0 = (char *****)s___trains_editor_cars_140aa43c0._0_8_;
    local_4f0[1] = (char *****)uVar13;
    *(undefined4 *)(local_4f0 + 2) = s___trains_editor_cars_140aa43c0._16_4_;
    *(char *)((longlong)local_4f0 + 0x14) = '\0';
    local_4c8 = local_3e8;
    uStack_4c0 = puStack_3e0;
    local_4b8 = (ulonglong *)local_3d8;
    uStack_4b0 = uStack_3d0;
    local_4a8 = (undefined4)local_3c8;
    uStack_4a4 = local_3c8._4_4_;
    uStack_4a0 = (undefined4)uStack_3c0;
    uStack_49c = uStack_3c0._4_4_;
    FUN_14080c140(param_2,&local_4f0);
    if (0xf < local_4d8) {
      FUN_140003040(&local_4f0,local_4f0);
    }
    if (local_478[0] == 0) {
      local_4b8 = (ulonglong *)0xf;
      uStack_4b0 = 0xf;
      local_4c8 = (ulonglong *)s___trains_editor_140aa4608._0_8_;
      uStack_4c0._0_6_ =
           CONCAT24(s___trains_editor_140aa4608._12_2_,s___trains_editor_140aa4608._8_4_);
      uStack_4c0._0_7_ = CONCAT16(s___trains_editor_140aa4608[0xe],(undefined6)uStack_4c0);
      uStack_4c0 = (ulonglong *)(ulonglong)(uint7)uStack_4c0;
      FUN_1407ee940(param_1 + 0x1cd,param_2,param_5,*(undefined8 *)(param_7 + 0x410),plVar10,uVar7);
    }
  }
  plVar10 = param_11;
  puVar12 = param_1 + 0x1cd;
  if ((char)local_3f8 != '\0') {
    FUN_1407fb620(param_1,param_5,param_11,*local_4f8);
  }
  if (local_3f8._1_1_ != '\0') {
    local_498 = puVar12;
    FUN_1407fb620(param_1,param_5,plVar10,*local_4f8);
    FUN_14032a5d0(param_1 + 0xbb);
    FUN_140329d50(param_1 + 0xbb,&local_4c8);
  }
  puVar8 = param_1 + 0x1cd;
  local_498 = puVar8;
  if ((char)param_1[0x487] != '\0') {
    FUN_1407ef7d0(puVar12,param_2,param_5);
    if (*(char *)((longlong)param_1 + 0x2432) != '\0') {
      plVar10 = (longlong *)FUN_1408078b0(param_6 + 0x228);
      lVar11 = *(longlong *)(param_6 + 0x250);
      (**(code **)(*plVar10 + 0x28))(plVar10);
      FUN_14074c5a0(lVar11 + 0x4e8,&local_4c8);
      local_4c8[1] = local_4c8[1] + 1;
      plVar10[3] = local_4c8[1];
      plVar10[4] = *puVar12;
      puVar8 = local_498;
      if ((ulonglong *)(plVar10 + 5) != param_1 + 0x1e5) {
        FUN_140341ea0(plVar10 + 5,param_1[0x1e5],param_1[0x1e6]);
        puVar8 = param_1 + 0x1cd;
      }
    }
  }
  FUN_1407eb420(param_1 + 0x2f6,param_2);
  uVar7 = FUN_14033b7f0(param_1 + 0x123,local_4f8);
  FUN_140339800(uVar7,puVar8);
  if (local_458[0] != '\0') {
    cVar3 = FUN_1403e3960(puVar8 + 0x18,puVar8 + 0x47);
    if (cVar3 != '\0') {
      FUN_1403f9fe0(param_1 + 0x123,local_4f8);
      puVar8 = param_1 + 0x1cd;
    }
    plVar10 = (longlong *)FUN_140808020(param_6 + 0x228);
    lVar11 = *(longlong *)(param_6 + 0x250);
    (**(code **)(*plVar10 + 0x28))(plVar10);
    FUN_14074c5a0(lVar11 + 0x4e8,&local_4c8);
    local_4c8[1] = local_4c8[1] + 1;
    plVar10[3] = local_4c8[1];
    plVar10[4] = *local_4f8;
    if ((undefined8 *******)(plVar10 + 5) != &local_450) {
      pppppppuVar16 = &local_450;
      if (0xf < local_438) {
        puVar8 = param_1 + 0x1cd;
        pppppppuVar16 = (undefined8 *******)local_450;
      }
      FUN_140030630(plVar10 + 5,pppppppuVar16,local_440);
    }
    if ((undefined8 *******)(plVar10 + 9) != &local_430) {
      pppppppuVar16 = &local_430;
      if (0xf < local_418) {
        pppppppuVar16 = (undefined8 *******)local_430;
      }
      FUN_140030630(plVar10 + 9,pppppppuVar16,local_420);
    }
    *(undefined4 *)(plVar10 + 0xd) = local_410;
    *(undefined4 *)((longlong)plVar10 + 0x6c) = local_40c;
    *(undefined1 *)(plVar10 + 0xe) = local_408;
    *(undefined1 *)((longlong)plVar10 + 0x71) = local_407;
  }
  if (local_3f8._2_1_ != '\0') {
    cVar3 = FUN_1403e3960(puVar8 + 0x18,puVar8 + 0x47);
    if (cVar3 != '\0') {
      FUN_1403f9fe0(param_1 + 0x123,local_4f8);
    }
    lVar11 = FUN_140807180(param_6);
    *(longlong *)(lVar11 + 0x20) = *local_4f8;
  }
  if (param_1[0x53d] != 0) {
    if (*(char *)((longlong)param_1 + 0x2bbc) != '\0') {
      lVar11 = FUN_140806c60(param_6);
      *(undefined8 *)(lVar11 + 0x20) = *(undefined8 *)param_1[0x53d];
      if (*(char *)((longlong)param_1 + 0x2bbc) == '\0') {
        uVar17 = *(undefined4 *)(param_1[0x53d] + 0xb8);
      }
      else {
        uVar17 = (undefined4)param_1[0x577];
      }
      *(undefined4 *)(lVar11 + 0x28) = uVar17;
    }
    if ((char)param_1[0x579] != '\0') {
      lVar11 = FUN_140806e10(param_6);
      *(undefined8 *)(lVar11 + 0x20) = *(undefined8 *)param_1[0x53d];
      *(ulonglong *)(lVar11 + 0x28) = param_1[0x578];
    }
    if ((char)param_1[0x57c] != '\0') {
      lVar11 = FUN_140806fc0(param_6);
      *(undefined8 *)(lVar11 + 0x20) = *(undefined8 *)param_1[0x53d];
      *(ulonglong *)(lVar11 + 0x28) = param_1[0x57a];
      *(ulonglong *)(lVar11 + 0x30) = param_1[0x57b];
      *(undefined1 *)(lVar11 + 0x38) = 1;
    }
    if ((char)param_1[0x57f] != '\0') {
      lVar11 = FUN_140806fc0(param_6);
      *(undefined8 *)(lVar11 + 0x20) = *(undefined8 *)param_1[0x53d];
      *(ulonglong *)(lVar11 + 0x28) = param_1[0x57d];
      *(ulonglong *)(lVar11 + 0x30) = param_1[0x57e];
      *(undefined1 *)(lVar11 + 0x38) = 0;
    }
    if ((char)param_1[0x544] != '\0') {
      param_1[0x541] = param_1[0x543];
    }
    if ((char)param_1[0x546] != '\0') {
      param_1[0x542] = param_1[0x545];
    }
    FUN_140573630(param_1 + 0x552);
    if ((char)param_1[0x54f] != '\0') {
      param_1[0x54c] = param_1[0x54e];
    }
    if ((char)param_1[0x551] != '\0') {
      param_1[0x54d] = param_1[0x550];
    }
  }
  FUN_140573630(param_1 + 0x151);
  if ((char)param_1[0x16d] != '\0') {
    uVar1 = param_1[0x16c];
    FUN_14031f8f0(&local_4c8,local_4f8 + 0x10);
    FUN_1403fcc70(&local_4c8,uVar1);
    lVar11 = FUN_140807330(param_6);
    *(longlong *)(lVar11 + 0x20) = *local_4f8;
    FUN_1402ec820(lVar11 + 0x28,&local_4c8);
    *(char *)(lVar11 + 0x40) = (char)local_4f8[0x13];
    if (local_4c8 != (ulonglong *)0x0) {
      free(local_4c8);
    }
  }
  if (local_3f4 != '\0') {
    lVar11 = FUN_140807330(param_6);
    *(longlong *)(lVar11 + 0x20) = *local_4f8;
    if ((longlong *)(lVar11 + 0x28) != local_4f8 + 0x10) {
      FUN_14032fe20((longlong *)(lVar11 + 0x28),local_4f8[0x10],local_4f8[0x11]);
    }
    *(bool *)(lVar11 + 0x40) = (char)local_4f8[0x13] == '\0';
  }
  if (local_3ec != '\0') {
    lVar11 = FUN_1408073a0(param_6);
    *(longlong *)(lVar11 + 0x20) = *local_4f8;
    *(undefined4 *)(lVar11 + 0x28) = local_3f0;
  }
  FUN_14057cd10(param_1 + 0x177,param_6);
  uVar7 = FUN_14031ff10(local_298,param_1 + 0x136);
  dVar2 = (double)FUN_14044c780(extraout_XMM0_Da,uVar7);
  puVar12 = (ulonglong *)FUN_140572550(&local_3e8,(longlong)(int)dVar2);
  local_468 = FUN_1402d82e0("train_intervention_dialog_desc",
                            "This intervention will reset the train run to follow its current orders, respawn it on its next destination, and relocate its pax to their next stops. The price is ${}."
                           );
  lStack_460 = -1;
  do {
    lStack_460 = lStack_460 + 1;
  } while (*(char *)(local_468 + lStack_460) != '\0');
  local_4c8 = puVar12;
  if (0xf < puVar12[3]) {
    local_4c8 = (ulonglong *)*puVar12;
  }
  uStack_4c0 = (ulonglong *)puVar12[2];
  ppuStack_490 = &local_4c8;
  local_258 = 0;
  local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_260 = local_248;
  local_250 = 500;
  local_498 = (ulonglong *)0xd;
  FUN_140022830(&local_268,&local_468,&local_498);
  local_4f0 = (char ******)0x0;
  uStack_4e8 = 0;
  local_4e0 = 0;
  local_4d8 = 0;
  FUN_140002c00(&local_4f0,local_260);
  local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_260 != local_248) {
    _Memory_00 = local_260;
    if ((0xfff < local_250) &&
       (_Memory_00 = *(undefined1 **)(local_260 + -8),
       (undefined1 *)0x1f < local_260 + (-8 - (longlong)_Memory_00))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory_00);
  }
  if (0xf < uStack_3d0) {
    puVar12 = local_3e8;
    if ((0xfff < uStack_3d0 + 1) &&
       (puVar12 = (ulonglong *)local_3e8[-1],
       0x1f < (ulonglong)((longlong)local_3e8 + (-8 - (longlong)puVar12)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar12);
  }
  local_3d8 = 0;
  uStack_3d0 = 0xf;
  local_3e8 = (ulonglong *)((ulonglong)local_3e8 & 0xffffffffffffff00);
  uVar7 = *param_2;
  local_468 = FUN_1402d82e0("cancel","Cancel");
  lStack_460 = FUN_1402d82e0("reset_train","Reset train");
  local_4c8 = (ulonglong *)0x0;
  uStack_4c0 = (ulonglong *)0x0;
  local_4b8 = (ulonglong *)0x0;
  local_4c8 = (ulonglong *)thunk_FUN_140983da8(0x10);
  uStack_4c0 = local_4c8 + 2;
  local_4b8 = uStack_4c0;
  memmove(local_4c8,&local_468,0x10);
  pppppppcVar15 = &local_4f0;
  if (0xf < local_4d8) {
    pppppppcVar15 = (char *******)local_4f0;
  }
  uVar13 = FUN_1402d82e0("train_intervention_dialog_title","Train intervention");
  iVar4 = FUN_14072f090(uVar7,param_2 + 0x13,uVar13,pppppppcVar15,&local_4c8,0);
  if (iVar4 == 1) {
    lVar11 = FUN_1405bc270(param_6);
    *(undefined4 *)(lVar11 + 0x20) = 0;
    FUN_140329d50(lVar11 + 0x28,&local_4c8);
  }
  if (0xf < local_4d8) {
    pppppppcVar15 = (char *******)local_4f0;
    if ((0xfff < local_4d8 + 1) &&
       (pppppppcVar15 = (char *******)local_4f0[-1],
       0x1f < (ulonglong)((longlong)local_4f0 + (-8 - (longlong)pppppppcVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppppcVar15);
  }
  *(undefined1 *)(param_1 + 0x492) = local_3f8._3_1_;
  *(undefined1 *)((longlong)param_1 + 0x2491) = param_9;
  if (0xf < local_418) {
    pppppppuVar16 = (undefined8 *******)local_430;
    if ((0xfff < local_418 + 1) &&
       (pppppppuVar16 = (undefined8 *******)local_430[-1],
       0x1f < (ulonglong)((longlong)local_430 + (-8 - (longlong)pppppppuVar16))))
    goto LAB_1407fe927;
    free(pppppppuVar16);
  }
  local_420 = 0;
  local_418 = 0xf;
  local_430 = (undefined8 ******)((ulonglong)local_430 & 0xffffffffffffff00);
  if (0xf < local_438) {
    pppppppuVar16 = (undefined8 *******)local_450;
    if ((0xfff < local_438 + 1) &&
       (pppppppuVar16 = (undefined8 *******)local_450[-1],
       0x1f < (ulonglong)((longlong)local_450 + (-8 - (longlong)pppppppuVar16)))) {
LAB_1407fe927:
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppppuVar16);
  }
  return;
}


// Incoming references
// 0xc3ac58 DATA caller none
// 0x8039aa UNCONDITIONAL_CALL caller 140803320
