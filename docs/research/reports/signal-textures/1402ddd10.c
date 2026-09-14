// Candidate VA 1402ddd10; RVA 0x2ddd10
// Ghidra inferred prototype: undefined FUN_1402ddd10()

/* WARNING: Type propagation algorithm not settling */

void FUN_1402ddd10(longlong param_1,longlong param_2)

{
  longlong *******ppppppplVar1;
  ulonglong uVar2;
  longlong *******ppppppplVar3;
  char cVar4;
  int iVar5;
  undefined8 uVar6;
  char *pcVar7;
  longlong *******ppppppplVar8;
  char *pcVar9;
  ulonglong uVar10;
  longlong *plVar11;
  longlong *plVar12;
  longlong lVar13;
  undefined8 *puVar14;
  longlong *plVar15;
  ulonglong uVar16;
  ulonglong uVar17;
  ulonglong uVar18;
  size_t sVar19;
  char *pcVar20;
  longlong *******ppppppplVar21;
  undefined8 *puVar22;
  undefined8 *puVar23;
  uint uVar24;
  longlong local_res10;
  uint local_res18 [2];
  char local_res20;
  char local_res21;
  char *local_228;
  undefined8 local_220;
  undefined8 *local_218;
  undefined8 *local_210;
  longlong local_208;
  longlong local_200;
  longlong *******local_1f8;
  undefined8 uStack_1f0;
  ulonglong local_1e8;
  ulonglong uStack_1e0;
  longlong *******local_1d8;
  undefined8 *local_1d0;
  undefined1 local_1c8 [16];
  longlong *******local_1b8;
  undefined8 uStack_1b0;
  longlong local_1a8;
  ulonglong uStack_1a0;
  uint local_198;
  uint local_194;
  ulonglong local_190;
  ulonglong uStack_188;
  longlong *******local_180;
  undefined8 uStack_178;
  void *local_170;
  undefined8 uStack_168;
  ulonglong local_160;
  ulonglong local_158;
  undefined8 *local_148;
  longlong local_140;
  undefined8 *local_138;
  longlong *******local_130;
  undefined8 uStack_128;
  ulonglong local_120;
  ulonglong uStack_118;
  longlong *******local_110;
  char *local_108;
  undefined8 local_100;
  char *local_f8;
  undefined8 local_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  undefined8 local_d8;
  undefined8 local_d0;
  undefined8 *local_c8;
  undefined8 *local_c0;
  undefined1 local_b0 [8];
  undefined1 local_a8 [16];
  undefined1 local_98 [16];
  undefined1 local_88 [32];
  undefined1 local_68 [40];
  
  uVar24 = 0;
  local_198 = 0;
  local_108 = "TrainUnit";
  local_100 = 9;
  local_res10 = param_2;
  local_200 = param_2;
  FUN_1402d7560(param_1,&local_218,&local_108);
  local_1d0 = local_218;
  if (local_218 != local_210) {
    lVar13 = param_2 + 0xe8;
    puVar14 = local_218;
    local_140 = lVar13;
    do {
      uVar6 = *puVar14;
      local_res18[0] = 1;
      local_1d0 = puVar14;
      FUN_1402e3fc0(lVar13,local_a8,local_res18);
      FUN_1402dec40(&local_200,uVar6,0);
      local_f8 = "allow_player_composition";
      local_f0 = 0x18;
      FUN_1402dcbf0(uVar6,&local_res20,&local_f8);
      if ((local_res21 != '\0') && (local_res20 != '\0')) {
        local_res18[0] = local_res18[0] & 0xffffff00;
        uStack_e0 = 0;
        local_d8 = 0;
        local_d0 = 0xf;
        local_e8 = 0;
        local_228 = "tags";
        local_220 = 4;
        uVar6 = FUN_140271540(uVar6,local_88,&local_228);
        FUN_140270ab0(&local_c8,uVar6,0x2c);
        FUN_140002d30(local_88);
        local_148 = local_c8;
        local_138 = local_c0;
        puVar22 = local_c0;
        puVar23 = local_c8;
        if (local_c8 != local_c0) {
          do {
            local_170 = (void *)0x0;
            uStack_168 = 0;
            local_160 = 0;
            local_158 = 0;
            uVar10 = puVar23[2];
            puVar14 = puVar23;
            if (0xf < (ulonglong)puVar23[3]) {
              puVar14 = (undefined8 *)*puVar23;
            }
            local_148 = puVar23;
            if (0x7fffffffffffffff < uVar10) {
                    /* WARNING: Subroutine does not return */
              FUN_140001c70();
            }
            if (uVar10 < 0x10) {
              local_158 = 0xf;
              local_170 = (void *)*puVar14;
              uStack_168 = puVar14[1];
              local_160 = uVar10;
            }
            else {
              uVar18 = uVar10 | 0xf;
              if (uVar18 < 0x8000000000000000) {
                if (uVar18 < 0x16) {
                  uVar18 = 0x16;
                }
              }
              else {
                uVar18 = 0x7fffffffffffffff;
              }
              local_170 = (void *)FUN_140003270(uVar18 + 1);
              local_160 = uVar10;
              local_158 = uVar18;
              memcpy(local_170,puVar14,uVar10 + 1);
            }
            local_180 = &local_170;
            pcVar7 = (char *)FUN_140019ca0(local_68,&local_170);
            pcVar20 = pcVar7;
            if (0xf < *(ulonglong *)(pcVar7 + 0x18)) {
              pcVar20 = *(char **)pcVar7;
            }
            pcVar9 = pcVar20 + *(longlong *)(pcVar7 + 0x10);
            for (; (pcVar20 != pcVar9 && ((*pcVar20 == ' ' || ((byte)(*pcVar20 - 9U) < 5))));
                pcVar20 = pcVar20 + 1) {
            }
            pcVar20 = pcVar7;
            if (0xf < *(ulonglong *)(pcVar7 + 0x18)) {
              pcVar20 = *(char **)pcVar7;
            }
            FUN_140244e70(pcVar7,local_b0,pcVar20);
            uVar10 = 0;
            local_1b8 = *(longlong ********)pcVar7;
            uStack_1b0 = *(undefined8 *)(pcVar7 + 8);
            local_1a8 = *(longlong *)(pcVar7 + 0x10);
            uStack_1a0 = *(ulonglong *)(pcVar7 + 0x18);
            pcVar7[0x10] = '\0';
            pcVar7[0x11] = '\0';
            pcVar7[0x12] = '\0';
            pcVar7[0x13] = '\0';
            pcVar7[0x14] = '\0';
            pcVar7[0x15] = '\0';
            pcVar7[0x16] = '\0';
            pcVar7[0x17] = '\0';
            pcVar7[0x18] = '\x0f';
            pcVar7[0x19] = '\0';
            pcVar7[0x1a] = '\0';
            pcVar7[0x1b] = '\0';
            pcVar7[0x1c] = '\0';
            pcVar7[0x1d] = '\0';
            pcVar7[0x1e] = '\0';
            pcVar7[0x1f] = '\0';
            *pcVar7 = '\0';
            ppppppplVar21 = (longlong *******)&local_1b8;
            if (0xf < uStack_1a0) {
              ppppppplVar21 = local_1b8;
            }
            ppppppplVar8 = (longlong *******)&local_1b8;
            if (0xf < uStack_1a0) {
              ppppppplVar8 = local_1b8;
            }
            ppppppplVar1 = (longlong *******)((longlong)ppppppplVar8 + local_1a8);
            do {
              ppppppplVar3 = ppppppplVar1;
              if (ppppppplVar3 == ppppppplVar8) break;
              ppppppplVar1 = (longlong *******)((longlong)ppppppplVar3 + -1);
            } while ((*(char *)ppppppplVar1 == ' ') || ((byte)(*(char *)ppppppplVar1 - 9U) < 5));
            FUN_140244e70(&local_1b8,&local_108,ppppppplVar3,
                          (char *)((longlong)ppppppplVar21 + local_1a8));
            local_180 = local_1b8;
            uStack_178 = uStack_1b0;
            local_130 = local_1b8;
            uStack_128 = uStack_1b0;
            local_190 = local_1a8;
            uStack_188 = uStack_1a0;
            local_120 = local_1a8;
            uStack_118 = uStack_1a0;
            local_1a8 = 0;
            uStack_1a0 = 0xf;
            local_1b8 = (longlong *******)((ulonglong)local_1b8 & 0xffffffffffffff00);
            FUN_140002d30(&local_170);
            uVar18 = local_190;
            uStack_1f0 = 0;
            local_1e8 = 0;
            uVar16 = 0xf;
            uStack_1e0 = 0xf;
            local_1f8 = (longlong *******)0x0;
            local_194 = uVar24 | 3;
            local_198 = uVar24 | 3;
            local_190 = uStack_188;
            local_1d8 = (longlong *******)&local_130;
            if (0xf < uStack_188) {
              local_1d8 = local_180;
            }
            local_110 = (longlong *******)((longlong)local_1d8 + uVar18);
            if (local_1d8 != (longlong *******)((longlong)local_1d8 + uVar18)) {
              do {
                ppppppplVar21 = local_1f8;
                cVar4 = *(char *)local_1d8;
                if ((byte)(cVar4 + 0xbfU) < 0x1a) {
                  iVar5 = tolower((int)cVar4);
                  cVar4 = (char)iVar5;
                }
                if (uVar10 < uVar16) {
                  local_1e8 = uVar10 + 1;
                  ppppppplVar8 = (longlong *******)&local_1f8;
                  if (0xf < uVar16) {
                    ppppppplVar8 = ppppppplVar21;
                  }
                  *(char *)((longlong)ppppppplVar8 + uVar10) = cVar4;
                  *(undefined1 *)((longlong)ppppppplVar8 + uVar10 + 1) = 0;
                  uVar18 = local_1e8;
                  uVar17 = uStack_1e0;
                }
                else {
                  if (uVar10 == 0x7fffffffffffffff) {
                    /* WARNING: Subroutine does not return */
                    FUN_140001c70();
                  }
                  uVar18 = uVar10 + 1;
                  uVar17 = uVar18 | 0xf;
                  if (uVar17 < 0x8000000000000000) {
                    if (0x7fffffffffffffff - (uVar16 >> 1) < uVar16) {
                      uVar17 = 0x7fffffffffffffff;
                    }
                    else {
                      uVar2 = (uVar16 >> 1) + uVar16;
                      if (uVar17 < uVar2) {
                        uVar17 = uVar2;
                      }
                    }
                  }
                  else {
                    uVar17 = 0x7fffffffffffffff;
                  }
                  ppppppplVar8 = (longlong *******)FUN_140003270(uVar17 + 1);
                  local_1e8 = uVar18;
                  uStack_1e0 = uVar17;
                  if (uVar16 < 0x10) {
                    memcpy(ppppppplVar8,&local_1f8,uVar10);
                    *(char *)(uVar10 + (longlong)ppppppplVar8) = cVar4;
                    *(undefined1 *)(uVar10 + 1 + (longlong)ppppppplVar8) = 0;
                    local_1f8 = ppppppplVar8;
                  }
                  else {
                    memcpy(ppppppplVar8,ppppppplVar21,uVar10);
                    *(char *)(uVar10 + (longlong)ppppppplVar8) = cVar4;
                    *(undefined1 *)(uVar10 + 1 + (longlong)ppppppplVar8) = 0;
                    FUN_140003040(&local_1f8,ppppppplVar21,uVar16);
                    local_1f8 = ppppppplVar8;
                  }
                }
                local_1d8 = (longlong *******)((longlong)local_1d8 + 1);
                uVar10 = uVar18;
                uVar16 = uVar17;
                puVar22 = local_138;
                puVar23 = local_148;
              } while (local_1d8 != local_110);
            }
            uVar24 = local_194;
            if (0xf < local_190) {
              FUN_140003040(&local_130,local_180);
            }
            local_180 = local_1f8;
            uStack_178 = uStack_1f0;
            local_130 = local_1f8;
            uStack_128 = uStack_1f0;
            local_190 = local_1e8;
            uStack_188 = uStack_1e0;
            local_120 = local_1e8;
            uStack_118 = uStack_1e0;
            local_1e8 = 0;
            uStack_1e0 = 0xf;
            local_1f8 = (longlong *******)((ulonglong)local_1f8 & 0xffffffffffffff00);
            uVar24 = uVar24 & 0xfffffffd;
            FUN_140002d30(&local_1f8);
            lVar13 = local_140;
            ppppppplVar21 = local_180;
            uVar10 = uStack_188;
            ppppppplVar8 = (longlong *******)&local_130;
            if (0xf < uStack_188) {
              ppppppplVar8 = local_180;
            }
            if (((local_190 == 10) && (*ppppppplVar8 == (longlong ******)0x69746f6d6f636f6c)) &&
               (*(short *)(ppppppplVar8 + 1) == 0x6576)) {
              local_res18[0] = 5;
              FUN_1402e3fc0(local_140,local_98,local_res18);
              local_res18[0] = CONCAT31(local_res18[0]._1_3_,1);
              puVar14 = local_1d0;
              if (0xf < uVar10) {
                FUN_140003040(&local_130,ppppppplVar21,uVar10);
                puVar14 = local_1d0;
              }
              break;
            }
            if (0xf < uStack_188) {
              FUN_140003040(&local_130,local_180,uStack_188);
            }
            local_148 = puVar23 + 4;
            lVar13 = local_140;
            puVar14 = local_1d0;
            puVar23 = local_148;
          } while (local_148 != puVar22);
        }
        FUN_140235860(&local_c8);
        if ((char)local_res18[0] == '\0') {
          local_res18[0] = 6;
          FUN_1402e3fc0(lVar13,local_1c8,local_res18);
        }
      }
      puVar14 = puVar14 + 1;
      param_2 = local_res10;
      local_1d0 = puVar14;
    } while (puVar14 != local_210);
  }
  FUN_140002cc0(&local_218);
  local_228 = "TrainMultipleUnit";
  local_220 = 0x11;
  FUN_1402d7560(param_1,&local_218,&local_228);
  puVar22 = local_210;
  for (puVar14 = local_218; puVar14 != puVar22; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 1;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6,1);
  }
  FUN_140002cc0(&local_218);
  local_228 = "DemandCurve";
  local_220 = 0xb;
  FUN_1402d7560(param_1,&local_218,&local_228);
  puVar22 = local_210;
  for (puVar14 = local_218; puVar14 != puVar22; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 8;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6,4);
  }
  FUN_140002cc0(&local_218);
  local_228 = "POILayer";
  local_220 = 8;
  FUN_1402d7560(param_1,&local_218,&local_228);
  puVar22 = local_210;
  for (puVar14 = local_218; puVar14 != puVar22; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 8;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6,4);
  }
  FUN_140002cc0(&local_218);
  local_228 = "TrackKind";
  local_220 = 9;
  FUN_1402d7560(param_1,&local_218,&local_228);
  puVar22 = local_210;
  for (puVar14 = local_218; puVar14 != puVar22; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 3;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6,2);
  }
  FUN_140002cc0(&local_218);
  local_228 = "BuildingKind";
  local_220 = 0xc;
  FUN_1402d7560(param_1,&local_218,&local_228);
  puVar22 = local_210;
  for (puVar14 = local_218; puVar14 != puVar22; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 2;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6,3);
  }
  FUN_140002cc0(&local_218);
  local_228 = "Script";
  local_220 = 6;
  FUN_1402d7560(param_1,&local_218);
  puVar23 = local_210;
  puVar22 = local_218;
  for (puVar14 = local_218; puVar14 != puVar23; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 9;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6);
  }
  if (puVar22 != (undefined8 *)0x0) {
    puVar14 = puVar22;
    if ((0xfff < (local_208 - (longlong)puVar22 & 0xfffffffffffffff8U)) &&
       (puVar14 = (undefined8 *)puVar22[-1],
       0x1f < (ulonglong)((longlong)puVar22 + (-8 - (longlong)puVar14)))) goto LAB_1402de754;
    free(puVar14);
  }
  local_228 = "SignalTextures";
  local_220 = 0xe;
  FUN_1402d7560(param_1,&local_218);
  for (puVar14 = local_218; puVar14 != local_210; puVar14 = puVar14 + 1) {
    uVar6 = *puVar14;
    local_res18[0] = 10;
    FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
    FUN_1402dec40(&local_200,uVar6);
  }
  if (local_218 != (undefined8 *)0x0) {
    puVar14 = local_218;
    if ((0xfff < (local_208 - (longlong)local_218 & 0xfffffffffffffff8U)) &&
       (puVar14 = (undefined8 *)local_218[-1],
       0x1f < (ulonglong)((longlong)local_218 + (-8 - (longlong)puVar14)))) {
LAB_1402de754:
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar14);
  }
  plVar12 = *(longlong **)(param_1 + 0x38);
  plVar11 = (longlong *)plVar12[1];
  plVar15 = plVar12;
  if (*(char *)((longlong)plVar11 + 0x19) == '\0') {
    do {
      uVar10 = plVar11[5];
      uVar18 = uVar10;
      if (8 < uVar10) {
        uVar18 = 8;
      }
      iVar5 = memcmp((void *)plVar11[4],"Language",uVar18);
      plVar15 = plVar11;
      if (iVar5 == 0) {
        if (uVar10 < 8) {
LAB_1402de848:
          plVar11 = plVar11 + 2;
          plVar15 = plVar12;
        }
      }
      else if (iVar5 < 0) goto LAB_1402de848;
      plVar11 = (longlong *)*plVar11;
      plVar12 = plVar15;
    } while (*(char *)((longlong)plVar11 + 0x19) == '\0');
    plVar12 = *(longlong **)(param_1 + 0x38);
  }
  if (*(char *)((longlong)plVar15 + 0x19) == '\0') {
    uVar10 = plVar15[5];
    sVar19 = 8;
    if (uVar10 < 8) {
      sVar19 = uVar10;
    }
    iVar5 = memcmp("Language",(void *)plVar15[4],sVar19);
    if (iVar5 == 0) {
      if (uVar10 < 9) {
LAB_1402de850:
        if (((plVar15 != plVar12) && (plVar15[6] != plVar15[7])) &&
           (*(longlong *)(plVar15[7] + -8) != 0)) {
          local_res18[0] = 0;
          FUN_1402e3fc0(param_2 + 0xe8,local_1c8,local_res18);
        }
      }
    }
    else if (-1 < iVar5) goto LAB_1402de850;
  }
  plVar12 = *(longlong **)(param_1 + 0x38);
  plVar11 = (longlong *)plVar12[1];
  plVar15 = plVar12;
  if (*(char *)((longlong)plVar11 + 0x19) == '\0') {
    do {
      uVar10 = plVar11[5];
      uVar18 = uVar10;
      if (10 < uVar10) {
        uVar18 = 10;
      }
      iVar5 = memcmp((void *)plVar11[4],"StylePoint",uVar18);
      plVar15 = plVar11;
      if (iVar5 == 0) {
        if (uVar10 < 10) {
LAB_1402de90b:
          plVar11 = plVar11 + 2;
          plVar15 = plVar12;
        }
      }
      else if (iVar5 < 0) goto LAB_1402de90b;
      plVar11 = (longlong *)*plVar11;
      plVar12 = plVar15;
    } while (*(char *)((longlong)plVar11 + 0x19) == '\0');
    plVar12 = *(longlong **)(param_1 + 0x38);
  }
  if (*(char *)((longlong)plVar15 + 0x19) == '\0') {
    uVar10 = plVar15[5];
    sVar19 = 10;
    if (uVar10 < 10) {
      sVar19 = uVar10;
    }
    iVar5 = memcmp("StylePoint",(void *)plVar15[4],sVar19);
    if (iVar5 == 0) {
      if (uVar10 < 0xb) {
LAB_1402de913:
        if (((plVar15 != plVar12) && (plVar15[6] != plVar15[7])) &&
           (*(longlong *)(plVar15[7] + -8) != 0)) goto LAB_1402debec;
      }
    }
    else if (-1 < iVar5) goto LAB_1402de913;
  }
  plVar11 = (longlong *)plVar12[1];
  cVar4 = *(char *)((longlong)plVar11 + 0x19);
  while (cVar4 == '\0') {
    uVar10 = plVar11[5];
    uVar18 = uVar10;
    if (9 < uVar10) {
      uVar18 = 9;
    }
    iVar5 = memcmp((void *)plVar11[4],"StyleLine",uVar18);
    plVar15 = plVar11;
    if (iVar5 == 0) {
      if (uVar10 < 9) {
LAB_1402dea59:
        plVar11 = plVar11 + 2;
        plVar15 = plVar12;
      }
    }
    else if (iVar5 < 0) goto LAB_1402dea59;
    plVar11 = (longlong *)*plVar11;
    plVar12 = plVar15;
    cVar4 = *(char *)((longlong)plVar11 + 0x19);
  }
  if (*(char *)((longlong)plVar12 + 0x19) == '\0') {
    uVar10 = plVar12[5];
    sVar19 = 9;
    if (uVar10 < 9) {
      sVar19 = uVar10;
    }
    iVar5 = memcmp("StyleLine",(void *)plVar12[4],sVar19);
    if (iVar5 == 0) {
      if (9 < uVar10) goto LAB_1402de9b0;
    }
    else if (iVar5 < 0) goto LAB_1402de9b0;
    plVar11 = *(longlong **)(param_1 + 0x38);
    if (((plVar12 != plVar11) && (plVar12[6] != plVar12[7])) &&
       (*(longlong *)(plVar12[7] + -8) != 0)) goto LAB_1402debec;
  }
  else {
LAB_1402de9b0:
    plVar11 = *(longlong **)(param_1 + 0x38);
  }
  plVar12 = (longlong *)plVar11[1];
  plVar15 = plVar11;
  if (*(char *)((longlong)plVar12 + 0x19) == '\0') {
    do {
      uVar10 = plVar12[5];
      uVar18 = uVar10;
      if (9 < uVar10) {
        uVar18 = 9;
      }
      iVar5 = memcmp((void *)plVar12[4],"StyleMesh",uVar18);
      plVar15 = plVar12;
      if (iVar5 == 0) {
        if (uVar10 < 9) {
LAB_1402dea9c:
          plVar12 = plVar12 + 2;
          plVar15 = plVar11;
        }
      }
      else if (iVar5 < 0) goto LAB_1402dea9c;
      plVar12 = (longlong *)*plVar12;
      plVar11 = plVar15;
    } while (*(char *)((longlong)plVar12 + 0x19) == '\0');
    plVar11 = *(longlong **)(param_1 + 0x38);
  }
  if (*(char *)((longlong)plVar15 + 0x19) == '\0') {
    uVar10 = plVar15[5];
    sVar19 = 9;
    if (uVar10 < 9) {
      sVar19 = uVar10;
    }
    iVar5 = memcmp("StyleMesh",(void *)plVar15[4],sVar19);
    if (iVar5 == 0) {
      if (9 < uVar10) {
LAB_1402deaa7:
        plVar11 = *(longlong **)(param_1 + 0x38);
        goto LAB_1402deaae;
      }
    }
    else if (iVar5 < 0) goto LAB_1402deaa7;
    if (((plVar15 != plVar11) && (plVar15[6] != plVar15[7])) &&
       (*(longlong *)(plVar15[7] + -8) != 0)) goto LAB_1402debec;
  }
