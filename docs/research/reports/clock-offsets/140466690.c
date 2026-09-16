
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140466690(longlong param_1,longlong param_2,longlong param_3,int param_4,int param_5,
                  longlong param_6,longlong param_7,double param_8,undefined8 param_9,
                  undefined4 param_10,longlong *param_11,longlong *param_12,longlong param_13,
                  int *param_14)

{
  char *pcVar1;
  char cVar2;
  uint uVar3;
  longlong *plVar4;
  longlong *plVar5;
  undefined8 *puVar6;
  uint uVar7;
  undefined1 auVar8 [16];
  ushort uVar9;
  ushort uVar10;
  double dVar11;
  double dVar12;
  undefined2 uVar13;
  char cVar14;
  uint uVar15;
  int iVar16;
  double *pdVar17;
  int *piVar18;
  ulonglong uVar19;
  uint *puVar20;
  uint *puVar21;
  longlong lVar22;
  uint *puVar23;
  void *pvVar24;
  uint *puVar25;
  undefined1 uVar26;
  ulonglong *puVar27;
  ulonglong *puVar28;
  longlong lVar29;
  uint *puVar30;
  int iVar31;
  char *pcVar32;
  longlong lVar33;
  longlong lVar34;
  double *pdVar35;
  double *pdVar36;
  undefined8 *puVar37;
  int iVar38;
  size_t sVar39;
  int *piVar40;
  int *piVar41;
  uint *puVar42;
  ulonglong *puVar43;
  byte bVar44;
  byte bVar45;
  uint **ppuVar46;
  ulonglong uVar47;
  ulonglong uVar48;
  int iVar49;
  longlong lVar50;
  uint *puVar51;
  longlong lVar52;
  bool bVar53;
  float fVar54;
  double dVar55;
  double dVar56;
  double dVar57;
  double dVar58;
  double dVar59;
  double dVar60;
  undefined1 auVar61 [16];
  undefined1 auVar62 [16];
  undefined1 auVar63 [16];
  undefined1 auVar64 [16];
  double dVar65;
  double dVar66;
  double dVar67;
  double dVar68;
  undefined1 auVar69 [16];
  char cVar71;
  char cVar72;
  uint uVar70;
  char cVar73;
  char cVar75;
  char cVar76;
  char cVar77;
  char cVar78;
  char cVar79;
  char cVar80;
  char cVar81;
  char cVar82;
  char cVar83;
  char cVar84;
  char cVar85;
  undefined1 auVar74 [16];
  char cVar86;
  double dVar87;
  double dVar88;
  undefined1 auVar89 [16];
  undefined1 auVar90 [16];
  undefined4 uVar91;
  undefined4 uVar92;
  undefined4 uVar93;
  undefined4 uVar94;
  undefined4 uVar95;
  undefined4 uVar96;
  undefined4 uVar97;
  undefined4 uVar98;
  uint local_2e8 [2];
  longlong local_2e0;
  undefined8 local_2d8;
  uint *local_2d0;
  uint *puStack_2c8;
  uint *local_2c0;
  longlong local_2b8;
  ulonglong local_2b0;
  uint *local_2a8;
  uint *local_2a0;
  longlong local_298;
  uint *local_290;
  uint *local_288;
  uint *puStack_280;
  uint *local_278;
  uint *local_270;
  uint *puStack_268;
  uint *local_260;
  ulonglong local_258;
  uint *puStack_250;
  double *local_248;
  double *pdStack_240;
  double *local_238;
  int *local_230;
  int *piStack_228;
  int *local_220;
  double *local_218;
  double *pdStack_210;
  double *local_208;
  int *local_200;
  int *piStack_1f8;
  int *local_1f0;
  ulonglong local_1e8;
  uint *puStack_1e0;
  int local_1d8;
  int iStack_1d4;
  uint local_1d0;
  int local_1c8;
  int iStack_1c4;
  undefined8 local_1b8;
  longlong local_1b0;
  longlong local_1a8;
  double local_198;
  double local_190;
  double local_188;
  double local_180;
  double local_178;
  double local_170;
  double local_168;
  double local_160;
  longlong local_158 [2];
  undefined1 local_148 [24];
  double *local_130;
  longlong local_128;
  undefined1 local_118 [24];
  double *local_100;
  longlong local_f8;

  lVar52 = (longlong)param_4 * 0x90;
  lVar50 = *(longlong *)(param_1 + 0xd8);
  uVar95 = SUB84(DAT_140aabc48,0);
  uVar96 = (undefined4)((ulonglong)DAT_140aabc48 >> 0x20);
  uVar97 = SUB84(DAT_140aabf88,0);
  uVar98 = (undefined4)((ulonglong)DAT_140aabf88 >> 0x20);
  local_2d8 = (uint *)CONCAT44(local_2d8._4_4_,
                               (int)((*(double *)(lVar52 + 0x18 + lVar50) * DAT_140aabc48 +
                                     *(double *)(lVar52 + 8 + lVar50)) * DAT_140aabf88));
  local_2e0 = lVar50;
  local_298 = lVar52;
  dVar55 = exp((*(double *)(lVar52 + 0x20 + lVar50) * DAT_140aabc48 +
               *(double *)(lVar52 + 0x10 + lVar50)) * DAT_140aab8f0);
  dVar55 = atan(dVar55);
  local_2d8 = (uint *)CONCAT44((int)(dVar55 * DAT_140aac2c0 - DAT_140aac298),(undefined4)local_2d8);
  FUN_140488630(param_3,local_2d8);
  dVar55 = 0.0;
  lVar29 = *(longlong *)(lVar52 + 0x68 + lVar50);
  for (lVar34 = *(longlong *)(lVar52 + 0x60 + lVar50); lVar34 != lVar29; lVar34 = lVar34 + 0x38) {
    FUN_1403bc470(param_1,&local_258,lVar34);
    fVar54 = (float)FUN_1404664b0(local_258 + 0x68);
    dVar55 = dVar55 + (double)fVar54 * (double)*(uint *)(lVar34 + 0x10);
  }
  dVar87 = 0.0;
  puVar28 = *(ulonglong **)(lVar52 + 0x80 + lVar50);
  puVar43 = *(ulonglong **)(lVar52 + 0x78 + lVar50);
  if (puVar43 != puVar28) {
    lVar29 = *(longlong *)(param_1 + 8);
    uVar3 = *(uint *)(param_1 + 0x10);
    do {
      for (puVar27 = *(ulonglong **)(lVar29 + (*puVar43 % (ulonglong)uVar3) * 8);
          puVar27 != (ulonglong *)0x0; puVar27 = (ulonglong *)puVar27[0xb8]) {
        if (*puVar43 == *puVar27) {
          if (puVar27 != (ulonglong *)0x0) goto LAB_140466876;
          break;
        }
      }
      puVar27 = *(ulonglong **)(lVar29 + *(longlong *)(param_1 + 0x10) * 8);
LAB_140466876:
      fVar54 = (float)FUN_1404664b0(puVar27 + 0x61);
      dVar87 = dVar87 + (double)fVar54 * (double)puVar43[2];
      puVar43 = puVar43 + 6;
    } while (puVar43 != puVar28);
  }
  dVar56 = log(dVar87);
  dVar87 = DAT_140aabd08;
  uVar93 = SUB84(DAT_140aabd08,0);
  uVar94 = (undefined4)((ulonglong)DAT_140aabd08 >> 0x20);
  dVar58 = dVar56 * _DAT_140aabb88;
  if (dVar56 * _DAT_140aabb88 <= DAT_140aabd08) {
    dVar58 = DAT_140aabd08;
  }
  param_8 = param_8 + dVar58 * dVar55 * _DAT_140aab870 *
                      (double)CONCAT62(param_9._2_6_,CONCAT11(param_9._1_1_,(byte)param_9)) *
                      (double)*(int *)(param_3 + 0x44);
  if (DAT_140aabd08 <= param_8) {
    local_248 = (double *)0x0;
    pdStack_240 = (double *)0x0;
    local_238 = (double *)0x0;
    local_200 = (int *)0x0;
    piStack_1f8 = (int *)0x0;
    local_1f0 = (int *)0x0;
    iVar49 = 0;
    lVar29 = *(longlong *)(lVar52 + 0x60 + lVar50);
    if (0 < (int)(*(longlong *)(lVar52 + 0x68 + lVar50) - lVar29 >> 3) * -0x49249249) {
      lVar50 = 0;
      pdVar36 = (double *)0x0;
      piVar41 = (int *)0x0;
      do {
        for (puVar28 = *(ulonglong **)
                        (*(longlong *)(param_1 + 8) +
                        (*(ulonglong *)(lVar29 + lVar50) % (ulonglong)*(uint *)(param_1 + 0x10)) * 8
                        ); puVar28 != (ulonglong *)0x0; puVar28 = (ulonglong *)puVar28[0xb8]) {
          if (*(ulonglong *)(lVar29 + lVar50) == *puVar28) {
            if (puVar28 != (ulonglong *)0x0) goto LAB_140466990;
            break;
          }
        }
        puVar28 = *(ulonglong **)(*(longlong *)(param_1 + 8) + *(longlong *)(param_1 + 0x10) * 8);
LAB_140466990:
        pdVar17 = local_238;
        fVar54 = (float)FUN_1404664b0(puVar28 + 0xd);
        dVar55 = (double)fVar54 * (double)*(uint *)(lVar29 + 0x10 + lVar50);
        if (0.0 < dVar55) {
          if (pdVar36 < pdVar17) {
            *pdVar36 = dVar55;
            pdVar35 = pdVar36;
          }
          else {
            sVar39 = (longlong)pdVar36 - (longlong)local_248;
            lVar29 = (longlong)sVar39 >> 3;
            if (lVar29 == 0) {
              lVar29 = 1;
LAB_1404669f8:
              pdVar17 = (double *)thunk_FUN_140983da8(lVar29 * 8);
            }
            else {
              lVar29 = lVar29 * 2;
              if (lVar29 != 0) goto LAB_1404669f8;
              pdVar17 = (double *)0x0;
            }
            pdVar35 = pdVar17;
            if (local_248 != pdVar36) {
              pvVar24 = memmove(pdVar17,local_248,sVar39);
              pdVar35 = (double *)
                        ((longlong)pvVar24 + ((longlong)pdVar36 - (longlong)local_248 >> 3) * 8);
            }
            *pdVar35 = dVar55;
            if (local_248 != (double *)0x0) {
              free(local_248);
            }
            local_238 = pdVar17 + lVar29;
            lVar52 = local_298;
            local_248 = pdVar17;
          }
          pdVar36 = pdVar35 + 1;
          pdStack_240 = pdVar36;
          if (piVar41 < local_1f0) {
            *piVar41 = iVar49;
            piVar40 = piVar41;
          }
          else {
            sVar39 = (longlong)piVar41 - (longlong)local_200;
            lVar29 = (longlong)sVar39 >> 2;
            if (lVar29 == 0) {
              lVar29 = 1;
LAB_140466a97:
              piVar18 = (int *)thunk_FUN_140983da8(lVar29 * 4);
            }
            else {
              lVar29 = lVar29 * 2;
              if (lVar29 != 0) goto LAB_140466a97;
              piVar18 = (int *)0x0;
            }
            piVar40 = piVar18;
            if (local_200 != piVar41) {
              pvVar24 = memmove(piVar18,local_200,sVar39);
              piVar40 = (int *)((longlong)pvVar24 +
                               ((longlong)piVar41 - (longlong)local_200 >> 2) * 4);
            }
            *piVar40 = iVar49;
            if (local_200 != (int *)0x0) {
              free(local_200);
            }
            local_1f0 = piVar18 + lVar29;
            lVar52 = local_298;
            local_200 = piVar18;
          }
          piVar41 = piVar40 + 1;
          piStack_1f8 = piVar41;
        }
        iVar49 = iVar49 + 1;
        lVar50 = lVar50 + 0x38;
        lVar29 = *(longlong *)(lVar52 + 0x60 + local_2e0);
      } while (iVar49 < (int)(*(longlong *)(lVar52 + 0x68 + local_2e0) - lVar29 >> 3) * -0x49249249)
      ;
      if (local_248 != pdVar36) {
        FUN_14046f4f0(local_148,local_248,pdVar36);
        local_218 = (double *)0x0;
        pdStack_210 = (double *)0x0;
        local_208 = (double *)0x0;
        local_230 = (int *)0x0;
        piStack_228 = (int *)0x0;
        local_220 = (int *)0x0;
        iVar49 = 0;
        lVar50 = *(longlong *)(lVar52 + 0x78 + local_2e0);
        if (0 < (int)(*(longlong *)(lVar52 + 0x80 + local_2e0) - lVar50 >> 4) * -0x55555555) {
          lVar34 = 0;
          pdVar36 = (double *)0x0;
          piVar41 = (int *)0x0;
          lVar29 = local_2e0;
          do {
            for (puVar28 = *(ulonglong **)
                            (*(longlong *)(param_1 + 8) +
                            (*(ulonglong *)(lVar50 + lVar34) % (ulonglong)*(uint *)(param_1 + 0x10))
                            * 8); puVar28 != (ulonglong *)0x0; puVar28 = (ulonglong *)puVar28[0xb8])
            {
              if (*(ulonglong *)(lVar50 + lVar34) == *puVar28) {
                if (puVar28 != (ulonglong *)0x0) goto LAB_140466c06;
                break;
              }
            }
            puVar28 = *(ulonglong **)
                       (*(longlong *)(param_1 + 8) + *(longlong *)(param_1 + 0x10) * 8);
LAB_140466c06:
            pdVar17 = local_208;
            fVar54 = (float)FUN_1404664b0(puVar28 + 0x61);
            dVar55 = (double)fVar54 * *(double *)(lVar50 + 0x10 + lVar34);
            if (0.0 < dVar55) {
              if (pdVar36 < pdVar17) {
                *pdVar36 = dVar55;
                pdVar35 = pdVar36;
              }
              else {
                sVar39 = (longlong)pdVar36 - (longlong)local_218;
                lVar50 = (longlong)sVar39 >> 3;
                if (lVar50 == 0) {
                  lVar50 = 1;
LAB_140466c6b:
                  pdVar17 = (double *)thunk_FUN_140983da8(lVar50 * 8);
                }
                else {
                  lVar50 = lVar50 * 2;
                  if (lVar50 != 0) goto LAB_140466c6b;
                  pdVar17 = (double *)0x0;
                }
                pdVar35 = pdVar17;
                if (local_218 != pdVar36) {
                  pvVar24 = memmove(pdVar17,local_218,sVar39);
                  pdVar35 = (double *)
                            ((longlong)pvVar24 + ((longlong)pdVar36 - (longlong)local_218 >> 3) * 8)
                  ;
                }
                *pdVar35 = dVar55;
                if (local_218 != (double *)0x0) {
                  free(local_218);
                }
                local_208 = pdVar17 + lVar50;
                lVar29 = local_2e0;
                lVar52 = local_298;
                local_218 = pdVar17;
              }
              pdVar36 = pdVar35 + 1;
              pdStack_210 = pdVar36;
              if (piVar41 < local_220) {
                *piVar41 = iVar49;
                piVar40 = piVar41;
              }
              else {
                sVar39 = (longlong)piVar41 - (longlong)local_230;
                lVar50 = (longlong)sVar39 >> 2;
                if (lVar50 == 0) {
                  lVar50 = 1;
LAB_140466d0f:
                  piVar18 = (int *)thunk_FUN_140983da8(lVar50 * 4);
                }
                else {
                  lVar50 = lVar50 * 2;
                  if (lVar50 != 0) goto LAB_140466d0f;
                  piVar18 = (int *)0x0;
                }
                piVar40 = piVar18;
                if (local_230 != piVar41) {
                  pvVar24 = memmove(piVar18,local_230,sVar39);
                  piVar40 = (int *)((longlong)pvVar24 +
                                   ((longlong)piVar41 - (longlong)local_230 >> 2) * 4);
                }
                *piVar40 = iVar49;
                if (local_230 != (int *)0x0) {
                  free(local_230);
                }
                local_220 = piVar18 + lVar50;
                lVar29 = local_2e0;
                lVar52 = local_298;
                local_230 = piVar18;
              }
              piVar41 = piVar40 + 1;
              piStack_228 = piVar41;
            }
            iVar49 = iVar49 + 1;
            lVar34 = lVar34 + 0x30;
            lVar50 = *(longlong *)(lVar52 + 0x78 + lVar29);
          } while (iVar49 < (int)(*(longlong *)(lVar52 + 0x80 + lVar29) - lVar50 >> 4) * -0x55555555
                  );
          if (local_218 != pdVar36) {
            FUN_14046f4f0(local_118,local_218,pdVar36);
            dVar55 = DAT_140aab8d8;
            uVar3 = *(uint *)(lVar52 + 0x38 + lVar29);
            uVar19 = ((ulonglong)(uVar3 >> 1) ^ CONCAT44(uVar3,param_10)) * -0xae502812aa7333;
            lVar50 = (uVar19 >> 0x21 ^ uVar19) * -0x3b314601e57a13ad;
            local_2e8[0] = ((uint)((ulonglong)lVar50 >> 0x21) ^ (uint)lVar50) % 0x7fffffff;
            if (local_2e8[0] == 0) {
              local_2e8[0] = 1;
            }
            local_270 = (uint *)0x0;
            puStack_268 = (uint *)0x0;
            local_260 = (uint *)0x0;
            local_2d0 = (uint *)0x0;
            puStack_2c8 = (uint *)0x0;
            local_2c0 = (uint *)0x0;
            local_288 = (uint *)0x0;
            puStack_280 = (uint *)0x0;
            local_278 = (uint *)0x0;
            if (dVar87 < param_8) {
              uVar91 = (undefined4)DAT_140aabb18;
              uVar92 = (undefined4)((ulonglong)DAT_140aabb18 >> 0x20);
LAB_140466e90:
              puVar51 = local_2c0;
              local_2d8 = local_270;
              puStack_268 = local_270;
              puStack_2c8 = local_2d0;
              local_2a8 = local_2d0;
              dVar87 = (double)FUN_14037e380(local_2e8);
              uVar19 = (local_128 - (longlong)local_130) + -8 >> 3;
              pdVar36 = local_130;
              while (uVar47 = uVar19, 0 < (longlong)uVar47) {
                uVar19 = uVar47 >> 1;
                pdVar17 = pdVar36 + uVar19;
                if (*pdVar17 <= dVar87 && dVar87 != *pdVar17) {
                  pdVar36 = pdVar17 + 1;
                  uVar19 = uVar47 + (-1 - uVar19);
                }
              }
              lVar50 = *(longlong *)(lVar52 + 0x60 + lVar29);
              plVar4 = *(longlong **)
                        ((longlong)local_200[(int)((longlong)pdVar36 - (longlong)local_130 >> 3)] *
                         0x38 + 0x18 + lVar50);
              if ((plVar4 == (longlong *)0x0) ||
                 (plVar5 = *(longlong **)
                            ((longlong)
                             local_200[(int)((longlong)pdVar36 - (longlong)local_130 >> 3)] * 0x38 +
                             0x28 + lVar50), plVar5 == (longlong *)0x0)) {
                bVar45 = 0;
                bVar44 = 0;
              }
              else {
                uVar19 = ((ulonglong)local_2e8[0] * 0xbc8f >> 0x1f) +
                         (ulonglong)((uint)((ulonglong)local_2e8[0] * 0xbc8f) & 0x7fffffff);
                uVar3 = (uint)uVar19;
                local_2e8[0] = uVar3 + 0x80000001;
                if (uVar19 < 0x7fffffff) {
                  local_2e8[0] = uVar3;
                }
                puVar30 = (uint *)*plVar4;
                uVar19 = plVar4[1] - (longlong)puVar30 >> 2;
                while (uVar47 = uVar19, 0 < (longlong)uVar47) {
                  uVar19 = uVar47 >> 1;
                  if (puVar30[uVar19] < (local_2e8[0] & 0x7fffffff)) {
                    puVar30 = puVar30 + uVar19 + 1;
                    uVar19 = uVar47 + (-1 - uVar19);
                  }
                }
                iVar49 = (int)((longlong)puVar30 - *plVar4 >> 2) * 2;
                lVar50 = *plVar5;
                bVar45 = *(byte *)(iVar49 + lVar50);
                bVar44 = *(byte *)((longlong)iVar49 + 1 + lVar50);
              }
              dVar87 = (((double)bVar45 + (double)CONCAT44(uVar96,uVar95)) /
                       (double)*(ushort *)(lVar52 + 0x58 + lVar29)) *
                       *(double *)(lVar52 + 0x18 + lVar29) + *(double *)(lVar52 + 8 + lVar29);
              dVar58 = (((double)bVar44 + (double)CONCAT44(uVar96,uVar95)) /
                       (double)*(ushort *)(lVar52 + 0x5a + lVar29)) *
                       *(double *)(lVar52 + 0x20 + lVar29) + *(double *)(lVar52 + 0x10 + lVar29);
              dVar56 = 0.0;
              auVar90._0_12_ = ZEXT812(0);
              auVar90._12_4_ = 0;
              puVar30 = *(uint **)(lVar52 + 0x40 + lVar29);
              puVar23 = *(uint **)(local_298 + 0x48 + lVar29);
              puVar25 = local_2a8;
              if (puVar30 != puVar23) {
                do {
                  uVar3 = *puVar30;
                  lVar29 = (longlong)(int)uVar3 * 1000;
                  lVar50 = *(longlong *)(param_1 + 0x90);
                  dVar65 = dVar87 - *(double *)(lVar29 + 8 + lVar50);
                  dVar57 = dVar58 - *(double *)(lVar29 + 0x10 + lVar50);
                  dVar57 = dVar57 * dVar57 + dVar65 * dVar65;
                  pdVar36 = (double *)(lVar29 + 0x28 + lVar50);
                  puVar42 = puVar25;
                  if (dVar57 < *pdVar36 || dVar57 == *pdVar36) {
                    if (puVar25 < puVar51) {
                      puVar42 = puVar25 + 1;
                      *puVar25 = uVar3;
                      puStack_2c8 = puVar42;
                    }
                    else {
                      sVar39 = (longlong)puVar25 - (longlong)local_2d0;
                      lVar50 = (longlong)sVar39 >> 2;
                      if (lVar50 == 0) {
                        lVar50 = 1;
LAB_14046714f:
                        puVar20 = (uint *)thunk_FUN_140983da8(lVar50 * 4);
                      }
                      else {
                        lVar50 = lVar50 * 2;
                        if (lVar50 != 0) goto LAB_14046714f;
                        puVar20 = (uint *)0x0;
                      }
                      puVar21 = puVar20;
                      if (local_2d0 != puVar25) {
                        pvVar24 = memmove(puVar20,local_2d0,sVar39);
                        puVar21 = (uint *)((longlong)pvVar24 +
                                          ((longlong)puVar25 - (longlong)local_2d0 >> 2) * 4);
                      }
                      *puVar21 = uVar3;
                      if (local_2d0 != (uint *)0x0) {
                        free(local_2d0);
                      }
                      puVar51 = puVar20 + lVar50;
                      puVar42 = puVar21 + 1;
                      local_2d0 = puVar20;
                      puStack_2c8 = puVar21 + 1;
                      local_2c0 = puVar51;
                    }
                  }
                  puVar20 = local_2d8;
                  puVar30 = puVar30 + 1;
                  puVar25 = puVar42;
                } while (puVar30 != puVar23);
                lVar29 = local_2e0;
                local_2a8 = puVar42;
                if (local_2d0 == puVar42) goto LAB_1404680fb;
                dVar57 = (double)FUN_14037e380(local_2e8);
                uVar19 = (local_f8 - (longlong)local_100) + -8 >> 3;
                pdVar36 = local_100;
                while (uVar47 = uVar19, 0 < (longlong)uVar47) {
                  uVar19 = uVar47 >> 1;
                  pdVar17 = pdVar36 + uVar19;
                  if (*pdVar17 <= dVar57 && dVar57 != *pdVar17) {
                    pdVar36 = pdVar17 + 1;
                    uVar19 = uVar47 + (-1 - uVar19);
                  }
                }
                lVar34 = (longlong)local_230[(int)((longlong)pdVar36 - (longlong)local_100 >> 3)];
                lVar50 = *(longlong *)(local_298 + 0x78 + local_2e0);
                dVar57 = (double)FUN_14037e380(local_2e8);
                puVar28 = *(ulonglong **)(lVar50 + 0x18 + lVar34 * 0x30);
                uVar19 = *(longlong *)(lVar50 + 0x20 + lVar34 * 0x30) - (longlong)puVar28 >> 4;
                while (uVar47 = uVar19, 0 < (longlong)uVar47) {
                  uVar19 = uVar47 >> 1;
                  if ((double)puVar28[uVar19 * 2 + 1] <= dVar57 &&
                      dVar57 != (double)puVar28[uVar19 * 2 + 1]) {
                    puVar28 = puVar28 + uVar19 * 2 + 2;
                    uVar19 = uVar47 + (-1 - uVar19);
                  }
                }
                lVar29 = local_2e0;
                if (puVar28 == *(ulonglong **)(lVar50 + 0x20 + lVar34 * 0x30)) goto LAB_1404680fb;
                uVar19 = *(ulonglong *)(lVar50 + 8 + lVar34 * 0x30);
                for (puVar43 = *(ulonglong **)
                                (*(longlong *)(param_1 + 0xf8) +
                                (uVar19 % (ulonglong)*(uint *)(param_1 + 0x100)) * 8);
                    puVar43 != (ulonglong *)0x0; puVar43 = (ulonglong *)puVar43[7]) {
                  if (uVar19 == *puVar43) {
                    if (puVar43 != (ulonglong *)0x0) goto LAB_140467296;
                    break;
                  }
                }
                puVar43 = *(ulonglong **)
                           (*(longlong *)(param_1 + 0xf8) + *(longlong *)(param_1 + 0x100) * 8);
LAB_140467296:
                for (puVar27 = *(ulonglong **)
                                (puVar43[2] + (*puVar28 % (ulonglong)(uint)puVar43[3]) * 8);
                    puVar27 != (ulonglong *)0x0; puVar27 = (ulonglong *)puVar27[7]) {
                  if (*puVar28 == *puVar27) {
                    if (puVar27 != (ulonglong *)0x0) goto LAB_1404672d1;
                    break;
                  }
                }
                puVar27 = *(ulonglong **)(puVar43[2] + puVar43[3] * 8);
LAB_1404672d1:
                dVar57 = (double)FUN_14037e380(local_2e8);
                piVar41 = (int *)puVar27[4];
                uVar19 = (longlong)(puVar27[5] - (longlong)piVar41) >> 4;
                while (uVar47 = uVar19, 0 < (longlong)uVar47) {
                  uVar19 = uVar47 >> 1;
                  if (*(double *)(piVar41 + uVar19 * 4 + 2) <= dVar57 &&
                      dVar57 != *(double *)(piVar41 + uVar19 * 4 + 2)) {
                    piVar41 = piVar41 + uVar19 * 4 + 4;
                    uVar19 = uVar47 + (-1 - uVar19);
                  }
                }
                lVar29 = local_2e0;
                if (piVar41 == (int *)puVar27[5]) goto LAB_1404680fb;
                lVar22 = (longlong)*piVar41;
                lVar29 = *(longlong *)(param_1 + 0xd8);
                lVar52 = *(longlong *)(lVar29 + 0x60 + lVar22 * 0x90);
                lVar33 = *(longlong *)(lVar29 + 0x68 + lVar22 * 0x90);
                puVar51 = local_2d8;
                if (lVar52 != lVar33) {
                  do {
                    if (*(longlong *)(lVar50 + 8 + lVar34 * 0x30) == *(longlong *)(lVar52 + 8)) {
                      FUN_1404665b0(lVar52,&param_9,local_2e8);
                      dVar56 = (((double)(byte)param_9 + (double)CONCAT44(uVar96,uVar95)) /
                               (double)*(ushort *)(lVar29 + 0x58 + lVar22 * 0x90)) *
                               *(double *)(lVar29 + 0x18 + lVar22 * 0x90) +
                               *(double *)(lVar29 + 8 + lVar22 * 0x90);
                      auVar90._0_8_ =
                           (((double)param_9._1_1_ + (double)CONCAT44(uVar96,uVar95)) /
                           (double)*(ushort *)(lVar29 + 0x5a + lVar22 * 0x90)) *
                           *(double *)(lVar29 + 0x20 + lVar22 * 0x90) +
                           *(double *)(lVar29 + 0x10 + lVar22 * 0x90);
                      auVar90._8_8_ = 0;
                      puVar30 = *(uint **)(lVar29 + 0x40 + lVar22 * 0x90);
                      puVar23 = *(uint **)(lVar29 + 0x48 + lVar22 * 0x90);
                      puVar51 = local_2d8;
                      goto joined_r0x0001404673e7;
                    }
                    lVar52 = lVar52 + 0x38;
                  } while (lVar52 != lVar33);
                }
LAB_1404674d6:
                local_2d8 = puVar51;
                puVar51 = local_270;
                lVar29 = local_2e0;
                if (local_270 == puVar20) goto LAB_1404680fb;
                dVar58 = exp(dVar58 * DAT_140aab8f0);
                dVar58 = atan(dVar58);
                dVar11 = (double)(int)(dVar58 * DAT_140aac2c0 - DAT_140aac298) * dVar55;
                dVar12 = (double)(int)(dVar87 * (double)CONCAT44(uVar98,uVar97)) * dVar55;
                local_188 = dVar12;
                local_180 = dVar11;
                dVar58 = cos(dVar11 * (double)CONCAT44(uVar92,uVar91));
                dVar57 = (double)CONCAT44(uVar94,uVar93) /
                         ((double)CONCAT44(uVar94,uVar93) -
                         ((double)CONCAT44(uVar94,uVar93) - dVar58 * dVar58) * DAT_140aabac0);
                dVar87 = SQRT(dVar57) * DAT_140aac160;
                dVar58 = dVar87 * dVar58;
                dVar87 = dVar87 * dVar57;
                local_190 = dVar87 * DAT_140aabcd8;
                auVar89._8_8_ = auVar90._8_8_;
                auVar89._0_8_ = auVar90._0_8_ * DAT_140aab8f0;
                local_198 = dVar58;
                dVar57 = exp(auVar89._0_8_);
                dVar57 = atan(dVar57);
                dVar57 = (double)(int)(dVar57 * DAT_140aac2c0 - DAT_140aac298) * dVar55;
                dVar65 = (double)(int)(dVar56 * DAT_140aabf88) * dVar55;
                local_168 = dVar65;
                local_160 = dVar57;
                dVar56 = cos(dVar57 * (double)CONCAT44(uVar92,uVar91));
                auVar74._0_8_ =
                     DAT_140aabd08 /
                     (DAT_140aabd08 - (DAT_140aabd08 - dVar56 * dVar56) * DAT_140aabac0);
                auVar74._8_8_ = 0;
                auVar90 = sqrtpd(auVar89,auVar74);
                dVar88 = auVar90._0_8_ * DAT_140aac160;
                dVar56 = dVar88 * dVar56;
                dVar88 = dVar88 * auVar74._0_8_;
                local_170 = dVar88 * DAT_140aabcd8;
                for (dVar66 = dVar12 - dVar65; dVar66 < DAT_140aac6c8;
                    dVar66 = dVar66 + DAT_140aac020) {
                }
                for (; DAT_140aabfe8 < dVar66; dVar66 = dVar66 + DAT_140aac6d0) {
                }
                dVar66 = SQRT(dVar87 * DAT_140aabcc8 * dVar87 *
                              (dVar11 - dVar57) * (dVar11 - dVar57) +
                              dVar58 * dVar66 * dVar58 * dVar66);
                iVar49 = 2000000000;
                if (dVar66 <= _DAT_140aac2d0) {
                  iVar49 = (int)dVar66;
                }
                lVar50 = (longlong)iVar49;
                iVar49 = *(int *)(param_3 + 0xd8);
                puVar30 = local_2d0;
                local_2b8 = lVar50;
                local_178 = dVar56;
                if (iVar49 == 0) {
                  FUN_14046f5c0(local_2d0,local_2a8,local_2e8);
                  FUN_14046f5c0(puVar51,puVar20,local_2e8);
LAB_14046829b:
                  iVar49 = 999999999;
                  local_2b0 = 0xffffffffffffffff;
                  puVar51 = (uint *)0xffffffffffffffff;
                  local_290 = (uint *)0xffffffffffffffff;
                  bVar53 = false;
LAB_1404682c4:
                  uVar3 = *puVar30;
                  local_2a0 = puVar30;
                  if (!bVar53) {
                    uVar19 = *(ulonglong *)
                              ((longlong)(int)uVar3 * 1000 + *(longlong *)(param_1 + 0x90));
                    for (puVar28 = *(ulonglong **)
                                    (*(longlong *)(param_2 + 0x138) +
                                    (uVar19 % (ulonglong)*(uint *)(param_2 + 0x140)) * 8);
                        puVar28 != (ulonglong *)0x0; puVar28 = (ulonglong *)puVar28[2]) {
                      if (uVar19 == *puVar28) {
                        if ((((puVar28 != (ulonglong *)0x0) &&
                             (puVar28 !=
                              *(ulonglong **)
                               (*(longlong *)(param_2 + 0x138) + *(longlong *)(param_2 + 0x140) * 8)
                             )) && (lVar50 = (longlong)(int)puVar28[1], (int)puVar28[1] != -1)) &&
                           ((puVar43 = (ulonglong *)(lVar50 * 1000 + *(longlong *)(param_2 + 0x160))
                            , *(int *)((longlong)puVar43 + 0x3dc) != 0 &&
                            (puVar43[0x6f] != puVar43[0x70])))) {
                          lVar29 = param_2;
                          lVar34 = local_2b8;
                          lVar52 = lVar50;
                          dVar87 = (double)FUN_140355270(&local_198,puVar28,uVar19,
                                                         (double)(int)puVar43[3] * dVar55,
                                                         (double)(int)(puVar43[3] >> 0x20) * dVar55)
                          ;
                          puVar30 = local_270;
                          if ((int)dVar87 <= lVar34) {
                            do {
                              if (uVar3 != *puVar30) {
                                uVar19 = *(ulonglong *)
                                          ((longlong)(int)*puVar30 * 1000 +
                                          *(longlong *)(param_1 + 0x90));
                                for (puVar28 = *(ulonglong **)
                                                (*(longlong *)(lVar29 + 0x138) +
                                                (uVar19 % (ulonglong)*(uint *)(lVar29 + 0x140)) * 8)
                                    ; puVar28 != (ulonglong *)0x0; puVar28 = (ulonglong *)puVar28[2]
                                    ) {
                                  if (uVar19 == *puVar28) {
                                    if (((puVar28 != (ulonglong *)0x0) &&
                                        (puVar28 !=
                                         *(ulonglong **)
                                          (*(longlong *)(lVar29 + 0x138) +
                                          *(longlong *)(lVar29 + 0x140) * 8))) &&
                                       (lVar34 = (longlong)(int)puVar28[1], (int)puVar28[1] != -1))
                                    {
                                      lVar33 = *(longlong *)(lVar29 + 0x160);
                                      puVar37 = (undefined8 *)(lVar34 * 1000 + lVar33);
                                      if (*(int *)(puVar37 + 0x7c) != 0) {
                                        dVar58 = (double)FUN_140355270(&local_178,lVar33,lVar34,
                                                                       (double)(int)puVar37[3] *
                                                                       dVar55,(double)(int)((
                                                  ulonglong)puVar37[3] >> 0x20) * dVar55);
                                        plVar4 = param_12;
                                        iVar38 = (int)dVar58 + (int)dVar87;
                                        lVar29 = param_2;
                                        if ((((iVar38 <= local_2b8) &&
                                             (iVar16 = (int)(*(longlong *)(param_2 + 0x168) - lVar33
                                                            >> 3) * 0x26e978d5, (int)lVar52 < iVar16
                                             )) && ((int)lVar34 < iVar16)) &&
                                           (*(int *)(lVar50 * 1000 + 0x3d8 + lVar33) ==
                                            *(int *)(puVar37 + 0x7b))) {
                                          if (*(int *)(param_3 + 0xd8) == 0) {
                                            local_1e8 = *puVar43;
                                            LOCK();
                                            puVar28 = (ulonglong *)(param_12 + 0xc);
                                            uVar19 = *puVar28;
                                            *puVar28 = *puVar28 + 1;
                                            UNLOCK();
                                            if (uVar19 < (ulonglong)param_12[0xd]) {
                                              uVar19 = (local_1e8 >> 0x21 ^ local_1e8) *
                                                       -0xae502812aa7333;
                                              uVar19 = (uVar19 >> 0x21 ^ uVar19) *
                                                       -0x3b314601e57a13ad;
                                              uVar19 = uVar19 >> 0x21 ^ uVar19;
                                              lVar50 = param_12[1];
                                              lVar29 = *param_12;
                                              if (lVar50 - lVar29 >> 3 == 0) goto LAB_1404686b8;
                                              goto LAB_140468670;
                                            }
                                            FUN_140002a90(&local_1b8,param_12 + 0xe);
                                            FUN_140364d10(plVar4 + 6,&local_1b0);
                                            puVar6 = *(undefined8 **)(local_1b0 + 0x10);
                                            if (puVar6 < *(undefined8 **)(local_1b0 + 0x18)) {
                                              *(undefined8 **)(local_1b0 + 0x10) = puVar6 + 1;
                                              *puVar6 = *puVar37;
                                            }
                                            else {
                                              FUN_140253b10();
                                            }
                                            _Mtx_unlock(local_1b8);
                                            bVar53 = true;
                                            goto LAB_140468325;
                                          }
                                          if (iVar38 < iVar49) {
                                            local_2b0 = *puVar43;
                                            puVar51 = (uint *)*puVar37;
                                            iVar49 = iVar38;
                                            local_290 = puVar51;
                                          }
                                        }
                                      }
                                    }
                                    break;
                                  }
                                }
                              }
                              puVar30 = puVar30 + 1;
                            } while (puVar30 != local_2d8);
                          }
                        }
                        break;
                      }
                    }
                    goto LAB_140468325;
                  }
                  goto LAB_14046833c;
                }
                if (iVar49 == 1) goto LAB_14046829b;
                if (iVar49 != 2) {
                  uVar91 = (undefined4)DAT_140aabb18;
                  uVar92 = (undefined4)((ulonglong)DAT_140aabb18 >> 0x20);
                  uVar93 = SUB84(DAT_140aabd08,0);
                  uVar94 = (undefined4)((ulonglong)DAT_140aabd08 >> 0x20);
                  uVar95 = SUB84(DAT_140aabc48,0);
                  uVar96 = (undefined4)((ulonglong)DAT_140aabc48 >> 0x20);
                  uVar97 = SUB84(DAT_140aabf88,0);
                  uVar98 = (undefined4)((ulonglong)DAT_140aabf88 >> 0x20);
                  lVar29 = local_2e0;
                  goto LAB_1404680fb;
                }
                puStack_280 = local_288;
                local_2a0 = local_2d0;
                lVar29 = param_2;
                puVar23 = local_288;
                puVar30 = local_288;
                do {
                  uVar3 = *local_2a0;
                  uVar47 = (ulonglong)(int)uVar3;
                  uVar19 = *(ulonglong *)(uVar47 * 1000 + *(longlong *)(param_1 + 0x90));
                  for (puVar28 = *(ulonglong **)
                                  (*(longlong *)(lVar29 + 0x138) +
                                  (uVar19 % (ulonglong)*(uint *)(lVar29 + 0x140)) * 8);
                      puVar25 = puVar51, puVar28 != (ulonglong *)0x0;
                      puVar28 = (ulonglong *)puVar28[2]) {
                    if (uVar19 == *puVar28) {
                      if ((puVar28 != (ulonglong *)0x0) &&
                         (puVar28 !=
                          *(ulonglong **)
                           (*(longlong *)(lVar29 + 0x138) + *(longlong *)(lVar29 + 0x140) * 8))) {
                        iVar49 = (int)puVar28[1];
                        if ((iVar49 != -1) &&
                           ((puVar42 = (uint *)((longlong)iVar49 * 1000 +
                                               *(longlong *)(lVar29 + 0x160)), local_290 = puVar42,
                            puVar42[0xf7] != 0 &&
                            (*(longlong *)(puVar42 + 0xde) != *(longlong *)(puVar42 + 0xe0))))) {
                          dVar66 = (double)FUN_140355270(&local_198,puVar28,uVar19,
                                                         (double)(int)*(undefined8 *)(puVar42 + 6) *
                                                         dVar55,(double)(int)((ulonglong)
                                                                              *(undefined8 *)
                                                                               (puVar42 + 6) >> 0x20
                                                                             ) * dVar55);
                          iVar38 = (int)dVar66;
                          local_2b0 = CONCAT44(local_2b0._4_4_,iVar38);
                          if (iVar38 <= lVar50) {
                            do {
                              if ((uint)uVar47 != *puVar51) {
                                uVar19 = *(ulonglong *)
                                          ((longlong)(int)*puVar51 * 1000 +
                                          *(longlong *)(param_1 + 0x90));
                                for (puVar28 = *(ulonglong **)
                                                (*(longlong *)(lVar29 + 0x138) +
                                                (uVar19 % (ulonglong)*(uint *)(lVar29 + 0x140)) * 8)
                                    ; puVar28 != (ulonglong *)0x0; puVar28 = (ulonglong *)puVar28[2]
                                    ) {
                                  if (uVar19 == *puVar28) {
                                    if ((puVar28 == (ulonglong *)0x0) ||
                                       (puVar28 ==
                                        *(ulonglong **)
                                         (*(longlong *)(lVar29 + 0x138) +
                                         *(longlong *)(lVar29 + 0x140) * 8))) break;
                                    uVar70 = (uint)puVar28[1];
                                    puVar23 = local_288;
                                    if ((uVar70 == 0xffffffff) ||
                                       (plVar4 = (longlong *)(lVar29 + 0x160), lVar50 = local_2b8,
                                       lVar29 = param_2,
                                       *(int *)((longlong)(int)uVar70 * 1000 + 0x3e0 + *plVar4) == 0
                                       )) break;
                                    cVar14 = FUN_14035e9c0(param_2,iVar49,uVar70);
                                    if (cVar14 == '\0') {
                                      uVar47 = (ulonglong)uVar3;
                                      lVar50 = local_2b8;
                                      puVar23 = local_288;
                                      break;
                                    }
                                    dVar66 = (double)FUN_140355270(&local_178);
                                    if (((int)dVar66 + iVar38 <= local_2b8) &&
                                       ((iVar16 = *(int *)(*(longlong *)(puVar42 + 0xd8) +
                                                          (longlong)(int)uVar70 * 4),
                                        iVar16 = iVar16 - (int)((float)iVar16 *
                                                               *(float *)(param_2 + 0x26c)),
                                        iVar31 = (int)dVar66 + iVar38 + iVar16, iVar16 == 1000000000
                                        || (iVar31 <= local_2b8)))) {
                                      local_1c8 = iVar31;
                                      iStack_1c4 = iVar49;
                                      if (local_278 <= puVar30) {
                                        sVar39 = (longlong)puVar30 - (longlong)local_288;
                                        lVar50 = (longlong)sVar39 >> 2;
                                        if (lVar50 * -0x5555555555555555 == 0) {
                                          lVar50 = 1;
LAB_140467a18:
                                          puVar23 = (uint *)thunk_FUN_140983da8(lVar50 * 0xc);
                                        }
                                        else {
                                          lVar50 = lVar50 * 0x5555555555555556;
                                          if (lVar50 != 0) goto LAB_140467a18;
                                          puVar23 = (uint *)0x0;
                                        }
                                        puVar25 = puVar23;
                                        if (local_288 != puVar30) {
                                          pvVar24 = memmove(puVar23,local_288,sVar39);
                                          puVar25 = (uint *)((longlong)pvVar24 +
                                                            ((longlong)puVar30 - (longlong)local_288
                                                            >> 2) * 4);
                                        }
                                        *(ulonglong *)puVar25 = CONCAT44(iStack_1c4,local_1c8);
                                        puVar25[2] = uVar70;
                                        puVar30 = puVar25 + 3;
                                        if (local_288 != (uint *)0x0) {
                                          free(local_288);
                                        }
                                        local_278 = puVar23 + lVar50 * 3;
                                        uVar47 = (ulonglong)uVar3;
                                        puVar42 = local_290;
                                        lVar50 = local_2b8;
                                        local_288 = puVar23;
                                        puStack_280 = puVar30;
                                        iVar38 = (int)local_2b0;
                                        break;
                                      }
                                      puStack_280 = puVar30 + 3;
                                      *(ulonglong *)puVar30 = CONCAT44(iVar49,iVar31);
                                      puVar30[2] = uVar70;
                                      puVar30 = puStack_280;
                                    }
                                    uVar47 = (ulonglong)uVar3;
                                    lVar50 = local_2b8;
                                    puVar23 = local_288;
                                    break;
                                  }
                                }
                              }
                              puVar51 = puVar51 + 1;
                              puVar25 = local_270;
                            } while (puVar51 != local_2d8);
                          }
                        }
                      }
                      break;
                    }
                  }
                  local_2a0 = local_2a0 + 1;
                  puVar51 = puVar25;
                } while (local_2a0 != local_2a8);
                FUN_140470f80(puVar23);
                uVar19 = 0;
                puVar51 = (uint *)0x0;
                local_1e8 = 0;
                puStack_1e0 = (uint *)0x0;
                lVar50 = -1;
                dVar66 = DAT_140aac6c8;
                local_2a0 = puVar23;
                if (puVar23 != puVar30) {
LAB_140467b40:
                  plVar4 = param_11;
                  local_1d8 = (int)*(undefined8 *)puVar23;
                  iStack_1d4 = (int)((ulonglong)*(undefined8 *)puVar23 >> 0x20);
                  uVar3 = puVar23[2];
                  local_2d8 = *(uint **)(param_2 + 0x160);
                  local_2a8 = (uint *)((longlong)iStack_1d4 * 1000);
                  local_290 = local_2d8 + (longlong)(int)uVar3 * 0xfa;
                  for (dVar67 = dVar12 - (double)(int)*(undefined8 *)
                                                       ((longlong)(local_2a8 + 6) +
                                                       (longlong)local_2d8) * dVar55;
                      dVar67 < dVar66; dVar67 = dVar67 + DAT_140aac020) {
                  }
                  for (; DAT_140aabfe8 < dVar67; dVar67 = dVar67 + DAT_140aac6d0) {
                  }
                  dVar59 = dVar11 - (double)(int)((ulonglong)
                                                  *(undefined8 *)
                                                   ((longlong)(local_2a8 + 6) + (longlong)local_2d8)
                                                 >> 0x20) * dVar55;
                  for (dVar68 = dVar65 - (double)(int)*(undefined8 *)(local_290 + 6) * dVar55;
                      dVar68 < dVar66; dVar68 = dVar68 + DAT_140aac020) {
                  }
                  for (; DAT_140aabfe8 < dVar68; dVar68 = dVar68 + DAT_140aac6d0) {
                  }
                  dVar60 = dVar57 - (double)(int)((ulonglong)*(undefined8 *)(local_290 + 6) >> 0x20)
                                    * dVar55;
                  iVar49 = (int)SQRT(dVar88 * DAT_140aabcc8 * dVar88 * dVar60 * dVar60 +
                                     dVar56 * dVar68 * dVar56 * dVar68) +
                           (int)SQRT(dVar87 * DAT_140aabcc8 * dVar87 * dVar59 * dVar59 +
                                     dVar58 * dVar67 * dVar58 * dVar67);
                  local_2a0 = puVar23;
                  local_1d0 = uVar3;
                  if (lVar50 < 1) {
LAB_140467ce6:
                    local_258 = *(ulonglong *)((longlong)local_2a8 + (longlong)local_2d8);
                    puStack_250 = *(uint **)local_290;
                    LOCK();
                    *param_14 = *param_14 + 1;
                    UNLOCK();
                    auVar8._8_8_ = 0;
                    auVar8._0_8_ = (((ulonglong)((longlong)puStack_250 * -0x395b586ca42e166b) >>
                                     0x2f ^ (longlong)puStack_250 * -0x395b586ca42e166b) *
                                    -0x395b586ca42e166b ^ local_258) * -0x395b586ca42e166b +
                                   0xe6546b64;
                    uVar47 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar8,8) +
                             SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar8,0);
                    lVar29 = 0;
                    uVar48 = uVar47 >> 7;
                    uVar26 = (undefined1)uVar47;
                    uVar13 = CONCAT11(uVar26,uVar26);
                    uVar70 = CONCAT22(uVar13,uVar13) & 0x7f7f7f7f;
                    do {
                      uVar48 = uVar48 & param_11[3];
                      auVar90 = *(undefined1 (*) [16])(*param_11 + uVar48);
                      cVar14 = (char)uVar70;
                      auVar61[0] = -(auVar90[0] == cVar14);
                      cVar71 = (char)(uVar70 >> 8);
                      auVar61[1] = -(auVar90[1] == cVar71);
                      cVar72 = (char)(uVar70 >> 0x10);
                      auVar61[2] = -(auVar90[2] == cVar72);
                      cVar73 = (char)(uVar70 >> 0x18);
                      auVar61[3] = -(auVar90[3] == cVar73);
                      auVar61[4] = -(auVar90[4] == cVar14);
                      auVar61[5] = -(auVar90[5] == cVar71);
                      auVar61[6] = -(auVar90[6] == cVar72);
                      auVar61[7] = -(auVar90[7] == cVar73);
                      auVar61[8] = -(auVar90[8] == cVar14);
                      auVar61[9] = -(auVar90[9] == cVar71);
                      auVar61[10] = -(auVar90[10] == cVar72);
                      auVar61[0xb] = -(auVar90[0xb] == cVar73);
                      auVar61[0xc] = -(auVar90[0xc] == cVar14);
                      auVar61[0xd] = -(auVar90[0xd] == cVar71);
                      auVar61[0xe] = -(auVar90[0xe] == cVar72);
                      auVar61[0xf] = -(auVar90[0xf] == cVar73);
                      uVar9 = (ushort)(SUB161(auVar61 >> 7,0) & 1) |
                              (ushort)(SUB161(auVar61 >> 0xf,0) & 1) << 1 |
                              (ushort)(SUB161(auVar61 >> 0x17,0) & 1) << 2 |
                              (ushort)(SUB161(auVar61 >> 0x1f,0) & 1) << 3 |
                              (ushort)(SUB161(auVar61 >> 0x27,0) & 1) << 4 |
                              (ushort)(SUB161(auVar61 >> 0x2f,0) & 1) << 5 |
                              (ushort)(SUB161(auVar61 >> 0x37,0) & 1) << 6 |
                              (ushort)(SUB161(auVar61 >> 0x3f,0) & 1) << 7 |
                              (ushort)(SUB161(auVar61 >> 0x47,0) & 1) << 8 |
                              (ushort)(SUB161(auVar61 >> 0x4f,0) & 1) << 9 |
                              (ushort)(SUB161(auVar61 >> 0x57,0) & 1) << 10 |
                              (ushort)(SUB161(auVar61 >> 0x5f,0) & 1) << 0xb |
                              (ushort)(SUB161(auVar61 >> 0x67,0) & 1) << 0xc |
                              (ushort)(SUB161(auVar61 >> 0x6f,0) & 1) << 0xd |
                              (ushort)(SUB161(auVar61 >> 0x77,0) & 1) << 0xe |
                              (ushort)(auVar61[0xf] >> 7) << 0xf;
                      uVar15 = (uint)uVar9;
                      cVar14 = DAT_140aad620;
                      cVar71 = DAT_140aad620._1_1_;
                      cVar86 = DAT_140aad620._15_1_;
                      cVar72 = DAT_140aad620._2_1_;
                      cVar73 = DAT_140aad620._3_1_;
                      cVar75 = DAT_140aad620._4_1_;
                      cVar76 = DAT_140aad620._5_1_;
                      cVar77 = DAT_140aad620._6_1_;
                      cVar78 = DAT_140aad620._7_1_;
                      cVar79 = DAT_140aad620._8_1_;
                      cVar80 = DAT_140aad620._9_1_;
                      cVar81 = DAT_140aad620._10_1_;
                      cVar82 = DAT_140aad620._11_1_;
                      cVar83 = DAT_140aad620._12_1_;
                      cVar84 = DAT_140aad620._13_1_;
                      cVar85 = DAT_140aad620._14_1_;
                      if (uVar9 != 0) {
LAB_140467da0:
                        local_2b0 = local_2b0 & 0xffffffff00000000;
                        uVar7 = 0;
                        if (uVar15 != 0) {
                          for (; (uVar15 >> uVar7 & 1) == 0; uVar7 = uVar7 + 1) {
                          }
                        }
                        uVar47 = uVar7 + uVar48 & param_11[3];
                        lVar34 = uVar47 * 0x20;
                        if ((*(ulonglong *)(lVar34 + param_11[1]) != local_258) ||
                           (*(uint **)(lVar34 + 8 + param_11[1]) != puStack_250))
                        goto LAB_140467dc5;
                        pcVar1 = (char *)(*param_11 + uVar47);
                        lVar29 = param_11[1];
                        if (pcVar1 == (char *)(*param_11 + param_11[3])) goto LAB_140467f62;
                        LOCK();
                        param_14[1] = param_14[1] + 1;
                        UNLOCK();
                        lVar52 = *(longlong *)(lVar34 + 0x18 + lVar29);
                        if (*(longlong *)(lVar34 + 0x10 + lVar29) <= param_6) {
                          pcVar32 = pcVar1 + 1;
                          cVar2 = *pcVar32;
                          while (cVar2 < -1) {
                            auVar69[0] = -(*pcVar32 < DAT_140aad760);
                            auVar69[1] = -(pcVar32[1] < UNK_140aad761);
                            auVar69[2] = -(pcVar32[2] < UNK_140aad762);
                            auVar69[3] = -(pcVar32[3] < UNK_140aad763);
                            auVar69[4] = -(pcVar32[4] < UNK_140aad764);
                            auVar69[5] = -(pcVar32[5] < UNK_140aad765);
                            auVar69[6] = -(pcVar32[6] < UNK_140aad766);
                            auVar69[7] = -(pcVar32[7] < UNK_140aad767);
                            auVar69[8] = -(pcVar32[8] < UNK_140aad768);
                            auVar69[9] = -(pcVar32[9] < UNK_140aad769);
                            auVar69[10] = -(pcVar32[10] < UNK_140aad76a);
                            auVar69[0xb] = -(pcVar32[0xb] < UNK_140aad76b);
                            auVar69[0xc] = -(pcVar32[0xc] < UNK_140aad76c);
                            auVar69[0xd] = -(pcVar32[0xd] < UNK_140aad76d);
                            auVar69[0xe] = -(pcVar32[0xe] < UNK_140aad76e);
                            auVar69[0xf] = -(pcVar32[0xf] < UNK_140aad76f);
                            uVar15 = (ushort)((ushort)(SUB161(auVar69 >> 7,0) & 1) |
                                              (ushort)(SUB161(auVar69 >> 0xf,0) & 1) << 1 |
                                              (ushort)(SUB161(auVar69 >> 0x17,0) & 1) << 2 |
                                              (ushort)(SUB161(auVar69 >> 0x1f,0) & 1) << 3 |
                                              (ushort)(SUB161(auVar69 >> 0x27,0) & 1) << 4 |
                                              (ushort)(SUB161(auVar69 >> 0x2f,0) & 1) << 5 |
                                              (ushort)(SUB161(auVar69 >> 0x37,0) & 1) << 6 |
                                              (ushort)(SUB161(auVar69 >> 0x3f,0) & 1) << 7 |
                                              (ushort)(SUB161(auVar69 >> 0x47,0) & 1) << 8 |
                                              (ushort)(SUB161(auVar69 >> 0x4f,0) & 1) << 9 |
                                              (ushort)(SUB161(auVar69 >> 0x57,0) & 1) << 10 |
                                              (ushort)(SUB161(auVar69 >> 0x5f,0) & 1) << 0xb |
                                              (ushort)(SUB161(auVar69 >> 0x67,0) & 1) << 0xc |
                                              (ushort)(SUB161(auVar69 >> 0x6f,0) & 1) << 0xd |
                                              (ushort)(SUB161(auVar69 >> 0x77,0) & 1) << 0xe |
                                             (ushort)(auVar69[0xf] >> 7) << 0xf) + 1;
                            uVar70 = 0;
                            if (uVar15 != 0) {
                              for (; (uVar15 >> uVar70 & 1) == 0; uVar70 = uVar70 + 1) {
                              }
                            }
                            pcVar32 = pcVar32 + uVar70;
                            cVar2 = *pcVar32;
                          }
                          param_11[2] = param_11[2] + -1;
                          lVar29 = *param_11;
                          auVar63[0] = -(*pcVar1 == cVar14);
                          auVar63[1] = -(pcVar1[1] == cVar71);
                          auVar63[2] = -(pcVar1[2] == cVar72);
                          auVar63[3] = -(pcVar1[3] == cVar73);
                          auVar63[4] = -(pcVar1[4] == cVar75);
                          auVar63[5] = -(pcVar1[5] == cVar76);
                          auVar63[6] = -(pcVar1[6] == cVar77);
                          auVar63[7] = -(pcVar1[7] == cVar78);
                          auVar63[8] = -(pcVar1[8] == cVar79);
                          auVar63[9] = -(pcVar1[9] == cVar80);
                          auVar63[10] = -(pcVar1[10] == cVar81);
                          auVar63[0xb] = -(pcVar1[0xb] == cVar82);
                          auVar63[0xc] = -(pcVar1[0xc] == cVar83);
                          auVar63[0xd] = -(pcVar1[0xd] == cVar84);
                          auVar63[0xe] = -(pcVar1[0xe] == cVar85);
                          auVar63[0xf] = -(pcVar1[0xf] == cVar86);
                          uVar9 = (ushort)(SUB161(auVar63 >> 7,0) & 1) |
                                  (ushort)(SUB161(auVar63 >> 0xf,0) & 1) << 1 |
                                  (ushort)(SUB161(auVar63 >> 0x17,0) & 1) << 2 |
                                  (ushort)(SUB161(auVar63 >> 0x1f,0) & 1) << 3 |
                                  (ushort)(SUB161(auVar63 >> 0x27,0) & 1) << 4 |
                                  (ushort)(SUB161(auVar63 >> 0x2f,0) & 1) << 5 |
                                  (ushort)(SUB161(auVar63 >> 0x37,0) & 1) << 6 |
                                  (ushort)(SUB161(auVar63 >> 0x3f,0) & 1) << 7 |
                                  (ushort)(SUB161(auVar63 >> 0x47,0) & 1) << 8 |
                                  (ushort)(SUB161(auVar63 >> 0x4f,0) & 1) << 9 |
                                  (ushort)(SUB161(auVar63 >> 0x57,0) & 1) << 10 |
                                  (ushort)(SUB161(auVar63 >> 0x5f,0) & 1) << 0xb |
                                  (ushort)(SUB161(auVar63 >> 0x67,0) & 1) << 0xc |
                                  (ushort)(SUB161(auVar63 >> 0x6f,0) & 1) << 0xd |
                                  (ushort)(SUB161(auVar63 >> 0x77,0) & 1) << 0xe |
                                  (ushort)(auVar63[0xf] >> 7) << 0xf;
                          pcVar32 = (char *)((param_11[3] & (ulonglong)(pcVar1 + (-0x10 - lVar29)))
                                            + lVar29);
                          auVar64[0] = -(*pcVar32 == cVar14);
                          auVar64[1] = -(pcVar32[1] == cVar71);
                          auVar64[2] = -(pcVar32[2] == cVar72);
                          auVar64[3] = -(pcVar32[3] == cVar73);
                          auVar64[4] = -(pcVar32[4] == cVar75);
                          auVar64[5] = -(pcVar32[5] == cVar76);
                          auVar64[6] = -(pcVar32[6] == cVar77);
                          auVar64[7] = -(pcVar32[7] == cVar78);
                          auVar64[8] = -(pcVar32[8] == cVar79);
                          auVar64[9] = -(pcVar32[9] == cVar80);
                          auVar64[10] = -(pcVar32[10] == cVar81);
                          auVar64[0xb] = -(pcVar32[0xb] == cVar82);
                          auVar64[0xc] = -(pcVar32[0xc] == cVar83);
                          auVar64[0xd] = -(pcVar32[0xd] == cVar84);
                          auVar64[0xe] = -(pcVar32[0xe] == cVar85);
                          auVar64[0xf] = -(pcVar32[0xf] == cVar86);
                          uVar10 = (ushort)(SUB161(auVar64 >> 7,0) & 1) |
                                   (ushort)(SUB161(auVar64 >> 0xf,0) & 1) << 1 |
                                   (ushort)(SUB161(auVar64 >> 0x17,0) & 1) << 2 |
                                   (ushort)(SUB161(auVar64 >> 0x1f,0) & 1) << 3 |
                                   (ushort)(SUB161(auVar64 >> 0x27,0) & 1) << 4 |
                                   (ushort)(SUB161(auVar64 >> 0x2f,0) & 1) << 5 |
                                   (ushort)(SUB161(auVar64 >> 0x37,0) & 1) << 6 |
                                   (ushort)(SUB161(auVar64 >> 0x3f,0) & 1) << 7 |
                                   (ushort)(SUB161(auVar64 >> 0x47,0) & 1) << 8 |
                                   (ushort)(SUB161(auVar64 >> 0x4f,0) & 1) << 9 |
                                   (ushort)(SUB161(auVar64 >> 0x57,0) & 1) << 10 |
                                   (ushort)(SUB161(auVar64 >> 0x5f,0) & 1) << 0xb |
                                   (ushort)(SUB161(auVar64 >> 0x67,0) & 1) << 0xc |
                                   (ushort)(SUB161(auVar64 >> 0x6f,0) & 1) << 0xd |
                                   (ushort)(SUB161(auVar64 >> 0x77,0) & 1) << 0xe |
                                   (ushort)(auVar64[0xf] >> 7) << 0xf;
                          if ((uVar10 == 0) || (uVar9 == 0)) {
LAB_140467f1f:
                            bVar45 = 0;
                          }
                          else {
                            iVar38 = 0;
                            if (uVar9 != 0) {
                              for (; (uVar9 >> iVar38 & 1) == 0; iVar38 = iVar38 + 1) {
                              }
                            }
                            iVar16 = 0x1f;
                            if (uVar10 != 0) {
                              for (; ((uint)uVar10 << 0x10) >> iVar16 == 0; iVar16 = iVar16 + -1) {
                              }
                            }
                            if (uVar10 == 0) {
                              iVar16 = 0x20;
                            }
                            else {
                              iVar16 = 0x1f - iVar16;
                            }
                            if (0xf < (uint)(iVar16 + iVar38)) goto LAB_140467f1f;
                            bVar45 = 1;
                          }
                          cVar14 = -2;
                          if (bVar45 != 0) {
                            cVar14 = -0x80;
                          }
                          *pcVar1 = cVar14;
                          *(char *)(((ulonglong)(pcVar1 + (-0x10 - lVar29)) & param_11[3]) +
                                    (ulonglong)((uint)param_11[3] & 0xf) + 1 + *param_11) = cVar14;
                          param_11[6] = param_11[6] + (ulonglong)bVar45;
                          goto LAB_140467f62;
                        }
                        if (lVar52 < 0) goto LAB_140468086;
                        if ((lVar52 < 1) || (lVar52 - param_6 < 1)) goto LAB_140467f62;
                        lVar29 = (longlong)iVar49 + (lVar52 - param_6);
                        local_2a0 = puVar23;
                        if ((lVar29 < local_2b8) && ((lVar50 == -1 || (lVar29 < lVar50)))) {
                          lVar50 = lVar29;
                          uVar19 = local_258;
                          puVar51 = puStack_250;
                          local_1e8 = local_258;
                          puStack_1e0 = puStack_250;
                        }
                        goto LAB_140468086;
                      }
LAB_140467dd3:
                      auVar62[0] = -(cVar14 == auVar90[0]);
                      auVar62[1] = -(cVar71 == auVar90[1]);
                      auVar62[2] = -(cVar72 == auVar90[2]);
                      auVar62[3] = -(cVar73 == auVar90[3]);
                      auVar62[4] = -(cVar75 == auVar90[4]);
                      auVar62[5] = -(cVar76 == auVar90[5]);
                      auVar62[6] = -(cVar77 == auVar90[6]);
                      auVar62[7] = -(cVar78 == auVar90[7]);
                      auVar62[8] = -(cVar79 == auVar90[8]);
                      auVar62[9] = -(cVar80 == auVar90[9]);
                      auVar62[10] = -(cVar81 == auVar90[10]);
                      auVar62[0xb] = -(cVar82 == auVar90[0xb]);
                      auVar62[0xc] = -(cVar83 == auVar90[0xc]);
                      auVar62[0xd] = -(cVar84 == auVar90[0xd]);
                      auVar62[0xe] = -(cVar85 == auVar90[0xe]);
                      auVar62[0xf] = -(cVar86 == auVar90[0xf]);
                      if ((((((((((((((((SUB161(auVar62 >> 7,0) & 1) != 0 ||
                                       (SUB161(auVar62 >> 0xf,0) & 1) != 0) ||
                                      (SUB161(auVar62 >> 0x17,0) & 1) != 0) ||
                                     (SUB161(auVar62 >> 0x1f,0) & 1) != 0) ||
                                    (SUB161(auVar62 >> 0x27,0) & 1) != 0) ||
                                   (SUB161(auVar62 >> 0x2f,0) & 1) != 0) ||
                                  (SUB161(auVar62 >> 0x37,0) & 1) != 0) ||
                                 (SUB161(auVar62 >> 0x3f,0) & 1) != 0) ||
                                (SUB161(auVar62 >> 0x47,0) & 1) != 0) ||
                               (SUB161(auVar62 >> 0x4f,0) & 1) != 0) ||
                              (SUB161(auVar62 >> 0x57,0) & 1) != 0) ||
                             (SUB161(auVar62 >> 0x5f,0) & 1) != 0) ||
                            (SUB161(auVar62 >> 0x67,0) & 1) != 0) ||
                           (SUB161(auVar62 >> 0x6f,0) & 1) != 0) ||
                          (SUB161(auVar62 >> 0x77,0) & 1) != 0) || auVar62[0xf] < '\0')
                      goto LAB_140467f62;
                      lVar29 = lVar29 + 0x10;
                      uVar48 = uVar48 + lVar29;
                    } while( true );
                  }
                  if (lVar50 <= iVar49) goto LAB_140468086;
                  if (local_1d8 < lVar50) goto LAB_140467ce6;
                  goto LAB_14046809b;
                }
                goto LAB_1404680c9;
              }
              goto LAB_1404680fb;
            }
LAB_140468127:
            if (local_288 != (uint *)0x0) {
              free(local_288);
            }
            if (local_2d0 != (uint *)0x0) {
              free(local_2d0);
            }
            if (local_270 != (uint *)0x0) {
              free(local_270);
            }
            FUN_140002cc0(&local_100);
            FUN_140002cc0(local_118);
          }
        }
        if (local_230 != (int *)0x0) {
          free(local_230);
        }
        if (local_218 != (double *)0x0) {
          free(local_218);
        }
        FUN_140002cc0(&local_130);
        FUN_140002cc0(local_148);
      }
    }
    pdVar36 = local_248;
    if (local_200 != (int *)0x0) {
      free(local_200);
    }
    if (pdVar36 != (double *)0x0) {
      free(pdVar36);
    }
  }
  return;
