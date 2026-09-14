// Candidate VA 140424c00; RVA 0x424c00
// Ghidra inferred prototype: undefined FUN_140424c00()

undefined8 FUN_140424c00(longlong *param_1,longlong param_2,ulonglong param_3,undefined8 param_4)

{
  char cVar1;
  ulonglong uVar2;
  undefined4 *puVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  longlong *plVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  int iVar12;
  ulonglong *puVar13;
  longlong lVar14;
  char *pcVar15;
  undefined8 *puVar16;
  ulonglong *puVar17;
  void *pvVar18;
  undefined8 *puVar19;
  longlong lVar20;
  longlong lVar21;
  undefined1 *_Memory;
  longlong *plVar22;
  ulonglong uVar23;
  undefined8 *puVar24;
  char *pcVar25;
  longlong *plVar26;
  longlong *plVar27;
  longlong *plVar28;
  bool bVar29;
  ulonglong *local_res10;
  ulonglong local_res18;
  longlong *local_658;
  longlong **pplStack_650;
  undefined8 local_638;
  undefined8 uStack_630;
  undefined8 uStack_628;
  ulonglong local_620;
  ulonglong uStack_618;
  longlong *local_610;
  ulonglong **local_608;
  longlong local_600;
  longlong *local_5f8;
  undefined8 local_5f0;
  undefined8 local_5e8;
  undefined8 uStack_5e0;
  undefined8 local_5d8;
  ulonglong uStack_5d0;
  char local_5c8;
  longlong *local_5c0;
  longlong *local_5b8;
  ulonglong **local_5b0;
  longlong *local_5a8;
  longlong **pplStack_5a0;
  longlong *local_598;
  longlong lStack_590;
  undefined8 *local_588;
  undefined8 local_580;
  char local_578 [4];
  uint local_574;
  longlong *local_570;
  undefined4 local_568;
  undefined4 uStack_564;
  undefined8 uStack_560;
  ulonglong local_558;
  char *local_548;
  undefined8 uStack_540;
  char *local_538;
  undefined8 uStack_530;
  char *local_528;
  undefined8 uStack_520;
  char *local_518;
  undefined8 uStack_510;
  char *local_508;
  undefined8 uStack_500;
  char *local_4f8;
  undefined8 uStack_4f0;
  char *local_4e8;
  undefined8 uStack_4e0;
  char *local_4d8;
  undefined8 uStack_4d0;
  char *local_4c8;
  undefined8 uStack_4c0;
  char *local_4b8;
  undefined8 uStack_4b0;
  char *local_4a8;
  undefined8 uStack_4a0;
  longlong *local_498;
  longlong **pplStack_490;
  undefined8 local_488;
  longlong **pplStack_480;
  char *local_478;
  undefined8 uStack_470;
  char *local_468;
  undefined8 uStack_460;
  char *local_458;
  undefined8 uStack_450;
  char *local_448;
  undefined8 uStack_440;
  char *local_438;
  undefined8 uStack_430;
  char *local_428;
  undefined8 uStack_420;
  char *local_418;
  undefined8 uStack_410;
  char *local_408;
  undefined8 uStack_400;
  char *local_3f8;
  undefined8 uStack_3f0;
  char *local_3e8;
  undefined8 uStack_3e0;
  char *local_3d8;
  undefined8 uStack_3d0;
  char *local_3c8;
  undefined8 uStack_3c0;
  char *local_3b8;
  undefined8 uStack_3b0;
  char *local_3a8;
  undefined8 uStack_3a0;
  char *local_398;
  undefined8 uStack_390;
  char *local_388;
  undefined8 uStack_380;
  char *local_378;
  undefined8 uStack_370;
  char *local_368;
  undefined8 uStack_360;
  char *local_358;
  undefined8 uStack_350;
  char *local_348;
  undefined8 uStack_340;
  char *local_338;
  undefined8 uStack_330;
  char *local_328;
  undefined8 uStack_320;
  char *local_318;
  undefined8 uStack_310;
  char *local_308;
  undefined8 uStack_300;
  char *local_2f8;
  undefined8 uStack_2f0;
  char *local_2e8;
  undefined8 uStack_2e0;
  longlong *local_2d8;
  undefined8 uStack_2d0;
  longlong *local_2c8;
  longlong lStack_2c0;
  undefined8 local_2b8;
  longlong **pplStack_2b0;
  longlong *local_2a8;
  undefined8 uStack_2a0;
  undefined8 local_298;
  longlong **pplStack_290;
  char *local_288;
  undefined8 uStack_280;
  undefined **local_278;
  undefined1 *local_270;
  undefined8 local_268;
  ulonglong local_260;
  undefined1 local_258 [544];
  
  local_res18 = param_3;
  puVar13 = (ulonglong *)FUN_140439810(param_2 + 0x300,param_4);
  if (puVar13 != (ulonglong *)0x0) {
    uVar23 = puVar13[0xc];
    local_res10 = puVar13;
    local_558 = uVar23;
    lVar14 = FUN_1402a2e80(param_1 + 2,param_4);
    puVar17 = local_res10;
    if (((lVar14 == 0) || (puVar13[0xe] != uVar23)) ||
       (*(char *)(lVar14 + 0x188) != (char)puVar13[0xd])) {
      puVar13[0x20] = puVar13[0x1f];
      local_res10[0x22] = 0;
      local_res10[0x23] = 0;
      uVar23 = local_res10[0x1c];
      uVar2 = local_res10[0x1d];
      if (uVar23 != uVar2) {
        do {
          FUN_140002d30(uVar23 + 8);
          uVar23 = uVar23 + 0x28;
        } while (uVar23 != uVar2);
        puVar17[0x1d] = puVar17[0x1c];
      }
      *(undefined1 *)(local_res10 + 0x1b) = 0;
      local_res18 = *local_res10;
      FUN_1403bbe50(param_1 + 9,&local_res18);
      FUN_1403bbe50(param_1 + 0xc,&local_res18);
      FUN_1404279a0(param_1,param_2,param_4);
      puVar13 = local_res10;
      if (((char)local_res10[0x1b] != '\0') &&
         (puVar17 = local_res10 + 0x1c, *puVar17 == local_res10[0x1d])) {
        local_600 = FUN_1402a2e80(param_1 + 2,param_4);
        if (local_600 == 0) {
          local_5f0 = 0;
          local_5e8 = (char *)0x0;
          uStack_5e0 = 0;
          pcVar15 = (char *)FUN_140003270(0x30);
          uVar4 = s_Internal_compilation_error__no_o_140a71a68._8_8_;
          local_5d8 = 0x2c;
          uStack_5d0 = 0x2f;
          *(undefined8 *)pcVar15 = s_Internal_compilation_error__no_o_140a71a68._0_8_;
          *(undefined8 *)(pcVar15 + 8) = uVar4;
          uVar11 = s_Internal_compilation_error__no_o_140a71a68._28_4_;
          uVar10 = s_Internal_compilation_error__no_o_140a71a68._24_4_;
          uVar9 = s_Internal_compilation_error__no_o_140a71a68._20_4_;
          *(undefined4 *)(pcVar15 + 0x10) = s_Internal_compilation_error__no_o_140a71a68._16_4_;
          *(undefined4 *)(pcVar15 + 0x14) = uVar9;
          *(undefined4 *)(pcVar15 + 0x18) = uVar10;
          *(undefined4 *)(pcVar15 + 0x1c) = uVar11;
          *(undefined8 *)(pcVar15 + 0x20) = s_Internal_compilation_error__no_o_140a71a68._32_8_;
          *(undefined4 *)(pcVar15 + 0x28) = s_Internal_compilation_error__no_o_140a71a68._40_4_;
          pcVar15[0x2c] = '\0';
          puVar16 = (undefined8 *)puVar13[0x1d];
          if (puVar16 == (undefined8 *)puVar13[0x1e]) {
            local_5e8 = pcVar15;
            FUN_1402a41e0(puVar17,puVar16,&local_5f0);
            uVar23 = uStack_5d0;
          }
          else {
            *puVar16 = 0;
            local_5e8._0_4_ = SUB84(pcVar15,0);
            local_5e8._4_4_ = (undefined4)((ulonglong)pcVar15 >> 0x20);
            *(undefined4 *)(puVar16 + 1) = (undefined4)local_5e8;
            *(undefined4 *)((longlong)puVar16 + 0xc) = local_5e8._4_4_;
            *(undefined4 *)(puVar16 + 2) = (undefined4)uStack_5e0;
            *(undefined4 *)((longlong)puVar16 + 0x14) = uStack_5e0._4_4_;
            puVar16[3] = 0x2c;
            puVar16[4] = 0x2f;
            local_5e8 = (char *)((ulonglong)pcVar15 & 0xffffffffffffff00);
            puVar13[0x1d] = puVar13[0x1d] + 0x28;
            uVar23 = 0xf;
          }
          if (uVar23 < 0x10) {
            return 0;
          }
          puVar16 = &local_5e8;
          pcVar15 = local_5e8;
        }
        else {
          lVar14 = *(longlong *)(local_600 + 0x18);
          if (((lVar14 != *(longlong *)(local_600 + 0x20)) && (lVar14 != -8)) &&
             (*(char *)(lVar14 + 0x28) == '\x04')) {
            puVar16 = (undefined8 *)FUN_1402a69e0();
            if (puVar16 != (undefined8 *)0x0) {
              plVar22 = puVar16 + 2;
              if (0xf < (ulonglong)puVar16[3]) {
                puVar16 = (undefined8 *)*puVar16;
              }
              if ((*plVar22 == 0xe) && (iVar12 = memcmp(puVar16,"nimbyscript.v1",0xe), iVar12 == 0))
              {
                puVar16 = (undefined8 *)FUN_1402a69e0(local_600 + 0x18);
                if (puVar16 != (undefined8 *)0x0) {
                  plVar22 = puVar16 + 2;
                  if (0xf < (ulonglong)puVar16[3]) {
                    puVar16 = (undefined8 *)*puVar16;
                  }
                  if ((*plVar22 == 0xd) &&
                     (iVar12 = memcmp(puVar16,"nimbyrails.v1",0xd), iVar12 == 0)) {
                    local_5c0 = &local_600;
                    local_5b0 = &local_res10;
                    local_610 = &local_600;
                    local_608 = &local_res10;
                    puVar24 = (undefined8 *)0x0;
                    for (puVar16 = (undefined8 *)**(undefined8 **)(local_600 + 0xc0);
                        puVar16 != *(undefined8 **)(local_600 + 0xc0);
                        puVar16 = (undefined8 *)*puVar16) {
                      if ((int)puVar24 < (int)*(uint *)(puVar16 + 0x15)) {
                        puVar24 = (undefined8 *)(ulonglong)*(uint *)(puVar16 + 0x15);
                      }
                    }
                    local_res10[0x20] = local_res10[0x1f];
                    local_5b8 = param_1;
                    FUN_140439280(local_res10 + 0x1f);
                    local_res18 = local_res18 & 0xffffffff00000000;
                    plVar22 = *(longlong **)(local_600 + 0xc0);
                    plVar28 = (longlong *)*plVar22;
                    local_5f8 = plVar28;
                    local_570 = plVar22;
                    if (plVar28 != plVar22) {
                      do {
                        pcVar15 = "{} has unknown type";
                        pcVar25 = "Enum type {} for field {} has zero options";
                        plVar27 = plVar28 + 3;
                        puVar24 = (undefined8 *)plVar28[0x12];
                        plVar8 = plVar28;
                        puVar13 = local_res10;
                        for (puVar16 = (undefined8 *)plVar28[0x11]; local_res10 = puVar13,
                            puVar16 != puVar24; puVar16 = puVar16 + 0xc) {
                          local_5f8 = plVar8;
                          if (*(int *)(puVar16 + 6) == 0) {
                            iVar12 = *(int *)(puVar16 + 5);
                            if (((iVar12 != 9) && (iVar12 != 0xc)) && (iVar12 != 2)) {
                              if (iVar12 != 0xe) {
                                if (iVar12 == 0xd) {
                                  if ((char)plVar28[0xd] != '\0') {
                                    lVar14 = *param_1 + 0xd00;
                                    goto LAB_140425207;
                                  }
                                  lVar14 = FUN_1404393c0(*param_1 + 0xd30);
                                  goto joined_r0x000140425216;
                                }
                                goto LAB_140425218;
                              }
                              lVar14 = FUN_1402a7b10(local_600 + 0x70);
                              puVar13 = local_res10;
                              if (lVar14 == 0) {
                                local_638 = 0;
                                local_598 = plVar27;
                                if (0xf < (ulonglong)plVar28[6]) {
                                  local_598 = (longlong *)*plVar27;
                                }
                                lStack_590 = plVar28[5];
                                local_2b8 = 0xd;
                                pplStack_2b0 = &local_598;
                                uStack_2a0 = 0x13;
                                local_568 = 0xd;
                                uStack_564 = 0;
                                pplStack_650 = (longlong **)&DAT_00000013;
                                local_658 = (longlong *)pcVar15;
                                local_2c8 = local_598;
                                lStack_2c0 = lStack_590;
                                local_2a8 = (longlong *)pcVar15;
                                uStack_560 = pplStack_2b0;
                                FUN_140021e90(&uStack_630,&local_658,&local_568);
                                FUN_1402a3bd0(puVar13 + 0x1c);
                              }
                              else {
                                if (*(longlong *)(lVar14 + 0x50) != 0) goto LAB_140425098;
                                local_638 = 0;
                                uStack_2d0 = 0x2a;
                                lStack_590 = 0x2a;
                                local_598 = (longlong *)pcVar25;
                                local_2d8 = (longlong *)pcVar25;
                                FUN_140438d40(&uStack_630,&local_598,lVar14,plVar27);
                                FUN_1402a3bd0(puVar13 + 0x1c);
                              }
                              goto LAB_140425079;
                            }
                          }
                          else {
                            if (((*(int *)(puVar16 + 6) == 1) && ((char)plVar28[0xd] != '\0')) &&
                               (*(int *)(puVar16 + 5) == 0xd)) {
                              lVar14 = *param_1 + 0xd18;
LAB_140425207:
                              lVar14 = FUN_1404393c0(lVar14);
joined_r0x000140425216:
                              if (lVar14 != 0) {
                                pcVar25 = "Enum type {} for field {} has zero options";
                                pcVar15 = "{} has unknown type";
                                goto LAB_140425098;
                              }
                            }
LAB_140425218:
                            puVar13 = local_res10;
                            local_638 = 0;
                            local_598 = plVar27;
                            if (0xf < (ulonglong)plVar28[6]) {
                              local_598 = (longlong *)*plVar27;
                            }
                            lStack_590 = plVar28[5];
                            local_588 = puVar16;
                            if (0xf < (ulonglong)puVar16[3]) {
                              local_588 = (undefined8 *)*puVar16;
                            }
                            local_580 = puVar16[2];
                            local_298 = 0xdd;
                            pplStack_650 = &local_598;
                            local_288 = "{}::{} is of an unsupported type";
                            uStack_280 = 0x20;
                            local_658 = (longlong *)0xdd;
                            local_568 = 0x40a71ce0;
                            uStack_564 = 1;
                            uStack_560._0_4_ = 0x20;
                            uStack_560._4_4_ = 0;
                            pplStack_290 = pplStack_650;
                            FUN_140021e90(&uStack_630,&local_568,&local_658);
                            puVar3 = (undefined4 *)puVar13[0x1d];
                            if (puVar3 == (undefined4 *)puVar13[0x1e]) {
                              FUN_1402a41e0(puVar13 + 0x1c,puVar3,&local_638);
                            }
                            else {
                              *puVar3 = (undefined4)local_638;
                              puVar3[1] = local_638._4_4_;
                              puVar3[2] = (undefined4)uStack_630;
                              puVar3[3] = uStack_630._4_4_;
                              puVar3[4] = (undefined4)uStack_628;
                              puVar3[5] = uStack_628._4_4_;
                              *(ulonglong *)(puVar3 + 6) = local_620;
                              *(ulonglong *)(puVar3 + 8) = uStack_618;
                              local_620 = 0;
                              uStack_618 = 0xf;
                              uStack_630 = (char *)((ulonglong)uStack_630 & 0xffffffffffffff00);
                              puVar13[0x1d] = puVar13[0x1d] + 0x28;
                            }
LAB_140425079:
                            FUN_140002d30(&uStack_630);
                            pcVar25 = "Enum type {} for field {} has zero options";
                            pcVar15 = "{} has unknown type";
                          }
LAB_140425098:
                          plVar22 = local_570;
                          plVar8 = local_5f8;
                          puVar13 = local_res10;
                        }
                        uVar23 = plVar28[7];
                        local_5f8 = (longlong *)(uVar23 % (ulonglong)(uint)puVar13[0x26]);
                        for (puVar17 = *(ulonglong **)(puVar13[0x25] + (longlong)local_5f8 * 8);
                            puVar17 != (ulonglong *)0x0; puVar17 = (ulonglong *)puVar17[4]) {
                          if (uVar23 == *puVar17) {
                            if (puVar17 != (ulonglong *)0x0) goto LAB_140425393;
                            break;
                          }
                        }
                        FUN_14001db20(puVar13 + 0x28,local_578,(int)puVar13[0x26],(int)puVar13[0x27]
                                      ,1);
                        puVar17 = (ulonglong *)thunk_FUN_140983da8(0x28);
                        *puVar17 = plVar28[7];
                        puVar17[1] = 0;
                        puVar17[2] = 0;
                        puVar17[3] = 0;
                        puVar17[4] = 0;
                        plVar26 = local_5f8;
                        if (local_578[0] != '\0') {
                          uVar2 = (ulonglong)local_574;
                          FUN_14033df60(puVar13 + 0x24);
                          plVar26 = (undefined8 *)(uVar23 % uVar2);
                        }
                        puVar17[4] = *(ulonglong *)(puVar13[0x25] + (longlong)plVar26 * 8);
                        *(ulonglong **)(puVar13[0x25] + (longlong)plVar26 * 8) = puVar17;
                        puVar13[0x27] = puVar13[0x27] + 1;
LAB_140425393:
                        puVar17[2] = puVar17[1];
                        lVar14 = plVar28[0x11];
                        lVar20 = plVar28[0x12];
                        if (lVar14 != lVar20) {
                          do {
                            puVar16 = (undefined8 *)
                                      FUN_140422d90(&local_658,local_600 + 0x70,lVar14);
                            uVar4 = *puVar16;
                            uVar5 = puVar16[1];
                            uVar6 = puVar16[2];
                            uVar7 = puVar16[3];
                            puVar16 = (undefined8 *)puVar17[2];
                            if (puVar16 < (undefined8 *)puVar17[3]) {
                              puVar17[2] = (ulonglong)(puVar16 + 4);
                              *puVar16 = uVar4;
                              puVar16[1] = uVar5;
                              puVar16[2] = uVar6;
                              puVar16[3] = uVar7;
                            }
                            else {
                              lVar21 = (longlong)((longlong)puVar16 - puVar17[1]) >> 5;
                              if (lVar21 == 0) {
                                lVar21 = 1;
LAB_14042541b:
                                puVar16 = (undefined8 *)thunk_FUN_140983da8(lVar21 << 5);
                              }
                              else {
                                lVar21 = lVar21 * 2;
                                puVar16 = (undefined8 *)0x0;
                                if (lVar21 != 0) goto LAB_14042541b;
                              }
                              pvVar18 = (void *)puVar17[1];
                              local_5f8 = puVar16;
                              if (pvVar18 != (void *)puVar17[2]) {
                                uVar23 = (longlong)puVar17[2] - (longlong)pvVar18;
                                pvVar18 = memmove(puVar16,pvVar18,uVar23);
                                puVar16 = (undefined8 *)
                                          ((longlong)pvVar18 + (uVar23 & 0xffffffffffffffe0));
                              }
                              *puVar16 = uVar4;
                              puVar16[1] = uVar5;
                              puVar16[2] = uVar6;
                              puVar16[3] = uVar7;
                              if ((void *)puVar17[1] != (void *)0x0) {
                                free((void *)puVar17[1]);
                              }
                              puVar17[1] = (ulonglong)local_5f8;
                              puVar17[2] = (ulonglong)(puVar16 + 4);
                              puVar17[3] = (ulonglong)(local_5f8 + lVar21 * 4);
                            }
                            lVar14 = lVar14 + 0x60;
                          } while (lVar14 != lVar20);
                          plVar27 = plVar8 + 3;
                          plVar22 = local_570;
                        }
                        puVar13 = local_res10;
                        plVar28 = (longlong *)
                                  ((longlong)(int)plVar27[0x12] * 0x78 + local_res10[0x1f]);
                        *plVar28 = 0;
                        plVar28[1] = 0;
                        plVar28[2] = 0;
                        plVar28[3] = 0;
                        plVar28[4] = 0;
                        plVar28[5] = 0;
                        plVar28[6] = 0;
                        plVar28[7] = 0;
                        plVar28[8] = 0;
                        plVar28[9] = 0;
                        plVar28[10] = 0;
                        plVar28[0xb] = 0;
                        plVar28[0xc] = 0;
                        plVar28[0xd] = 0;
                        plVar28[0xe] = 0;
                        plVar28[1] = local_558;
                        *plVar28 = plVar8[2];
                        if (((char)plVar27[10] == '\0') || (uVar23 = plVar27[9], uVar23 == 0)) {
LAB_140425a81:
                          if ((char)plVar27[10] == '\0') {
                            if (plVar27[9] == 0) {
                              local_468 = "task_run";
                              uStack_460 = 8;
                              local_658 = (longlong *)0x140a71d38;
                              pplStack_650 = (longlong **)0x8;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x1690,
                                            plVar28 + 0xd);
                              local_458 = "task_event_train";
                              uStack_450 = 0x10;
                              local_658 = (longlong *)0x140a71d20;
                              pplStack_650 = (longlong **)&DAT_00000010;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x1768,
                                            plVar28 + 0xe);
                            }
                            if ((char)plVar27[10] == '\0') {
                              local_448 = "event_signal_check";
                              uStack_440 = 0x12;
                              local_658 = (longlong *)0x140a71ca8;
                              pplStack_650 = (longlong **)0x12;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_438 = "event_signal_lookahead";
                              uStack_430 = 0x16;
                              local_658 = (longlong *)0x140a71c90;
                              pplStack_650 = (longlong **)0x16;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_428 = "event_signal_change_path";
                              uStack_420 = 0x18;
                              local_658 = (longlong *)0x140a71c70;
                              pplStack_650 = (longlong **)0x18;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_418 = "event_signal_pass_by";
                              uStack_410 = 0x14;
                              local_658 = (longlong *)0x140a71c58;
                              pplStack_650 = (longlong **)0x14;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_408 = "event_signal_marker_reserved";
                              uStack_400 = 0x1c;
                              local_658 = (longlong *)0x140a71c38;
                              pplStack_650 = (longlong **)0x1c;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_3f8 = "event_signal_texture_state";
                              uStack_3f0 = 0x1a;
                              local_658 = (longlong *)0x140a71da8;
                              pplStack_650 = (longlong **)0x1a;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_3e8 = "event_train_shift_setup";
                              uStack_3e0 = 0x17;
                              local_658 = (longlong *)0x140a71d80;
                              pplStack_650 = (longlong **)0x17;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_3d8 = "event_train_shift_allow";
                              uStack_3d0 = 0x17;
                              local_658 = (longlong *)0x140a71d68;
                              pplStack_650 = (longlong **)0x17;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_3c8 = "event_line_stop";
                              uStack_3c0 = 0xf;
                              local_658 = (longlong *)0x140a71d58;
                              pplStack_650 = (longlong **)0xf;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_3b8 = "event_shift";
                              uStack_3b0 = 0xb;
                              local_658 = (longlong *)0x140a71d48;
                              pplStack_650 = (longlong **)0xb;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                              local_3a8 = "control_train";
                              uStack_3a0 = 0xd;
                              local_658 = (longlong *)0x140a71d98;
                              pplStack_650 = (longlong **)0xd;
                              FUN_1404265a0(&local_610,plVar27,&local_658);
                            }
                          }
                          if ((char)plVar27[10] != '\0') {
                            local_398 = "task_run";
                            uStack_390 = 8;
                            local_658 = (longlong *)0x140a71d38;
                            pplStack_650 = (longlong **)0x8;
                            FUN_1404265a0(&local_610,plVar27,&local_658);
                            local_388 = "task_event_train";
                            uStack_380 = 0x10;
                            local_658 = (longlong *)0x140a71d20;
                            pplStack_650 = (longlong **)&DAT_00000010;
                            FUN_1404265a0(&local_610,plVar27,&local_658);
                            if ((char)plVar27[10] != '\0') {
                              if (plVar27[9] != *(longlong *)(*param_1 + 600)) {
                                local_378 = "event_signal_check";
                                uStack_370 = 0x12;
                                local_658 = (longlong *)0x140a71ca8;
                                pplStack_650 = (longlong **)0x12;
                                FUN_1404265a0(&local_610,plVar27,&local_658);
                                local_368 = "event_signal_lookahead";
                                uStack_360 = 0x16;
                                local_658 = (longlong *)0x140a71c90;
                                pplStack_650 = (longlong **)0x16;
                                FUN_1404265a0(&local_610,plVar27,&local_658);
                                local_358 = "event_signal_change_path";
                                uStack_350 = 0x18;
                                local_658 = (longlong *)0x140a71c70;
                                pplStack_650 = (longlong **)0x18;
                                FUN_1404265a0(&local_610,plVar27,&local_658);
                                local_348 = "event_signal_pass_by";
                                uStack_340 = 0x14;
                                local_658 = (longlong *)0x140a71c58;
                                pplStack_650 = (longlong **)0x14;
                                FUN_1404265a0(&local_610,plVar27,&local_658);
                                local_338 = "event_signal_marker_reserved";
                                uStack_330 = 0x1c;
                                local_658 = (longlong *)0x140a71c38;
                                pplStack_650 = (longlong **)0x1c;
                                FUN_1404265a0(&local_610,plVar27,&local_658);
                                local_328 = "event_signal_texture_state";
                                uStack_320 = 0x1a;
                                local_658 = (longlong *)0x140a71da8;
                                pplStack_650 = (longlong **)0x1a;
                                FUN_1404265a0(&local_610,plVar27,&local_658);
                              }
                              if ((char)plVar27[10] != '\0') {
                                if (plVar27[9] != *(longlong *)(*param_1 + 0x208)) {
                                  local_318 = "control_train";
                                  uStack_310 = 0xd;
                                  local_658 = (longlong *)0x140a71d98;
                                  pplStack_650 = (longlong **)0xd;
                                  FUN_1404265a0(&local_610,plVar27,&local_658);
                                  local_308 = "event_train_shift_setup";
                                  uStack_300 = 0x17;
                                  local_658 = (longlong *)0x140a71d80;
                                  pplStack_650 = (longlong **)0x17;
                                  FUN_1404265a0(&local_610,plVar27,&local_658);
                                  local_2f8 = "event_train_shift_allow";
                                  uStack_2f0 = 0x17;
                                  local_658 = (longlong *)0x140a71d68;
                                  pplStack_650 = (longlong **)0x17;
                                  FUN_1404265a0(&local_610,plVar27,&local_658);
                                }
                                if ((char)plVar27[10] != '\0') {
                                  if (plVar27[9] != *(longlong *)(*param_1 + 0x2f8)) {
                                    local_2e8 = "event_line_stop";
                                    uStack_2e0 = 0xf;
                                    local_658 = (longlong *)0x140a71d58;
                                    pplStack_650 = (longlong **)0xf;
                                    FUN_1404265a0(&local_610,plVar27,&local_658);
                                  }
                                  if (((char)plVar27[10] != '\0') &&
                                     (plVar27[9] != *(longlong *)(*param_1 + 0x2d0))) {
                                    local_5a8 = (longlong *)0x140a71d48;
                                    pplStack_5a0 = (longlong **)0xb;
                                    local_658 = (longlong *)0x140a71d48;
                                    pplStack_650 = (longlong **)&DAT_0000000b;
                                    FUN_1404265a0(&local_610,plVar27,&local_658);
                                  }
                                }
                              }
                            }
                          }
                        }
                        else {
                          puVar16 = (undefined8 *)(*(undefined8 **)(*param_1 + 0x19f0))[1];
                          cVar1 = *(char *)((longlong)puVar16 + 0x19);
                          puVar24 = *(undefined8 **)(*param_1 + 0x19f0);
                          while (cVar1 == '\0') {
                            if ((ulonglong)puVar16[4] < uVar23) {
                              puVar19 = (undefined8 *)puVar16[2];
                              puVar16 = puVar24;
                            }
                            else {
                              puVar19 = (undefined8 *)*puVar16;
                            }
                            puVar24 = puVar16;
                            puVar16 = puVar19;
                            cVar1 = *(char *)((longlong)puVar19 + 0x19);
                          }
                          if ((*(char *)((longlong)puVar24 + 0x19) == '\0') &&
                             ((ulonglong)puVar24[4] <= uVar23)) {
                            lVar14 = *param_1;
                            if (uVar23 == *(ulonglong *)(lVar14 + 0x280)) {
                              local_res18 = CONCAT44(local_res18._4_4_,(int)local_res18 + 1);
                            }
                            if (uVar23 == *(ulonglong *)(lVar14 + 600)) {
                              local_548 = "event_signal_check";
                              uStack_540 = 0x12;
                              local_658 = (longlong *)0x140a71ca8;
                              pplStack_650 = (longlong **)0x12;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,lVar14 + 0xd48,plVar28 + 2
                                           );
                              local_538 = "event_signal_lookahead";
                              uStack_530 = 0x16;
                              local_658 = (longlong *)0x140a71c90;
                              pplStack_650 = (longlong **)0x16;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0xe20,
                                            plVar28 + 3);
                              local_528 = "event_signal_change_path";
                              uStack_520 = 0x18;
                              local_658 = (longlong *)0x140a71c70;
                              pplStack_650 = (longlong **)0x18;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0xef8,
                                            plVar28 + 4);
                              local_518 = "event_signal_pass_by";
                              uStack_510 = 0x14;
                              local_658 = (longlong *)0x140a71c58;
                              pplStack_650 = (longlong **)0x14;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0xfd0,
                                            plVar28 + 5);
                              local_508 = "event_signal_marker_reserved";
                              uStack_500 = 0x1c;
                              local_658 = (longlong *)0x140a71c38;
                              pplStack_650 = (longlong **)0x1c;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x10a8,
                                            plVar28 + 6);
                              local_4f8 = "event_signal_texture_state";
                              uStack_4f0 = 0x1a;
                              local_658 = (longlong *)0x140a71da8;
                              pplStack_650 = (longlong **)0x1a;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x1180,
                                            plVar28 + 7);
                            }
                            if (plVar27[9] == *(longlong *)(*param_1 + 0x208)) {
                              local_4e8 = "control_train";
                              uStack_4e0 = 0xd;
                              local_658 = (longlong *)0x140a71d98;
                              pplStack_650 = (longlong **)0xd;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x1258,
                                            plVar28 + 8);
                              local_4d8 = "event_train_shift_setup";
                              uStack_4d0 = 0x17;
                              local_658 = (longlong *)0x140a71d80;
                              pplStack_650 = (longlong **)0x17;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x1330,
                                            plVar28 + 9);
                              local_4c8 = "event_train_shift_allow";
                              uStack_4c0 = 0x17;
                              local_658 = (longlong *)0x140a71d68;
                              pplStack_650 = (longlong **)0x17;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x1408,
                                            plVar28 + 10);
                            }
                            if (plVar27[9] == *(longlong *)(*param_1 + 0x2f8)) {
                              local_4b8 = "event_line_stop";
                              uStack_4b0 = 0xf;
                              local_658 = (longlong *)0x140a71d58;
                              pplStack_650 = (longlong **)0xf;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x14e0,
                                            plVar28 + 0xb);
                            }
                            if (plVar27[9] == *(longlong *)(*param_1 + 0x2d0)) {
                              local_4a8 = "event_shift";
                              uStack_4a0 = 0xb;
                              local_658 = (longlong *)0x140a71d48;
                              pplStack_650 = (longlong **)&DAT_0000000b;
                              FUN_140426a00(&local_5c0,plVar27,&local_658,*param_1 + 0x15b8,
                                            plVar28 + 0xc);
                            }
                            goto LAB_140425a81;
                          }
                          local_5f0 = 0;
                          local_658 = plVar27;
                          if (0xf < (ulonglong)plVar27[3]) {
                            local_658 = (longlong *)*plVar27;
                          }
                          pplStack_650 = (longlong **)plVar27[2];
                          local_488 = 0xd;
                          pplStack_480 = &local_658;
                          local_478 = "{} extends an unsupported type";
                          uStack_470 = 0x1e;
                          local_268 = 0;
                          local_278 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>
                                      ::vftable;
                          local_270 = local_258;
                          local_260 = 500;
                          local_568 = 0xd;
                          uStack_564 = 0;
                          local_598 = (longlong *)0x140a71cc0;
                          lStack_590 = 0x1e;
                          local_498 = local_658;
                          pplStack_490 = pplStack_650;
                          uStack_560 = pplStack_480;
                          FUN_140022830(&local_278,&local_598,&local_568);
                          local_5e8 = (char *)0x0;
                          uStack_5e0 = 0;
                          local_5d8 = 0;
                          uStack_5d0 = 0;
                          FUN_140002c00(&local_5e8,local_270);
                          local_278 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>
                                      ::vftable;
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
                          puVar3 = (undefined4 *)puVar13[0x1d];
                          if (puVar3 == (undefined4 *)puVar13[0x1e]) {
                            FUN_1402a41e0(puVar13 + 0x1c);
                            uVar23 = uStack_5d0;
                          }
                          else {
                            *puVar3 = (undefined4)local_5f0;
                            puVar3[1] = local_5f0._4_4_;
                            puVar3[2] = (undefined4)local_5e8;
                            puVar3[3] = local_5e8._4_4_;
                            puVar3[4] = (undefined4)uStack_5e0;
                            puVar3[5] = uStack_5e0._4_4_;
                            *(undefined8 *)(puVar3 + 6) = local_5d8;
                            *(ulonglong *)(puVar3 + 8) = uStack_5d0;
                            local_5e8 = (char *)((ulonglong)local_5e8 & 0xffffffffffffff00);
                            puVar13[0x1d] = puVar13[0x1d] + 0x28;
                            uVar23 = 0xf;
                          }
                          if (0xf < uVar23) {
                            pcVar15 = local_5e8;
                            if ((0xfff < uVar23 + 1) &&
                               (pcVar15 = *(char **)(local_5e8 + -8),
                               (char *)0x1f < local_5e8 + (-8 - (longlong)pcVar15))) {
                    /* WARNING: Subroutine does not return */
                              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
                            }
                            free(pcVar15);
                          }
                        }
                        puVar13 = local_res10;
                        plVar28 = (longlong *)*plVar8;
                      } while (plVar28 != plVar22);
                      local_5f8 = plVar28;
                      if (1 < (int)local_res18) {
                        local_638 = 0;
                        uStack_630 = (char *)0x0;
                        uStack_628 = 0;
                        uStack_630 = (char *)FUN_140003270(0x60);
                        uVar4 = s_More_than_1_struct_extends_Scrip_140a71e40._8_8_;
                        local_620 = 0x5e;
                        uStack_618 = 0x5f;
                        *(undefined8 *)uStack_630 =
                             s_More_than_1_struct_extends_Scrip_140a71e40._0_8_;
                        *(undefined8 *)(uStack_630 + 8) = uVar4;
                        uVar4 = s_More_than_1_struct_extends_Scrip_140a71e40._24_8_;
                        *(undefined8 *)(uStack_630 + 0x10) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._16_8_;
                        *(undefined8 *)(uStack_630 + 0x18) = uVar4;
                        uVar4 = s_More_than_1_struct_extends_Scrip_140a71e40._40_8_;
                        *(undefined8 *)(uStack_630 + 0x20) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._32_8_;
                        *(undefined8 *)(uStack_630 + 0x28) = uVar4;
                        uVar11 = s_More_than_1_struct_extends_Scrip_140a71e40._60_4_;
                        uVar10 = s_More_than_1_struct_extends_Scrip_140a71e40._56_4_;
                        uVar9 = s_More_than_1_struct_extends_Scrip_140a71e40._52_4_;
                        *(undefined4 *)(uStack_630 + 0x30) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._48_4_;
                        *(undefined4 *)(uStack_630 + 0x34) = uVar9;
                        *(undefined4 *)(uStack_630 + 0x38) = uVar10;
                        *(undefined4 *)(uStack_630 + 0x3c) = uVar11;
                        uVar4 = s_More_than_1_struct_extends_Scrip_140a71e40._72_8_;
                        *(undefined8 *)(uStack_630 + 0x40) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._64_8_;
                        *(undefined8 *)(uStack_630 + 0x48) = uVar4;
                        *(undefined8 *)(uStack_630 + 0x50) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._80_8_;
                        *(undefined4 *)(uStack_630 + 0x58) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._88_4_;
                        *(undefined2 *)(uStack_630 + 0x5c) =
                             s_More_than_1_struct_extends_Scrip_140a71e40._92_2_;
                        uStack_630[0x5e] = '\0';
                        FUN_1402a3bd0(puVar13 + 0x1c,&local_638);
                        if (0xf < uStack_618) {
                          FUN_140003040(&uStack_630,uStack_630);
                        }
                      }
                    }
                    puVar13 = local_res10;
                    lVar14 = *param_1;
                    local_5a8 = (longlong *)0x140a71e20;
                    pplStack_5a0 = (longlong **)0x16;
                    local_638 = 0;
                    uStack_630 = (char *)0x0;
                    uStack_628 = 0;
                    local_620 = 0;
                    FUN_140002c00(&local_638,"global_event_line_stop",0x16);
                    uStack_618 = FUN_1402a9cb0("global_event_line_stop",0x16);
                    bVar29 = false;
                    lVar20 = FUN_1402a7050(local_600 + 0x30,uStack_618);
                    if (lVar20 != 0) {
                      local_658 = local_5a8;
                      pplStack_650 = pplStack_5a0;
                      FUN_1404245c0(local_5a8._0_4_,&local_5f0,lVar14 + 0x1840,lVar20,&local_658,0);
                      bVar29 = local_5c8 == '\0';
                      if (bVar29) {
                        puVar13[0x22] = *(ulonglong *)(lVar20 + 0x28);
                      }
                      else {
                        FUN_140438dc0(local_res10 + 0x1c,&local_5f0);
                      }
                      if (local_5c8 != '\0') {
                        FUN_140002d30(&local_5e8);
                      }
                    }
                    if (0xf < local_620) {
                      FUN_140003040(&local_638,local_638);
                    }
                    if (bVar29) {
                      FUN_1403f2cb0(param_1 + 9,&local_658,local_res10);
                    }
                    puVar13 = local_res10;
                    lVar14 = *param_1;
                    local_5a8 = (longlong *)0x140a71e08;
                    pplStack_5a0 = (longlong **)0x12;
                    local_638 = 0;
                    uStack_630 = (char *)0x0;
                    uStack_628 = 0;
                    local_620 = 0;
                    FUN_140002c00(&local_638,"global_event_shift",0x12);
                    uStack_618 = FUN_1402a9cb0("global_event_shift",0x12);
                    bVar29 = false;
                    lVar20 = FUN_1402a7050(local_600 + 0x30,uStack_618);
                    if (lVar20 != 0) {
                      local_658 = local_5a8;
                      pplStack_650 = pplStack_5a0;
                      FUN_1404245c0(local_5a8._0_4_,&local_5f0,lVar14 + 0x1918,lVar20,&local_658,0);
                      bVar29 = local_5c8 == '\0';
                      if (bVar29) {
                        puVar13[0x23] = *(ulonglong *)(lVar20 + 0x28);
                      }
                      else {
                        FUN_140438dc0(local_res10 + 0x1c,&local_5f0);
                      }
                      if (local_5c8 != '\0') {
                        FUN_140002d30(&local_5e8);
                      }
                    }
                    if (0xf < local_620) {
                      FUN_140003040(&local_638,local_638);
                    }
                    if (bVar29) {
                      FUN_1403f2cb0(param_1 + 0xc,&local_658,local_res10);
                    }
                    return 1;
                  }
                }
                puVar13 = local_res10;
                local_638 = 0;
                uStack_630 = (char *)0x0;
                uStack_628 = 0;
                uStack_630 = (char *)FUN_140003270(0x20);
                uVar11 = s_Invalid_meta___api___140a71ba8._12_4_;
                uVar10 = s_Invalid_meta___api___140a71ba8._8_4_;
                uVar9 = s_Invalid_meta___api___140a71ba8._4_4_;
                local_620 = 0x14;
                uStack_618 = 0x1f;
                *(undefined4 *)uStack_630 = s_Invalid_meta___api___140a71ba8._0_4_;
                *(undefined4 *)(uStack_630 + 4) = uVar9;
                *(undefined4 *)(uStack_630 + 8) = uVar10;
                *(undefined4 *)(uStack_630 + 0xc) = uVar11;
                *(undefined4 *)(uStack_630 + 0x10) = s_Invalid_meta___api___140a71ba8._16_4_;
                uStack_630[0x14] = '\0';
                FUN_1402a3bd0(puVar13 + 0x1c,&local_638);
                FUN_140002d30(&uStack_630);
                return 0;
              }
            }
            puVar13 = local_res10;
            local_638 = 0;
            uStack_630 = (char *)0x0;
            uStack_628 = 0;
            uStack_630 = (char *)FUN_140003270(0x20);
            uVar11 = s_Invalid_meta___lang___140a71bd8._12_4_;
            uVar10 = s_Invalid_meta___lang___140a71bd8._8_4_;
            uVar9 = s_Invalid_meta___lang___140a71bd8._4_4_;
            local_620 = 0x15;
            uStack_618 = 0x1f;
            *(undefined4 *)uStack_630 = s_Invalid_meta___lang___140a71bd8._0_4_;
            *(undefined4 *)(uStack_630 + 4) = uVar9;
            *(undefined4 *)(uStack_630 + 8) = uVar10;
            *(undefined4 *)(uStack_630 + 0xc) = uVar11;
            *(undefined4 *)(uStack_630 + 0x10) = s_Invalid_meta___lang___140a71bd8._16_4_;
            uStack_630[0x14] = s_Invalid_meta___lang___140a71bd8[0x14];
            uStack_630[0x15] = '\0';
            FUN_1402a3bd0(puVar13 + 0x1c,&local_638);
            FUN_140002d30(&uStack_630);
            return 0;
          }
          local_638 = 0;
          uStack_630 = (char *)0x0;
          uStack_628 = 0;
          uStack_630 = (char *)FUN_140003270(0x30);
          uVar4 = s_Missing__invalid_or_empty_top_le_140a71c08._8_8_;
          local_620 = 0x28;
          uStack_618 = 0x2f;
          *(undefined8 *)uStack_630 = s_Missing__invalid_or_empty_top_le_140a71c08._0_8_;
          *(undefined8 *)(uStack_630 + 8) = uVar4;
          uVar11 = s_Missing__invalid_or_empty_top_le_140a71c08._28_4_;
          uVar10 = s_Missing__invalid_or_empty_top_le_140a71c08._24_4_;
          uVar9 = s_Missing__invalid_or_empty_top_le_140a71c08._20_4_;
          *(undefined4 *)(uStack_630 + 0x10) = s_Missing__invalid_or_empty_top_le_140a71c08._16_4_;
          *(undefined4 *)(uStack_630 + 0x14) = uVar9;
          *(undefined4 *)(uStack_630 + 0x18) = uVar10;
          *(undefined4 *)(uStack_630 + 0x1c) = uVar11;
          *(undefined8 *)(uStack_630 + 0x20) = s_Missing__invalid_or_empty_top_le_140a71c08._32_8_;
          uStack_630[0x28] = '\0';
          FUN_1402a3bd0(puVar17,&local_638);
          if (uStack_618 < 0x10) {
            return 0;
          }
          puVar16 = &uStack_630;
          pcVar15 = uStack_630;
          uVar23 = uStack_618;
        }
        FUN_140003040(puVar16,pcVar15,uVar23);
      }
    }
  }
  return 0;
}


// Incoming references
// 0xc2288c DATA caller none
// 0x427237 UNCONDITIONAL_CALL caller 140427220
// 0x491bf2 UNCONDITIONAL_CALL caller 140491b50