LAB_1402deaae:
  plVar12 = (longlong *)plVar11[1];
  plVar15 = plVar11;
  if (*(char *)((longlong)plVar12 + 0x19) == '\0') {
    do {
      uVar10 = plVar12[5];
      uVar18 = uVar10;
      if (0x11 < uVar10) {
        uVar18 = 0x11;
      }
      iVar5 = memcmp((void *)plVar12[4],"StyleSpeedPalette",uVar18);
      plVar15 = plVar12;
      if (iVar5 == 0) {
        if (uVar10 < 0x11) {
LAB_1402deb46:
          plVar12 = plVar12 + 2;
          plVar15 = plVar11;
        }
      }
      else if (iVar5 < 0) goto LAB_1402deb46;
      plVar12 = (longlong *)*plVar12;
      plVar11 = plVar15;
    } while (*(char *)((longlong)plVar12 + 0x19) == '\0');
    plVar11 = *(longlong **)(param_1 + 0x38);
  }
  if (*(char *)((longlong)plVar15 + 0x19) == '\0') {
    uVar10 = plVar15[5];
    sVar19 = 0x11;
    if (uVar10 < 0x11) {
      sVar19 = uVar10;
    }
    iVar5 = memcmp("StyleSpeedPalette",(void *)plVar15[4],sVar19);
    if (iVar5 == 0) {
      if (0x11 < uVar10) {
LAB_1402deb4e:
        plVar11 = *(longlong **)(param_1 + 0x38);
        goto LAB_1402deb55;
      }
    }
    else if (iVar5 < 0) goto LAB_1402deb4e;
    if (((plVar15 != plVar11) && (plVar15[6] != plVar15[7])) &&
       (*(longlong *)(plVar15[7] + -8) != 0)) goto LAB_1402debec;
  }