joined_r0x0001404673e7:
  if (puVar30 == puVar23) goto LAB_1404674d6;
  uVar3 = *puVar30;
  lVar29 = (longlong)(int)uVar3 * 1000;
  lVar50 = *(longlong *)(param_1 + 0x90);
  dVar65 = dVar56 - *(double *)(lVar29 + 8 + lVar50);
  dVar57 = auVar90._0_8_ - *(double *)(lVar29 + 0x10 + lVar50);
  dVar57 = dVar57 * dVar57 + dVar65 * dVar65;
  pdVar36 = (double *)(lVar29 + 0x28 + lVar50);
  if (dVar57 < *pdVar36 || dVar57 == *pdVar36) {
    if (puVar20 < local_260) {
      *puVar20 = uVar3;
      puVar25 = puVar20;
    }
    else {
      sVar39 = (longlong)puVar20 - (longlong)local_270;
      lVar50 = (longlong)sVar39 >> 2;
      if (lVar50 == 0) {
        lVar50 = 1;
LAB_140467468:
        puVar51 = (uint *)thunk_FUN_140983da8(lVar50 * 4);
      }
      else {
        lVar50 = lVar50 * 2;
        if (lVar50 != 0) goto LAB_140467468;
        puVar51 = (uint *)0x0;
      }
      puVar25 = puVar51;
      if (local_270 != puVar20) {
        pvVar24 = memmove(puVar51,local_270,sVar39);
        puVar25 = (uint *)((longlong)pvVar24 + ((longlong)puVar20 - (longlong)local_270 >> 2) * 4);
      }
      *puVar25 = uVar3;
      if (local_270 != (uint *)0x0) {
        free(local_270);
      }
      local_260 = puVar51 + lVar50;
      local_270 = puVar51;
    }
    puVar20 = puVar25 + 1;
    puStack_268 = puVar20;
  }
  puVar30 = puVar30 + 1;
  puVar51 = puVar20;
  goto joined_r0x0001404673e7;
