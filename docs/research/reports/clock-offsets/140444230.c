
/* WARNING: Type propagation algorithm not settling */

void FUN_140444230(longlong *param_1,ulonglong *param_2,undefined8 *param_3)

{
  float fVar1;
  code *pcVar2;
  undefined8 *******pppppppuVar3;
  undefined8 *puVar4;
  undefined8 uVar5;
  undefined1 auVar6 [16];
  float fVar7;
  undefined1 auVar8 [16];
  float fVar9;
  double dVar10;
  float fVar11;
  double dVar12;
  double *pdVar13;
  char cVar14;
  short sVar15;
  int iVar16;
  longlong lVar17;
  longlong **pplVar18;
  longlong *plVar19;
  char *pcVar20;
  undefined1 *puVar21;
  int *piVar22;
  double **ppdVar23;
  double *pdVar24;
  double *pdVar25;
  undefined1 *puVar26;
  longlong *plVar27;
  undefined8 *******pppppppuVar28;
  undefined8 *******pppppppuVar29;
  undefined8 *puVar30;
  longlong lVar31;
  int iVar32;
  longlong **pplVar33;
  int *piVar34;
  longlong lVar35;
  double *pdVar36;
  double *pdVar37;
  ulonglong *puVar38;
  double *pdVar39;
  undefined8 *puVar40;
  int iVar41;
  double *pdVar42;
  undefined8 *******pppppppuVar43;
  ulonglong uVar44;
  ulonglong uVar45;
  ulonglong uVar46;
  uint uVar47;
  uint uVar48;
  bool bVar49;
  double dVar50;
  double dVar51;
  undefined4 uVar52;
  undefined8 *local_res18;
  longlong *local_res20;
  char *in_stack_fffffffffffffad0;
  undefined4 uVar53;
  uint local_4f8;
  undefined1 *local_4f0;
  int local_4e8;
  longlong local_4e0;
  char *local_4d8;
  undefined1 *local_4d0;
  int local_4c8;
  int local_4c4;
  undefined8 *local_4c0;
  undefined8 local_4b8;
  undefined1 *puStack_4b0;
  undefined1 auStack_4a8 [16];
  undefined1 *local_498;
  longlong *local_490;
  longlong *local_488;
  undefined8 *******local_480;
  undefined1 *puStack_478;
  double *local_470;
  undefined2 local_468;
  undefined2 local_466;
  undefined4 local_464;
  undefined8 *******local_460;
  undefined8 *******pppppppuStack_458;
  undefined1 local_450 [16];
  ulonglong uStack_440;
  undefined1 auStack_438 [32];
  double local_418;
  char local_410;
  undefined2 local_40f;
  undefined8 *******local_408;
  undefined1 *puStack_400;
  undefined8 local_3f8;
  longlong *local_3f0;
  int local_3e8 [2];
  longlong *local_3e0;
  longlong *local_3d8 [2];
  longlong local_3c8 [2];
  double *local_3b8;
  char local_3a0;
  int local_39c;
  ulonglong local_398;
  float fStack_390;
  int iStack_38c;
  undefined8 local_388;
  undefined1 uStack_380;
  undefined1 uStack_37f;
  undefined6 uStack_37e;
  undefined1 local_378;
  undefined1 auStack_377 [15];
  undefined1 local_368;
  uint7 uStack_367;
  int local_358;
  int local_354;
  undefined1 **local_350;
  longlong local_348;
  longlong local_340;
  longlong local_338;
  longlong *local_330;
  undefined8 *local_328;
  longlong *local_320;
  undefined1 *local_318;
  undefined1 *local_310;
  double *local_308;
  undefined1 *local_2f8;
  undefined1 local_2f0 [40];
  undefined8 local_2c8;
  undefined8 uStack_2c0;
  undefined8 local_2b8;
  double local_2b0;
  undefined8 uStack_2a8;
  undefined8 local_2a0;
  undefined8 local_258;
  longlong lStack_250;
  undefined8 local_248;
  undefined8 uStack_240;
  undefined8 local_238;
  undefined8 uStack_230;
  undefined1 local_228 [16];
  undefined1 local_218 [16];
  longlong local_208;
  longlong *local_200;
  longlong local_1f8;
  undefined8 local_1f0;
  undefined1 *puStack_1e8;
  undefined8 local_1e0;
  undefined8 uStack_1d8;
  undefined8 *******local_1d0;
  undefined8 *******pppppppuStack_1c8;
  undefined8 *******local_1c0;
  undefined1 *puStack_1b8;
  double *local_1b0;
  undefined8 local_1a8;
  longlong lStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined8 uStack_180;
  undefined4 local_178;
  undefined4 uStack_174;
  float fStack_170;
  int iStack_16c;
  longlong local_168;
  undefined8 uStack_160;
  undefined1 local_158 [16];
  longlong local_148;
  char local_140;
  double *local_138;
  double *local_130;
  double **local_128;
  double *local_118;
  double local_110 [8];
  double *local_d0;
  longlong local_c8;
  longlong local_c0;
  longlong *local_b8;
  undefined8 **local_b0;

  uVar46 = 0;
  local_b8 = param_1;
  local_b0 = &local_res18;
  local_408 = (undefined8 *******)0x0;
  puStack_400 = (undefined1 *)0x0;
  local_3f8 = CONCAT62(local_3f8._2_6_,0x101);
  local_410 = '\0';
  if (*(char *)(param_3 + 0x3a) == '\0') {
    local_40f = 0x100;
  }
  else {
    local_40f = 0x101;
    local_408 = (undefined8 *******)param_3[0x17];
    puStack_400 = (undefined1 *)param_3[0x18];
    local_3f8 = param_3[0x19];
  }
  plVar19 = *(longlong **)(param_1[3] + 0x2108);
  local_3f0 = plVar19;
  if (*(char *)(param_3 + 0xb3) != '\0') {
    *(int *)(param_3 + 0xad) = *(int *)(param_3 + 0xad) + 1;
  }
  local_338 = *param_1;
  plVar27 = param_1 + 0x15;
  local_320 = plVar27;
  puVar30 = (undefined8 *)param_2[0x28];
  puVar40 = (undefined8 *)param_2[0x29];
  local_res18 = param_3;
  if (puVar30 != puVar40) {
    lVar31 = *(longlong *)*plVar27;
    do {
      lVar17 = FUN_14033f6a0(lVar31 + 0x300,*puVar30);
      if (((lVar17 != 0) && (*(char *)(lVar17 + 0xd8) != '\0')) &&
         (*(longlong *)(lVar17 + 0xe0) == *(longlong *)(lVar17 + 0xe8))) {
        if (((ulonglong)*(uint *)(puVar30 + 9) <
             (ulonglong)
             ((*(longlong *)(lVar17 + 0x100) - *(longlong *)(lVar17 + 0xf8) >> 3) *
             -0x1111111111111111)) &&
           (pcVar2 = *(code **)((ulonglong)*(uint *)(puVar30 + 9) * 0x78 + 0x48 +
                               *(longlong *)(lVar17 + 0xf8)), pcVar2 != (code *)0x0)) {
          in_stack_fffffffffffffad0 = &local_410;
          (*pcVar2)(*(undefined8 *)(local_338 + 0x1580),puVar30[2],plVar27,param_2,local_res18,
                    in_stack_fffffffffffffad0);
          plVar19 = local_3f0;
          break;
        }
      }
      puVar30 = puVar30 + 10;
    } while (puVar30 != puVar40);
  }
  lVar31 = *(longlong *)(param_1[3] + 0x2108);
  local_488 = (longlong *)(lVar31 + 0x93a80);
  pplVar33 = &local_res20;
  if (lVar31 + -0x93a80 <= (longlong)plVar19) {
    pplVar33 = &local_3f0;
  }
  pplVar18 = &local_488;
  if ((longlong)plVar19 <= (longlong)local_488) {
    pplVar18 = pplVar33;
  }
  local_3f0 = *pplVar18;
  local_res20 = (longlong *)((longlong)*pplVar18 - *(longlong *)(param_1[3] + 0x2100));
  if ((longlong)local_res20 < 0) {
    local_res20 = (longlong *)((longlong)local_res20 % 0x93a80);
    if ((longlong)local_res20 < 0) {
      local_res20 = local_res20 + 0x12750;
    }
  }
  local_3d8[0] = (longlong *)0x93a80;
  local_488 = (longlong *)0x0;
  pplVar33 = &local_488;
  if (-1 < (longlong)local_res20) {
    pplVar33 = &local_res20;
  }
  pplVar18 = local_3d8;
  if ((longlong)local_res20 < 0x93a81) {
    pplVar18 = pplVar33;
  }
  local_200 = *pplVar18;
  iVar16 = 0;
  if (local_410 != '\0') {
    if (*(char *)(local_res18 + 0xb3) == '\0') {
      return;
    }
    *(undefined4 *)((longlong)local_res18 + 0x56c) = 0;
    *(undefined4 *)(local_res18 + 0xae) = 0;
    *(undefined4 *)((longlong)local_res18 + 0x574) = 0;
    return;
  }
  local_4c0 = (undefined8 *)0x0;
  if (*(char *)(local_res18 + 0xb3) != '\0') {
    local_res18[0xb2] = 0;
    local_res18[0xb0] = local_res18[0xaf];
    lVar31 = param_1[3];
    puVar38 = *(ulonglong **)
               (*(longlong *)(lVar31 + 0x2260) +
               (*param_2 % (ulonglong)*(uint *)(lVar31 + 0x2268)) * 8);
    uVar44 = uVar46;
    if (puVar38 != (ulonglong *)0x0) {
      do {
        uVar45 = uVar44 + 1;
        if (*param_2 != *puVar38) {
          uVar45 = uVar44;
        }
        puVar38 = (ulonglong *)puVar38[1];
        uVar44 = uVar45;
      } while (puVar38 != (ulonglong *)0x0);
      if (uVar45 != 0) {
        local_res18[0xb2] = *(undefined8 *)(lVar31 + 0x2108);
        local_4c0 = local_res18 + 0xaf;
      }
    }
  }
  puVar30 = local_4c0;
  local_140 = '\0';
  local_118 = local_110;
  local_130 = local_110;
  local_138 = local_110;
  local_128 = &local_d0;
  local_d0 = (double *)0x0;
  local_4c4 = 0;
  local_4f8 = 0;
  local_4c8 = 0;
  plVar19 = (longlong *)FUN_1403f21b0(param_1[7],*param_2);
  dVar12 = DAT_140aabd08;
  fVar11 = DAT_140aabac8;
  dVar10 = DAT_140aaba08;
  fVar9 = DAT_140aab908;
  local_488 = (longlong *)*plVar19;
  plVar19 = (longlong *)plVar19[1];
  iVar32 = iVar16;
  iVar41 = iVar16;
  local_490 = plVar19;
  if (local_488 != plVar19) {
    uVar52 = SUB84(DAT_140aabd08,0);
    do {
      if ((*local_488 != 0) &&
         (pcVar20 = (char *)FUN_14033f710(*(longlong *)param_1[7] + 0x280), plVar19 = local_490,
         local_4d8 = pcVar20, pcVar20 != (char *)0x0)) {
        if (*(int *)(pcVar20 + 0x70) == 1) {
          FUN_14043dd80(pcVar20,&local_348,*param_2);
          plVar19 = local_490;
          if (local_348 != local_340) {
LAB_140444700:
            plVar19 = (longlong *)0x0;
            local_4c4 = local_4c4 + 1;
            local_4f0 = *(undefined1 **)(local_348 + 8);
            auVar6._8_8_ = 0;
            auVar6._0_8_ = local_4f0;
            FUN_14045b1e0(param_1 + 0x1c,&local_c8,&local_4f0,
                          SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,8) +
                          SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,0));
            puVar26 = local_4f0;
            if ((local_c8 == param_1[0x1f] + param_1[0x1c]) || (*(longlong *)(local_c0 + 8) == 0)) {
              for (plVar27 = *(longlong **)
                              (*(longlong *)(pcVar20 + 0x1b8) +
                              ((ulonglong)local_4f0 % (ulonglong)*(uint *)(pcVar20 + 0x1c0)) * 8);
                  plVar27 != (longlong *)0x0; plVar27 = (longlong *)plVar27[0x3b]) {
                if (local_4f0 == (undefined1 *)*plVar27) {
                  if ((plVar27 != (longlong *)0x0) &&
                     (local_3e0 = plVar27 + 1,
                     plVar27 !=
                     *(longlong **)
                      (*(longlong *)(pcVar20 + 0x1b8) + *(longlong *)(pcVar20 + 0x1c0) * 8)))
                  goto LAB_1404447ed;
                  break;
                }
              }
              local_3e0 = plVar19;
LAB_1404447ed:
              if (((local_3e0 != (longlong *)0x0) &&
                  (FUN_14043ea50(local_3c8,param_1[6],local_3e0,(ulonglong)local_200 & 0xffffffff),
                  pdVar42 = local_3b8, iStack_38c < 2)) &&
                 (bVar49 = local_388._4_1_ == '\0', bVar49)) {
                if (((local_3a0 != '\0') ||
                    ((local_3c8[0] != 0 &&
                     (*(longlong *)(local_3c8[0] + 0x50) == *(longlong *)(local_3c8[0] + 0x58)))))
                   || (local_3b8 == (double *)0x0)) {
                  if (puVar30 != (undefined8 *)0x0) {
                    local_4b8 = *(undefined8 *)pcVar20;
                    puStack_4b0 = puVar26;
                    auStack_4a8._0_12_ = ZEXT812(0);
                    auStack_4a8._12_4_ = 3;
                    FUN_1403bc2c0(puVar30,&local_4b8);
                  }
                  goto LAB_140445bb8;
                }
                puVar21 = (undefined1 *)
                          FUN_14033f7f0(*(longlong *)(param_1[1] + 0x380) + 0x180,local_3b8);
                local_498 = puVar21;
                if ((puVar21 == (undefined1 *)0x0) || (*(int *)(puVar21 + 0xfc) == 0)) {
                  if (puVar30 != (undefined8 *)0x0) {
                    local_480 = *(undefined8 ********)pcVar20;
                    puStack_478 = puVar26;
                    local_470 = pdVar42;
                    local_468 = 0;
                    local_464 = 4;
                    local_466 = (short)local_39c;
                    FUN_1403bc2c0(puVar30,&local_480);
                  }
                  goto LAB_140445bb8;
                }
                local_330 = (longlong *)(puVar21 + 0x118);
                iVar32 = (int)(*(longlong *)(puVar21 + 0x120) - *local_330 >> 3) * 0x2fa0be83;
                iVar16 = 0;
                if (0 < (int)local_398) {
                  iVar16 = (int)local_398;
                }
                if ((iVar32 < 1) ||
                   (lVar31 = (longlong)((iVar32 + iVar16 % iVar32) % iVar32) * 0x158 + *local_330,
                   pdVar42 = local_3b8, lVar31 == 0)) {
                  if (puVar30 != (undefined8 *)0x0) {
                    local_480 = *(undefined8 ********)pcVar20;
                    puStack_478 = puVar26;
                    local_468 = (undefined2)local_398;
                    local_464 = 5;
                    local_470 = pdVar42;
                    local_466 = (short)local_39c;
                    FUN_1403bc2c0(puVar30,&local_480);
                  }
                  goto LAB_140445bb8;
                }
                local_3e8[0] = local_39c;
                lVar17 = local_3e0[10];
                local_354 = (int)(local_3e0[0xb] - lVar17 >> 3);
                local_358 = 0;
                piVar34 = &local_358;
                if (-1 < local_39c) {
                  piVar34 = local_3e8;
                }
                piVar22 = &local_354;
                if (local_39c <= local_354) {
                  piVar22 = piVar34;
                }
                local_3e8[0] = *piVar22;
                lVar35 = (longlong)*(int *)(lVar17 + 4 + (longlong)local_3e8[0] * 8) * 0x20;
                puVar40 = (undefined8 *)(lVar35 + local_3e0[0xd]);
                puStack_4b0 = (undefined1 *)puVar40[1];
                auStack_4a8 = *(undefined1 (*) [16])(lVar35 + 0x10 + local_3e0[0xd]);
                iVar16 = *(int *)(lVar17 + (longlong)local_3e8[0] * 8);
                local_4b8 = CONCAT44((int)((ulonglong)*puVar40 >> 0x20),iVar16);
                local_3d8[0] = (longlong *)
                               ((longlong)(int)local_388 +
                               *(longlong *)(param_1[3] + 0x2100) + (longlong)iVar16);
                iVar16 = FUN_1403554e0(&local_4b8,local_330,local_398 & 0xffffffff);
                local_208 = (longlong)local_3d8[0] + (longlong)iVar16;
                if ((longlong)local_3f0 < local_208) {
                  if (puVar30 != (undefined8 *)0x0) {
                    local_480 = *(undefined8 ********)pcVar20;
                    puStack_478 = puVar26;
                    local_470 = local_3b8;
                    local_468 = (undefined2)local_398;
                    local_466 = (undefined2)local_39c;
                    local_464 = 6;
                    FUN_1403bc2c0(puVar30,&local_480);
                  }
                  goto LAB_140445bb8;
                }
                local_218[8] = 0;
                local_res20 = (longlong *)((ulonglong)local_res20 & 0xffffffffffffff00);
                if ((char)local_40f != '\0') {
                  if (local_138 == local_130) {
                    local_480 = local_408;
                    puStack_478 = puStack_400;
                    local_470._1_7_ = (undefined7)((ulonglong)local_3f8 >> 8);
                    local_470 = (double *)CONCAT71(local_470._1_7_,-(char)local_3f8);
                    ppdVar23 = (double **)
                               FUN_140377010(&local_318,param_1[5],&local_480,
                                             SUB84((double)*(float *)(local_res18 + 8),0));
                    if (&local_138 != ppdVar23) {
                      local_130 = local_138;
                      FUN_140341810(&local_138,*ppdVar23,ppdVar23[1]);
                    }
                    local_d0 = ppdVar23[0xd];
                    if ((local_318 != (undefined1 *)0x0) && (local_318 != local_2f8)) {
                      free(local_318);
                    }
                  }
                  pdVar13 = local_130;
                  pdVar37 = local_138;
                  local_328 = (undefined8 *)(lVar31 + 8);
                  pdVar42 = *(double **)(lVar31 + 0x10);
                  pdVar39 = (double *)*local_328;
                  puVar26 = puVar21;
                  if ((pdVar39 != pdVar42) && (local_138 != local_130)) {
                    local_4e0 = lVar31;
                    local_4d0 = puVar21;
                    cVar14 = FUN_1403856e0(local_328,*local_138,SUB84(local_138[1],0),
                                           (int)*(char *)(local_138 + 3));
                    if ((cVar14 == '\0') &&
                       (local_4e0 = lVar31, local_4d0 = puVar21,
                       cVar14 = FUN_1403856e0(local_328,pdVar13[-4],SUB84(pdVar13[-2],0),
                                              (int)*(char *)(pdVar13 + -1)), cVar14 == '\0')) {
                      pdVar25 = pdVar37 + 2;
                      do {
                        if (*pdVar39 == pdVar25[-2]) {
                          dVar50 = pdVar39[1];
                          if (*(char *)(pdVar39 + 3) != *(char *)(pdVar25 + 1)) {
                            dVar50 = dVar12 - pdVar39[1];
                          }
                          pdVar36 = pdVar25 + -1;
                          pdVar24 = pdVar36;
                          if (*pdVar36 < *pdVar25) {
                            pdVar24 = pdVar25;
                          }
                          if (*pdVar25 < *pdVar36) {
                            pdVar36 = pdVar25;
                          }
                          local_498 = puVar21;
                          if ((*pdVar36 <= dVar50) &&
                             (local_4e0 = lVar31, local_4d0 = puVar21,
                             dVar50 < *pdVar24 || dVar50 == *pdVar24)) goto LAB_140444d18;
                        }
                        pdVar36 = pdVar25 + 2;
                        pdVar25 = pdVar25 + 4;
                      } while (pdVar36 != pdVar13);
                      pdVar39 = pdVar37 + 2;
                      do {
                        if (pdVar42[-4] == pdVar39[-2]) {
                          dVar50 = pdVar42[-2];
                          if (*(char *)(pdVar42 + -1) != *(char *)(pdVar39 + 1)) {
                            dVar50 = dVar12 - pdVar42[-2];
                          }
                          pdVar37 = pdVar39 + -1;
                          pdVar25 = pdVar37;
                          if (*pdVar37 < *pdVar39) {
                            pdVar25 = pdVar39;
                          }
                          if (*pdVar39 < *pdVar37) {
                            pdVar37 = pdVar39;
                          }
                          local_498 = puVar21;
                          if ((*pdVar37 <= dVar50) &&
                             (local_4e0 = lVar31, local_4d0 = puVar21,
                             dVar50 < *pdVar25 || dVar50 == *pdVar25)) goto LAB_140444d18;
                        }
                        pdVar37 = pdVar39 + 2;
                        pdVar39 = pdVar39 + 4;
                        puVar26 = local_498;
                      } while (pdVar37 != pdVar13);
                      goto LAB_140444c4c;
                    }
LAB_140444d18:
                    local_4e8 = 0;
                    puVar30 = local_328;
LAB_140444ef7:
                    pcVar20 = local_4d8;
                    if (puVar30 != (undefined8 *)0x0) {
                      if (local_40f._1_1_ == '\0') {
                        in_stack_fffffffffffffad0 =
                             (char *)CONCAT44((int)((ulonglong)in_stack_fffffffffffffad0 >> 0x20),
                                              *(undefined4 *)(local_res18 + 8));
                        puVar30 = (undefined8 *)
                                  FUN_140442db0(param_1,&local_480,puVar21,local_4e0,local_4e8,
                                                in_stack_fffffffffffffad0);
                        local_258 = *puVar30;
                        lStack_250 = puVar30[1];
                        local_248 = puVar30[2];
                        uStack_240 = puVar30[3];
                        local_238 = puVar30[4];
                        uStack_230 = puVar30[5];
                        local_228 = *(undefined1 (*) [16])(puVar30 + 6);
                        local_218._9_7_ = (undefined7)((ulonglong)puVar30[9] >> 8);
                        local_218._0_9_ = *(unkbyte9 *)(puVar30 + 8);
                        pcVar20 = local_4d8;
                        goto LAB_140445797;
                      }
                      plVar19 = (longlong *)param_1[5];
                      pppppppuVar3 = (undefined8 *******)puVar30[0xe];
                      if (pppppppuVar3 == (undefined8 *******)0x0) {
                        plVar27 = (longlong *)0x0;
                      }
                      else {
                        plVar27 = (longlong *)FUN_14032c420(*plVar19,pppppppuVar3);
                      }
                      pppppppuVar43 = local_408;
                      if (((*(char *)((longlong)puVar30 + 0x81) == '\0') ||
                          (plVar27 == (longlong *)0x0)) ||
                         ((undefined8 *******)*plVar27 != pppppppuVar3)) {
                        dVar50 = (double)puVar30[0xf];
                      }
                      else {
                        dVar50 = (double)puVar30[0xf];
                        if (*(char *)((longlong)plVar27 + 0x2c) !=
                            *(char *)((longlong)puVar30 + 0x81)) {
                          dVar50 = dVar12 - dVar50;
                        }
                      }
                      sVar15 = (short)((ulonglong)local_408 >> 0x30);
                      uVar48 = (uint)((ulonglong)local_408 >> 0x10);
                      if ((local_408 == (undefined8 *******)0x0) || (lVar31 = *plVar19, sVar15 != 1)
                         ) {
LAB_140444fcd:
                        plVar27 = (longlong *)0x0;
                      }
                      else {
                        uVar47 = *(uint *)(lVar31 + 0x10) & uVar48;
                        uVar46 = ((longlong)local_408 >> 0x10 & 0xffffffffU) >>
                                 ((byte)*(undefined4 *)(lVar31 + 4) & 0x3f);
                        if (((((ulonglong)
                               (*(longlong *)(lVar31 + 0x20) - *(longlong *)(lVar31 + 0x18) >> 3) <=
                               uVar46) ||
                             ((*(uint *)(lVar31 + 8) <= uVar47 ||
                              (lVar31 = *(longlong *)(*(longlong *)(lVar31 + 0x18) + uVar46 * 8),
                              lVar31 == 0)))) ||
                            (plVar27 = (longlong *)((ulonglong)uVar47 * 0x4e8 + lVar31),
                            plVar27 == (longlong *)0x0)) ||
                           (local_408 != (undefined8 *******)*plVar27)) goto LAB_140444fcd;
                      }
                      puVar26 = puStack_400;
                      if ((((local_3f8._1_1_ != '\0') && (plVar27 != (longlong *)0x0)) &&
                          ((undefined8 *******)*plVar27 == local_408)) &&
                         (*(char *)((longlong)plVar27 + 0x2c) != local_3f8._1_1_)) {
                        puVar26 = (undefined1 *)(dVar12 - (double)puStack_400);
                      }
                      if (local_408 != (undefined8 *******)0x0) {
                        lVar31 = *plVar19;
                        if (sVar15 == 1) {
                          uVar48 = *(uint *)(lVar31 + 0x10) & uVar48;
                          uVar46 = ((longlong)local_408 >> 0x10 & 0xffffffffU) >>
                                   ((byte)*(undefined4 *)(lVar31 + 4) & 0x3f);
                          if ((((ulonglong)
                                (*(longlong *)(lVar31 + 0x20) - *(longlong *)(lVar31 + 0x18) >> 3)
                                <= uVar46) || (*(uint *)(lVar31 + 8) <= uVar48)) ||
                             ((lVar17 = *(longlong *)(*(longlong *)(lVar31 + 0x18) + uVar46 * 8),
                              lVar17 == 0 ||
                              ((plVar27 = (longlong *)((ulonglong)uVar48 * 0x4e8 + lVar17),
                               plVar27 == (longlong *)0x0 ||
                               (local_408 != (undefined8 *******)*plVar27)))))) goto LAB_14044505e;
                        }
                        else {
LAB_14044505e:
                          plVar27 = (longlong *)0x0;
                        }
                        if (((plVar27 == (longlong *)0x0) ||
                            (pppppppuVar3 == (undefined8 *******)0x0)) ||
                           (lVar17 = FUN_14032c420(lVar31,pppppppuVar3),
                           pppppppuVar29 = pppppppuVar43, pppppppuVar28 = pppppppuVar43, lVar17 == 0
                           )) goto LAB_140445508;
                        while ((pppppppuVar28 != pppppppuVar3 && (pppppppuVar29 != pppppppuVar3))) {
                          if (pppppppuVar28 == (undefined8 *******)0x0) {
                            if (pppppppuVar29 == (undefined8 *******)0x0) {
                              local_2f8 = local_2f0;
                              local_310 = local_2f0;
                              local_318 = local_2f0;
                              local_308 = &local_2b0;
                              local_2b0 = 0.0;
                              goto LAB_140445103;
                            }
LAB_1404451c1:
                            pppppppuVar28 = (undefined8 *******)0x0;
                          }
                          else {
                            lVar17 = FUN_14032c420(lVar31,pppppppuVar28);
                            if (lVar17 == 0) goto LAB_1404451c1;
                            pppppppuVar28 = *(undefined8 ********)(lVar17 + 8);
                          }
                          if ((pppppppuVar29 == (undefined8 *******)0x0) ||
                             (lVar17 = FUN_14032c420(lVar31,pppppppuVar29), lVar17 == 0)) {
                            pppppppuVar29 = (undefined8 *******)0x0;
                          }
                          else {
                            pppppppuVar29 = *(undefined8 ********)(lVar17 + 0x10);
                          }
                          if (pppppppuVar28 == pppppppuVar43) {
                            pppppppuVar28 = (undefined8 *******)0x0;
                          }
                          if (pppppppuVar29 == pppppppuVar43) {
                            pppppppuVar29 = (undefined8 *******)0x0;
                          }
                        }
                        local_470 = &local_418;
                        local_418 = 0.0;
                        puStack_478 = auStack_438;
                        pppppppuStack_458 = pppppppuVar43;
                        uStack_440 = (ulonglong)*(byte *)((longlong)plVar27 + 0x2c);
                        local_450._8_8_ = 0;
                        local_450._0_8_ = puVar26;
                        local_480 = &pppppppuStack_458;
                        local_460 = &pppppppuStack_458;
                        if (pppppppuVar43 == pppppppuVar3) {
                          local_450._8_8_ = dVar50;
                          local_418 = (double)FUN_14038a650(plVar27,puStack_478,SUB84(dVar50,0));
                          goto LAB_14044543c;
                        }
                        if (pppppppuVar28 == pppppppuVar3) {
                          local_450._8_8_ = 0;
                          local_418 = (double)FUN_14038a650(plVar27,puStack_478,0);
                          pppppppuVar43 = (undefined8 *******)plVar27[1];
                          while ((pppppppuVar43 != (undefined8 *******)0x0 &&
                                 (lVar31 = FUN_14032c420(*plVar19,pppppppuVar43), lVar31 != 0))) {
                            puVar30 = (undefined8 *)FUN_14037b180(&local_480);
                            *puVar30 = pppppppuVar43;
                            *(undefined1 *)(puVar30 + 3) = *(undefined1 *)(lVar31 + 0x2c);
                            puVar30[1] = 0x3ff0000000000000;
                            if (pppppppuVar43 == pppppppuVar3) goto LAB_14044541e;
                            puVar30[2] = 0;
                            uVar46 = *(longlong *)(lVar31 + 0x1b8) - *(longlong *)(lVar31 + 0x1b0)
                                     >> 4;
                            if (((uVar46 < 2) ||
                                (*(longlong *)(lVar31 + 0x350) - *(longlong *)(lVar31 + 0x348) >> 2
                                 != uVar46)) ||
                               (dVar51 = *(double *)(lVar31 + 0x88), dVar51 < dVar10)) {
                              dVar51 = 0.0;
                            }
                            local_418 = local_418 + dVar51;
                            pppppppuVar43 = *(undefined8 ********)(lVar31 + 8);
                          }
                          goto LAB_14044543c;
                        }
                        if (pppppppuVar29 == pppppppuVar3) {
                          local_450._8_8_ = 0x3ff0000000000000;
                          local_418 = (double)FUN_14038a650(plVar27,puStack_478,uVar52);
                          pppppppuVar43 = (undefined8 *******)plVar27[2];
                          while ((pppppppuVar43 != (undefined8 *******)0x0 &&
                                 (lVar31 = FUN_14032c420(*plVar19,pppppppuVar43), lVar31 != 0))) {
                            puVar30 = (undefined8 *)FUN_14037b180(&local_480);
                            *puVar30 = pppppppuVar43;
                            *(undefined1 *)(puVar30 + 3) = *(undefined1 *)(lVar31 + 0x2c);
                            puVar30[1] = 0;
                            if (pppppppuVar43 == pppppppuVar3) goto LAB_14044541e;
                            puVar30[2] = 0x3ff0000000000000;
                            uVar46 = *(longlong *)(lVar31 + 0x1b8) - *(longlong *)(lVar31 + 0x1b0)
                                     >> 4;
                            if (((uVar46 < 2) ||
                                (*(longlong *)(lVar31 + 0x350) - *(longlong *)(lVar31 + 0x348) >> 2
                                 != uVar46)) ||
                               (dVar51 = *(double *)(lVar31 + 0x88), dVar51 < dVar10)) {
                              dVar51 = 0.0;
                            }
                            local_418 = local_418 + dVar51;
                            pppppppuVar43 = *(undefined8 ********)(lVar31 + 0x10);
                          }
                          goto LAB_14044543c;
                        }
                        local_2f8 = local_2f0;
                        local_310 = local_2f0;
                        local_318 = local_2f0;
                        local_308 = &local_2b0;
                        local_2b0 = 0.0;
                        goto LAB_1404454a4;
                      }
LAB_140445508:
                      local_2f8 = local_2f0;
                      local_310 = local_2f0;
                      local_318 = local_2f0;
                      local_308 = &local_2b0;
                      local_2b0 = 0.0;
                      goto LAB_140445103;
                    }
                    goto LAB_140445797;
                  }
LAB_140444c4c:
                  local_498 = puVar26;
                  local_4e8 = 1;
                  local_350 = (undefined1 **)(lVar31 + 0xf0);
                  puVar26 = *local_350;
                  iVar16 = (int)(*(longlong *)(lVar31 + 0xf8) - (longlong)puVar26 >> 3) * 0x3cf3cf3d
                  ;
                  local_4e0 = lVar31;
                  local_4d0 = puVar26;
                  if (0 < iVar16) {
                    lVar17 = 0;
                    do {
                      pdVar37 = local_138;
                      puVar30 = (undefined8 *)(puVar26 + lVar17);
                      pdVar42 = (double *)puVar30[1];
                      pdVar39 = (double *)*puVar30;
                      if ((pdVar39 != pdVar42) && (local_138 != pdVar13)) {
                        cVar14 = FUN_1403856e0(puVar30,*local_138,SUB84(local_138[1],0),
                                               (int)*(char *)(local_138 + 3));
                        if ((cVar14 != '\0') ||
                           (cVar14 = FUN_1403856e0(puVar30,pdVar13[-4],SUB84(pdVar13[-2],0),
                                                   (int)*(char *)(pdVar13 + -1)), cVar14 != '\0')) {
LAB_140444e6d:
                          local_4d0 = local_498;
                          if (-1 < local_4e8) {
                            puVar30 = local_328;
                            puVar21 = local_498;
                            if (local_4e8 == 0) goto LAB_140444ef7;
                            pcVar20 = local_4d8;
                            if ((int)((longlong)local_350[1] - (longlong)*local_350 >> 3) *
                                0x3cf3cf3d < local_4e8) goto LAB_140445797;
                            puVar30 = (undefined8 *)(*local_350 + (longlong)(local_4e8 + -1) * 0xa8)
                            ;
                            goto LAB_140444ef7;
                          }
                          break;
                        }
                        pdVar25 = pdVar37 + 2;
                        do {
                          if (*pdVar39 == pdVar25[-2]) {
                            dVar50 = pdVar39[1];
                            if (*(char *)(pdVar39 + 3) != *(char *)(pdVar25 + 1)) {
                              dVar50 = dVar12 - pdVar39[1];
                            }
                            pdVar36 = pdVar25 + -1;
                            pdVar24 = pdVar36;
                            if (*pdVar36 < *pdVar25) {
                              pdVar24 = pdVar25;
                            }
                            if (*pdVar25 < *pdVar36) {
                              pdVar36 = pdVar25;
                            }
                            if ((*pdVar36 <= dVar50) && (dVar50 < *pdVar24 || dVar50 == *pdVar24))
                            goto LAB_140444e6d;
                          }
                          pdVar36 = pdVar25 + 2;
                          pdVar25 = pdVar25 + 4;
                        } while (pdVar36 != pdVar13);
                        pdVar39 = pdVar37 + 2;
                        do {
                          if (pdVar42[-4] == pdVar39[-2]) {
                            dVar50 = pdVar42[-2];
                            if (*(char *)(pdVar42 + -1) != *(char *)(pdVar39 + 1)) {
                              dVar50 = dVar12 - pdVar42[-2];
                            }
                            pdVar37 = pdVar39 + -1;
                            pdVar25 = pdVar37;
                            if (*pdVar37 < *pdVar39) {
                              pdVar25 = pdVar39;
                            }
                            if (*pdVar39 < *pdVar37) {
                              pdVar37 = pdVar39;
                            }
                            if ((*pdVar37 <= dVar50) && (dVar50 < *pdVar25 || dVar50 == *pdVar25))
                            goto LAB_140444e6d;
                          }
                          pdVar37 = pdVar39 + 2;
                          puVar26 = local_4d0;
                          pdVar39 = pdVar39 + 4;
                        } while (pdVar37 != pdVar13);
                      }
                      local_4e8 = local_4e8 + 1;
                      lVar17 = lVar17 + 0xa8;
                    } while (local_4e8 <= iVar16);
                  }
                  pdVar42 = local_3b8;
                  pcVar20 = local_4d8;
                  puVar26 = local_4f0;
                  cVar14 = '\x01';
                  local_res20 = (longlong *)CONCAT71(local_res20._1_7_,1);
                  iVar16 = (int)local_398;
                  if (local_4c0 != (undefined8 *)0x0) {
                    local_480 = *(undefined8 ********)local_4d8;
                    puStack_478 = local_4f0;
                    local_470 = local_3b8;
                    local_468 = (undefined2)local_398;
                    local_466 = (undefined2)local_39c;
                    local_464 = 7;
                    FUN_1403bc2c0(local_4c0,&local_480);
                    goto LAB_1404457a9;
                  }
                  goto LAB_1404457b0;
                }
                lVar35 = (*(longlong *)(lVar31 + 0xf8) - *(longlong *)(lVar31 + 0xf0) >> 3) *
                         -0x30c30c30c30c30c3;
                pdVar42 = local_3b8;
                cVar14 = '\0';
                lVar17 = lVar31;
                if (lVar35 != -1) {
                  do {
                    local_4e0 = lVar17;
                    in_stack_fffffffffffffad0 =
                         (char *)CONCAT44((int)((ulonglong)in_stack_fffffffffffffad0 >> 0x20),
                                          *(undefined4 *)(local_res18 + 8));
                    puVar30 = (undefined8 *)
                              FUN_140442db0(param_1,&local_480,puVar21,lVar31,
                                            (ulonglong)plVar19 & 0xffffffff,
                                            in_stack_fffffffffffffad0);
                    local_258 = *puVar30;
                    lStack_250 = puVar30[1];
                    local_248 = puVar30[2];
                    uStack_240 = puVar30[3];
                    local_238 = puVar30[4];
                    uStack_230 = puVar30[5];
                    local_228 = *(undefined1 (*) [16])(puVar30 + 6);
                    local_218 = *(undefined1 (*) [16])(puVar30 + 8);
                    if (local_218[8] != '\0') {
                      if (plVar19 == (longlong *)0x0) {
                        fVar1 = *(float *)(lVar31 + 0xa8);
                      }
                      else {
                        fVar1 = *(float *)((longlong)plVar19 * 0xa8 + -8 +
                                          *(longlong *)(lVar31 + 0xf0));
                      }
                      local_470._1_7_ = (undefined7)((ulonglong)local_248 >> 8);
                      local_470 = (double *)CONCAT71(local_470._1_7_,-(char)local_248);
                      fVar7 = fVar1 * fVar11;
                      if (fVar1 * fVar11 <= *(float *)(local_res18 + 8)) {
                        fVar7 = *(float *)(local_res18 + 8);
                      }
                      local_480 = (undefined8 *******)*puVar30;
                      puStack_478 = (undefined1 *)puVar30[1];
                      FUN_140377010(&local_318,param_1[5],&local_480,SUB84((double)fVar7,0));
                      if ((local_40f._1_1_ == '\0') ||
                         (iVar16 = FUN_140442f40(param_1,param_2,&local_318), iVar16 == 0)) {
                        if ((local_318 != (undefined1 *)0x0) && (local_318 != local_2f8)) {
                          free(local_318);
                        }
                        goto LAB_140445797;
                      }
                      if ((local_318 != (undefined1 *)0x0) && (local_318 != local_2f8)) {
                        free(local_318);
                      }
                    }
                    local_218[8] = 0;
                    plVar19 = (longlong *)((longlong)plVar19 + 1);
                    lVar17 = local_4e0;
                  } while (plVar19 < (longlong *)(lVar35 + 1));
                  pdVar42 = local_3b8;
                  puVar26 = local_4f0;
                  cVar14 = (char)local_res20;
                }
                goto LAB_14044574d;
              }
              goto LAB_140445bb8;
            }
            local_4f8 = (int)uVar46 + 1;
            uVar46 = (ulonglong)local_4f8;
            if (puVar30 != (undefined8 *)0x0) {
              local_4b8 = *(undefined8 *)pcVar20;
              puStack_4b0 = local_4f0;
              auStack_4a8._0_12_ = ZEXT812(0);
              auStack_4a8._12_4_ = 2;
              FUN_1403bc2c0(puVar30,&local_4b8);
            }
            goto LAB_140445bbd;
          }
        }
        else if (puVar30 != (undefined8 *)0x0) {
          local_4b8 = *(undefined8 *)pcVar20;
          puStack_4b0 = (undefined1 *)0x0;
          auStack_4a8._0_12_ = ZEXT812(0);
          auStack_4a8._12_4_ = 1;
          FUN_1403bc2c0(puVar30,&local_4b8);
          plVar19 = local_490;
        }
      }
LAB_140445bf9:
      puVar40 = local_res18;
      uVar53 = (undefined4)((ulonglong)in_stack_fffffffffffffad0 >> 0x20);
      plVar27 = (longlong *)0x0;
      iVar16 = (int)uVar46;
      local_488 = local_488 + 1;
    } while (local_488 != plVar19);
    iVar32 = local_4c4;
    iVar41 = local_4c8;
    if (local_140 != '\0') {
      local_res18[0xb4] = local_1f0;
      local_res18[0xb5] = puStack_1e8;
      local_res18[0xb6] = local_1e0;
      local_res18[0xb7] = uStack_1d8;
      local_res18[0xb8] = local_1d0;
      local_res18[0xb9] = pppppppuStack_1c8;
      if (*(char *)(local_res18 + 0xba) == '\0') {
        *(undefined1 *)(local_res18 + 0xba) = 1;
      }
      local_res18[0xbb] = local_1c0;
      local_res18[0xbc] = puStack_1b8;
      local_res18[0xbd] = local_1b0;
      if (*(char *)(local_res18 + 0xbe) == '\0') {
        *(undefined1 *)(local_res18 + 0xbe) = 1;
      }
      local_res18[0x44] = local_1a8;
      local_res18[0x45] = lStack_1a0;
      local_res18[0x46] = local_198;
      local_res18[0x47] = uStack_190;
      local_res18[0x48] = local_188;
      local_res18[0x49] = uStack_180;
      local_res18[0x4a] = CONCAT44(uStack_174,local_178);
      local_res18[0x4b] = CONCAT44(iStack_16c,fStack_170);
      local_res18[0x4c] = local_168;
      local_res18[0x4d] = uStack_160;
      local_res18[0x4e] = local_158._0_8_;
      local_res18[0x4f] = local_158._8_8_;
      local_res18[0x50] = local_148;
      if (*(char *)(local_res18 + 0x51) == '\0') {
        *(undefined1 *)(local_res18 + 0x51) = 1;
      }
      if (*(char *)(local_res18 + 0xb3) != '\0') {
        if ((void *)local_res18[0xaf] != (void *)0x0) {
          free((void *)local_res18[0xaf]);
        }
        *(undefined1 *)(puVar40 + 0xb3) = 0;
      }
      uVar5 = *local_res18;
      puVar30 = (undefined8 *)FUN_140456ab0(param_1 + 0x1c,&puStack_1b8);
      *puVar30 = uVar5;
      lVar31 = FUN_14033f710(*param_1 + 0x280);
      if (lVar31 != 0) {
        for (plVar19 = *(longlong **)
                        (*(longlong *)(lVar31 + 0x1b8) +
                        ((ulonglong)puStack_1b8 % (ulonglong)*(uint *)(lVar31 + 0x1c0)) * 8);
            plVar19 != (longlong *)0x0; plVar19 = (longlong *)plVar19[0x3b]) {
          if (puStack_1b8 == (undefined1 *)*plVar19) {
            if ((plVar19 != (longlong *)0x0) &&
               (plVar19 !=
                *(longlong **)(*(longlong *)(lVar31 + 0x1b8) + *(longlong *)(lVar31 + 0x1c0) * 8)))
            {
              plVar27 = plVar19 + 1;
            }
            break;
          }
        }
        if (plVar27 != (longlong *)0x0) {
          FUN_140442740(param_1,lVar31,plVar27,param_2,local_res18,CONCAT44(uVar53,2));
        }
      }
      goto LAB_140445e36;
    }
  }
  if (*(char *)(local_res18 + 0xb3) != '\0') {
    *(int *)((longlong)local_res18 + 0x56c) = iVar32;
    *(int *)(local_res18 + 0xae) = iVar16;
    *(int *)((longlong)local_res18 + 0x574) = iVar41;
  }
