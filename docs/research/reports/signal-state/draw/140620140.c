// Candidate VA 140620140; RVA 0x620140
// Ghidra inferred prototype: undefined FUN_140620140()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140620140(longlong param_1,longlong *param_2,longlong param_3,ulonglong param_4,
                  longlong param_5,longlong param_6,int param_7,double param_8,longlong param_9,
                  int param_10,byte param_11,longlong param_12)

{
  int iVar1;
  undefined8 *puVar2;
  ulonglong *puVar3;
  ulonglong uVar4;
  ulonglong *puVar5;
  undefined1 auVar6 [16];
  ushort uVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  int iVar11;
  undefined2 uVar12;
  ulonglong uVar13;
  uint uVar14;
  longlong lVar15;
  undefined2 *puVar16;
  int *piVar17;
  undefined1 *puVar18;
  undefined1 uVar19;
  int *piVar20;
  longlong *plVar21;
  ulonglong *puVar22;
  ulonglong uVar23;
  longlong lVar24;
  longlong lVar25;
  longlong lVar26;
  ulonglong uVar27;
  double *pdVar28;
  ulonglong *puVar29;
  longlong *plVar30;
  int iVar31;
  undefined8 uVar33;
  double dVar34;
  double dVar35;
  double dVar36;
  double dVar37;
  double dVar38;
  undefined1 auVar39 [16];
  undefined1 auVar40 [16];
  float fVar44;
  ulonglong uVar45;
  double dVar46;
  double dVar47;
  double dVar48;
  undefined4 uVar49;
  undefined4 uVar50;
  uint uVar51;
  uint uVar52;
  undefined4 uVar53;
  undefined4 uVar54;
  uint uVar55;
  uint uVar56;
  undefined4 uVar57;
  undefined4 uVar58;
  undefined8 local_res20;
  int local_3b0 [2];
  undefined8 *local_3a8;
  longlong local_3a0;
  double local_398;
  int local_390 [4];
  int local_380 [2];
  double local_378;
  double local_370;
  longlong local_368;
  undefined8 uStack_360;
  ulonglong *local_358;
  undefined8 uStack_350;
  undefined1 local_348 [32];
  undefined2 local_328;
  undefined2 uStack_326;
  undefined2 uStack_324;
  undefined2 uStack_322;
  undefined2 uStack_320;
  undefined2 uStack_31e;
  char cStack_31c;
  undefined1 uStack_31b;
  undefined2 uStack_31a;
  double local_318;
  ulonglong *local_310;
  undefined4 local_2f8;
  undefined4 uStack_2f4;
  uint uStack_2f0;
  uint uStack_2ec;
  int local_2e0;
  char local_2d8;
  longlong local_2c8;
  longlong local_2c0;
  double local_2b8;
  void *local_2b0;
  undefined8 local_2a8;
  undefined8 uStack_2a0;
  void *local_298;
  undefined8 uStack_290;
  undefined8 local_288;
  undefined1 local_280;
  undefined4 local_27c;
  undefined4 local_278;
  ushort local_274;
  int local_270;
  undefined4 local_26c;
  short local_268;
  double local_258;
  double local_250;
  undefined1 local_248 [4];
  undefined2 local_244;
  char local_23c;
  undefined1 local_238 [16];
  double local_228;
  double local_220;
  undefined1 local_218 [4];
  undefined2 local_214;
  char local_20c;
  undefined1 local_208 [16];
  double local_1f8;
  double local_1f0;
  undefined1 local_1e8 [16];
  double local_1d8;
  double local_1d0;
  undefined1 local_1c8 [16];
  longlong local_1b8;
  longlong local_1b0;
  double local_1a8;
  void *local_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  void *local_188;
  undefined8 uStack_180;
  undefined8 local_178;
  undefined1 local_170;
  undefined4 local_16c;
  undefined4 local_168;
  ushort local_164;
  int local_160;
  undefined4 local_15c;
  short local_158;
  undefined1 local_128 [16];
  undefined1 local_118 [16];
  undefined1 local_108 [16];
  undefined1 local_f8 [184];
  char cVar32;
  char cVar41;
  char cVar42;
  char cVar43;

  lVar25 = param_12;
  local_res20 = param_4;
  if (*(longlong *)(param_12 + 0x18) != 0) {
    local_3a0 = *param_2 + 0x1270;
    FUN_140556590(local_3a0,&local_368,param_1 + 0x208);
    if ((uStack_360._4_1_ != '\0') && ((short)local_368 != -1)) {
      lVar26 = *(longlong *)(param_3 + 0x260);
      param_9 = lVar26;
      if (lVar26 == 0) {
                    /* WARNING: Subroutine does not return */
        abort();
      }
      local_2c8 = param_2[0x23];
      local_2c0 = param_2[0x24];
      local_2b8 = DAT_140aabd08;
      local_2b0 = (void *)0x0;
      local_2a8 = 0;
      uStack_2a0 = 0;
      local_298 = (void *)0x0;
      uStack_290 = 0;
      local_288 = 0;
      local_280 = 0;
      local_27c = 0;
      local_1a8 = DAT_140aabd08;
      local_1a0 = (void *)0x0;
      local_198 = 0;
      uStack_190 = 0;
      local_188 = (void *)0x0;
      uStack_180 = 0;
      local_178 = 0;
      local_170 = 0;
      local_16c = 0;
      local_274 = (ushort)param_11;
      local_278 = 3;
      local_26c = 6;
      local_268 = (short)local_368;
      local_168 = 3;
      local_15c = 6;
      local_158 = (short)local_368;
      local_res20._0_4_ = param_10;
      if (param_10 == 100) {
        local_res20._0_4_ = 4;
        iVar31 = 4;
      }
      else {
        iVar31 = param_10;
        if (param_10 == -100) {
          local_res20._0_4_ = -4;
          iVar31 = -4;
        }
      }
      param_8 = (double)CONCAT44(param_8._4_4_,5);
      param_7 = -5;
      piVar20 = &param_7;
      if (-6 < iVar31) {
        piVar20 = (int *)&local_res20;
      }
      piVar17 = (int *)&param_8;
      if (iVar31 < 6) {
        piVar17 = piVar20;
      }
      local_270 = *piVar17 * 1000 + (int)param_2[0x5e] + *(int *)((longlong)param_2 + 0x2d4);
      local_res20 = CONCAT44(local_res20._4_4_,param_10);
      if (param_10 == 100) {
        local_res20 = CONCAT44(local_res20._4_4_,4);
        iVar31 = 4;
      }
      else {
        iVar31 = param_10;
        if (param_10 == -100) {
          local_res20 = CONCAT44(local_res20._4_4_,0xfffffffc);
          iVar31 = -4;
        }
      }
      param_8 = (double)CONCAT44(param_8._4_4_,5);
      param_7 = -5;
      piVar20 = &param_7;
      if (-6 < iVar31) {
        piVar20 = (int *)&local_res20;
      }
      piVar17 = (int *)&param_8;
      if (iVar31 < 6) {
        piVar17 = piVar20;
      }
      local_160 = *piVar17 * 1000 + *(int *)((longlong)param_2 + 0x2ec) +
                  *(int *)((longlong)param_2 + 0x2d4);
      plVar21 = *(longlong **)(lVar25 + 8);
      local_3a8 = (undefined8 *)*plVar21;
      plVar30 = plVar21;
      if (local_3a8 == (undefined8 *)0x0) {
        plVar30 = plVar21 + 1;
        local_3a8 = (undefined8 *)*plVar30;
        while (local_3a8 == (undefined8 *)0x0) {
          plVar30 = plVar30 + 1;
          local_3a8 = (undefined8 *)*plVar30;
        }
      }
      puVar2 = (undefined8 *)plVar21[*(longlong *)(lVar25 + 0x10)];
      local_1b8 = local_2c8;
      local_1b0 = local_2c0;
      local_164 = local_274;
      if (local_3a8 != puVar2) {
        param_7 = CONCAT22(0xffff,uStack_324);
        do {
          local_358 = (ulonglong *)*local_3a8;
          uStack_350 = local_3a8[1];
          puVar3 = (ulonglong *)local_3a8[1];
          uVar13 = puVar3[6];
          if ((int)uVar13 < 7) {
            lVar25 = *(longlong *)(param_5 + 0x120);
            local_res20 = local_res20 & 0xffffffffffffff00;
            local_3b0[0] = 0;
            FUN_140362640(param_3 + 0x130,local_348);
            puVar29 = local_358;
            lVar26 = *(longlong *)(lVar26 + 0x2200);
            auVar6._8_8_ = 0;
            auVar6._0_8_ = *puVar3;
            uVar45 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,8) +
                     SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,0);
            uVar4 = *(ulonglong *)(lVar26 + 0x390);
            lVar24 = 0;
            uVar27 = uVar45 >> 7;
            lVar15 = *(longlong *)(lVar26 + 0x378);
            uVar19 = (undefined1)uVar45;
            uVar12 = CONCAT11(uVar19,uVar19);
            uVar14 = CONCAT22(uVar12,uVar12);
            uVar51 = uVar14 & 0x7f7f7f7f;
            uVar45 = CONCAT44(uVar14,uVar14) & 0x7f7f7f7f7f7f7f7f;
            while( true ) {
              uVar27 = uVar27 & uVar4;
              auVar6 = *(undefined1 (*) [16])(lVar15 + uVar27);
              cVar32 = (char)uVar51;
              auVar39[0] = -(cVar32 == auVar6[0]);
              cVar41 = (char)(uVar51 >> 8);
              auVar39[1] = -(cVar41 == auVar6[1]);
              auVar39[2] = -((char)(uVar45 >> 0x10) == auVar6[2]);
              auVar39[3] = -((char)(uVar45 >> 0x18) == auVar6[3]);
              auVar39[4] = -((char)(uVar45 >> 0x20) == auVar6[4]);
              auVar39[5] = -((char)(uVar45 >> 0x28) == auVar6[5]);
              auVar39[6] = -((char)(uVar45 >> 0x30) == auVar6[6]);
              auVar39[7] = -((char)(uVar45 >> 0x38) == auVar6[7]);
              auVar39[8] = -(cVar32 == auVar6[8]);
              auVar39[9] = -(cVar41 == auVar6[9]);
              cVar42 = (char)(uVar51 >> 0x10);
              auVar39[10] = -(cVar42 == auVar6[10]);
              cVar43 = (char)(uVar51 >> 0x18);
              auVar39[0xb] = -(cVar43 == auVar6[0xb]);
              auVar39[0xc] = -(cVar32 == auVar6[0xc]);
              auVar39[0xd] = -(cVar41 == auVar6[0xd]);
              auVar39[0xe] = -(cVar42 == auVar6[0xe]);
              auVar39[0xf] = -(cVar43 == auVar6[0xf]);
              uVar7 = (ushort)(SUB161(auVar39 >> 7,0) & 1) |
                      (ushort)(SUB161(auVar39 >> 0xf,0) & 1) << 1 |
                      (ushort)(SUB161(auVar39 >> 0x17,0) & 1) << 2 |
                      (ushort)(SUB161(auVar39 >> 0x1f,0) & 1) << 3 |
                      (ushort)(SUB161(auVar39 >> 0x27,0) & 1) << 4 |
                      (ushort)(SUB161(auVar39 >> 0x2f,0) & 1) << 5 |
                      (ushort)(SUB161(auVar39 >> 0x37,0) & 1) << 6 |
                      (ushort)(SUB161(auVar39 >> 0x3f,0) & 1) << 7 |
                      (ushort)(SUB161(auVar39 >> 0x47,0) & 1) << 8 |
                      (ushort)(SUB161(auVar39 >> 0x4f,0) & 1) << 9 |
                      (ushort)(SUB161(auVar39 >> 0x57,0) & 1) << 10 |
                      (ushort)(SUB161(auVar39 >> 0x5f,0) & 1) << 0xb |
                      (ushort)(SUB161(auVar39 >> 0x67,0) & 1) << 0xc |
                      (ushort)(SUB161(auVar39 >> 0x6f,0) & 1) << 0xd |
                      (ushort)(SUB161(auVar39 >> 0x77,0) & 1) << 0xe |
                      (ushort)(auVar39[0xf] >> 7) << 0xf;
              uVar14 = (uint)uVar7;
              if (uVar7 != 0) {
                do {
                  param_8 = (double)((ulonglong)param_8 & 0xffffffff00000000);
                  uVar52 = 0;
                  if (uVar14 != 0) {
                    for (; (uVar14 >> uVar52 & 1) == 0; uVar52 = uVar52 + 1) {
                    }
                  }
                  uVar23 = uVar52 + uVar27 & uVar4;
                  if (*(ulonglong *)(*(longlong *)(lVar26 + 0x380) + uVar23 * 0x10) == *puVar3) {
                    uStack_360 = *(longlong *)(lVar26 + 0x380) + uVar23 * 0x10;
                    uVar4 = uVar23;
                    goto LAB_1406205bb;
                  }
                  uVar14 = uVar14 & uVar14 - 1;
                } while (uVar14 != 0);
              }
              auVar40[0] = -(DAT_140aad620 == auVar6[0]);
              auVar40[1] = -(UNK_140aad621 == auVar6[1]);
              auVar40[2] = -(UNK_140aad622 == auVar6[2]);
              auVar40[3] = -(UNK_140aad623 == auVar6[3]);
              auVar40[4] = -(UNK_140aad624 == auVar6[4]);
              auVar40[5] = -(UNK_140aad625 == auVar6[5]);
              auVar40[6] = -(UNK_140aad626 == auVar6[6]);
              auVar40[7] = -(UNK_140aad627 == auVar6[7]);
              auVar40[8] = -(UNK_140aad628 == auVar6[8]);
              auVar40[9] = -(UNK_140aad629 == auVar6[9]);
              auVar40[10] = -(UNK_140aad62a == auVar6[10]);
              auVar40[0xb] = -(UNK_140aad62b == auVar6[0xb]);
              auVar40[0xc] = -(UNK_140aad62c == auVar6[0xc]);
              auVar40[0xd] = -(UNK_140aad62d == auVar6[0xd]);
              auVar40[0xe] = -(UNK_140aad62e == auVar6[0xe]);
              auVar40[0xf] = -(UNK_140aad62f == auVar6[0xf]);
              if ((((((((((((((((SUB161(auVar40 >> 7,0) & 1) != 0 ||
                               (SUB161(auVar40 >> 0xf,0) & 1) != 0) ||
                              (SUB161(auVar40 >> 0x17,0) & 1) != 0) ||
                             (SUB161(auVar40 >> 0x1f,0) & 1) != 0) ||
                            (SUB161(auVar40 >> 0x27,0) & 1) != 0) ||
                           (SUB161(auVar40 >> 0x2f,0) & 1) != 0) ||
                          (SUB161(auVar40 >> 0x37,0) & 1) != 0) ||
                         (SUB161(auVar40 >> 0x3f,0) & 1) != 0) ||
                        (SUB161(auVar40 >> 0x47,0) & 1) != 0) ||
                       (SUB161(auVar40 >> 0x4f,0) & 1) != 0) || (SUB161(auVar40 >> 0x57,0) & 1) != 0
                      ) || (SUB161(auVar40 >> 0x5f,0) & 1) != 0) ||
                    (SUB161(auVar40 >> 0x67,0) & 1) != 0) || (SUB161(auVar40 >> 0x6f,0) & 1) != 0)
                  || (SUB161(auVar40 >> 0x77,0) & 1) != 0) || auVar40[0xf] < '\0') break;
              lVar24 = lVar24 + 0x10;
              uVar27 = uVar27 + lVar24;
            }
LAB_1406205bb:
            local_368 = lVar15 + uVar4;
            iVar31 = 0;
            if (local_368 != *(longlong *)(*(longlong *)(param_9 + 0x2200) + 0x390) + lVar15) {
              local_3b0[0] = *(int *)(uStack_360 + 8);
              iVar31 = local_3b0[0];
            }
            if ((((int)puVar3[6] == 0) && (local_358[0x1a] != 0)) &&
               ((local_358[1] == 0 || (local_358[2] == 0)))) {
              local_res20 = CONCAT71(local_res20._1_7_,1);
              cVar32 = '\x01';
            }
            else {
              cVar32 = (char)local_res20;
            }
            lVar15 = FUN_1404148d0(param_5,(int)puVar3[6],puVar3[7]);
            lVar26 = param_9;
            if (lVar15 != 0) {
              cStack_31c = '\0';
              iVar11 = (int)(*(longlong *)(lVar15 + 0x80) - *(longlong *)(lVar15 + 0x78) >> 4) *
                       -0x33333333;
              local_390[0] = iVar11 + -1;
              local_390[1] = 0;
              piVar20 = local_390;
              if (local_390[0] < 1) {
                piVar20 = local_390 + 1;
              }
              local_390[2] = 0;
              piVar17 = local_390 + 2;
              if (-1 < iVar31) {
                piVar17 = local_3b0;
              }
              if (*piVar20 < iVar31) {
                piVar17 = piVar20;
              }
              iVar1 = *piVar17;
              if ((-1 < iVar1) && (iVar1 < iVar11)) {
                puVar16 = (undefined2 *)
                          FUN_140556790(local_3a0,local_108,
                                        (longlong)iVar1 * 0x50 + *(longlong *)(lVar15 + 0x78));
                local_328 = *puVar16;
                uStack_326 = puVar16[1];
                uStack_324 = puVar16[2];
                uStack_322 = puVar16[3];
                uStack_320 = puVar16[4];
                uStack_31e = puVar16[5];
                uStack_31a = puVar16[7];
                cStack_31c = (char)puVar16[6];
                uStack_31b = (undefined1)((ushort)puVar16[6] >> 8);
                param_7 = CONCAT22(param_7._2_2_,uStack_324);
              }
              cVar41 = cStack_31c;
              if (cStack_31c == '\0') {
                lVar15 = FUN_1404148d0(param_5,(int)puVar3[6],0);
                lVar26 = param_9;
                if (lVar15 == 0) goto LAB_140620e46;
                iVar11 = (int)(*(longlong *)(lVar15 + 0x80) - *(longlong *)(lVar15 + 0x78) >> 4) *
                         -0x33333333;
                local_390[3] = iVar11 + -1;
                local_380[0] = 0;
                piVar20 = local_390 + 3;
                if (local_390[3] < 1) {
                  piVar20 = local_380;
                }
                local_380[1] = 0;
                piVar17 = local_380 + 1;
                if (-1 < iVar31) {
                  piVar17 = local_3b0;
                }
                if (*piVar20 < iVar31) {
                  piVar17 = piVar20;
                }
                iVar31 = *piVar17;
                if ((iVar31 < 0) || (iVar11 <= iVar31)) goto LAB_14062078f;
                puVar16 = (undefined2 *)
                          FUN_140556790(local_3a0,local_f8,
                                        (longlong)iVar31 * 0x50 + *(longlong *)(lVar15 + 0x78));
                local_328 = *puVar16;
                uStack_326 = puVar16[1];
                uStack_324 = puVar16[2];
                uStack_322 = puVar16[3];
                uStack_320 = puVar16[4];
                uStack_31e = puVar16[5];
                uStack_31a = puVar16[7];
                cStack_31c = (char)puVar16[6];
                uStack_31b = (undefined1)((ushort)puVar16[6] >> 8);
                cVar41 = (char)((ulonglong)*(undefined8 *)(puVar16 + 4) >> 0x20);
                param_7 = CONCAT22(param_7._2_2_,uStack_324);
                uVar12 = uStack_324;
              }
              else {
LAB_14062078f:
                uVar12 = (undefined2)param_7;
              }
              lVar26 = param_9;
              if ((cVar41 != '\0') &&
                 (FUN_14061b580(&local_318,puVar29,puVar3), lVar26 = param_9, local_2d8 != '\0')) {
                local_398 = (double)CONCAT44(uStack_2ec,uStack_2f0);
                local_370 = (double)CONCAT44(uStack_2ec ^ DAT_140aad600._4_4_,
                                             uStack_2f0 ^ (uint)DAT_140aad600);
                uVar51 = 0;
                uVar52 = 0;
                uVar14 = (uint)puVar3[0xc];
                if ((*(char *)((longlong)puVar3 + 0x51) == '\0') ||
                   ((puVar29 == (ulonglong *)0x0 || (*puVar29 != puVar3[8])))) {
                  cVar41 = (char)puVar3[10];
                }
                else {
                  cVar41 = (char)puVar3[10];
                  if (*(char *)((longlong)puVar29 + 0x2c) != *(char *)((longlong)puVar3 + 0x51)) {
                    cVar41 = -(char)puVar3[10];
                  }
                }
                if ((cVar41 == -1) &&
                   (*(char *)((longlong)(int)puVar3[6] * 0x38 + 0x33 +
                             *(longlong *)(param_5 + 0x120)) != '\0')) {
                  uVar14 = uVar14 - 2 & 3;
                }
                uVar56 = DAT_140aabae4;
                if (uVar14 == 1) {
                  uVar51 = DAT_140aabae4;
                  uVar55 = 0;
                }
                else {
                  if (uVar14 == 2) {
                    uVar51 = DAT_140aabae4;
                    uVar55 = 0;
                  }
                  else {
                    uVar55 = DAT_140aabae4;
                    if (uVar14 != 3) goto LAB_140620899;
                  }
                  uVar52 = DAT_140aabae4;
                  uVar56 = 0;
                }
LAB_140620899:
                uVar33 = FUN_140249120(DAT_140aabd68,puVar29 + 6);
                dVar34 = pow(DAT_140aabd80,(double)param_2[0x25]);
                uVar50 = uStack_2f4;
                uVar49 = local_2f8;
                fVar44 = (float)((double)CONCAT44((uint)((ulonglong)uVar33 >> 0x20) & _UNK_140aad5e4
                                                  ,(uint)uVar33 & _DAT_140aad5e0) * dVar34 *
                                _DAT_140aab978);
                if (fVar44 <= DAT_140aac234) {
                  fVar44 = DAT_140aac234;
                }
                dVar34 = (DAT_140aac148 / dVar34) * (double)fVar44 *
                         (double)*(int *)((longlong)puVar3 + 0x5c) +
                         (DAT_140aac170 / dVar34) * (double)fVar44;
                uVar57 = (undefined4)((ulonglong)dVar34 >> 0x20);
                uVar58 = SUB84(dVar34,0);
                param_8 = dVar34 * DAT_140aabc48;
                local_378 = local_318;
                dVar46 = (double)CONCAT44(uStack_2f4,local_2f8) * param_8;
                dVar35 = (local_318 - dVar46) - local_370 * param_8;
                if (local_2e0 == 0) {
                  local_1e8._8_4_ = uVar58;
                  local_1e8._0_8_ = dVar34;
                  local_1e8._12_4_ = uVar57;
                  local_1d0 = ((double)local_310 - local_398 * param_8) - dVar46;
                  puVar18 = local_1e8;
                  pdVar28 = &local_1d8;
                  plVar21 = &local_1b8;
                  local_1d8 = dVar35;
                }
                else {
                  local_1c8._8_4_ = uVar58;
                  local_1c8._0_8_ = dVar34;
                  local_1c8._12_4_ = uVar57;
                  local_250 = ((double)local_310 - local_398 * param_8) - dVar46;
                  puVar18 = local_1c8;
                  pdVar28 = &local_258;
                  plVar21 = &local_2c8;
                  local_258 = dVar35;
                }
                local_358 = local_310;
                FUN_14056d7f0(plVar21,CONCAT44(uVar52,uVar51),pdVar28,local_348,puVar18,
                              CONCAT44(uVar52,uVar51),CONCAT44(uVar51,uVar56),
                              CONCAT44(uVar55,uVar52),CONCAT44(uVar56,uVar55),uVar12,0xffffffff);
                if (*(char *)((longlong)(int)uVar13 * 0x38 + lVar25 + 0x34) == '\0') {
                  lVar26 = param_9;
                  if (cVar32 != '\0') {
                    uVar53 = SUB84(param_8,0);
                    uVar54 = (undefined4)((ulonglong)param_8 >> 0x20);
LAB_140620d27:
                    FUN_140556590(local_3a0,local_218,param_1 + 600);
                    lVar26 = param_9;
                    if (local_20c != '\0') {
                      local_1f0 = (double)CONCAT44(uVar50,uVar49) * (double)CONCAT44(uVar54,uVar53);
                      local_208._8_4_ = uVar58;
                      local_208._0_8_ = dVar34;
                      local_208._12_4_ = uVar57;
                      local_1f8 = (local_378 - local_1f0) -
                                  (double)CONCAT44(uVar54,uVar53) * local_370;
                      local_1f0 = ((double)local_358 - local_398 * (double)CONCAT44(uVar54,uVar53))
                                  - local_1f0;
                      FUN_14056d7f0(&local_2c8,local_118,&local_1f8,local_348,local_208,0,
                                    DAT_140aabae4,(ulonglong)DAT_140aabae4 << 0x20,
                                    CONCAT44(DAT_140aabae4,DAT_140aabae4),local_214,0xffffffff);
                      lVar26 = param_9;
                    }
                  }
                }
                else {
                  iVar31 = 0;
                  lVar25 = 0x208;
                  if ((int)puVar3[0xe] != 0) {
                    lVar25 = 0x230;
                  }
                  puVar29 = (ulonglong *)puVar3[0xf];
                  if (puVar29 != (ulonglong *)puVar3[0x10]) {
                    do {
                      lVar26 = 0;
                      puVar22 = *(ulonglong **)
                                 (*(longlong *)(param_6 + 0x10) +
                                 (*puVar29 % (ulonglong)*(uint *)(param_6 + 0x18)) * 8);
                      if (puVar22 != (ulonglong *)0x0) {
                        do {
                          lVar15 = lVar26 + 1;
                          if (*puVar29 != *puVar22) {
                            lVar15 = lVar26;
                          }
                          puVar22 = (ulonglong *)puVar22[0x10];
                          lVar26 = lVar15;
                        } while (puVar22 != (ulonglong *)0x0);
                        if (lVar15 != 0) {
                          iVar31 = iVar31 + 1;
                        }
                      }
                      puVar29 = puVar29 + 1;
                    } while (puVar29 != (ulonglong *)puVar3[0x10]);
                  }
                  FUN_140556590(local_3a0,local_248,lVar25 + param_1);
                  uVar12 = local_244;
                  uVar14 = DAT_140aabae4;
                  if (local_23c == '\0') {
LAB_140620cfa:
                    uVar53 = SUB84(param_8,0);
                    uVar54 = (undefined4)((ulonglong)param_8 >> 0x20);
                  }
                  else {
                    uVar53 = SUB84(param_8,0);
                    uVar54 = (undefined4)((ulonglong)param_8 >> 0x20);
                    if (0 < iVar31) {
                      dVar48 = (double)CONCAT44(uStack_2f4,local_2f8) * param_8;
                      dVar8 = local_370 * _DAT_140aabc78;
                      dVar9 = (double)CONCAT44(uStack_2f4,local_2f8) * _DAT_140aabc78;
                      dVar47 = (double)iVar31 * DAT_140aabc48;
                      dVar35 = local_378 - dVar48;
                      dVar36 = param_8 * local_370;
                      dVar37 = dVar8 * DAT_140aabc48;
                      dVar46 = (double)local_358 - local_398 * param_8;
                      dVar38 = dVar9 * DAT_140aabc48;
                      dVar10 = 0.0;
                      puVar29 = (ulonglong *)puVar3[0x10];
                      puVar22 = (ulonglong *)puVar3[0xf];
                      uVar49 = local_2f8;
                      uVar50 = uStack_2f4;
                      if (puVar22 != puVar29) {
                        do {
                          for (puVar5 = *(ulonglong **)
                                         (*(longlong *)(param_6 + 0x10) +
                                         (*puVar22 % (ulonglong)*(uint *)(param_6 + 0x18)) * 8);
                              puVar5 != (ulonglong *)0x0; puVar5 = (ulonglong *)puVar5[0x10]) {
                            if (*puVar22 == *puVar5) {
                              if (puVar5 != *(ulonglong **)
                                             (*(longlong *)(param_6 + 0x10) +
                                             *(longlong *)(param_6 + 0x18) * 8)) {
                                local_238._8_4_ = uVar58;
                                local_238._0_8_ = dVar34;
                                local_238._12_4_ = uVar57;
                                local_228 = dVar8 * dVar10 +
                                            ((dVar35 - dVar36) - dVar47 * dVar8) + dVar37;
                                local_220 = dVar9 * dVar10 +
                                            ((dVar46 - dVar48) - dVar47 * dVar9) + dVar38;
                                FUN_14056d7f0(&local_2c8,local_128,&local_228,local_348,local_238,0,
                                              uVar14,(ulonglong)uVar14 << 0x20,
                                              CONCAT44(uVar14,uVar14),uVar12,
                                              (uint)puVar5[7] | 0xff000000);
                                dVar10 = dVar10 + DAT_140aabd08;
                              }
                              break;
                            }
                          }
                          puVar22 = puVar22 + 1;
                          uVar49 = local_2f8;
                          uVar50 = uStack_2f4;
                        } while (puVar22 != puVar29);
                        goto LAB_140620cfa;
                      }
                    }
                  }
                  if (((char)local_res20 != '\0') ||
                     ((lVar26 = param_9, iVar31 == 0 && ((int)puVar3[0xe] == 1))))
                  goto LAB_140620d27;
                }
              }
            }
          }
LAB_140620e46:
          local_3a8 = (undefined8 *)local_3a8[2];
          while (local_3a8 == (undefined8 *)0x0) {
            plVar30 = plVar30 + 1;
            local_3a8 = (undefined8 *)*plVar30;
          }
          lVar25 = param_12;
        } while (local_3a8 != puVar2);
      }
      FUN_14056dc60(&local_1b8,param_2);
      FUN_14056dc60(&local_2c8,param_2);
      if (local_188 != (void *)0x0) {
        free(local_188);
      }
      if (local_1a0 != (void *)0x0) {
        free(local_1a0);
      }
      if (local_298 != (void *)0x0) {
        free(local_298);
      }
      if (local_2b0 != (void *)0x0) {
        free(local_2b0);
      }
    }
  }
  FUN_1402516d0(lVar25);
  return;
}


// Incoming references
// 0xc312e0 DATA caller none
// 0x6211a4 UNCONDITIONAL_CALL caller 140620f40
// 0x62142a UNCONDITIONAL_CALL caller 1406211d0