LAB_140468670:
  uVar19 = uVar19 & (lVar50 - lVar29 >> 3) - 1U;
  puVar28 = (ulonglong *)(lVar29 + uVar19 * 8);
  LOCK();
  bVar53 = *puVar28 == 0;
  if (bVar53) {
    *puVar28 = local_1e8;
  }
  UNLOCK();
  if (!bVar53) goto code_r0x000140468687;
  *(undefined8 *)(param_12[3] + uVar19 * 8) = *puVar37;
LAB_1404686b8:
  bVar53 = true;
  goto LAB_140468325;
code_r0x000140468687:
  uVar19 = uVar19 + 1;
  lVar50 = param_12[1];
  lVar29 = *param_12;
  if (lVar50 - lVar29 >> 3 == 0) goto code_r0x0001404686a0;
  goto LAB_140468670;
code_r0x0001404686a0:
  bVar53 = true;
LAB_140468325:
  puVar30 = local_2a0 + 1;
  local_2a0 = puVar30;
  if (puVar30 == local_2a8) goto LAB_14046833c;
  goto LAB_1404682c4;
LAB_14046833c:
  if (((*(int *)(param_3 + 0xd8) != 1) || ((longlong)local_2b0 < 1)) || ((longlong)puVar51 < 1))
  goto LAB_1404680c9;
  ppuVar46 = &local_290;
  uVar19 = local_2b0;