LAB_140445e36:
  if ((local_138 != (double *)0x0) && (local_138 != local_118)) {
    free(local_138);
  }
  return;
LAB_14044541e:
  puVar30[2] = dVar50;
  dVar50 = (double)FUN_14038a650(lVar31);
  local_418 = local_418 + dVar50;
LAB_14044543c:
  local_350 = &local_318;
  local_2f8 = local_2f0;
  local_310 = local_2f0;
  local_318 = local_2f0;
  local_308 = &local_2b0;
  FUN_140341810(&local_318,local_480,puStack_478);
  local_2b0 = local_418;
LAB_1404454a4:
  local_418 = local_2b0;
  if ((local_480 != (undefined8 *******)0x0) && (local_480 != local_460)) {
    free(local_480);
  }
LAB_140445103:
  cVar14 = FUN_140445e90(&local_b8,&local_318);
  if (cVar14 == '\0') {
    in_stack_fffffffffffffad0 =
         (char *)CONCAT44((int)((ulonglong)in_stack_fffffffffffffad0 >> 0x20),
                          *(undefined4 *)(local_res18 + 8));
    puVar30 = (undefined8 *)
              FUN_140442db0(param_1,&local_480,local_4d0,local_4e0,local_4e8,
                            in_stack_fffffffffffffad0);
    local_258 = *puVar30;
    lStack_250 = puVar30[1];
    local_248 = puVar30[2];
    uStack_240 = puVar30[3];
    local_238 = puVar30[4];
    uStack_230 = puVar30[5];
    local_228 = *(undefined1 (*) [16])(puVar30 + 6);
    local_218 = *(undefined1 (*) [16])(puVar30 + 8);
  }
  pcVar20 = local_4d8;
  if ((local_318 != (undefined1 *)0x0) && (local_318 != local_2f8)) {
    free(local_318);
    pcVar20 = local_4d8;
  }
