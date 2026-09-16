// Candidate VA 14044a330; RVA 0x44a330
// Ghidra inferred prototype: undefined FUN_14044a330()

int FUN_14044a330(longlong *param_1,undefined8 *param_2,undefined8 *param_3,longlong param_4,
                 longlong param_5)

{
  undefined4 uVar1;
  ulonglong uVar2;
  undefined8 *puVar3;
  undefined1 auVar4 [16];
  float fVar5;
  double dVar6;
  float fVar7;
  char cVar8;
  int iVar9;
  int iVar10;
  char *pcVar11;
  longlong lVar12;
  longlong lVar13;
  longlong *plVar14;
  longlong lVar15;
  undefined8 uVar16;
  char *pcVar17;
  void *pvVar18;
  ulonglong *puVar19;
  char *pcVar20;
  ulonglong *puVar21;
  ulonglong *puVar22;
  bool bVar23;
  undefined4 uVar24;
  uint uVar25;
  uint uVar26;
  longlong *in_stack_fffffffffffffbb8;
  undefined8 *in_stack_fffffffffffffbc0;
  uint local_428;
  undefined4 local_418;
  undefined4 uStack_414;
  undefined4 uStack_410;
  undefined4 uStack_40c;
  undefined8 local_408;
  undefined8 local_3f8;
  undefined8 local_3f0;
  undefined8 local_3e8;
  undefined8 local_3e0;
  undefined8 local_3d8;
  undefined8 local_3d0;
  longlong local_3c8 [3];
  undefined1 local_3b0 [16];
  undefined8 local_3a0;
  undefined8 uStack_398;
  undefined4 local_388;
  longlong local_378;
  char *local_370;
  char *local_368;
  undefined4 local_360;
  undefined4 uStack_35c;
  undefined8 uStack_358;
  longlong local_350;
  longlong lStack_348;
  undefined4 uStack_340;
  undefined4 uStack_33c;
  ulonglong *local_338;
  undefined1 auStack_330 [16];
  undefined8 local_320;
  undefined8 uStack_318;
  undefined1 local_310 [16];
  undefined8 uStack_300;
  undefined8 uStack_2f8;
  undefined8 local_2f0;
  undefined8 uStack_2e8;
  undefined8 local_2e0;
  undefined8 local_2d8;
  undefined8 uStack_2d0;
  undefined8 local_2c8;
  char local_2c0;
  undefined4 local_2bc;
  undefined2 local_2b8;
  undefined8 local_2b0;
  void *local_2a8;
  undefined8 uStack_2a0;
  undefined8 local_298;
  void *pvStack_290;
  undefined8 local_288;
  undefined8 uStack_280;
  double local_278;
  uint local_270;
  undefined8 local_268;
  undefined8 local_260;
  undefined8 uStack_258;
  undefined8 local_250;
  undefined8 uStack_248;
  undefined8 local_240;
  undefined8 uStack_238;
  undefined8 local_230;
  undefined8 uStack_228;
  undefined8 local_220;
  undefined8 uStack_218;
  undefined8 local_210;
  double dStack_208;
  ulonglong local_200;
  undefined1 *local_1f8;
  undefined1 *local_1f0;
  undefined8 *local_1e8;
  undefined1 *local_1d8;
  undefined1 local_1d0 [64];
  undefined8 local_190;
  undefined8 local_188;
  undefined8 uStack_180;
  undefined8 local_178;
  undefined8 local_170;
  undefined4 local_168;
  longlong local_160;
  undefined8 uStack_158;
  undefined1 local_150;
  undefined1 local_140;
  undefined8 local_138;
  longlong local_130;
  longlong local_128;
  longlong local_120;
  longlong local_118;
  longlong local_110;
  longlong local_108;
  void *local_100 [4];
  void *local_e0;

  fVar7 = DAT_140aac3a8;
  dVar6 = DAT_140aabb00;
  fVar5 = DAT_140aab6f0;
  puVar22 = (ulonglong *)0x0;
  local_428 = 0;
  if (0 < param_5) {
    pcVar11 = (char *)(param_3 + 0xbe);
    pcVar17 = (char *)(param_3 + 0xab);
    puVar19 = puVar22;
    uVar26 = (uint)param_5;
    while( true ) {
      pcVar20 = (char *)(param_3 + 0xab);
      if (4 < (int)puVar19) break;
      if (*(char *)(param_3 + 0x3e) != '\0') {
        if (*(char *)(param_3 + 0xb3) != '\0') {
          if ((void *)param_3[0xaf] != (void *)0x0) {
            free((void *)param_3[0xaf]);
          }
          *(undefined1 *)(param_3 + 0xb3) = 0;
          pcVar11 = (char *)(param_3 + 0xbe);
          pcVar17 = pcVar20;
        }
        *(undefined1 *)(param_3 + 0xba) = 0;
        *pcVar11 = '\0';
        *(undefined1 *)(param_3 + 0x9e) = 0;
        *pcVar17 = '\0';
        *(undefined1 *)(param_3 + 0xa2) = 0;
        *(undefined1 *)(param_3 + 0x9a) = 0;
        FUN_14045f560(param_3 + 0x52);
        *(undefined1 *)(param_3 + 0xc1) = 0;
        *(undefined1 *)((longlong)param_3 + 0x611) = 0;
        *(undefined1 *)(param_3 + 0x51) = 0;
        *(undefined1 *)(param_3 + 0x43) = 0;
        if (*(char *)(param_3 + 0x3a) != '\0') {
          pvVar18 = (void *)param_3[0x1c];
          if ((pvVar18 != (void *)0x0) && (pvVar18 != (void *)param_3[0x20])) {
            free(pvVar18);
          }
          *(undefined1 *)(param_3 + 0x3a) = 0;
        }
      }
      if (((*pcVar20 != '\0') && (*(char *)((longlong)param_3 + 0x54c) != '\0')) &&
         (param_3[0xa3] + 5000000 < *(longlong *)(param_1[3] + 0x28) * 10000)) {
        *pcVar20 = '\0';
      }
      local_368 = pcVar20;
      if ((*(char *)(param_3 + 0xb3) != '\0') &&
         ((longlong)param_3[0xac] <= *(longlong *)(param_1[3] + 0x28) * 10000)) {
        if ((void *)param_3[0xaf] != (void *)0x0) {
          free((void *)param_3[0xaf]);
        }
        *(undefined1 *)(param_3 + 0xb3) = 0;
      }
      local_370 = (char *)(param_3 + 0xbe);
      if (*local_370 != '\0') {
        if (((param_3[0xbb] != 0) &&
            (lVar12 = FUN_14033f710(*(longlong *)param_1[7] + 0x280), lVar12 != 0)) &&
           (*(int *)(lVar12 + 0x70) == 1)) {
          lVar13 = *(longlong *)(lVar12 + 0x1b8);
          for (puVar19 = *(ulonglong **)
                          (lVar13 + ((ulonglong)param_3[0xbc] % (ulonglong)*(uint *)(lVar12 + 0x1c0)
                                    ) * 8); puVar19 != (ulonglong *)0x0;
              puVar19 = (ulonglong *)puVar19[0x3b]) {
            if (param_3[0xbc] == *puVar19) goto LAB_14044a57c;
          }
          puVar19 = *(ulonglong **)(lVar13 + *(longlong *)(lVar12 + 0x1c0) * 8);
LAB_14044a57c:
          puVar21 = puVar19 + 1;
          if (puVar19 == *(ulonglong **)(lVar13 + *(longlong *)(lVar12 + 0x1c0) * 8)) {
            puVar21 = puVar22;
          }
          if ((puVar21 != (ulonglong *)0x0) &&
             (cVar8 = FUN_14043e1f0(param_3,puVar21), cVar8 == '\0')) {
            if (*(char *)(param_3 + 0xba) == '\0') goto LAB_14044b41a;
            goto LAB_14044a5b7;
          }
        }
        if (*(longlong *)(param_4 + 0x38) == 0) goto LAB_14044b4d4;
        puVar3 = (undefined8 *)param_1[0x2a];
        if (puVar3 < (undefined8 *)param_1[0x2b]) {
          param_1[0x2a] = (longlong)(puVar3 + 1);
          *puVar3 = *param_2;
          goto LAB_14044b4d4;
        }
LAB_14044b4cf:
        FUN_140253b10(param_1 + 0x29,param_2);
LAB_14044b4d4:
        *(undefined1 *)(param_3 + 0xba) = 0;
        *(undefined1 *)(param_3 + 0xbe) = 0;
        *(undefined1 *)(param_3 + 0x9a) = 0;
        *(undefined1 *)(param_3 + 0xa2) = 0;
        *(undefined1 *)(param_3 + 0xab) = 0;
        *(undefined1 *)(param_3 + 0x9e) = 0;
        if (*(char *)(param_3 + 0x3a) != '\0') {
          pvVar18 = (void *)param_3[0x1c];
          if ((pvVar18 != (void *)0x0) && (pvVar18 != (void *)param_3[0x20])) {
            free(pvVar18);
          }
          *(undefined1 *)(param_3 + 0x3a) = 0;
        }
        *(undefined1 *)(param_3 + 0x3e) = 0;
        FUN_14045f560(param_3 + 0x52);
        *(undefined1 *)(param_3 + 0xc1) = 0;
        *(undefined1 *)((longlong)param_3 + 0x611) = 0;
        *(undefined1 *)(param_3 + 0x16) = 0;
        *(undefined1 *)(param_3 + 0x51) = 0;
        *(undefined1 *)(param_3 + 0x43) = 0;
        FUN_140458a00(param_3 + 0xc3);
        return 0;
      }
LAB_14044a5b7:
      if (*(int *)(param_2 + 0x17) == 2) {
        if (*(longlong *)(param_4 + 0x38) == 0) goto LAB_14044b4d4;
        puVar3 = (undefined8 *)param_1[0x2a];
        param_2 = param_3;
        if (puVar3 < (undefined8 *)param_1[0x2b]) {
          param_1[0x2a] = (longlong)(puVar3 + 1);
          *puVar3 = *param_3;
          goto LAB_14044b4d4;
        }
        goto LAB_14044b4cf;
      }
      if (*(char *)(param_3 + 0x16) != '\0') {
        return 0;
      }
      if (*(char *)(param_3 + 0x3a) != '\0') {
        if ((param_3[0x17] == 0) ||
           (lVar12 = FUN_14032c420(*(undefined8 *)(param_1[1] + 0x28)), lVar12 == 0)) {
          puVar3 = (undefined8 *)param_1[0x2a];
          if (puVar3 < (undefined8 *)param_1[0x2b]) {
            param_1[0x2a] = (longlong)(puVar3 + 1);
            *puVar3 = *param_3;
            FUN_14043e300(param_3);
            return 0;
          }
          FUN_140253b10(param_1 + 0x29,param_3);
LAB_14044b41a:
          FUN_14043e300(param_3);
          return 0;
        }
        *(undefined1 *)(param_3 + 0x43) = 0;
      }
      if (((*(char *)(param_3 + 0xb3) == '\0') && (*(char *)(param_3 + 0x43) != '\0')) &&
         (*(char *)(param_3 + 0x9a) == '\0')) {
        if (param_1[0x1b] == 0) goto LAB_14044b427;
        param_3[0xac] = (*(longlong *)(param_1[3] + 0x28) + 500) * 10000;
        param_3[0xad] = 0;
        param_3[0xae] = 0;
        param_3[0xaf] = 0;
        param_3[0xb0] = 0;
        param_3[0xb1] = 0;
        param_3[0xb2] = 0;
        *(undefined1 *)(param_3 + 0xb3) = 1;
        local_418 = *(undefined4 *)(param_3 + 0x3f);
        uStack_414 = *(undefined4 *)((longlong)param_3 + 0x1fc);
        uStack_410 = *(undefined4 *)(param_3 + 0x40);
        uStack_40c = *(undefined4 *)((longlong)param_3 + 0x204);
        local_408 = param_3[0x41];
        cVar8 = FUN_140445fe0(param_1,param_2,param_3,&local_418);
        if (cVar8 != '\0') {
          *(undefined1 *)(param_3 + 0x1b) = *(undefined1 *)(param_3 + 0x42);
          *(undefined1 *)(param_3 + 0x43) = 0;
          goto LAB_14044a6d5;
        }
      }
      else {
LAB_14044a6d5:
        if (*(char *)(param_3 + 0x3a) == '\0') {
          if (*(char *)(param_3 + 0x51) == '\0') goto LAB_14044b09e;
          if (param_1[0x1b] == 0) {
LAB_14044b427:
            FUN_140456fd0(param_1 + 0x23,param_2,&param_5);
            return 1;
          }
          local_418 = *(undefined4 *)(param_3 + 0x44);
          uStack_414 = *(undefined4 *)((longlong)param_3 + 0x224);
          uStack_410 = *(undefined4 *)(param_3 + 0x45);
          uStack_40c = *(undefined4 *)((longlong)param_3 + 0x22c);
          local_408 = param_3[0x46];
          cVar8 = FUN_140445fe0(param_1,param_2,param_3,&local_418);
          if (cVar8 == '\0') {
            *(undefined1 *)(param_3 + 0xba) = 0;
            *(undefined1 *)(param_3 + 0xbe) = 0;
            *(undefined1 *)(param_3 + 0x9a) = 0;
            *(undefined1 *)(param_3 + 0xa2) = 0;
            *(undefined1 *)(param_3 + 0xab) = 0;
            *(undefined1 *)(param_3 + 0x9e) = 0;
            if (*(char *)(param_3 + 0x3a) != '\0') {
              pvVar18 = (void *)param_3[0x1c];
              if ((pvVar18 != (void *)0x0) && (pvVar18 != (void *)param_3[0x20])) {
                free(pvVar18);
              }
              *(undefined1 *)(param_3 + 0x3a) = 0;
            }
            *(undefined1 *)(param_3 + 0x3e) = 0;
            FUN_14045f560(param_3 + 0x52);
            *(undefined1 *)(param_3 + 0xc1) = 0;
            *(undefined1 *)((longlong)param_3 + 0x611) = 0;
            *(undefined1 *)(param_3 + 0x16) = 0;
            *(undefined1 *)(param_3 + 0x51) = 0;
            *(undefined1 *)(param_3 + 0x43) = 0;
            FUN_140458a00(param_3 + 0xc3);
          }
          else {
            *(undefined1 *)(param_3 + 0x51) = 0;
            local_360 = 0;
            uStack_358 = 0;
            *(undefined4 *)(param_3 + 0xbf) = 0;
            *(undefined4 *)((longlong)param_3 + 0x5fc) = uStack_35c;
            *(undefined4 *)(param_3 + 0xc0) = 0;
            *(undefined4 *)((longlong)param_3 + 0x604) = 0;
            if (*(char *)(param_3 + 0xc1) == '\0') {
              *(undefined1 *)(param_3 + 0xc1) = 1;
            }
          }
        }
        else if (*(char *)((longlong)param_3 + 0x611) == '\0') {
          if ((*(char *)(param_3 + 0x51) == '\0') || (*(char *)(param_3 + 0x9a) != '\0')) {
            if (*(char *)(param_3 + 0xc1) != '\0') {
              if (*(char *)(param_3 + 0xba) == '\0') {
                *(undefined1 *)(param_3 + 0xc1) = 0;
              }
              else {
                if (param_3 == (undefined8 *)0x0) {
LAB_14044b18b:
                  local_3d0 = 1;
                }
                else {
                  FUN_14043f150(&local_120,param_1[7],param_3);
                  if ((*(char *)(param_3 + 0xbe) == '\0') || ((local_120 != 0 && (local_118 != 0))))
                  {
                    FUN_14043f1d0(local_3c8,param_3);
                    if (((char)local_3a0 == '\0') &&
                       (((local_3c8[0] == 0 ||
                         (*(longlong *)(local_3c8[0] + 0x50) != *(longlong *)(local_3c8[0] + 0x58)))
                        && (local_3c8[2] != 0)))) {
                      local_418 = (undefined4)local_3c8[2];
                      uStack_414 = (undefined4)((ulonglong)local_3c8[2] >> 0x20);
                      uStack_410 = local_3a0._4_4_;
                      uStack_40c = (undefined4)uStack_398;
                      local_408 = CONCAT44(local_408._4_4_,local_388);
                      if (((local_3c8[2] != 0) &&
                          (uVar24 = (undefined4)uStack_398,
                          lVar12 = FUN_14033f7f0(*(longlong *)param_1[6] + 0x180), lVar12 != 0)) &&
                         (lVar12 = FUN_1404225e0(lVar12,uVar24), lVar12 != 0)) {
                        local_418 = *(undefined4 *)(param_3 + 0x17);
                        uStack_414 = *(undefined4 *)((longlong)param_3 + 0xbc);
                        uStack_410 = *(undefined4 *)(param_3 + 0x18);
                        uStack_40c = *(undefined4 *)((longlong)param_3 + 0xc4);
                        local_408._0_1_ = (char)param_3[0x19];
                        local_408._1_7_ = (undefined7)((ulonglong)param_3[0x19] >> 8);
                        local_408 = CONCAT71(local_408._1_7_,-(char)local_408);
                        FUN_140377010(local_100,param_1[5],&local_418);
                        iVar10 = FUN_14036b180(lVar12,local_100);
                        if ((local_100[0] != (void *)0x0) && (local_100[0] != local_e0)) {
                          free(local_100[0]);
                        }
                        if (-1 < iVar10) {
                          FUN_140443200(param_1,param_2,param_3,param_4);
                          goto LAB_14044b09e;
                        }
                      }
                    }
                  }
                  if (((param_3 == (undefined8 *)0x0) || (*(char *)(param_3 + 0x3a) == '\0')) ||
                     (*(char *)(param_3 + 0xba) == '\0')) goto LAB_14044b18b;
                  *(undefined1 *)(param_3 + 0xc1) = 0;
                  FUN_14043f150(&local_110,param_1[7],param_3);
                  uVar24 = (undefined4)((ulonglong)in_stack_fffffffffffffbc0 >> 0x20);
                  if ((*(char *)(param_3 + 0xbe) == '\0') || ((local_110 != 0 && (local_108 != 0))))
                  {
                    FUN_14043f1d0(local_3c8,param_3);
                    in_stack_fffffffffffffbc0 =
                         (undefined8 *)CONCAT44(uVar24,*(undefined4 *)(param_3 + 0xb4));
                    in_stack_fffffffffffffbb8 =
                         (longlong *)((ulonglong)in_stack_fffffffffffffbb8 & 0xffffffffffffff00);
                    FUN_140443ac0(param_1,local_3c8,param_2,param_3,in_stack_fffffffffffffbb8,
                                  in_stack_fffffffffffffbc0,1);
                    if (*(char *)(param_3 + 0x51) != '\0') {
                      param_3[0x4a] = 0;
                    }
                    goto LAB_14044b09e;
                  }
                  local_3d8 = 1;
                }
                param_3[0x15] = 1;
                if (*(char *)(param_3 + 0x16) == '\0') {
                  *(undefined1 *)(param_3 + 0x16) = 1;
                }
              }
            }
LAB_14044b09e:
            if (((*(char *)(param_3 + 0x3e) == '\0') && (*(char *)(param_3 + 0xb3) == '\0')) &&
               (*(char *)(param_3 + 0xba) == '\0')) {
              if (*(char *)(param_3 + 0x9a) == '\0') {
                if (param_1[0x1b] == 0) {
                  FUN_140456fd0(param_1 + 0x23,param_2,&param_5);
                  return 1;
                }
                local_3c8[0] = (*(longlong *)(param_1[3] + 0x28) + 500) * 10000;
                local_3c8[1] = 0;
                local_3c8[2] = 0;
                local_3b0 = ZEXT816(0);
                local_3a0 = 0;
                uStack_398 = 0;
                FUN_14045bfc0(param_3 + 0xac,local_3c8);
                if ((void *)local_3b0._0_8_ != (void *)0x0) {
                  free((void *)local_3b0._0_8_);
                }
                FUN_140444230(param_1,param_2,param_3);
              }
              else {
LAB_14044b2cb:
                in_stack_fffffffffffffbb8 = &param_5;
                FUN_140449e40(param_1,param_2,param_3,param_4,in_stack_fffffffffffffbb8);
              }
            }
            else {
              if (*(char *)(param_3 + 0x9a) != '\0') goto LAB_14044b2cb;
              if (*(char *)(param_3 + 0x3a) == '\0') {
                return 0;
              }
              if (*(char *)(param_3 + 0x96) == '\0') {
                return 0;
              }
              if (param_4 == 0) {
                in_stack_fffffffffffffbb8 = (longlong *)0x0;
              }
              else {
                uVar2 = *(ulonglong *)(param_4 + 0x38);
                if ((longlong)uVar2 < 0) {
                  in_stack_fffffffffffffbb8 = (longlong *)(double)((float)uVar2 * fVar7);
                }
                else {
                  in_stack_fffffffffffffbb8 = (longlong *)(double)((float)(longlong)uVar2 * fVar7);
                }
              }
              in_stack_fffffffffffffbc0 = &param_5;
              iVar10 = FUN_140448710(param_1,param_2,param_3,param_4,in_stack_fffffffffffffbb8,
                                     in_stack_fffffffffffffbc0);
              if (iVar10 != 0) {
                return iVar10;
              }
            }
          }
          else {
            plVar14 = (longlong *)FUN_140450030(param_1[9],local_3c8,param_1[1],*param_2);
            local_378 = param_1[0x1a];
            lVar12 = param_1[3];
            lVar13 = *param_1;
            if (*(char *)(param_3 + 0x3a) == '\0') {
              uVar25 = 0;
              bVar23 = true;
            }
            else {
              auVar4._8_8_ = 0;
              auVar4._0_8_ = local_310._8_8_;
              local_310 = auVar4 << 0x40;
              local_320 = 0;
              uStack_318 = 0;
              local_310._0_2_ = 0x101;
              local_310._8_8_ = 0;
              uStack_300 = 0;
              uStack_2f8 = 0x101;
              local_2f0 = 0;
              uStack_2e8 = 0;
              local_2e0 = 0x101;
              local_2d8 = 0;
              uStack_2d0 = 0;
              local_2c8 = 0x101;
              local_2c0 = '\0';
              local_2bc = 0;
              local_2b8 = 0;
              local_2b0 = 0;
              local_2a8 = (void *)0x0;
              uStack_2a0 = 0;
              pvVar18 = (void *)0x0;
              local_298 = 0;
              pvStack_290 = (void *)0x0;
              local_288 = 0;
              uStack_280 = 0;
              local_278 = 0.0;
              local_270 = 0;
              local_268 = 0;
              local_260 = 0;
              uStack_258 = 0;
              local_250 = 0;
              uStack_248 = 0;
              local_240 = 0xffffffff;
              uStack_238 = 0;
              local_230 = 0;
              uStack_228 = 0x101;
              local_220 = 0;
              uStack_218 = 0;
              local_210 = 0;
              dStack_208 = 0.0;
              local_200 = 0;
              local_1d8 = local_1d0;
              local_1f0 = local_1d0;
              local_1f8 = local_1d0;
              local_1e8 = &local_190;
              local_190 = 0;
              local_188 = 0;
              uStack_180 = 0;
              local_178 = 0;
              local_170 = 0;
              local_168 = 0;
              local_160 = 0;
              uStack_158 = 0;
              local_150 = 0;
              local_140 = 0;
              local_138 = 0;
              local_350 = *plVar14;
              lStack_348 = plVar14[1];
              uStack_340 = (undefined4)plVar14[2];
              uStack_33c = *(undefined4 *)((longlong)plVar14 + 0x14);
              local_338 = (ulonglong *)plVar14[3];
              auStack_330 = *(undefined1 (*) [16])(plVar14 + 4);
              if ((local_350 == 0) ||
                 (lVar15 = FUN_14032c420(*(undefined8 *)(lVar13 + 0x428)), lVar15 == 0)) {
                bVar23 = true;
                uVar25 = 0;
              }
              else {
                if (dVar6 < (double)param_3[0x1a]) {
                  local_338 = puVar22;
                }
                local_320 = param_3[0x44];
                uStack_318 = param_3[0x45];
                local_310 = *(undefined1 (*) [16])(param_3 + 0x46);
                uStack_300 = param_3[0x48];
                uStack_2f8 = param_3[0x49];
                local_2b8 = *(undefined2 *)(param_3 + 0x4d);
                local_178 = CONCAT44(*(undefined4 *)(param_3 + 0x4a),(undefined4)local_178);
                if (*(char *)(param_3 + 0x4e) == '\0') {
                  uVar24 = 0;
                }
                else {
                  uVar24 = *(undefined4 *)((longlong)param_3 + 0x26c);
                }
                local_170 = CONCAT44(uVar24,*(undefined4 *)((longlong)param_3 + 0x254));
                local_168 = *(undefined4 *)(param_3 + 0x4b);
                local_160 = *(longlong *)(lVar12 + 0x28) * 10000;
                if (*(char *)(param_3 + 0x50) != '\0') {
                  local_160 = param_3[0x4f];
                }
                uStack_158 = param_3[0x4c];
                if (*(char *)(param_3 + 0x96) != '\0') {
                  local_188 = param_3[0x8b];
                  uStack_180 = param_3[0x8c];
                  local_138 = param_3[0x95];
                }
                lVar15 = FUN_14044ffe0(lVar12 + 0xa0,lVar13 + 0x400,*param_2);
                puVar19 = (ulonglong *)(lVar15 + 200);
                if (lVar15 == 0) {
                  puVar19 = puVar22;
                }
                in_stack_fffffffffffffbb8 = &local_350;
                FUN_140376940(lVar13 + 0x428,puVar19,param_2,param_3 + 1,in_stack_fffffffffffffbb8);
                uVar25 = local_270;
                if (local_2c0 == '\0') {
                  bVar23 = true;
                  if ((local_1f8 != (undefined1 *)0x0) && (local_1f8 != local_1d8)) {
                    free(local_1f8);
                  }
                  if (pvStack_290 != (void *)0x0) {
                    free(pvStack_290);
                  }
                  if (local_2a8 != (void *)0x0) {
                    free(local_2a8);
                  }
                  goto LAB_14044ac44;
                }
                if (local_178._4_4_ < fVar5) {
                  local_178 = CONCAT44((float)local_278,(undefined4)local_178);
                }
                if ((int)local_170 < 1) {
                  iVar9 = (int)((param_3[0x4c] + *(longlong *)(lVar12 + 0x28) * -10000) / 1000000);
                  iVar10 = 0;
                  if (0 < iVar9) {
                    iVar10 = iVar9;
                  }
                  local_170 = CONCAT44(local_170._4_4_,iVar10);
                }
                local_240 = CONCAT44(local_240._4_4_,0xffffffff);
                uStack_238 = local_2f0;
                local_230 = uStack_2e8;
                uStack_228 = local_2e0;
                local_220 = 0;
                local_200 = local_200 & 0xffffffff00000000;
                uStack_218 = param_3[0x1a];
                local_210 = 0;
                dStack_208 = local_278;
                FUN_14045b070(param_3 + 0x52,&local_350);
                local_418 = *(undefined4 *)(param_3 + 0x61);
                uStack_414 = *(undefined4 *)((longlong)param_3 + 0x30c);
                uStack_410 = *(undefined4 *)(param_3 + 0x62);
                uStack_40c = *(undefined4 *)((longlong)param_3 + 0x314);
                local_408._0_1_ = (char)param_3[99];
                local_408._1_7_ = (undefined7)((ulonglong)param_3[99] >> 8);
                local_408 = CONCAT71(local_408._1_7_,-(char)local_408);
                uVar16 = FUN_140377010(local_100,lVar13 + 0x428,&local_418);
                FUN_14036b140(param_3 + 0x7d,uVar16);
                if ((local_100[0] != (void *)0x0) && (local_100[0] != local_e0)) {
                  free(local_100[0]);
                }
                *(undefined4 *)(param_3 + 0x17) = *(undefined4 *)(param_3 + 0x5e);
                *(undefined4 *)((longlong)param_3 + 0xbc) =
                     *(undefined4 *)((longlong)param_3 + 0x2f4);
                *(undefined4 *)(param_3 + 0x18) = *(undefined4 *)(param_3 + 0x5f);
                *(undefined4 *)((longlong)param_3 + 0xc4) =
                     *(undefined4 *)((longlong)param_3 + 0x2fc);
                param_3[0x19] = param_3[0x60];
                if ((param_3[0x17] == plVar14[3]) && (*(char *)(param_3 + 0x19) == (char)plVar14[5])
                   ) {
                  cVar8 = FUN_1403e1220(param_3 + 1,*(undefined8 *)(lVar13 + 0x408),param_2 + 0x10);
                  if (cVar8 == '\0') {
                    bVar23 = false;
                  }
                  else {
                    bVar23 = *(char *)(param_3 + 0x1b) == '\0';
                  }
                  *(bool *)(param_3 + 0x1b) = bVar23;
                  param_3[0x1d] = param_3[0x1c];
                }
                if ((local_378 != 0) && (param_3[0x8b] == 0)) {
                  in_stack_fffffffffffffbb8 = (longlong *)0x0;
                  FUN_140451c90(local_378,lVar13,param_2,param_3,0);
                }
                bVar23 = false;
                pvVar18 = pvStack_290;
                uVar25 = uVar26;
                if ((local_1f8 != (undefined1 *)0x0) && (local_1f8 != local_1d8)) {
                  free(local_1f8);
                  pvVar18 = pvStack_290;
                }
              }
              if (pvVar18 != (void *)0x0) {
                free(pvVar18);
              }
              if (local_2a8 != (void *)0x0) {
                free(local_2a8);
              }
            }
LAB_14044ac44:
            if (bVar23) {
              param_3[0x15] = (ulonglong)uVar25 << 0x20;
              if (*(char *)(param_3 + 0x16) != '\0') {
                return 0;
              }
              *(undefined1 *)(param_3 + 0x16) = 1;
              return 0;
            }
            *(undefined1 *)(param_3 + 0x51) = 0;
            uVar26 = uVar25;
          }
        }
        else {
          if ((*(char *)(param_3 + 0xba) != '\0') &&
             (cVar8 = *(char *)(param_3 + 0x51), param_3 != (undefined8 *)0x0)) {
            FUN_14043f150(&local_130,param_1[7],param_3);
            lVar12 = local_128;
            if (*(char *)(param_3 + 0xbe) == '\0') {
LAB_14044a770:
              if (param_3[0xb5] != 0) {
                lVar13 = FUN_14033f7f0(*(longlong *)param_1[6] + 0x180);
                uVar24 = (undefined4)((ulonglong)in_stack_fffffffffffffbc0 >> 0x20);
                if (lVar13 != 0) {
                  if (*(int *)(lVar13 + 0xfc) != 0) {
                    uVar1 = *(undefined4 *)(param_3 + 0xb4);
                    iVar9 = FUN_140355790(param_3 + 0xb4,
                                          (*(longlong *)(lVar13 + 0x120) -
                                           *(longlong *)(lVar13 + 0x118) >> 3) * -0x7d05f417d05f417d
                                         );
                    iVar10 = *(int *)(param_3 + 0xb9);
                    FUN_14043f1d0(local_3c8,param_3,lVar12);
                    FUN_14043ef00(local_3c8,param_1[6]);
                    in_stack_fffffffffffffbc0 = (undefined8 *)CONCAT44(uVar24,uVar1);
                    in_stack_fffffffffffffbb8 =
                         (longlong *)
                         CONCAT71((int7)((ulonglong)in_stack_fffffffffffffbb8 >> 8),
                                  iVar9 < iVar10 + 1);
                    FUN_140443ac0(param_1,local_3c8,param_2,param_3,in_stack_fffffffffffffbb8,
                                  in_stack_fffffffffffffbc0,cVar8 == '\0');
                    *(undefined1 *)((longlong)param_3 + 0x611) = 0;
                    goto LAB_14044b374;
                  }
                  local_3e8 = 3;
                  uVar16 = 3;
                  goto LAB_14044a84b;
                }
              }
              local_3e0 = 1;
              uVar16 = 1;
            }
            else if ((local_130 == 0) || (local_128 == 0)) {
              local_3f0 = 1;
              uVar16 = 1;
            }
            else {
              if (*(int *)(local_130 + 0x70) == 1) goto LAB_14044a770;
              local_3f8 = 4;
              uVar16 = 4;
            }
LAB_14044a84b:
            param_3[0x15] = uVar16;
            if (*(char *)(param_3 + 0x16) == '\0') {
              *(undefined1 *)(param_3 + 0x16) = 1;
            }
          }
          *(undefined1 *)((longlong)param_3 + 0x611) = 0;
        }
      }
LAB_14044b374:
      local_428 = local_428 + 1;
      puVar19 = (ulonglong *)(ulonglong)local_428;
      pcVar11 = local_370;
      pcVar17 = local_368;
      if (CONCAT44(param_5._4_4_,(uint)param_5) < 1) {
        return 0;
      }
    }
  }
  return 0;
}


// Incoming references
// 0xc23690 DATA caller none
// 0x44e3cc UNCONDITIONAL_CALL caller 14044d6b0
// 0x44f1ab UNCONDITIONAL_CALL caller 14044ef70