LAB_1404680bd:
  FUN_14045a270(param_12,uVar19,ppuVar46);
LAB_1404680c9:
  uVar91 = (undefined4)DAT_140aabb18;
  uVar92 = (undefined4)((ulonglong)DAT_140aabb18 >> 0x20);
  uVar93 = SUB84(DAT_140aabd08,0);
  uVar94 = (undefined4)((ulonglong)DAT_140aabd08 >> 0x20);
  uVar95 = SUB84(DAT_140aabc48,0);
  uVar96 = (undefined4)((ulonglong)DAT_140aabc48 >> 0x20);
  uVar97 = SUB84(DAT_140aabf88,0);
  uVar98 = (undefined4)((ulonglong)DAT_140aabf88 >> 0x20);
  lVar29 = local_2e0;
LAB_1404680fb:
  param_8 = param_8 + DAT_140aac668;
  lVar52 = local_298;
  if (param_8 < (double)CONCAT44(uVar94,uVar93) || param_8 == (double)CONCAT44(uVar94,uVar93))
  goto LAB_140468127;
  goto LAB_140466e90;
LAB_140467dc5:
  uVar15 = uVar15 & uVar15 - 1;
  if (uVar15 == 0) goto LAB_140467dd3;
  goto LAB_140467da0;
LAB_140467f62:
  lVar29 = 0x7fffffffffffffff;
  if ((lVar50 < 1) || (lVar29 = lVar50 - iVar49, -1 < lVar29)) {
    FUN_14035ea20(param_2,param_13,*(undefined8 *)((longlong)local_2a8 + (longlong)local_2d8),
                  param_5,*(undefined8 *)local_290,0,lVar29);
    FUN_14035e8e0(param_13,local_158,param_2);
    if ((*(char *)(param_13 + 0xe8) == '\0') || (local_158[0] == 0)) {
      dVar66 = DAT_140aac6c8;
      if (lVar29 == 999999999) {
        FUN_1404724a0(plVar4,&local_1b0,&local_258);
        *(longlong *)(local_1a8 + 0x10) = param_6 + 300;
        *(undefined8 *)(local_1a8 + 0x18) = 0xffffffffffffffff;
        dVar66 = DAT_140aac6c8;
      }
    }
    else {
      lVar34 = param_7 + *(int *)(local_158[0] + 0x24);
      lVar29 = lVar34 + 0x93a80;
      if (param_5 <= *(int *)(local_158[0] + 0x24)) {
        lVar29 = lVar34;
      }
      lVar34 = *(int *)(*(longlong *)(param_13 + 0x20) + (longlong)(int)uVar3 * 8) + lVar29;
      FUN_1404724a0(plVar4,&local_1b0,&local_258);
      *(longlong *)(local_1a8 + 0x10) = lVar29;
      *(longlong *)(local_1a8 + 0x18) = lVar34;
      lVar34 = lVar34 - param_6;
      dVar66 = DAT_140aac6c8;
      if (((0 < lVar34) && (lVar34 = iVar49 + lVar34, lVar34 < local_2b8)) &&
         ((lVar50 == -1 || (lVar34 < lVar50)))) {
        local_1e8 = local_258;
        puStack_1e0 = puStack_250;
        lVar50 = lVar34;
        uVar19 = local_258;
        puVar51 = puStack_250;
      }
    }
  }
LAB_140468086:
  puVar23 = local_2a0 + 3;
  local_2a0 = puVar23;
  if (puVar23 == puVar30) goto LAB_14046809b;
  goto LAB_140467b40;
LAB_14046809b:
  if (((uVar19 == 0) || (puVar51 == (uint *)0x0)) || (lVar50 < 1)) goto LAB_1404680c9;
  ppuVar46 = &puStack_1e0;
  goto LAB_1404680bd;
}
