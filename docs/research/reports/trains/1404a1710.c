// Candidate VA 1404a1710; RVA 0x4a1710
// Ghidra inferred prototype: undefined FUN_1404a1710()

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 *
FUN_1404a1710(undefined8 *param_1,basic_ostream<char,std::char_traits<char>_> **param_2,
             longlong param_3)

{
  char *pcVar1;
  int iVar2;
  longlong *plVar3;
  undefined8 uVar4;
  __int64 _Var5;
  void *pvVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  ulonglong uVar12;
  ulonglong uVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  ulonglong uVar16;
  undefined8 uVar17;
  undefined8 uVar18;
  undefined8 *puVar19;
  undefined8 uVar20;
  longlong lVar21;
  basic_ostream<char,std::char_traits<char>_> *pbVar22;
  undefined1 *puVar23;
  undefined8 *puVar24;
  void *pvVar25;
  undefined8 *******pppppppuVar26;
  undefined1 *puVar27;
  undefined2 *puVar28;
  ulonglong uVar29;
  longlong *plVar30;
  longlong lVar31;
  uint uVar32;
  uint uVar33;
  longlong *plVar34;
  ulonglong uVar35;
  char *pcVar36;
  char *pcVar37;
  basic_ostream<char,std::char_traits<char>_> *pbVar38;
  basic_ostream<char,std::char_traits<char>_> *pbVar39;
  basic_ostream<char,std::char_traits<char>_> *local_res10;
  longlong local_res18;
  basic_ostream<char,std::char_traits<char>_> *local_res20;
  undefined8 local_598;
  undefined8 uStack_590;
  undefined8 local_588;
  ulonglong uStack_580;
  basic_ostream<char,std::char_traits<char>_> *local_578;
  void *local_570;
  void *pvStack_568;
  undefined8 local_560;
  basic_ostream<char,std::char_traits<char>_> **local_558;
  basic_ostream<char,std::char_traits<char>_> **local_550;
  undefined8 local_548;
  undefined4 local_540;
  undefined8 *******local_538;
  undefined8 uStack_530;
  undefined8 local_528;
  ulonglong uStack_520;
  undefined4 local_518;
  undefined8 local_510;
  undefined8 uStack_508;
  undefined8 local_500;
  ulonglong uStack_4f8;
  undefined1 local_4f0;
  undefined7 uStack_4ef;
  longlong local_4e0;
  ulonglong uStack_4d8;
  undefined2 local_4d0;
  undefined6 uStack_4ce;
  undefined8 uStack_4c8;
  undefined8 local_4c0;
  ulonglong uStack_4b8;
  char *local_4b0;
  char *pcStack_4a8;
  undefined8 local_4a0;
  undefined8 *******local_498 [2];
  longlong local_488;
  ulonglong local_480;
  basic_ostream<char,std::char_traits<char>_> **local_478;
  basic_ostream<char,std::char_traits<char>_> **local_470;
  undefined8 local_468;
  undefined8 *puStack_460;
  char *local_458;
  undefined8 uStack_450;
  undefined8 local_448;
  undefined8 uStack_440;
  undefined8 local_438;
  undefined8 uStack_430;
  undefined8 local_428;
  undefined8 *puStack_420;
  char *local_418;
  undefined8 uStack_410;
  undefined1 local_408 [32];
  undefined **local_3e8 [3];
  longlong *local_3d0;
  longlong *local_3b0;
  int *local_398;
  undefined1 local_378 [12];
  char local_36c;
  longlong local_368;
  longlong local_360;
  undefined8 local_358;
  basic_ostream<char,std::char_traits<char>_> local_348 [112];
  undefined **local_2d8;
  undefined1 *local_2d0;
  undefined8 local_2c8;
  ulonglong local_2c0;
  undefined1 local_2b8 [632];
  
  uVar29 = 0;
  local_518 = 0;
  pbVar22 = param_2[0x82];
  pbVar38 = pbVar22 + 0x20;
  local_res18 = param_3;
  if (0x7fffffffffffffffU - *(longlong *)(pbVar22 + 0x30) < 0x11) {
                    /* WARNING: Subroutine does not return */
    FUN_140001c70();
  }
  pbVar39 = pbVar38;
  if (0xf < *(ulonglong *)(pbVar22 + 0x38)) {
    pbVar39 = *(basic_ostream<char,std::char_traits<char>_> **)pbVar38;
  }
  FUN_140235900(local_498,(ulonglong)local_res10 & 0xff,pbVar38,pbVar39,
                *(longlong *)(pbVar22 + 0x30)," Timetable Export",0x11);
  if (local_488 != 0x7fffffffffffffff) {
    pppppppuVar26 = local_498;
    if (0xf < local_480) {
      pppppppuVar26 = local_498[0];
    }
    FUN_140235900(&local_4f0,(ulonglong)local_res10 & 0xff,local_498,pppppppuVar26,local_488,
                  &DAT_140a4d470,1);
    puVar19 = (undefined8 *)
              FUN_14049adc0(&local_598,
                            (*(longlong *)(param_3 + 0x20) * 100 + *(longlong *)(param_3 + 0x28)) *
                            10000);
    puVar24 = puVar19 + 2;
    if (0xf < (ulonglong)puVar19[3]) {
      puVar19 = (undefined8 *)*puVar19;
    }
    FUN_1400254f0(&local_4f0,puVar19,*puVar24);
    FUN_140002d30(&local_598);
    FUN_140247b90(local_408,&DAT_140b77d10);
    uStack_530 = 0;
    local_528 = _DAT_140aac900;
    uStack_520 = _UNK_140aac908;
    local_538 = (undefined8 *******)0x0;
    puVar23 = &local_4f0;
    if (0xf < uStack_4d8) {
      puVar23 = (undefined1 *)CONCAT71(uStack_4ef,local_4f0);
    }
    puVar27 = &local_4f0;
    if (0xf < uStack_4d8) {
      puVar27 = (undefined1 *)CONCAT71(uStack_4ef,local_4f0);
    }
    FUN_14020e5c0(&local_res10,puVar27,puVar23 + local_4e0,&local_538);
    uVar20 = FUN_140499010(&local_598,&local_538);
    FUN_1402522e0(&local_538,uVar20);
    FUN_140025470(&local_598);
    uVar12 = _UNK_140aac968;
    uVar20 = _DAT_140aac960;
    uStack_508 = 0;
    local_500 = _DAT_140aac960;
    uStack_4f8 = _UNK_140aac968;
    local_510 = (void *)(ulonglong)CONCAT14(DAT_140a75da0,DAT_140a75d9c);
    FUN_14020e5c0(&local_res10,&local_510,(longlong)&local_510 + 5,&local_538);
    pppppppuVar26 = &local_538;
    if (7 < uStack_520) {
      pppppppuVar26 = local_538;
    }
    local_598 = (char *)0x0;
    uStack_590 = 0;
    local_588 = 0;
    uStack_580 = 0;
    FUN_140025200(&local_598,pppppppuVar26,local_528);
    FUN_14026ce20(&local_4d0,local_408,&local_598);
    FUN_140025470(&local_598);
    FUN_14027c140(local_3e8);
    puVar28 = &local_4d0;
    if (7 < uStack_4b8) {
      puVar28 = (undefined2 *)CONCAT62(uStack_4ce,local_4d0);
    }
    lVar21 = FUN_14027c000(local_3e8,puVar28);
    if (lVar21 == 0) {
      *(undefined1 *)(param_1 + 4) = 0;
      FUN_140278670(local_3e8);
      FUN_140025470(&local_4d0);
      if (0xf < uStack_4f8) {
        FUN_140003040(&local_510,local_510);
      }
      local_500 = _DAT_140aac910;
      uStack_4f8 = _UNK_140aac918;
      local_510 = (void *)((ulonglong)local_510 & 0xffffffffffffff00);
      if (7 < uStack_520) {
        FUN_1400260a0(_DAT_140aac910,local_538);
      }
      local_528 = _DAT_140aac900;
      uStack_520 = _UNK_140aac908;
      local_538 = (undefined8 *******)((ulonglong)local_538 & 0xffffffffffff0000);
      FUN_140025470(local_408);
      if (0xf < uStack_4d8) {
        FUN_140003040(&local_4f0,CONCAT71(uStack_4ef,local_4f0));
      }
      local_4e0 = _DAT_140aac910;
      uStack_4d8 = _UNK_140aac918;
      local_4f0 = 0;
      FUN_140002d30(local_498);
    }
    else {
      std::basic_ostream<char,std::char_traits<char>_>::basic_ostream<char,std::char_traits<char>_>
                (local_348,(basic_streambuf<char,std::char_traits<char>_> *)local_3e8,false);
      local_578 = local_348;
      local_570 = (void *)0x0;
      pvStack_568 = (void *)0x0;
      local_560 = 0;
      FUN_1404a0ac0(&local_578);
      FUN_1404a0ba0(&local_578);
      uStack_590 = 0;
      local_588 = uVar20;
      uStack_580 = uVar12;
      local_598._0_5_ = CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
      local_598 = (char *)(ulonglong)(uint5)local_598;
      FUN_1404a0c80(&local_578,&local_598);
      FUN_140002d30(&local_598);
      FUN_140002b40(&local_598,"ExportMeta");
      FUN_1404a13e0(&local_578,&local_598);
      FUN_140002d30(&local_598);
      FUN_140002b40(&local_598,"company_name");
      FUN_1404a0c80(&local_578,&local_598);
      FUN_140002d30(&local_598);
      FUN_1404a13e0(&local_578,param_2[0x82] + 0x20);
      FUN_140002b40(&local_598,"clock_epoch_s");
      FUN_1404a0c80(&local_578,&local_598);
      FUN_140002d30(&local_598);
      lVar21 = *(longlong *)(param_3 + 0x28);
      lVar31 = *(longlong *)(param_3 + 0x20);
      if (local_570 != pvStack_568) {
        if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
           (*(int *)((longlong)pvStack_568 + -8) == 0)) {
          FUN_1402ccc90(local_578,&DAT_140a4d304);
        }
        *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
        *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
      }
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                (local_578,(lVar21 * 10000) / 1000000 + lVar31);
      local_588 = _DAT_140aac9e0;
      uStack_580 = _UNK_140aac9e8;
      local_598._0_1_ = s_model_version_140a75ce0[0];
      local_598._1_1_ = s_model_version_140a75ce0[1];
      local_598._2_1_ = s_model_version_140a75ce0[2];
      local_598._3_1_ = s_model_version_140a75ce0[3];
      local_598._4_1_ = s_model_version_140a75ce0[4];
      local_598._5_1_ = s_model_version_140a75ce0[5];
      local_598._6_1_ = s_model_version_140a75ce0[6];
      local_598._7_1_ = s_model_version_140a75ce0[7];
      uStack_590 = (ulonglong)
                   CONCAT14(s_model_version_140a75ce0[0xc],s_model_version_140a75ce0._8_4_);
      FUN_1404a0c80(&local_578,&local_598);
      FUN_140002d30(&local_598);
      FUN_1404a0ef0(&local_578,0xe6);
      pvStack_568 = (void *)((longlong)pvStack_568 + -0xc);
      std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
      FUN_1404a1690(&local_578);
      FUN_1402ccc90(local_578,&DAT_140a508fc);
      uVar13 = _UNK_140aac9a8;
      uVar14 = _DAT_140aac9a0;
      uVar35 = _UNK_140aac958;
      uVar11 = _DAT_140aac950;
      uVar10 = _UNK_140aac948;
      uVar9 = _DAT_140aac940;
      uVar8 = _UNK_140aac938;
      uVar7 = _DAT_140aac930;
      local_478 = &local_578;
      plVar3 = *(longlong **)(param_2[0x84] + 0x10);
      plVar34 = (longlong *)*plVar3;
      plVar30 = plVar3;
      if (plVar34 == (longlong *)0x0) {
        plVar30 = plVar3 + 1;
        plVar34 = (longlong *)*plVar30;
        while (plVar34 == (longlong *)0x0) {
          plVar30 = plVar30 + 1;
          plVar34 = (longlong *)*plVar30;
        }
      }
      plVar3 = (longlong *)plVar3[*(longlong *)(param_2[0x84] + 0x18)];
      local_470 = param_2;
      while (plVar34 != plVar3) {
        if (*plVar34 != 0) {
          if (local_570 != pvStack_568) {
            if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
               (*(int *)((longlong)pvStack_568 + -8) == 0)) {
              FUN_1402ccc90(local_578,&DAT_140a4d304);
            }
            *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
            *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
          }
          local_548 = 0;
          local_540 = 1;
          FUN_1404b78d0(&local_570,&local_548);
          FUN_1402ccc90(local_578,&DAT_140a75b4c);
          std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
          FUN_1404a1690(&local_578);
          uStack_590 = 0;
          local_588 = uVar20;
          uStack_580 = uVar12;
          local_598._0_5_ = CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
          local_598 = (char *)(ulonglong)(uint5)local_598;
          FUN_1404a0c80(&local_578,&local_598);
          FUN_140002d30(&local_598);
          uStack_590 = 0;
          local_588 = uVar9;
          uStack_580 = uVar10;
          local_598 = (char *)(ulonglong)(uint3)DAT_140a72354;
          FUN_1404a13e0(&local_578,&local_598);
          FUN_140002d30(&local_598);
          uStack_590 = 0;
          local_588 = uVar7;
          uStack_580 = uVar8;
          local_598 = (char *)(ulonglong)DAT_140a4d880;
          FUN_1404a0c80(&local_578,&local_598);
          FUN_140002d30(&local_598);
          FUN_1404a1200(&local_578,*plVar34);
          uStack_590 = 0;
          local_588 = uVar11;
          uStack_580 = uVar35;
          local_598 = (char *)(ulonglong)DAT_140a4d7f8;
          FUN_1404a0c80(&local_578,&local_598);
          if (0xf < uStack_580) {
            FUN_140003040(&local_598,local_598);
          }
          FUN_1404a13e0(&local_578,plVar34 + 3);
          local_588 = uVar14;
          uStack_580 = uVar13;
          local_598._0_1_ = s_parent_id_140a726b8[0];
          local_598._1_1_ = s_parent_id_140a726b8[1];
          local_598._2_1_ = s_parent_id_140a726b8[2];
          local_598._3_1_ = s_parent_id_140a726b8[3];
          local_598._4_1_ = s_parent_id_140a726b8[4];
          local_598._5_1_ = s_parent_id_140a726b8[5];
          local_598._6_1_ = s_parent_id_140a726b8[6];
          local_598._7_1_ = s_parent_id_140a726b8[7];
          uStack_590 = (ulonglong)(byte)s_parent_id_140a726b8[8];
          FUN_1404a0c80(&local_578,&local_598);
          FUN_140002d30(&local_598);
          FUN_1404a1200(&local_578,plVar34[2]);
          pvStack_568 = (void *)((longlong)pvStack_568 + -0xc);
          std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
          FUN_1404a1690(&local_578);
          FUN_1402ccc90(local_578,&DAT_140a508fc);
        }
        plVar34 = (longlong *)plVar34[0x10];
        while (plVar34 == (longlong *)0x0) {
          plVar30 = plVar30 + 1;
          plVar34 = (longlong *)*plVar30;
        }
      }
      local_550 = &local_578;
      pbVar22 = param_2[0x14];
      pbVar38 = param_2[0x13];
      local_res10 = pbVar22;
      local_558 = param_2;
      if (pbVar38 != pbVar22) {
        do {
          lVar21 = *(longlong *)pbVar38;
          if ((lVar21 != 0) &&
             (uVar33 = *(uint *)(param_2 + 0x11), lVar31 = lVar21, uVar35 = uVar29, uVar33 != 0)) {
            do {
              if ((*(short *)(lVar31 + 6) != -1) && (*(short *)(lVar31 + 6) != 0)) {
                FUN_1404a3450(&local_558,lVar21 + uVar35 * 1000);
              }
              uVar32 = (int)uVar35 + 1;
              pbVar22 = local_res10;
              lVar31 = lVar31 + 1000;
              uVar35 = (ulonglong)uVar32;
            } while (uVar32 < uVar33);
          }
          pbVar38 = pbVar38 + 8;
        } while (pbVar38 != pbVar22);
      }
      local_558 = &local_578;
      local_550 = (basic_ostream<char,std::char_traits<char>_> **)&local_478;
      pbVar22 = param_2[0x34];
      pbVar38 = param_2[0x33];
      local_res10 = pbVar22;
      if (pbVar38 != pbVar22) {
        do {
          lVar21 = *(longlong *)pbVar38;
          if ((lVar21 != 0) &&
             (uVar33 = *(uint *)(param_2 + 0x31), lVar31 = lVar21, uVar35 = uVar29, uVar33 != 0)) {
            do {
              if ((*(short *)(lVar31 + 6) != -1) && (*(short *)(lVar31 + 6) != 0)) {
                FUN_1404a3940(&local_558,lVar21 + uVar35 * 0x280);
              }
              uVar32 = (int)uVar35 + 1;
              pbVar22 = local_res10;
              lVar31 = lVar31 + 0x280;
              uVar35 = (ulonglong)uVar32;
            } while (uVar32 < uVar33);
          }
          pbVar38 = pbVar38 + 8;
        } while (pbVar38 != pbVar22);
      }
      local_res10 = (basic_ostream<char,std::char_traits<char>_> *)&local_578;
      pbVar22 = param_2[0x44];
      pbVar38 = param_2[0x43];
      local_res20 = pbVar22;
      if (pbVar38 != pbVar22) {
        do {
          lVar21 = *(longlong *)pbVar38;
          if ((lVar21 != 0) &&
             (uVar33 = *(uint *)(param_2 + 0x41), lVar31 = lVar21, uVar35 = uVar29, uVar33 != 0)) {
            do {
              if ((*(short *)(lVar31 + 6) != -1) && (*(short *)(lVar31 + 6) != 0)) {
                FUN_1404a44d0(&local_res10,uVar35 * 0x178 + lVar21);
              }
              uVar32 = (int)uVar35 + 1;
              pbVar22 = local_res20;
              lVar31 = lVar31 + 0x178;
              uVar35 = (ulonglong)uVar32;
            } while (uVar32 < uVar33);
          }
          pbVar38 = pbVar38 + 8;
        } while (pbVar38 != pbVar22);
      }
      local_558 = &local_578;
      pbVar22 = param_2[0x54];
      pbVar38 = param_2[0x53];
      local_res10 = pbVar22;
      local_550 = param_2;
      if (pbVar38 != pbVar22) {
        do {
          lVar21 = *(longlong *)pbVar38;
          if ((lVar21 != 0) &&
             (uVar33 = *(uint *)(param_2 + 0x51), lVar31 = lVar21, uVar35 = uVar29, uVar33 != 0)) {
            do {
              if ((*(short *)(lVar31 + 6) != -1) && (*(short *)(lVar31 + 6) != 0)) {
                FUN_1404a48e0(&local_558,lVar21 + uVar35 * 0x290);
              }
              uVar32 = (int)uVar35 + 1;
              pbVar22 = local_res10;
              lVar31 = lVar31 + 0x290;
              uVar35 = (ulonglong)uVar32;
            } while (uVar32 < uVar33);
          }
          pbVar38 = pbVar38 + 8;
        } while (pbVar38 != pbVar22);
      }
      local_4b0 = (char *)0x0;
      pcStack_4a8 = (char *)0x0;
      local_4a0 = 0;
      local_518 = 0x3e0;
      FUN_14045ece0(local_res18 + 0xa0,&local_4b0);
      pcVar37 = pcStack_4a8;
      uVar18 = _UNK_140aacaa8;
      uVar17 = _DAT_140aacaa0;
      uVar16 = _UNK_140aaca38;
      uVar15 = _DAT_140aaca30;
      uVar14 = _UNK_140aaca08;
      uVar11 = _DAT_140aaca00;
      uVar13 = _UNK_140aac998;
      uVar10 = _DAT_140aac990;
      uVar9 = _UNK_140aac988;
      uVar8 = _DAT_140aac980;
      uVar35 = _UNK_140aac978;
      uVar7 = _DAT_140aac970;
      if (local_4b0 != pcStack_4a8) {
        pcVar36 = local_4b0 + 0x5d0;
        do {
          if ((pcVar36[0x20] != '\0') && (*pcVar36 != '\0')) {
            if (local_570 != pvStack_568) {
              if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
                 (*(int *)((longlong)pvStack_568 + -8) == 0)) {
                FUN_1402ccc90(local_578,&DAT_140a4d304);
              }
              *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
              *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
            }
            local_548 = 0;
            local_540 = 1;
            FUN_1404b78d0(&local_570,&local_548);
            FUN_1402ccc90(local_578,&DAT_140a75b4c);
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
            FUN_1404a1690(&local_578);
            uStack_590 = 0;
            local_588 = uVar20;
            uStack_580 = uVar12;
            local_598._0_5_ = CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
            local_598 = (char *)(ulonglong)(uint5)local_598;
            FUN_1404a0c80(&local_578,&local_598);
            if (0xf < uStack_580) {
              pvVar25 = local_598;
              if ((0xfff < uStack_580 + 1) &&
                 (pvVar25 = *(void **)((longlong)local_598 + -8),
                 0x1f < (ulonglong)((longlong)local_598 + (-8 - (longlong)pvVar25)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar25);
            }
            uStack_590 = 0;
            local_588 = uVar7;
            uStack_580 = uVar35;
            local_598._0_6_ = CONCAT24(s_Motion_140a71fd4._4_2_,s_Motion_140a71fd4._0_4_);
            local_598 = (char *)(ulonglong)(uint6)local_598;
            FUN_1404a13e0(&local_578,&local_598);
            if (0xf < uStack_580) {
              pvVar25 = local_598;
              if ((0xfff < uStack_580 + 1) &&
                 (pvVar25 = *(void **)((longlong)local_598 + -8),
                 0x1f < (ulonglong)((longlong)local_598 + (-8 - (longlong)pvVar25)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar25);
            }
            local_588 = uVar10;
            uStack_580 = uVar13;
            local_598 = (void *)0x64695f6e69617274;
            uStack_590 = 0;
            FUN_1404a0c80(&local_578,&local_598);
            if (0xf < uStack_580) {
              pvVar25 = local_598;
              if ((0xfff < uStack_580 + 1) &&
                 (pvVar25 = *(void **)((longlong)local_598 + -8),
                 0x1f < (ulonglong)((longlong)local_598 + (-8 - (longlong)pvVar25)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar25);
            }
            uVar4 = *(undefined8 *)(pcVar36 + -0x5d0);
            if (local_570 != pvStack_568) {
              if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
                 (*(int *)((longlong)pvStack_568 + -8) == 0)) {
                FUN_1402ccc90(local_578,&DAT_140a4d304);
              }
              *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
              *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
            }
            pbVar22 = local_578;
            uStack_430 = uStack_440;
            local_468 = 3;
            puStack_460 = &local_438;
            local_458 = "\"0x{:x}\"";
            uStack_450 = 8;
            local_2c8 = 0;
            local_2d8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            local_2d0 = local_2b8;
            local_2c0 = 500;
            local_428 = 3;
            local_418 = "\"0x{:x}\"";
            uStack_410 = 8;
            local_448 = uVar4;
            local_438 = uVar4;
            puStack_420 = puStack_460;
            FUN_140022830(&local_2d8,&local_418,&local_428);
            local_598 = (char *)0x0;
            uStack_590 = 0;
            local_588 = 0;
            uStack_580 = 0;
            FUN_140002c00(&local_598,local_2d0);
            local_2d8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            if (local_2d0 != local_2b8) {
              puVar23 = local_2d0;
              if ((0xfff < local_2c0) &&
                 (puVar23 = *(undefined1 **)(local_2d0 + -8),
                 (undefined1 *)0x1f < local_2d0 + (-8 - (longlong)puVar23))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(puVar23);
            }
            puVar24 = &local_598;
            if (0xf < uStack_580) {
              puVar24 = (undefined8 *)local_598;
            }
            FUN_1402e46d0(pbVar22,puVar24);
            if (0xf < uStack_580) {
              puVar24 = (undefined8 *)local_598;
              if ((0xfff < uStack_580 + 1) &&
                 (puVar24 = *(undefined8 **)((longlong)local_598 + -8),
                 0x1f < (ulonglong)((longlong)local_598 + (-8 - (longlong)puVar24)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(puVar24);
            }
            local_598 = (char *)0x0;
            uStack_590 = 0;
            local_588 = 0;
            uStack_580 = 0;
            local_598 = (char *)FUN_140003270(0x20);
            uVar4 = s_schedule_dispatch_140a729c8._8_8_;
            local_588 = uVar15;
            uStack_580 = uVar16;
            *(undefined8 *)local_598 = s_schedule_dispatch_140a729c8._0_8_;
            *(undefined8 *)(local_598 + 8) = uVar4;
            local_598[0x10] = s_schedule_dispatch_140a729c8[0x10];
            local_598[0x11] = '\0';
            FUN_1404a0c80(&local_578,&local_598);
            if (0xf < uStack_580) {
              FUN_140003040(&local_598,local_598);
            }
            if (local_570 != pvStack_568) {
              if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
                 (*(int *)((longlong)pvStack_568 + -8) == 0)) {
                FUN_1402ccc90(local_578,&DAT_140a4d304);
              }
              *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
              *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
            }
            local_558 = (basic_ostream<char,std::char_traits<char>_> **)0x0;
            local_550 = (basic_ostream<char,std::char_traits<char>_> **)CONCAT44(local_550._4_4_,1);
            FUN_1404b78d0(&local_570,&local_558);
            FUN_1402ccc90(local_578,&DAT_140a75b4c);
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
            FUN_1404a1690(&local_578);
            uStack_590 = 0;
            local_588 = uVar20;
            uStack_580 = uVar12;
            local_598._0_5_ = CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
            local_598 = (char *)(ulonglong)(uint5)local_598;
            FUN_1404a0c80(&local_578,&local_598);
            FUN_140002d30(&local_598);
            local_598 = (char *)0x0;
            uStack_590 = 0;
            local_588 = 0;
            uStack_580 = 0;
            local_598 = (char *)FUN_140003270(0x20);
            uVar4 = s_Motion__ScheduleDispatch_140a720a8._8_8_;
            local_588 = uVar17;
            uStack_580 = uVar18;
            *(undefined8 *)local_598 = s_Motion__ScheduleDispatch_140a720a8._0_8_;
            *(undefined8 *)(local_598 + 8) = uVar4;
            *(undefined8 *)(local_598 + 0x10) = s_Motion__ScheduleDispatch_140a720a8._16_8_;
            local_598[0x18] = '\0';
            FUN_1404a13e0(&local_578,&local_598);
            FUN_140002d30(&local_598);
            local_588 = uVar10;
            uStack_580 = uVar13;
            local_598 = (char *)0x64695f6465686373;
            uStack_590 = 0;
            FUN_1404a0c80(&local_578,&local_598);
            if (0xf < uStack_580) {
              FUN_140003040(&local_598,local_598);
            }
            FUN_1404a1200(&local_578,*(undefined8 *)(pcVar36 + 8));
            local_588 = uVar10;
            uStack_580 = uVar13;
            local_598 = (char *)0x64695f7466696873;
            uStack_590 = 0;
            FUN_1404a0c80(&local_578,&local_598);
            FUN_140002d30(&local_598);
            FUN_1404a1200(&local_578,*(undefined8 *)(pcVar36 + 0x10));
            uStack_590 = 0;
            local_588 = uVar8;
            uStack_580 = uVar9;
            local_598._0_6_ = CONCAT24(DAT_140a75cd8._4_2_,(undefined4)DAT_140a75cd8);
            local_598 = (char *)(ulonglong)CONCAT16(DAT_140a75cd8._6_1_,(uint6)local_598);
            FUN_1404a0c80(&local_578,&local_598);
            FUN_140002d30(&local_598);
            iVar2 = *(int *)(pcVar36 + 0x18);
            if (local_570 != pvStack_568) {
              if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
                 (*(int *)((longlong)pvStack_568 + -8) == 0)) {
                FUN_1402ccc90(local_578,&DAT_140a4d304);
              }
              *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
              *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
            }
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,iVar2);
            local_588 = uVar11;
            uStack_580 = uVar14;
            local_598._0_1_ = s_run_epoch_start_140a72dd0[0];
            local_598._1_1_ = s_run_epoch_start_140a72dd0[1];
            local_598._2_1_ = s_run_epoch_start_140a72dd0[2];
            local_598._3_1_ = s_run_epoch_start_140a72dd0[3];
            local_598._4_1_ = s_run_epoch_start_140a72dd0[4];
            local_598._5_1_ = s_run_epoch_start_140a72dd0[5];
            local_598._6_1_ = s_run_epoch_start_140a72dd0[6];
            local_598._7_1_ = s_run_epoch_start_140a72dd0[7];
            uStack_590 = (ulonglong)
                         CONCAT16(s_run_epoch_start_140a72dd0[0xe],
                                  CONCAT24(s_run_epoch_start_140a72dd0._12_2_,
                                           s_run_epoch_start_140a72dd0._8_4_));
            FUN_1404a0c80(&local_578,&local_598);
            FUN_140002d30(&local_598);
            _Var5 = *(__int64 *)(pcVar36 + -0x10);
            if (local_570 != pvStack_568) {
              if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
                 (*(int *)((longlong)pvStack_568 + -8) == 0)) {
                FUN_1402ccc90(local_578,&DAT_140a4d304);
              }
              *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
              *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
            }
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,_Var5);
            local_588 = uVar10;
            uStack_580 = uVar13;
            local_598._0_1_ = 's';
            local_598._1_1_ = 't';
            local_598._2_1_ = 'o';
            local_598._3_1_ = 'p';
            local_598._4_1_ = '_';
            local_598._5_1_ = 'i';
            local_598._6_1_ = 'd';
            local_598._7_1_ = 'x';
            uStack_590 = 0;
            FUN_1404a0c80(&local_578,&local_598);
            FUN_140002d30(&local_598);
            iVar2 = *(int *)(pcVar36 + -8);
            if (local_570 != pvStack_568) {
              if ((*(int *)((longlong)pvStack_568 + -0xc) != 0) &&
                 (*(int *)((longlong)pvStack_568 + -8) == 0)) {
                FUN_1402ccc90(local_578,&DAT_140a4d304);
              }
              *(undefined4 *)((longlong)pvStack_568 + -8) = 0;
              *(int *)((longlong)pvStack_568 + -0xc) = *(int *)((longlong)pvStack_568 + -0xc) + 1;
            }
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,iVar2);
            pvStack_568 = (void *)((longlong)pvStack_568 + -0xc);
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
            FUN_1404a1690(&local_578);
            FUN_1402ccc90(local_578,&DAT_140a508fc);
            pvStack_568 = (void *)((longlong)pvStack_568 + -0xc);
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
            FUN_1404a1690(&local_578);
            FUN_1402ccc90(local_578,&DAT_140a508fc);
          }
          pcVar1 = pcVar36 + 0x68;
          pcVar36 = pcVar36 + 0x638;
        } while (pcVar1 != pcVar37);
      }
      pvStack_568 = (void *)((longlong)pvStack_568 + -0xc);
      std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_578,FUN_1402cce70);
      if (0 < (int)((longlong)pvStack_568 - (longlong)local_570 >> 2) * -0x55555555) {
        do {
          FUN_1402ccc90(local_578,&DAT_140a5a868);
          uVar33 = (int)uVar29 + 1;
          uVar29 = (ulonglong)uVar33;
        } while ((int)uVar33 < (int)((longlong)pvStack_568 - (longlong)local_570 >> 2) * -0x55555555
                );
      }
      FUN_1402ccc90(local_578,&DAT_140a4b2fc);
      std::basic_ostream<char,std::char_traits<char>_>::flush(local_348);
      FUN_140279a60(local_3e8);
      pcVar36 = pcStack_4a8;
      *param_1 = CONCAT62(uStack_4ce,local_4d0);
      param_1[1] = uStack_4c8;
      param_1[2] = local_4c0;
      param_1[3] = uStack_4b8;
      uStack_4b8 = _UNK_140aac908;
      local_4c0 = _DAT_140aac900;
      local_4d0 = 0;
      *(undefined1 *)(param_1 + 4) = 1;
      for (pcVar37 = local_4b0; pcVar37 != pcVar36; pcVar37 = pcVar37 + 0x638) {
        FUN_140351080(pcVar37);
      }
      if (local_4b0 != (char *)0x0) {
        free(local_4b0);
      }
      if (local_570 != (void *)0x0) {
        free(local_570);
      }
      std::basic_ostream<char,std::char_traits<char>_>::_vbase_destructor_(local_348);
      local_3e8[0] = std::basic_filebuf<char,std::char_traits<char>_>::vftable;
      if ((local_368 != 0) && ((undefined1 *)*local_3d0 == local_378)) {
        *local_3d0 = local_360;
        *local_3b0 = local_360;
        *local_398 = (int)local_358 - (int)local_360;
      }
      if (local_36c != '\0') {
        FUN_140279a60(local_3e8);
      }
      std::basic_streambuf<char,std::char_traits<char>_>::
      ~basic_streambuf<char,std::char_traits<char>_>
                ((basic_streambuf<char,std::char_traits<char>_> *)local_3e8);
      if (7 < uStack_4b8) {
        pvVar6 = (void *)CONCAT62(uStack_4ce,local_4d0);
        pvVar25 = pvVar6;
        if ((0xfff < uStack_4b8 * 2 + 2) &&
           (pvVar25 = *(void **)((longlong)pvVar6 + -8),
           0x1f < (ulonglong)((longlong)pvVar6 + (-8 - (longlong)pvVar25)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar25);
      }
      local_4c0 = _DAT_140aac900;
      uStack_4b8 = _UNK_140aac908;
      local_4d0 = 0;
      if (0xf < uStack_4f8) {
        pvVar25 = local_510;
        if ((0xfff < uStack_4f8 + 1) &&
           (pvVar25 = *(void **)((longlong)local_510 + -8),
           0x1f < (ulonglong)((longlong)local_510 + (-8 - (longlong)pvVar25)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar25);
      }
      local_500 = _DAT_140aac910;
      uStack_4f8 = _UNK_140aac918;
      local_510 = (void *)((ulonglong)local_510 & 0xffffffffffffff00);
      if (7 < uStack_520) {
        pppppppuVar26 = local_538;
        if ((0xfff < uStack_520 * 2 + 2) &&
           (pppppppuVar26 = (undefined8 *******)local_538[-1],
           0x1f < (ulonglong)((longlong)local_538 + (-8 - (longlong)pppppppuVar26)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppuVar26);
      }
      local_538 = (undefined8 *******)((ulonglong)local_538 & 0xffffffffffff0000);
      local_528 = _DAT_140aac900;
      uStack_520 = _UNK_140aac908;
      FUN_140025470(local_408);
      if (0xf < uStack_4d8) {
        pvVar6 = (void *)CONCAT71(uStack_4ef,local_4f0);
        pvVar25 = pvVar6;
        if ((0xfff < uStack_4d8 + 1) &&
           (pvVar25 = *(void **)((longlong)pvVar6 + -8),
           0x1f < (ulonglong)((longlong)pvVar6 + (-8 - (longlong)pvVar25)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar25);
      }
      local_4e0 = _DAT_140aac910;
      uStack_4d8 = _UNK_140aac918;
      local_4f0 = 0;
      if (0xf < local_480) {
        pppppppuVar26 = local_498[0];
        if ((0xfff < local_480 + 1) &&
           (pppppppuVar26 = (undefined8 *******)local_498[0][-1],
           0x1f < (ulonglong)((longlong)local_498[0] + (-8 - (longlong)pppppppuVar26)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppuVar26);
      }
    }
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  FUN_140001c70();
}


// Incoming references
// 0xc2684c DATA caller none
// 0x74df89 UNCONDITIONAL_CALL caller 14074df70