LAB_1402deb55:
  local_228 = "StylePopulationPalette";
  local_220 = 0x16;
  plVar12 = (longlong *)plVar11[1];
  plVar15 = plVar11;
  if (*(char *)((longlong)plVar12 + 0x19) == '\0') {
    do {
      uVar10 = plVar12[5];
      uVar18 = uVar10;
      if (0x16 < uVar10) {
        uVar18 = 0x16;
      }
      iVar5 = memcmp((void *)plVar12[4],"StylePopulationPalette",uVar18);
      plVar11 = plVar12;
      if (iVar5 == 0) {
        if (uVar10 < 0x16) {
LAB_1402dec24:
          plVar12 = plVar12 + 2;
          plVar11 = plVar15;
        }
      }
      else if (iVar5 < 0) goto LAB_1402dec24;
      plVar12 = (longlong *)*plVar12;
      plVar15 = plVar11;
    } while (*(char *)((longlong)plVar12 + 0x19) == '\0');
    plVar15 = *(longlong **)(param_1 + 0x38);
    param_2 = local_res10;
  }
  if (*(char *)((longlong)plVar11 + 0x19) != '\0') {
    return;
  }
  cVar4 = FUN_140239220();
  if (cVar4 != '\0') {
    return;
  }
  if (plVar11 == plVar15) {
    return;
  }
  if (plVar11[6] == plVar11[7]) {
    return;
  }
  if (*(longlong *)(plVar11[7] + -8) == 0) {
    return;
  }
LAB_1402debec:
  local_res10 = CONCAT44(local_res10._4_4_,7);
  FUN_1402e3fc0(param_2 + 0xe8,local_1c8,&local_res10);
  return;
}


// Incoming references
// 0xc15b18 DATA caller none
// 0x2df3c5 UNCONDITIONAL_CALL caller 1402df0c0