LAB_140445797:
  pdVar42 = local_3b8;
  puVar26 = local_4f0;
  iVar16 = (int)local_398;
LAB_1404457a9:
  cVar14 = (char)local_res20;
LAB_1404457b0:
  puVar30 = local_res18;
  if (local_218[8] == '\0') {
LAB_14044574d:
    local_4c8 = local_4c8 + 1;
    puVar30 = local_4c0;
    if ((cVar14 == '\0') && (local_4c0 != (undefined8 *)0x0)) {
      local_480 = *(undefined8 ********)pcVar20;
      local_464 = 8;
      puStack_478 = puVar26;
LAB_140445ad6:
      puVar30 = local_4c0;
      local_468 = (undefined2)local_398;
      local_466 = (undefined2)local_39c;
      local_470 = pdVar42;
      FUN_1403bc2c0(local_4c0,&local_480);
    }
  }
  else {
    lVar31 = (local_208 - *(longlong *)(param_1[3] + 0x20)) * 1000000;
    puVar4 = (undefined8 *)param_2[0x29];
    for (puVar40 = (undefined8 *)param_2[0x28]; puVar40 != puVar4; puVar40 = puVar40 + 10) {
      lVar17 = FUN_14033f6a0(*(longlong *)*local_320 + 0x300,*puVar40);
      if (((lVar17 != 0) && (*(char *)(lVar17 + 0xd8) != '\0')) &&
         (*(longlong *)(lVar17 + 0xe0) == *(longlong *)(lVar17 + 0xe8))) {
        if ((((ulonglong)*(uint *)(puVar40 + 9) <
              (ulonglong)
              ((*(longlong *)(lVar17 + 0x100) - *(longlong *)(lVar17 + 0xf8) >> 3) *
              -0x1111111111111111)) &&
            (pcVar2 = *(code **)((ulonglong)*(uint *)(puVar40 + 9) * 0x78 + 0x50 +
                                *(longlong *)(lVar17 + 0xf8)), pcVar2 != (code *)0x0)) &&
           (in_stack_fffffffffffffad0 = pcVar20,
           iVar32 = (*pcVar2)(*(undefined8 *)(local_338 + 0x1580),puVar40[2],local_320,param_2,
                              puVar30,pcVar20,local_3e0,&local_4b8,(longlong)iVar16,lVar31,
                              &local_258), iVar32 != 0)) {
          puVar30 = local_4c0;
          if (local_4c0 == (undefined8 *)0x0) goto LAB_140445bb8;
          local_480 = *(undefined8 ********)pcVar20;
          puStack_478 = local_4f0;
          local_464 = 9;
          goto LAB_140445ad6;
        }
      }
    }
    uVar53 = (int)local_398;
    iVar16 = FUN_140355610(&local_4b8,local_330,local_398 & 0xffffffff);
    puVar30 = local_4c0;
    if ((local_140 == '\0') || ((longlong)iVar16 + (longlong)local_3d8[0] < local_1f8)) {
      local_2c8 = 0;
      uStack_2c0 = 0;
      local_2b8 = 0;
      local_2b0 = 0.0;
      uStack_2a8 = 0;
      local_2a0 = 0;
      if (local_140 == '\0') {
        local_140 = '\x01';
      }
      local_460 = (undefined8 *******)
                  (*(longlong *)(param_1[3] + 0x2100) + (longlong)(int)local_388 +
                  (longlong)(int)local_4b8);
      pppppppuStack_458 = (undefined8 *******)CONCAT44(pppppppuStack_458._4_4_,uVar53);
      local_1f0 = local_4b8;
      puStack_1e8 = puStack_4b0;
      local_1e0 = auStack_4a8._0_8_;
      uStack_1d8 = auStack_4a8._8_8_;
      pppppppuStack_1c8 = pppppppuStack_458;
      local_480 = *(undefined8 ********)pcVar20;
      puStack_478 = (undefined1 *)*local_3e0;
      local_470 = (double *)CONCAT44(local_470._4_4_,local_39c);
      local_1b0 = local_470;
      fStack_390 = *(float *)(local_4e0 + 200) * fVar9;
      uStack_380 = *(int *)(local_4e0 + 0xe0) + *(int *)(local_4e0 + 0xc0) != 0;
      uStack_37f = 0;
      local_378 = 0;
      local_368 = 0;
      local_1a8 = local_258;
      lStack_1a0 = lStack_250;
      local_198 = local_248;
      uStack_190 = uStack_240;
      local_188 = local_238;
      uStack_180 = uStack_230;
      local_398 = 0;
      local_178 = 0;
      uStack_174 = 0;
      iStack_16c = iStack_38c;
      uStack_160 = CONCAT62(uStack_37e,(ushort)(byte)uStack_380);
      auVar8[0xf] = 0;
      auVar8._0_15_ = auStack_377;
      local_158 = auVar8 << 8;
      local_148 = (ulonglong)uStack_367 << 8;
      local_388 = lVar31;
      local_1f8 = (longlong)iVar16 + (longlong)local_3d8[0];
      local_1d0 = local_460;
      local_1c0 = local_480;
      puStack_1b8 = puStack_478;
      fStack_170 = fStack_390;
      local_168 = lVar31;
    }
  }
LAB_140445bb8:
  uVar46 = (ulonglong)local_4f8;
LAB_140445bbd:
  local_348 = local_348 + 0x10;
  plVar19 = local_490;
  if (local_348 == local_340) goto LAB_140445bf9;
  goto LAB_140444700;
}
