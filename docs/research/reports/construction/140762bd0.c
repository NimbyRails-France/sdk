
void FUN_140762bd0(undefined8 *param_1,longlong param_2)

{
  longlong *plVar1;
  undefined8 *puVar2;
  ulonglong *puVar3;
  bool bVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined8 *puVar7;
  undefined8 *******pppppppuVar8;
  undefined8 ******ppppppuVar9;
  longlong lVar10;
  undefined8 *******pppppppuVar11;
  undefined8 *******pppppppuVar12;
  undefined8 *puVar13;
  undefined8 uVar14;
  undefined8 *puVar15;
  undefined4 uVar16;
  undefined4 extraout_XMM0_Da;
  undefined4 extraout_XMM0_Da_00;
  undefined4 extraout_XMM0_Da_01;
  undefined4 extraout_XMM0_Da_02;
  undefined4 extraout_XMM0_Da_03;
  undefined4 extraout_XMM0_Da_04;
  undefined4 extraout_XMM0_Da_05;
  undefined4 extraout_XMM0_Da_06;
  undefined4 extraout_XMM0_Da_07;
  undefined1 local_res18 [8];
  undefined1 local_res20 [8];
  undefined8 *****local_448;
  undefined1 *local_440;
  longlong local_438;
  undefined1 *local_430;
  longlong local_428;
  undefined8 ******local_420;
  undefined8 ******local_418;
  undefined8 ******ppppppuStack_410;
  undefined8 *local_408;
  undefined8 uStack_400;
  undefined8 local_3f8;
  undefined8 ******local_3e8;
  undefined8 ******ppppppuStack_3e0;
  undefined8 *local_3d8;
  undefined8 uStack_3d0;
  undefined8 local_3c8;
  undefined8 ******local_3b8;
  undefined8 ******ppppppuStack_3b0;
  undefined8 *local_3a8;
  undefined8 uStack_3a0;
  undefined8 local_398;
  undefined8 ******local_388;
  undefined8 ******ppppppuStack_380;
  undefined8 *local_378;
  undefined8 uStack_370;
  undefined8 local_368;
  undefined8 ******local_358;
  undefined8 ******ppppppuStack_350;
  undefined8 *local_348;
  undefined8 uStack_340;
  undefined8 local_338;
  undefined8 ******local_328;
  undefined8 ******ppppppuStack_320;
  undefined8 *local_318;
  undefined8 uStack_310;
  undefined8 local_308;
  undefined8 ******local_2f8;
  undefined8 ******ppppppuStack_2f0;
  undefined8 *local_2e8;
  undefined8 uStack_2e0;
  undefined8 local_2d8;
  undefined8 ******local_2c8;
  undefined8 ******ppppppuStack_2c0;
  undefined8 *local_2b8;
  undefined8 uStack_2b0;
  undefined8 local_2a8;
  undefined8 ******local_298;
  undefined8 ******ppppppuStack_290;
  undefined8 *local_288;
  undefined8 uStack_280;
  undefined8 local_278;
  undefined8 ******local_268;
  undefined8 ******ppppppuStack_260;
  undefined8 local_258;
  undefined8 uStack_250;
  undefined8 local_248;
  undefined8 ******local_238;
  undefined8 ******ppppppuStack_230;
  undefined8 local_228;
  undefined8 uStack_220;
  undefined8 local_218;
  undefined8 ******local_208;
  undefined8 ******ppppppuStack_200;
  undefined8 local_1f8;
  undefined8 uStack_1f0;
  undefined8 local_1e8;
  undefined8 ******local_1d8;
  undefined8 ******ppppppuStack_1d0;
  undefined8 local_1c8;
  undefined8 uStack_1c0;
  undefined8 local_1b8;
  undefined8 ******local_1a8;
  undefined8 ******ppppppuStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined8 ******local_178;
  undefined8 ******ppppppuStack_170;
  undefined8 local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined8 ******local_148;
  undefined8 ******ppppppuStack_140;
  undefined8 local_138;
  undefined8 uStack_130;
  undefined8 local_128;
  undefined1 *local_118;
  longlong local_110;
  undefined1 *local_108;
  longlong local_100;
  undefined1 **ppuStack_f8;
  undefined1 **local_f0;
  undefined1 *local_e8;
  longlong lStack_e0;
  undefined1 local_d8 [8];
  undefined1 auStack_d0 [8];
  undefined1 local_c8 [8];
  undefined1 local_c0 [8];
  undefined1 local_b8 [8];
  undefined1 auStack_b0 [8];
  undefined1 local_a8 [8];
  undefined1 auStack_a0 [8];
  undefined1 local_98 [16];
  undefined1 local_88 [16];
  undefined1 local_78 [16];
  undefined1 local_68 [16];
  undefined1 local_58 [24];
  
  local_440 = local_res18;
  local_118 = local_res18;
  local_430 = local_res18;
  local_108 = local_res18;
  ppuStack_f8 = &local_440;
  local_f0 = &local_430;
  local_e8 = local_res20;
  puVar2 = (undefined8 *)*param_1;
  local_438 = param_2;
  local_428 = param_2;
  local_110 = param_2;
  local_100 = param_2;
  lStack_e0 = param_2;
  do {
    if (puVar2 == param_1) {
      return;
    }
    puVar13 = puVar2 + 2;
    local_148 = &local_148;
    ppppppuStack_140 = &local_148;
    local_138 = 0;
    uStack_130 = 0;
    local_128 = 0;
    FUN_14032a510(&local_148,puVar13);
    FUN_14032ae90(puVar13,puVar2[4]);
    *puVar13 = puVar13;
    puVar2[3] = puVar13;
    puVar2[4] = 0;
    *(undefined1 *)(puVar2 + 5) = 0;
    puVar2[6] = 0;
    local_420 = ppppppuStack_140;
    if ((undefined8 *******)ppppppuStack_140 != &local_148) {
      do {
        ppppppuVar9 = local_420;
        uVar16 = FUN_140764bc0(&ppuStack_f8,local_420 + 5);
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,ppppppuVar9[4],local_438 + 8);
        puVar7 = puVar13;
        puVar15 = (undefined8 *)puVar2[4];
        while (puVar15 != (undefined8 *)0x0) {
          if ((longlong)puVar15[4] < (longlong)local_448) {
            puVar15 = (undefined8 *)*puVar15;
          }
          else {
            puVar7 = puVar15;
            puVar15 = (undefined8 *)puVar15[1];
          }
        }
        if ((puVar7 == puVar13) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_140337da0(puVar13,local_d8);
          puVar7 = (undefined8 *)*puVar7;
        }
        FUN_14032cab0(puVar7 + 5,ppppppuVar9 + 5);
        FUN_140320030(&local_420);
      } while ((undefined8 *******)local_420 != &local_148);
    }
    FUN_140325df0(&local_148);
    local_268 = &local_268;
    ppppppuStack_260 = &local_268;
    local_258 = 0;
    uStack_250 = 0;
    local_248 = 0;
    puVar13 = puVar2 + 8;
    FUN_14032a510(&local_268,puVar13);
    FUN_14032ae90(puVar13,puVar2[10]);
    *puVar13 = puVar13;
    puVar2[9] = puVar13;
    puVar2[10] = 0;
    *(undefined1 *)(puVar2 + 0xb) = 0;
    puVar2[0xc] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_260;
    if ((undefined8 *******)ppppppuStack_260 != &local_268) {
      do {
        uVar16 = FUN_140764bc0(&ppuStack_f8,pppppppuVar8 + 5);
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],local_438 + 8);
        puVar7 = puVar13;
        puVar15 = (undefined8 *)puVar2[10];
        while (puVar15 != (undefined8 *)0x0) {
          if ((longlong)puVar15[4] < (longlong)local_448) {
            puVar15 = (undefined8 *)*puVar15;
          }
          else {
            puVar7 = puVar15;
            puVar15 = (undefined8 *)puVar15[1];
          }
        }
        if ((puVar7 == puVar13) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_140337da0(puVar13,auStack_d0);
          puVar7 = (undefined8 *)*puVar7;
        }
        FUN_14032cab0(puVar7 + 5,pppppppuVar8 + 5);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
      } while (pppppppuVar8 != &local_268);
    }
    FUN_140325df0(&local_268);
    local_238 = &local_238;
    ppppppuStack_230 = &local_238;
    local_228 = 0;
    uStack_220 = 0;
    local_218 = 0;
    puVar13 = puVar2 + 0xe;
    FUN_14032a510(&local_238,puVar13);
    uVar16 = FUN_1402450d0(puVar13,puVar2[0x10]);
    *puVar13 = puVar13;
    puVar2[0xf] = puVar13;
    puVar2[0x10] = 0;
    *(undefined1 *)(puVar2 + 0x11) = 0;
    puVar2[0x12] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_230;
    if ((undefined8 *******)ppppppuStack_230 != &local_238) {
      do {
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],local_438 + 8);
        FUN_14031c010(puVar13,local_68,&local_448);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        uVar16 = extraout_XMM0_Da;
      } while (pppppppuVar8 != &local_238);
    }
    FUN_14031ffe0(&local_238);
    local_208 = &local_208;
    ppppppuStack_200 = &local_208;
    local_1f8 = 0;
    uStack_1f0 = 0;
    local_1e8 = 0;
    puVar13 = puVar2 + 0x14;
    FUN_14032a510(&local_208,puVar13);
    uVar16 = FUN_1402450d0(puVar13,puVar2[0x16]);
    *puVar13 = puVar13;
    puVar2[0x15] = puVar13;
    puVar2[0x16] = 0;
    *(undefined1 *)(puVar2 + 0x17) = 0;
    puVar2[0x18] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_200;
    if ((undefined8 *******)ppppppuStack_200 != &local_208) {
      do {
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],local_438 + 8);
        FUN_14031c010(puVar13,local_78,&local_448);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        uVar16 = extraout_XMM0_Da_00;
      } while (pppppppuVar8 != &local_208);
    }
    FUN_14031ffe0(&local_208);
    local_1d8 = &local_1d8;
    ppppppuStack_1d0 = &local_1d8;
    local_1c8 = 0;
    uStack_1c0 = 0;
    local_1b8 = 0;
    puVar13 = puVar2 + 0x32;
    FUN_14032a510(&local_1d8,puVar13);
    FUN_14032af50(puVar13,puVar2[0x34]);
    *puVar13 = puVar13;
    puVar2[0x33] = puVar13;
    puVar2[0x34] = 0;
    *(undefined1 *)(puVar2 + 0x35) = 0;
    puVar2[0x36] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_1d0;
    if ((undefined8 *******)ppppppuStack_1d0 != &local_1d8) {
      do {
        ppppppuVar9 = (undefined8 ******)FUN_140764ad0(&local_118,pppppppuVar8[5]);
        pppppppuVar8[5] = ppppppuVar9;
        ppppppuVar9 = (undefined8 ******)FUN_140764a80(&local_440,pppppppuVar8[0x1a]);
        pppppppuVar8[0x1a] = ppppppuVar9;
        local_448 = (undefined8 *****)
                    FUN_140764a30(extraout_XMM0_Da_01,pppppppuVar8[4],param_2 + 0x38);
        puVar7 = puVar13;
        puVar15 = (undefined8 *)puVar2[0x34];
        while (puVar15 != (undefined8 *)0x0) {
          if ((longlong)puVar15[4] < (longlong)local_448) {
            puVar15 = (undefined8 *)*puVar15;
          }
          else {
            puVar7 = puVar15;
            puVar15 = (undefined8 *)puVar15[1];
          }
        }
        if ((puVar7 == puVar13) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_14033e740(puVar13,local_c8);
          puVar7 = (undefined8 *)*puVar7;
        }
        puVar7[5] = pppppppuVar8[5];
        *(undefined4 *)(puVar7 + 6) = *(undefined4 *)(pppppppuVar8 + 6);
        *(undefined4 *)((longlong)puVar7 + 0x34) = *(undefined4 *)((longlong)pppppppuVar8 + 0x34);
        puVar7[7] = pppppppuVar8[7];
        *(undefined4 *)(puVar7 + 8) = *(undefined4 *)(pppppppuVar8 + 8);
        *(undefined4 *)((longlong)puVar7 + 0x44) = *(undefined4 *)((longlong)pppppppuVar8 + 0x44);
        *(undefined4 *)(puVar7 + 9) = *(undefined4 *)(pppppppuVar8 + 9);
        *(undefined1 *)((longlong)puVar7 + 0x4c) = *(undefined1 *)((longlong)pppppppuVar8 + 0x4c);
        ppppppuVar9 = pppppppuVar8[0xb];
        puVar7[10] = pppppppuVar8[10];
        puVar7[0xb] = ppppppuVar9;
        puVar7[0xc] = pppppppuVar8[0xc];
        puVar7[0xd] = pppppppuVar8[0xd];
        *(undefined4 *)(puVar7 + 0xe) = *(undefined4 *)(pppppppuVar8 + 0xe);
        *(undefined4 *)((longlong)puVar7 + 0x74) = *(undefined4 *)((longlong)pppppppuVar8 + 0x74);
        pppppppuVar11 = pppppppuVar8 + 0xf;
        pppppppuVar12 = (undefined8 *******)(puVar7 + 0xf);
        if (*(char *)(pppppppuVar8 + 0x19) == '\0') {
          if (*(char *)(puVar7 + 0x19) != '\0') {
            FUN_140002d30(pppppppuVar12);
            *(undefined1 *)(puVar7 + 0x19) = 0;
          }
        }
        else if (*(char *)(puVar7 + 0x19) == '\0') {
          FUN_1403273e0(pppppppuVar12,pppppppuVar11);
        }
        else {
          if (pppppppuVar12 != pppppppuVar11) {
            if ((undefined8 ******)0xf < pppppppuVar8[0x12]) {
              pppppppuVar11 = (undefined8 *******)*pppppppuVar11;
            }
            FUN_140030630(pppppppuVar12,pppppppuVar11,pppppppuVar8[0x11]);
          }
          *(undefined4 *)(puVar7 + 0x13) = *(undefined4 *)(pppppppuVar8 + 0x13);
          *(undefined4 *)((longlong)puVar7 + 0x9c) = *(undefined4 *)((longlong)pppppppuVar8 + 0x9c);
          *(undefined1 *)(puVar7 + 0x14) = *(undefined1 *)(pppppppuVar8 + 0x14);
          ppppppuVar9 = pppppppuVar8[0x16];
          puVar7[0x15] = pppppppuVar8[0x15];
          puVar7[0x16] = ppppppuVar9;
          *(undefined4 *)(puVar7 + 0x17) = *(undefined4 *)(pppppppuVar8 + 0x17);
          puVar7[0x18] = pppppppuVar8[0x18];
        }
        puVar7[0x1a] = pppppppuVar8[0x1a];
        *(undefined4 *)(puVar7 + 0x1b) = *(undefined4 *)(pppppppuVar8 + 0x1b);
        *(undefined4 *)((longlong)puVar7 + 0xdc) = *(undefined4 *)((longlong)pppppppuVar8 + 0xdc);
        *(undefined4 *)(puVar7 + 0x1c) = *(undefined4 *)(pppppppuVar8 + 0x1c);
        *(undefined4 *)((longlong)puVar7 + 0xe4) = *(undefined4 *)((longlong)pppppppuVar8 + 0xe4);
        *(undefined1 *)(puVar7 + 0x1d) = *(undefined1 *)(pppppppuVar8 + 0x1d);
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0xf4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x1f);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0xfc);
        *(undefined4 *)(puVar7 + 0x1e) = *(undefined4 *)(pppppppuVar8 + 0x1e);
        *(undefined4 *)((longlong)puVar7 + 0xf4) = uVar16;
        *(undefined4 *)(puVar7 + 0x1f) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0xfc) = uVar6;
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x104);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x21);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x10c);
        *(undefined4 *)(puVar7 + 0x20) = *(undefined4 *)(pppppppuVar8 + 0x20);
        *(undefined4 *)((longlong)puVar7 + 0x104) = uVar16;
        *(undefined4 *)(puVar7 + 0x21) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x10c) = uVar6;
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
      } while (pppppppuVar8 != &local_1d8);
    }
    FUN_140325eb0(&local_1d8);
    local_1a8 = &local_1a8;
    ppppppuStack_1a0 = &local_1a8;
    local_198 = 0;
    uStack_190 = 0;
    local_188 = 0;
    puVar13 = puVar2 + 0x38;
    FUN_14032a510(&local_1a8,puVar13);
    FUN_14032af50(puVar13,puVar2[0x3a]);
    *puVar13 = puVar13;
    puVar2[0x39] = puVar13;
    puVar2[0x3a] = 0;
    *(undefined1 *)(puVar2 + 0x3b) = 0;
    puVar2[0x3c] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_1a0;
    if ((undefined8 *******)ppppppuStack_1a0 != &local_1a8) {
      do {
        ppppppuVar9 = (undefined8 ******)FUN_140764ad0(&local_118,pppppppuVar8[5]);
        pppppppuVar8[5] = ppppppuVar9;
        ppppppuVar9 = (undefined8 ******)FUN_140764a80(&local_440,pppppppuVar8[0x1a]);
        pppppppuVar8[0x1a] = ppppppuVar9;
        local_448 = (undefined8 *****)
                    FUN_140764a30(extraout_XMM0_Da_02,pppppppuVar8[4],param_2 + 0x38);
        puVar7 = puVar13;
        puVar15 = (undefined8 *)puVar2[0x3a];
        while (puVar15 != (undefined8 *)0x0) {
          if ((longlong)puVar15[4] < (longlong)local_448) {
            puVar15 = (undefined8 *)*puVar15;
          }
          else {
            puVar7 = puVar15;
            puVar15 = (undefined8 *)puVar15[1];
          }
        }
        if ((puVar7 == puVar13) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_14033e740(puVar13,local_c0);
          puVar7 = (undefined8 *)*puVar7;
        }
        puVar7[5] = pppppppuVar8[5];
        *(undefined4 *)(puVar7 + 6) = *(undefined4 *)(pppppppuVar8 + 6);
        *(undefined4 *)((longlong)puVar7 + 0x34) = *(undefined4 *)((longlong)pppppppuVar8 + 0x34);
        puVar7[7] = pppppppuVar8[7];
        *(undefined4 *)(puVar7 + 8) = *(undefined4 *)(pppppppuVar8 + 8);
        *(undefined4 *)((longlong)puVar7 + 0x44) = *(undefined4 *)((longlong)pppppppuVar8 + 0x44);
        *(undefined4 *)(puVar7 + 9) = *(undefined4 *)(pppppppuVar8 + 9);
        *(undefined1 *)((longlong)puVar7 + 0x4c) = *(undefined1 *)((longlong)pppppppuVar8 + 0x4c);
        ppppppuVar9 = pppppppuVar8[0xb];
        puVar7[10] = pppppppuVar8[10];
        puVar7[0xb] = ppppppuVar9;
        puVar7[0xc] = pppppppuVar8[0xc];
        puVar7[0xd] = pppppppuVar8[0xd];
        *(undefined4 *)(puVar7 + 0xe) = *(undefined4 *)(pppppppuVar8 + 0xe);
        *(undefined4 *)((longlong)puVar7 + 0x74) = *(undefined4 *)((longlong)pppppppuVar8 + 0x74);
        pppppppuVar11 = pppppppuVar8 + 0xf;
        pppppppuVar12 = (undefined8 *******)(puVar7 + 0xf);
        if (*(char *)(pppppppuVar8 + 0x19) == '\0') {
          if (*(char *)(puVar7 + 0x19) != '\0') {
            FUN_140002d30(pppppppuVar12);
            *(undefined1 *)(puVar7 + 0x19) = 0;
          }
        }
        else if (*(char *)(puVar7 + 0x19) == '\0') {
          FUN_1403273e0(pppppppuVar12,pppppppuVar11);
        }
        else {
          if (pppppppuVar12 != pppppppuVar11) {
            if ((undefined8 ******)0xf < pppppppuVar8[0x12]) {
              pppppppuVar11 = (undefined8 *******)*pppppppuVar11;
            }
            FUN_140030630(pppppppuVar12,pppppppuVar11,pppppppuVar8[0x11]);
          }
          *(undefined4 *)(puVar7 + 0x13) = *(undefined4 *)(pppppppuVar8 + 0x13);
          *(undefined4 *)((longlong)puVar7 + 0x9c) = *(undefined4 *)((longlong)pppppppuVar8 + 0x9c);
          *(undefined1 *)(puVar7 + 0x14) = *(undefined1 *)(pppppppuVar8 + 0x14);
          ppppppuVar9 = pppppppuVar8[0x16];
          puVar7[0x15] = pppppppuVar8[0x15];
          puVar7[0x16] = ppppppuVar9;
          *(undefined4 *)(puVar7 + 0x17) = *(undefined4 *)(pppppppuVar8 + 0x17);
          puVar7[0x18] = pppppppuVar8[0x18];
        }
        puVar7[0x1a] = pppppppuVar8[0x1a];
        *(undefined4 *)(puVar7 + 0x1b) = *(undefined4 *)(pppppppuVar8 + 0x1b);
        *(undefined4 *)((longlong)puVar7 + 0xdc) = *(undefined4 *)((longlong)pppppppuVar8 + 0xdc);
        *(undefined4 *)(puVar7 + 0x1c) = *(undefined4 *)(pppppppuVar8 + 0x1c);
        *(undefined4 *)((longlong)puVar7 + 0xe4) = *(undefined4 *)((longlong)pppppppuVar8 + 0xe4);
        *(undefined1 *)(puVar7 + 0x1d) = *(undefined1 *)(pppppppuVar8 + 0x1d);
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0xf4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x1f);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0xfc);
        *(undefined4 *)(puVar7 + 0x1e) = *(undefined4 *)(pppppppuVar8 + 0x1e);
        *(undefined4 *)((longlong)puVar7 + 0xf4) = uVar16;
        *(undefined4 *)(puVar7 + 0x1f) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0xfc) = uVar6;
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x104);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x21);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x10c);
        *(undefined4 *)(puVar7 + 0x20) = *(undefined4 *)(pppppppuVar8 + 0x20);
        *(undefined4 *)((longlong)puVar7 + 0x104) = uVar16;
        *(undefined4 *)(puVar7 + 0x21) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x10c) = uVar6;
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
      } while (pppppppuVar8 != &local_1a8);
    }
    FUN_140325eb0(&local_1a8);
    local_298 = &local_298;
    ppppppuStack_290 = &local_298;
    local_288 = (undefined8 *)0x0;
    uStack_280 = 0;
    local_278 = 0;
    puVar13 = puVar2 + 0x3e;
    FUN_14032a510(&local_298,puVar13);
    uVar16 = FUN_1402450d0(puVar13,puVar2[0x40]);
    *puVar13 = puVar13;
    puVar2[0x3f] = puVar13;
    puVar2[0x40] = 0;
    *(undefined1 *)(puVar2 + 0x41) = 0;
    puVar2[0x42] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_290;
    puVar15 = local_288;
    if ((undefined8 *******)ppppppuStack_290 != &local_298) {
      do {
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],param_2 + 0x38);
        FUN_14031c010(puVar13,local_88,&local_448);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        uVar16 = extraout_XMM0_Da_03;
        puVar15 = local_288;
      } while (pppppppuVar8 != &local_298);
    }
    while (puVar15 != (undefined8 *)0x0) {
      FUN_1402450d0(&local_298,*puVar15);
      puVar13 = (undefined8 *)puVar15[1];
      free(puVar15);
      puVar15 = puVar13;
    }
    local_2c8 = &local_2c8;
    ppppppuStack_2c0 = &local_2c8;
    local_2b8 = (undefined8 *)0x0;
    uStack_2b0 = 0;
    local_2a8 = 0;
    puVar13 = puVar2 + 0x44;
    FUN_14032a510(&local_2c8,puVar13);
    puVar15 = (undefined8 *)puVar2[0x46];
    pppppppuVar8 = (undefined8 *******)ppppppuStack_2c0;
    while (ppppppuStack_2c0 = pppppppuVar8, puVar15 != (undefined8 *)0x0) {
      FUN_1402450d0(puVar13,*puVar15);
      puVar7 = (undefined8 *)puVar15[1];
      free(puVar15);
      puVar15 = puVar7;
      pppppppuVar8 = (undefined8 *******)ppppppuStack_2c0;
    }
    *puVar13 = puVar13;
    puVar2[0x45] = puVar13;
    puVar2[0x46] = 0;
    *(undefined1 *)(puVar2 + 0x47) = 0;
    puVar2[0x48] = 0;
    puVar13 = local_2b8;
    if (pppppppuVar8 != &local_2c8) {
      do {
        local_448 = (undefined8 *****)FUN_140764a30();
        FUN_14031c010(puVar2 + 0x44,local_98,&local_448);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        puVar13 = local_2b8;
      } while (pppppppuVar8 != &local_2c8);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_1402450d0(&local_2c8,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      free(puVar13);
      puVar13 = puVar15;
    }
    local_178 = &local_178;
    ppppppuStack_170 = &local_178;
    local_168 = 0;
    uStack_160 = 0;
    local_158 = 0;
    puVar13 = puVar2 + 0x1a;
    FUN_14032a510(&local_178,puVar13);
    FUN_14032aef0(puVar13,puVar2[0x1c]);
    *puVar13 = puVar13;
    puVar2[0x1b] = puVar13;
    puVar2[0x1c] = 0;
    *(undefined1 *)(puVar2 + 0x1d) = 0;
    puVar2[0x1e] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_170;
    if ((undefined8 *******)ppppppuStack_170 != &local_178) {
      do {
        ppppppuVar9 = (undefined8 ******)FUN_140764b20(&local_430,pppppppuVar8[5]);
        pppppppuVar8[5] = ppppppuVar9;
        uVar16 = FUN_1407652d0(extraout_XMM0_Da_04,pppppppuVar8 + 0x11,param_2 + 8);
        uVar16 = FUN_1407654d0(uVar16,pppppppuVar8 + 0x56,param_2 + 0x68);
        uVar16 = FUN_1407650e0(uVar16,pppppppuVar8 + 0x6b,param_2 + 0x98);
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],local_428 + 0x68);
        puVar7 = puVar13;
        puVar15 = (undefined8 *)puVar2[0x1c];
        while (puVar15 != (undefined8 *)0x0) {
          if ((longlong)puVar15[4] < (longlong)local_448) {
            puVar15 = (undefined8 *)*puVar15;
          }
          else {
            puVar7 = puVar15;
            puVar15 = (undefined8 *)puVar15[1];
          }
        }
        if ((puVar7 == puVar13) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_14033e4b0(puVar13,local_b8);
          puVar7 = (undefined8 *)*puVar7;
        }
        puVar7[5] = pppppppuVar8[5];
        *(undefined4 *)(puVar7 + 6) = *(undefined4 *)(pppppppuVar8 + 6);
        *(undefined1 *)((longlong)puVar7 + 0x34) = *(undefined1 *)((longlong)pppppppuVar8 + 0x34);
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3c);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 8);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x44);
        *(undefined4 *)(puVar7 + 7) = *(undefined4 *)(pppppppuVar8 + 7);
        *(undefined4 *)((longlong)puVar7 + 0x3c) = uVar16;
        *(undefined4 *)(puVar7 + 8) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x44) = uVar6;
        pppppppuVar11 = pppppppuVar8 + 9;
        if ((undefined8 *******)(puVar7 + 9) != pppppppuVar11) {
          if ((undefined8 ******)0xf < pppppppuVar8[0xc]) {
            pppppppuVar11 = (undefined8 *******)*pppppppuVar11;
          }
          FUN_140030630(puVar7 + 9,pppppppuVar11,pppppppuVar8[0xb]);
        }
        *(undefined1 *)(puVar7 + 0xd) = *(undefined1 *)(pppppppuVar8 + 0xd);
        *(undefined4 *)((longlong)puVar7 + 0x6c) = *(undefined4 *)((longlong)pppppppuVar8 + 0x6c);
        if ((undefined8 *******)(puVar7 + 0xe) != pppppppuVar8 + 0xe) {
          FUN_14032fe20(puVar7 + 0xe,pppppppuVar8[0xe],pppppppuVar8[0xf]);
        }
        pppppppuVar11 = (undefined8 *******)(puVar7 + 0x11);
        if (pppppppuVar11 != pppppppuVar8 + 0x11) {
          puVar7[0x12] = *pppppppuVar11;
          FUN_14032ff40(pppppppuVar11,pppppppuVar8[0x11],pppppppuVar8[0x12]);
        }
        pppppppuVar11 = (undefined8 *******)(puVar7 + 0x56);
        if (pppppppuVar11 != pppppppuVar8 + 0x56) {
          puVar7[0x57] = *pppppppuVar11;
          FUN_14032ff40(pppppppuVar11,pppppppuVar8[0x56],pppppppuVar8[0x57]);
        }
        pppppppuVar11 = (undefined8 *******)(puVar7 + 0x6b);
        if (pppppppuVar11 != pppppppuVar8 + 0x6b) {
          puVar7[0x6c] = *pppppppuVar11;
          FUN_14032ff40(pppppppuVar11,pppppppuVar8[0x6b],pppppppuVar8[0x6c]);
        }
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3a4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x75);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3ac);
        *(undefined4 *)(puVar7 + 0x74) = *(undefined4 *)(pppppppuVar8 + 0x74);
        *(undefined4 *)((longlong)puVar7 + 0x3a4) = uVar16;
        *(undefined4 *)(puVar7 + 0x75) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x3ac) = uVar6;
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3b4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x77);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3bc);
        *(undefined4 *)(puVar7 + 0x76) = *(undefined4 *)(pppppppuVar8 + 0x76);
        *(undefined4 *)((longlong)puVar7 + 0x3b4) = uVar16;
        *(undefined4 *)(puVar7 + 0x77) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x3bc) = uVar6;
        *(undefined4 *)(puVar7 + 0x78) = *(undefined4 *)(pppppppuVar8 + 0x78);
        *(undefined4 *)((longlong)puVar7 + 0x3c4) = *(undefined4 *)((longlong)pppppppuVar8 + 0x3c4);
        *(undefined4 *)(puVar7 + 0x79) = *(undefined4 *)(pppppppuVar8 + 0x79);
        *(undefined1 *)((longlong)puVar7 + 0x3cc) = *(undefined1 *)((longlong)pppppppuVar8 + 0x3cc);
        *(undefined4 *)(puVar7 + 0x7a) = *(undefined4 *)(pppppppuVar8 + 0x7a);
        FUN_140339240(puVar7 + 0x7b,pppppppuVar8 + 0x7b);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
      } while (pppppppuVar8 != &local_178);
    }
    FUN_140325e50(&local_178);
    local_2f8 = &local_2f8;
    ppppppuStack_2f0 = &local_2f8;
    local_2e8 = (undefined8 *)0x0;
    uStack_2e0 = 0;
    local_2d8 = 0;
    puVar13 = puVar2 + 0x20;
    FUN_14032a510(&local_2f8,puVar13);
    FUN_14032aef0(puVar13,puVar2[0x22]);
    *puVar13 = puVar13;
    puVar2[0x21] = puVar13;
    puVar2[0x22] = 0;
    *(undefined1 *)(puVar2 + 0x23) = 0;
    puVar2[0x24] = 0;
    pppppppuVar8 = (undefined8 *******)ppppppuStack_2f0;
    puVar15 = local_2e8;
    if ((undefined8 *******)ppppppuStack_2f0 != &local_2f8) {
      do {
        ppppppuVar9 = (undefined8 ******)FUN_140764b20(&local_430,pppppppuVar8[5]);
        pppppppuVar8[5] = ppppppuVar9;
        uVar16 = FUN_1407652d0(extraout_XMM0_Da_05,pppppppuVar8 + 0x11,param_2 + 8);
        uVar16 = FUN_1407654d0(uVar16,pppppppuVar8 + 0x56,param_2 + 0x68);
        uVar16 = FUN_1407650e0(uVar16,pppppppuVar8 + 0x6b,param_2 + 0x98);
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],local_428 + 0x68);
        puVar7 = puVar13;
        puVar15 = (undefined8 *)puVar2[0x22];
        while (puVar15 != (undefined8 *)0x0) {
          if ((longlong)puVar15[4] < (longlong)local_448) {
            puVar15 = (undefined8 *)*puVar15;
          }
          else {
            puVar7 = puVar15;
            puVar15 = (undefined8 *)puVar15[1];
          }
        }
        if ((puVar7 == puVar13) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_14033e4b0(puVar13,auStack_a0);
          puVar7 = (undefined8 *)*puVar7;
        }
        puVar7[5] = pppppppuVar8[5];
        *(undefined4 *)(puVar7 + 6) = *(undefined4 *)(pppppppuVar8 + 6);
        *(undefined1 *)((longlong)puVar7 + 0x34) = *(undefined1 *)((longlong)pppppppuVar8 + 0x34);
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3c);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 8);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x44);
        *(undefined4 *)(puVar7 + 7) = *(undefined4 *)(pppppppuVar8 + 7);
        *(undefined4 *)((longlong)puVar7 + 0x3c) = uVar16;
        *(undefined4 *)(puVar7 + 8) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x44) = uVar6;
        pppppppuVar11 = pppppppuVar8 + 9;
        if ((undefined8 *******)(puVar7 + 9) != pppppppuVar11) {
          if ((undefined8 ******)0xf < pppppppuVar8[0xc]) {
            pppppppuVar11 = (undefined8 *******)*pppppppuVar11;
          }
          FUN_140030630(puVar7 + 9,pppppppuVar11,pppppppuVar8[0xb]);
        }
        *(undefined1 *)(puVar7 + 0xd) = *(undefined1 *)(pppppppuVar8 + 0xd);
        *(undefined4 *)((longlong)puVar7 + 0x6c) = *(undefined4 *)((longlong)pppppppuVar8 + 0x6c);
        if ((undefined8 *******)(puVar7 + 0xe) != pppppppuVar8 + 0xe) {
          FUN_14032fe20(puVar7 + 0xe,pppppppuVar8[0xe],pppppppuVar8[0xf]);
        }
        pppppppuVar11 = (undefined8 *******)(puVar7 + 0x11);
        if (pppppppuVar11 != pppppppuVar8 + 0x11) {
          puVar7[0x12] = *pppppppuVar11;
          FUN_14032ff40(pppppppuVar11,pppppppuVar8[0x11],pppppppuVar8[0x12]);
        }
        pppppppuVar11 = (undefined8 *******)(puVar7 + 0x56);
        if (pppppppuVar11 != pppppppuVar8 + 0x56) {
          puVar7[0x57] = *pppppppuVar11;
          FUN_14032ff40(pppppppuVar11,pppppppuVar8[0x56],pppppppuVar8[0x57]);
        }
        pppppppuVar11 = (undefined8 *******)(puVar7 + 0x6b);
        if (pppppppuVar11 != pppppppuVar8 + 0x6b) {
          puVar7[0x6c] = *pppppppuVar11;
          FUN_14032ff40(pppppppuVar11,pppppppuVar8[0x6b],pppppppuVar8[0x6c]);
        }
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3a4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x75);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3ac);
        *(undefined4 *)(puVar7 + 0x74) = *(undefined4 *)(pppppppuVar8 + 0x74);
        *(undefined4 *)((longlong)puVar7 + 0x3a4) = uVar16;
        *(undefined4 *)(puVar7 + 0x75) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x3ac) = uVar6;
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3b4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x77);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x3bc);
        *(undefined4 *)(puVar7 + 0x76) = *(undefined4 *)(pppppppuVar8 + 0x76);
        *(undefined4 *)((longlong)puVar7 + 0x3b4) = uVar16;
        *(undefined4 *)(puVar7 + 0x77) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x3bc) = uVar6;
        *(undefined4 *)(puVar7 + 0x78) = *(undefined4 *)(pppppppuVar8 + 0x78);
        *(undefined4 *)((longlong)puVar7 + 0x3c4) = *(undefined4 *)((longlong)pppppppuVar8 + 0x3c4);
        *(undefined4 *)(puVar7 + 0x79) = *(undefined4 *)(pppppppuVar8 + 0x79);
        *(undefined1 *)((longlong)puVar7 + 0x3cc) = *(undefined1 *)((longlong)pppppppuVar8 + 0x3cc);
        *(undefined4 *)(puVar7 + 0x7a) = *(undefined4 *)(pppppppuVar8 + 0x7a);
        FUN_140339240(puVar7 + 0x7b,pppppppuVar8 + 0x7b);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        puVar15 = local_2e8;
      } while (pppppppuVar8 != &local_2f8);
    }
    while (puVar15 != (undefined8 *)0x0) {
      FUN_14032aef0(&local_2f8,*puVar15);
      puVar13 = (undefined8 *)puVar15[1];
      FUN_14032c9f0(puVar15 + 5);
      free(puVar15);
      puVar15 = puVar13;
    }
    plVar1 = puVar2 + 0x26;
    uVar16 = 0;
    local_358 = &local_358;
    pppppppuVar8 = &local_358;
    ppppppuStack_350 = pppppppuVar8;
    local_348 = (undefined8 *)0x0;
    uStack_340 = 0;
    local_338 = puVar2[0x2a];
    puVar2[0x2a] = 0;
    puVar13 = (undefined8 *)puVar2[0x28];
    if (puVar13 != (undefined8 *)0x0) {
      local_358 = (undefined8 *******)*plVar1;
      pppppppuVar8 = (undefined8 *******)puVar2[0x27];
      ppppppuStack_350 = pppppppuVar8;
      local_348 = puVar13;
      puVar13[2] = &local_358;
      *plVar1 = (longlong)plVar1;
      puVar2[0x27] = plVar1;
      puVar2[0x28] = 0;
    }
    *plVar1 = (longlong)plVar1;
    puVar2[0x27] = plVar1;
    puVar2[0x28] = 0;
    *(undefined1 *)(puVar2 + 0x29) = 0;
    puVar2[0x2a] = 0;
    ppppppuStack_350 = pppppppuVar8;
    puVar13 = local_348;
    if (pppppppuVar8 != &local_358) {
      do {
        local_448 = (undefined8 *****)FUN_140764a30(uVar16,pppppppuVar8[4],local_428 + 0x68);
        FUN_14031c010(puVar2 + 0x26,local_58,&local_448);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        uVar16 = extraout_XMM0_Da_06;
        puVar13 = local_348;
      } while (pppppppuVar8 != &local_358);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_1402450d0(&local_358,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      free(puVar13);
      puVar13 = puVar15;
    }
    plVar1 = puVar2 + 0x2c;
    local_388 = &local_388;
    pppppppuVar8 = &local_388;
    ppppppuStack_380 = pppppppuVar8;
    local_378 = (undefined8 *)0x0;
    uStack_370 = 0;
    local_368 = puVar2[0x30];
    puVar2[0x30] = 0;
    puVar13 = (undefined8 *)puVar2[0x2e];
    if (puVar13 != (undefined8 *)0x0) {
      local_388 = (undefined8 *******)*plVar1;
      pppppppuVar8 = (undefined8 *******)puVar2[0x2d];
      ppppppuStack_380 = pppppppuVar8;
      local_378 = puVar13;
      puVar13[2] = &local_388;
      *plVar1 = (longlong)plVar1;
      puVar2[0x2d] = plVar1;
      puVar2[0x2e] = 0;
    }
    *plVar1 = (longlong)plVar1;
    puVar2[0x2d] = plVar1;
    puVar2[0x2e] = 0;
    *(undefined1 *)(puVar2 + 0x2f) = 0;
    puVar2[0x30] = 0;
    ppppppuStack_380 = pppppppuVar8;
    puVar13 = local_378;
    if (pppppppuVar8 != &local_388) {
      puVar15 = puVar2 + 0x2c;
      do {
        ppppppuVar9 = pppppppuVar8[4];
        for (puVar3 = *(ulonglong **)
                       (*(longlong *)(local_428 + 0x70) +
                       ((ulonglong)ppppppuVar9 % (ulonglong)*(uint *)(local_428 + 0x78)) * 8);
            puVar3 != (ulonglong *)0x0; puVar3 = (ulonglong *)puVar3[2]) {
          if (ppppppuVar9 == (undefined8 ******)*puVar3) {
            if ((puVar3 != (ulonglong *)0x0) &&
               (puVar3 != *(ulonglong **)
                           (*(longlong *)(local_428 + 0x70) + *(longlong *)(local_428 + 0x78) * 8)))
            {
              ppppppuVar9 = (undefined8 ******)puVar3[1];
            }
            break;
          }
        }
        bVar4 = true;
        puVar13 = puVar15;
        puVar7 = (undefined8 *)puVar2[0x2e];
        while (puVar7 != (undefined8 *)0x0) {
          bVar4 = (longlong)ppppppuVar9 < (longlong)puVar7[4];
          puVar13 = puVar7;
          if ((longlong)ppppppuVar9 < (longlong)puVar7[4]) {
            puVar7 = (undefined8 *)puVar7[1];
          }
          else {
            puVar7 = (undefined8 *)*puVar7;
          }
        }
        puVar7 = puVar13;
        if (bVar4) {
          if (puVar13 != (undefined8 *)puVar2[0x2d]) {
            puVar7 = (undefined8 *)FUN_14001dcf0(puVar13);
            goto LAB_140764002;
          }
LAB_14076400b:
          lVar10 = thunk_FUN_140983da8(0x28);
          *(undefined8 *******)(lVar10 + 0x20) = ppppppuVar9;
          if ((puVar13 == puVar15) || (uVar14 = 1, (longlong)ppppppuVar9 < (longlong)puVar13[4])) {
            uVar14 = 0;
          }
          FUN_14001de10(lVar10,puVar13,puVar15,uVar14);
          puVar2[0x30] = puVar2[0x30] + 1;
        }
        else {
LAB_140764002:
          if ((longlong)puVar7[4] < (longlong)ppppppuVar9) goto LAB_14076400b;
        }
        pppppppuVar11 = (undefined8 *******)*pppppppuVar8;
        if (pppppppuVar11 == (undefined8 *******)0x0) {
          pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
          pppppppuVar12 = (undefined8 *******)0x0;
          if (pppppppuVar8 == (undefined8 *******)*pppppppuVar11) {
            do {
              pppppppuVar8 = pppppppuVar11;
              pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
            } while (pppppppuVar8 == (undefined8 *******)*pppppppuVar11);
            pppppppuVar12 = (undefined8 *******)*pppppppuVar8;
          }
          if (pppppppuVar12 != pppppppuVar11) {
            pppppppuVar8 = pppppppuVar11;
          }
        }
        else {
          for (pppppppuVar12 = (undefined8 *******)pppppppuVar11[1]; pppppppuVar8 = pppppppuVar11,
              pppppppuVar12 != (undefined8 *******)0x0;
              pppppppuVar12 = (undefined8 *******)pppppppuVar12[1]) {
            pppppppuVar11 = pppppppuVar12;
          }
        }
        puVar13 = local_378;
      } while (pppppppuVar8 != &local_388);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_1402450d0(&local_388,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      free(puVar13);
      puVar13 = puVar15;
    }
    plVar1 = puVar2 + 0xaa;
    local_3b8 = &local_3b8;
    pppppppuVar8 = &local_3b8;
    ppppppuStack_3b0 = pppppppuVar8;
    local_3a8 = (undefined8 *)0x0;
    uStack_3a0 = 0;
    local_398 = puVar2[0xae];
    puVar2[0xae] = 0;
    puVar13 = (undefined8 *)puVar2[0xac];
    if (puVar13 != (undefined8 *)0x0) {
      local_3b8 = (undefined8 *******)*plVar1;
      pppppppuVar8 = (undefined8 *******)puVar2[0xab];
      ppppppuStack_3b0 = pppppppuVar8;
      local_3a8 = puVar13;
      puVar13[2] = &local_3b8;
      *plVar1 = (longlong)plVar1;
      puVar2[0xab] = plVar1;
      puVar2[0xac] = 0;
    }
    *plVar1 = (longlong)plVar1;
    puVar2[0xab] = plVar1;
    puVar2[0xac] = 0;
    *(undefined1 *)(puVar2 + 0xad) = 0;
    puVar2[0xae] = 0;
    ppppppuStack_3b0 = pppppppuVar8;
    puVar13 = local_3a8;
    if (pppppppuVar8 != &local_3b8) {
      puVar15 = puVar2 + 0xaa;
      do {
        ppppppuVar9 = (undefined8 ******)FUN_140764b70(&local_108,pppppppuVar8[5]);
        pppppppuVar8[5] = ppppppuVar9;
        ppppppuVar9 = (undefined8 ******)FUN_140764a80(&local_440);
        pppppppuVar8[0xd] = ppppppuVar9;
        local_448 = pppppppuVar8[4];
        for (puVar3 = *(ulonglong **)
                       (*(longlong *)(param_2 + 0xa0) +
                       ((ulonglong)local_448 % (ulonglong)*(uint *)(param_2 + 0xa8)) * 8);
            puVar3 != (ulonglong *)0x0; puVar3 = (ulonglong *)puVar3[2]) {
          if ((undefined8 ******)local_448 == (undefined8 ******)*puVar3) {
            if ((puVar3 != (ulonglong *)0x0) &&
               (puVar3 != *(ulonglong **)
                           (*(longlong *)(param_2 + 0xa0) + *(longlong *)(param_2 + 0xa8) * 8))) {
              local_448 = (undefined8 *****)puVar3[1];
            }
            break;
          }
        }
        puVar7 = puVar15;
        puVar13 = (undefined8 *)puVar2[0xac];
        while (puVar13 != (undefined8 *)0x0) {
          if ((longlong)puVar13[4] < (longlong)local_448) {
            puVar13 = (undefined8 *)*puVar13;
          }
          else {
            puVar7 = puVar13;
            puVar13 = (undefined8 *)puVar13[1];
          }
        }
        if ((puVar7 == puVar15) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_14033f410(puVar15,local_a8,local_448,puVar7,&local_448);
          puVar7 = (undefined8 *)*puVar7;
        }
        puVar7[5] = pppppppuVar8[5];
        *(undefined1 *)(puVar7 + 6) = *(undefined1 *)(pppppppuVar8 + 6);
        pppppppuVar11 = pppppppuVar8 + 7;
        if ((undefined8 *******)(puVar7 + 7) != pppppppuVar11) {
          if ((undefined8 ******)0xf < pppppppuVar8[10]) {
            pppppppuVar11 = (undefined8 *******)*pppppppuVar11;
          }
          FUN_140030630(puVar7 + 7,pppppppuVar11,pppppppuVar8[9]);
        }
        *(undefined4 *)(puVar7 + 0xb) = *(undefined4 *)(pppppppuVar8 + 0xb);
        puVar7[0xc] = pppppppuVar8[0xc];
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x6c);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0xe);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x74);
        *(undefined4 *)(puVar7 + 0xd) = *(undefined4 *)(pppppppuVar8 + 0xd);
        *(undefined4 *)((longlong)puVar7 + 0x6c) = uVar16;
        *(undefined4 *)(puVar7 + 0xe) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x74) = uVar6;
        puVar7[0xf] = pppppppuVar8[0xf];
        *(undefined4 *)(puVar7 + 0x10) = *(undefined4 *)(pppppppuVar8 + 0x10);
        *(undefined4 *)((longlong)puVar7 + 0x84) = *(undefined4 *)((longlong)pppppppuVar8 + 0x84);
        *(undefined4 *)(puVar7 + 0x11) = *(undefined4 *)(pppppppuVar8 + 0x11);
        *(undefined1 *)((longlong)puVar7 + 0x8c) = *(undefined1 *)((longlong)pppppppuVar8 + 0x8c);
        *(undefined4 *)(puVar7 + 0x12) = *(undefined4 *)(pppppppuVar8 + 0x12);
        *(undefined1 *)((longlong)puVar7 + 0x94) = *(undefined1 *)((longlong)pppppppuVar8 + 0x94);
        *(undefined1 *)((longlong)puVar7 + 0x95) = *(undefined1 *)((longlong)pppppppuVar8 + 0x95);
        *(undefined4 *)(puVar7 + 0x13) = *(undefined4 *)(pppppppuVar8 + 0x13);
        if ((undefined8 *******)(puVar7 + 0x14) != pppppppuVar8 + 0x14) {
          FUN_14032fe20(puVar7 + 0x14,pppppppuVar8[0x14],pppppppuVar8[0x15]);
        }
        if ((undefined8 *******)(puVar7 + 0x17) != pppppppuVar8 + 0x17) {
          FUN_1403416a0(puVar7 + 0x17,pppppppuVar8[0x17],pppppppuVar8[0x18]);
        }
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0xd4);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0x1b);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0xdc);
        *(undefined4 *)(puVar7 + 0x1a) = *(undefined4 *)(pppppppuVar8 + 0x1a);
        *(undefined4 *)((longlong)puVar7 + 0xd4) = uVar16;
        *(undefined4 *)(puVar7 + 0x1b) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0xdc) = uVar6;
        ppppppuVar9 = pppppppuVar8[0x1d];
        puVar7[0x1c] = pppppppuVar8[0x1c];
        puVar7[0x1d] = ppppppuVar9;
        pppppppuVar11 = (undefined8 *******)*pppppppuVar8;
        if (pppppppuVar11 == (undefined8 *******)0x0) {
          pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
          pppppppuVar12 = (undefined8 *******)0x0;
          if (pppppppuVar8 == (undefined8 *******)*pppppppuVar11) {
            do {
              pppppppuVar8 = pppppppuVar11;
              pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
            } while (pppppppuVar8 == (undefined8 *******)*pppppppuVar11);
            pppppppuVar12 = (undefined8 *******)*pppppppuVar8;
          }
          if (pppppppuVar12 != pppppppuVar11) {
            pppppppuVar8 = pppppppuVar11;
          }
        }
        else {
          for (pppppppuVar12 = (undefined8 *******)pppppppuVar11[1]; pppppppuVar8 = pppppppuVar11,
              pppppppuVar12 != (undefined8 *******)0x0;
              pppppppuVar12 = (undefined8 *******)pppppppuVar12[1]) {
            pppppppuVar11 = pppppppuVar12;
          }
        }
        puVar13 = local_3a8;
      } while (pppppppuVar8 != &local_3b8);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_14032b140(&local_3b8,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      FUN_1402f79c0(puVar13 + 5);
      free(puVar13);
      puVar13 = puVar15;
    }
    plVar1 = puVar2 + 0xb0;
    local_3e8 = &local_3e8;
    pppppppuVar8 = &local_3e8;
    ppppppuStack_3e0 = pppppppuVar8;
    local_3d8 = (undefined8 *)0x0;
    uStack_3d0 = 0;
    local_3c8 = puVar2[0xb4];
    puVar2[0xb4] = 0;
    puVar13 = (undefined8 *)puVar2[0xb2];
    if (puVar13 != (undefined8 *)0x0) {
      local_3e8 = (undefined8 *******)*plVar1;
      pppppppuVar8 = (undefined8 *******)puVar2[0xb1];
      ppppppuStack_3e0 = pppppppuVar8;
      local_3d8 = puVar13;
      puVar13[2] = &local_3e8;
      *plVar1 = (longlong)plVar1;
      puVar2[0xb1] = plVar1;
      puVar2[0xb2] = 0;
    }
    *plVar1 = (longlong)plVar1;
    puVar2[0xb1] = plVar1;
    puVar2[0xb2] = 0;
    *(undefined1 *)(puVar2 + 0xb3) = 0;
    puVar2[0xb4] = 0;
    ppppppuStack_3e0 = pppppppuVar8;
    puVar13 = local_3d8;
    if (pppppppuVar8 != &local_3e8) {
      puVar15 = puVar2 + 0xb0;
      do {
        ppppppuVar9 = (undefined8 ******)FUN_140764b70(&local_108,pppppppuVar8[5]);
        pppppppuVar8[5] = ppppppuVar9;
        ppppppuVar9 = (undefined8 ******)FUN_140764a80(&local_440,pppppppuVar8[0xd]);
        pppppppuVar8[0xd] = ppppppuVar9;
        local_448 = (undefined8 *****)
                    FUN_140764a30(extraout_XMM0_Da_07,pppppppuVar8[4],param_2 + 0x98);
        puVar7 = puVar15;
        puVar13 = (undefined8 *)puVar2[0xb2];
        while (puVar13 != (undefined8 *)0x0) {
          if ((longlong)puVar13[4] < (longlong)local_448) {
            puVar13 = (undefined8 *)*puVar13;
          }
          else {
            puVar7 = puVar13;
            puVar13 = (undefined8 *)puVar13[1];
          }
        }
        if ((puVar7 == puVar15) || ((longlong)local_448 < (longlong)puVar7[4])) {
          puVar7 = (undefined8 *)FUN_14033f410(puVar15,auStack_b0);
          puVar7 = (undefined8 *)*puVar7;
        }
        puVar7[5] = pppppppuVar8[5];
        *(undefined1 *)(puVar7 + 6) = *(undefined1 *)(pppppppuVar8 + 6);
        pppppppuVar11 = pppppppuVar8 + 7;
        if ((undefined8 *******)(puVar7 + 7) != pppppppuVar11) {
          if ((undefined8 ******)0xf < pppppppuVar8[10]) {
            pppppppuVar11 = (undefined8 *******)*pppppppuVar11;
          }
          FUN_140030630(puVar7 + 7,pppppppuVar11,pppppppuVar8[9]);
        }
        *(undefined4 *)(puVar7 + 0xb) = *(undefined4 *)(pppppppuVar8 + 0xb);
        puVar7[0xc] = pppppppuVar8[0xc];
        uVar16 = *(undefined4 *)((longlong)pppppppuVar8 + 0x6c);
        uVar5 = *(undefined4 *)(pppppppuVar8 + 0xe);
        uVar6 = *(undefined4 *)((longlong)pppppppuVar8 + 0x74);
        *(undefined4 *)(puVar7 + 0xd) = *(undefined4 *)(pppppppuVar8 + 0xd);
        *(undefined4 *)((longlong)puVar7 + 0x6c) = uVar16;
        *(undefined4 *)(puVar7 + 0xe) = uVar5;
        *(undefined4 *)((longlong)puVar7 + 0x74) = uVar6;
        puVar7[0xf] = pppppppuVar8[0xf];
        *(undefined4 *)(puVar7 + 0x10) = *(undefined4 *)(pppppppuVar8 + 0x10);
        *(undefined4 *)((longlong)puVar7 + 0x84) = *(undefined4 *)((longlong)pppppppuVar8 + 0x84);
        *(undefined4 *)(puVar7 + 0x11) = *(undefined4 *)(pppppppuVar8 + 0x11);
        *(undefined1 *)((longlong)puVar7 + 0x8c) = *(undefined1 *)((longlong)pppppppuVar8 + 0x8c);
        *(undefined4 *)(puVar7 + 0x12) = *(undefined4 *)(pppppppuVar8 + 0x12);
        *(undefined1 *)((longlong)puVar7 + 0x94) = *(undefined1 *)((longlong)pppppppuVar8 + 0x94);
        *(undefined1 *)((longlong)puVar7 + 0x95) = *(undefined1 *)((longlong)pppppppuVar8 + 0x95);
        *(undefined4 *)(puVar7 + 0x13) = *(undefined4 *)(pppppppuVar8 + 0x13);
        if ((undefined8 *******)(puVar7 + 0x14) != pppppppuVar8 + 0x14) {
          FUN_14032fe20(puVar7 + 0x14,pppppppuVar8[0x14],pppppppuVar8[0x15]);
        }
        FUN_140339240(puVar7 + 0x17,pppppppuVar8 + 0x17);
        pppppppuVar8 = (undefined8 *******)FUN_14001dca0(pppppppuVar8);
        puVar13 = local_3d8;
      } while (pppppppuVar8 != &local_3e8);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_14032b140(&local_3e8,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      FUN_1402f79c0(puVar13 + 5);
      free(puVar13);
      puVar13 = puVar15;
    }
    plVar1 = puVar2 + 0xb6;
    local_418 = &local_418;
    pppppppuVar8 = &local_418;
    ppppppuStack_410 = pppppppuVar8;
    local_408 = (undefined8 *)0x0;
    uStack_400 = 0;
    local_3f8 = puVar2[0xba];
    puVar2[0xba] = 0;
    puVar13 = (undefined8 *)puVar2[0xb8];
    if (puVar13 != (undefined8 *)0x0) {
      local_418 = (undefined8 *******)*plVar1;
      pppppppuVar8 = (undefined8 *******)puVar2[0xb7];
      ppppppuStack_410 = pppppppuVar8;
      local_408 = puVar13;
      puVar13[2] = &local_418;
      *plVar1 = (longlong)plVar1;
      puVar2[0xb7] = plVar1;
      puVar2[0xb8] = 0;
    }
    *plVar1 = (longlong)plVar1;
    puVar2[0xb7] = plVar1;
    puVar2[0xb8] = 0;
    *(undefined1 *)(puVar2 + 0xb9) = 0;
    puVar2[0xba] = 0;
    ppppppuStack_410 = pppppppuVar8;
    puVar13 = local_408;
    if (pppppppuVar8 != &local_418) {
      puVar15 = puVar2 + 0xb6;
      do {
        ppppppuVar9 = pppppppuVar8[4];
        for (puVar3 = *(ulonglong **)
                       (*(longlong *)(param_2 + 0xa0) +
                       ((ulonglong)ppppppuVar9 % (ulonglong)*(uint *)(param_2 + 0xa8)) * 8);
            puVar3 != (ulonglong *)0x0; puVar3 = (ulonglong *)puVar3[2]) {
          if (ppppppuVar9 == (undefined8 ******)*puVar3) {
            if ((puVar3 != (ulonglong *)0x0) &&
               (puVar3 != *(ulonglong **)
                           (*(longlong *)(param_2 + 0xa0) + *(longlong *)(param_2 + 0xa8) * 8))) {
              ppppppuVar9 = (undefined8 ******)puVar3[1];
            }
            break;
          }
        }
        bVar4 = true;
        puVar13 = puVar15;
        puVar7 = (undefined8 *)puVar2[0xb8];
        while (puVar7 != (undefined8 *)0x0) {
          bVar4 = (longlong)ppppppuVar9 < (longlong)puVar7[4];
          puVar13 = puVar7;
          if ((longlong)ppppppuVar9 < (longlong)puVar7[4]) {
            puVar7 = (undefined8 *)puVar7[1];
          }
          else {
            puVar7 = (undefined8 *)*puVar7;
          }
        }
        puVar7 = puVar13;
        if (bVar4) {
          if (puVar13 != (undefined8 *)puVar2[0xb7]) {
            puVar7 = (undefined8 *)FUN_14001dcf0(puVar13);
            goto LAB_140764742;
          }
LAB_14076474b:
          lVar10 = thunk_FUN_140983da8(0x28);
          *(undefined8 *******)(lVar10 + 0x20) = ppppppuVar9;
          if ((puVar13 == puVar15) || (uVar14 = 1, (longlong)ppppppuVar9 < (longlong)puVar13[4])) {
            uVar14 = 0;
          }
          FUN_14001de10(lVar10,puVar13,puVar15,uVar14);
          puVar2[0xba] = puVar2[0xba] + 1;
        }
        else {
LAB_140764742:
          if ((longlong)puVar7[4] < (longlong)ppppppuVar9) goto LAB_14076474b;
        }
        pppppppuVar11 = (undefined8 *******)*pppppppuVar8;
        if (pppppppuVar11 == (undefined8 *******)0x0) {
          pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
          pppppppuVar12 = (undefined8 *******)0x0;
          if (pppppppuVar8 == (undefined8 *******)*pppppppuVar11) {
            do {
              pppppppuVar8 = pppppppuVar11;
              pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
            } while (pppppppuVar8 == (undefined8 *******)*pppppppuVar11);
            pppppppuVar12 = (undefined8 *******)*pppppppuVar8;
          }
          if (pppppppuVar12 != pppppppuVar11) {
            pppppppuVar8 = pppppppuVar11;
          }
        }
        else {
          for (pppppppuVar12 = (undefined8 *******)pppppppuVar11[1]; pppppppuVar8 = pppppppuVar11,
              pppppppuVar12 != (undefined8 *******)0x0;
              pppppppuVar12 = (undefined8 *******)pppppppuVar12[1]) {
            pppppppuVar11 = pppppppuVar12;
          }
        }
        puVar13 = local_408;
      } while (pppppppuVar8 != &local_418);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_1402450d0(&local_418,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      free(puVar13);
      puVar13 = puVar15;
    }
    plVar1 = puVar2 + 0xbc;
    local_328 = &local_328;
    pppppppuVar8 = &local_328;
    ppppppuStack_320 = pppppppuVar8;
    local_318 = (undefined8 *)0x0;
    uStack_310 = 0;
    local_308 = puVar2[0xc0];
    puVar2[0xc0] = 0;
    puVar13 = (undefined8 *)puVar2[0xbe];
    if (puVar13 != (undefined8 *)0x0) {
      local_328 = (undefined8 *******)*plVar1;
      pppppppuVar8 = (undefined8 *******)puVar2[0xbd];
      ppppppuStack_320 = pppppppuVar8;
      local_318 = puVar13;
      puVar13[2] = &local_328;
      *plVar1 = (longlong)plVar1;
      puVar2[0xbd] = plVar1;
      puVar2[0xbe] = 0;
    }
    *plVar1 = (longlong)plVar1;
    puVar2[0xbd] = plVar1;
    puVar2[0xbe] = 0;
    *(undefined1 *)(puVar2 + 0xbf) = 0;
    puVar2[0xc0] = 0;
    ppppppuStack_320 = pppppppuVar8;
    puVar13 = local_318;
    if (pppppppuVar8 != &local_328) {
      puVar15 = puVar2 + 0xbc;
      do {
        ppppppuVar9 = pppppppuVar8[4];
        for (puVar3 = *(ulonglong **)
                       (*(longlong *)(param_2 + 0xa0) +
                       ((ulonglong)ppppppuVar9 % (ulonglong)*(uint *)(param_2 + 0xa8)) * 8);
            puVar3 != (ulonglong *)0x0; puVar3 = (ulonglong *)puVar3[2]) {
          if (ppppppuVar9 == (undefined8 ******)*puVar3) {
            if ((puVar3 != (ulonglong *)0x0) &&
               (puVar3 != *(ulonglong **)
                           (*(longlong *)(param_2 + 0xa0) + *(longlong *)(param_2 + 0xa8) * 8))) {
              ppppppuVar9 = (undefined8 ******)puVar3[1];
            }
            break;
          }
        }
        bVar4 = true;
        puVar13 = puVar15;
        puVar7 = (undefined8 *)puVar2[0xbe];
        while (puVar7 != (undefined8 *)0x0) {
          bVar4 = (longlong)ppppppuVar9 < (longlong)puVar7[4];
          puVar13 = puVar7;
          if ((longlong)ppppppuVar9 < (longlong)puVar7[4]) {
            puVar7 = (undefined8 *)puVar7[1];
          }
          else {
            puVar7 = (undefined8 *)*puVar7;
          }
        }
        puVar7 = puVar13;
        if (bVar4) {
          if (puVar13 != (undefined8 *)puVar2[0xbd]) {
            puVar7 = (undefined8 *)FUN_14001dcf0(puVar13);
            goto LAB_140764942;
          }
LAB_14076494b:
          lVar10 = thunk_FUN_140983da8(0x28);
          *(undefined8 *******)(lVar10 + 0x20) = ppppppuVar9;
          if ((puVar13 == puVar15) || (uVar14 = 1, (longlong)ppppppuVar9 < (longlong)puVar13[4])) {
            uVar14 = 0;
          }
          FUN_14001de10(lVar10,puVar13,puVar15,uVar14);
          puVar2[0xc0] = puVar2[0xc0] + 1;
        }
        else {
LAB_140764942:
          if ((longlong)puVar7[4] < (longlong)ppppppuVar9) goto LAB_14076494b;
        }
        pppppppuVar11 = (undefined8 *******)*pppppppuVar8;
        if (pppppppuVar11 == (undefined8 *******)0x0) {
          pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
          pppppppuVar12 = (undefined8 *******)0x0;
          if (pppppppuVar8 == (undefined8 *******)*pppppppuVar11) {
            do {
              pppppppuVar8 = pppppppuVar11;
              pppppppuVar11 = (undefined8 *******)pppppppuVar8[2];
            } while (pppppppuVar8 == (undefined8 *******)*pppppppuVar11);
            pppppppuVar12 = (undefined8 *******)*pppppppuVar8;
          }
          if (pppppppuVar12 != pppppppuVar11) {
            pppppppuVar8 = pppppppuVar11;
          }
        }
        else {
          for (pppppppuVar12 = (undefined8 *******)pppppppuVar11[1]; pppppppuVar8 = pppppppuVar11,
              pppppppuVar12 != (undefined8 *******)0x0;
              pppppppuVar12 = (undefined8 *******)pppppppuVar12[1]) {
            pppppppuVar11 = pppppppuVar12;
          }
        }
        puVar13 = local_318;
      } while (pppppppuVar8 != &local_328);
    }
    while (puVar13 != (undefined8 *)0x0) {
      FUN_1402450d0(&local_328,*puVar13);
      puVar15 = (undefined8 *)puVar13[1];
      free(puVar13);
      puVar13 = puVar15;
    }
    puVar2 = (undefined8 *)*puVar2;
  } while( true );
}

