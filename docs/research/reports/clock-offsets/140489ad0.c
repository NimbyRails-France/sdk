
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140489ad0(longlong param_1)

{
  undefined8 *puVar1;
  longlong lVar2;
  undefined1 *puVar3;
  void *pvVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  char cVar13;
  bool bVar14;
  longlong lVar15;
  longlong *plVar16;
  void *pvVar17;
  undefined8 *puVar18;
  longlong *plVar19;
  longlong lVar20;
  longlong *plVar21;
  ulonglong uVar22;
  ulonglong uVar23;
  undefined8 *puVar24;
  ulonglong uVar25;
  ulonglong uVar26;
  longlong lVar27;
  undefined8 local_5a8;
  undefined8 uStack_5a0;
  undefined8 local_598;
  undefined8 uStack_590;
  undefined8 local_588;
  undefined8 uStack_580;
  ulonglong local_578;
  ulonglong uStack_570;
  undefined8 *local_568;
  undefined8 *puStack_560;
  longlong local_558;
  void *local_550;
  undefined8 local_548;
  longlong local_538;
  undefined4 local_530;
  undefined4 uStack_52c;
  undefined4 uStack_528;
  undefined4 uStack_524;
  undefined4 local_520;
  undefined4 uStack_51c;
  undefined4 uStack_518;
  undefined4 uStack_514;
  char local_510;
  undefined8 *local_508;
  void *local_4f8;
  undefined8 uStack_4f0;
  ulonglong local_4e8;
  ulonglong uStack_4e0;
  undefined4 local_4d8 [2];
  undefined1 local_4d0 [32];
  undefined8 local_4b0;
  undefined8 uStack_4a8;
  undefined8 local_4a0;
  undefined8 local_498;
  undefined8 local_490;
  undefined1 local_488 [104];
  undefined1 local_420 [48];
  undefined8 local_3f0;
  undefined8 local_3e8;
  undefined8 local_3e0;
  undefined8 local_3d8;
  undefined8 local_3d0;
  undefined8 local_3c8;
  undefined8 local_3c0;
  undefined8 local_3b8;
  undefined8 local_3b0;
  longlong local_3a8 [2];
  basic_streambuf<char,std::char_traits<char>_> local_398 [8];
  basic_istream<char,std::char_traits<char>_> local_390 [152];
  basic_ios<char,std::char_traits<char>_> local_2f8 [96];
  undefined1 local_298 [28];
  int iStack_27c;
  longlong local_278 [2];
  basic_ostream<char,std::char_traits<char>_> local_268 [8];
  undefined **local_260;
  basic_iostream<char,std::char_traits<char>_> local_258 [120];
  basic_ios<char,std::char_traits<char>_> local_1e0 [104];
  undefined4 local_178 [2];
  undefined1 local_170 [264];
  undefined8 local_68;

  FUN_1402e0680();
  FUN_1402e0960();
  FUN_1402d8e30();
  lVar2 = param_1 + 0xa80;
  FUN_1403fd500(local_488);
  FUN_1403fee50(param_1 + 0xfe8,local_488);
  puVar3 = (undefined1 *)(param_1 + 0x1050);
  if (puVar3 != local_420) {
    FUN_140364730(puVar3);
    FUN_14032a510(puVar3,local_420);
  }
  *(undefined8 *)(param_1 + 0x1080) = local_3f0;
  *(undefined8 *)(param_1 + 0x1088) = local_3e8;
  *(undefined8 *)(param_1 + 0x1090) = local_3e0;
  *(undefined8 *)(param_1 + 0x1098) = local_3d8;
  *(undefined8 *)(param_1 + 0x10a0) = local_3d0;
  *(undefined8 *)(param_1 + 0x10a8) = local_3c8;
  *(undefined8 *)(param_1 + 0x10b0) = local_3c0;
  *(undefined8 *)(param_1 + 0x10b8) = local_3b8;
  *(undefined8 *)(param_1 + 0x10c0) = local_3b0;
  FUN_14029aae0(local_420);
  FUN_14034f3e0(local_488);
  FUN_1403fe640(lVar2);
  FUN_140407310(lVar2);
  FUN_140409b80(lVar2);
  FUN_140414610(lVar2);
  FUN_1404149a0(lVar2);
  plVar21 = (longlong *)*DAT_140b77eb0;
  cVar13 = *(char *)((longlong)plVar21 + 0x19);
  while (cVar13 == '\0') {
    if (((char)plVar21[0x2a] != '\0') && (cVar13 = FUN_1402dd820(plVar21 + 9), cVar13 != '\0')) {
      FUN_14048f930(param_1 + 0x10c8,plVar21 + 9);
    }
    plVar16 = (longlong *)plVar21[2];
    if (*(char *)((longlong)plVar16 + 0x19) == '\0') {
      cVar13 = *(char *)(*plVar16 + 0x19);
      plVar21 = plVar16;
      plVar16 = (longlong *)*plVar16;
      while (cVar13 == '\0') {
        cVar13 = *(char *)(*plVar16 + 0x19);
        plVar21 = plVar16;
        plVar16 = (longlong *)*plVar16;
      }
    }
    else {
      cVar13 = *(char *)(plVar21[1] + 0x19);
      plVar19 = (longlong *)plVar21[1];
      plVar16 = plVar21;
      while ((plVar21 = plVar19, cVar13 == '\0' && (plVar16 == (longlong *)plVar21[2]))) {
        cVar13 = *(char *)(plVar21[1] + 0x19);
        plVar19 = (longlong *)plVar21[1];
        plVar16 = plVar21;
      }
    }
    cVar13 = *(char *)((longlong)plVar21 + 0x19);
  }
  uVar22 = *(ulonglong *)(param_1 + 0x10d0);
  uVar26 = *(ulonglong *)(param_1 + 0x10d8);
  if (uVar22 != uVar26) {
    uVar25 = uVar22 + 0x110;
    do {
      cVar13 = FUN_1402dd820(uVar22);
      if (cVar13 == '\0') {
        if (uVar25 < uVar26) {
          FUN_140497730(uVar25,uVar26);
        }
        *(longlong *)(param_1 + 0x10d8) = *(longlong *)(param_1 + 0x10d8) + -0x110;
        FUN_1402dfc60(*(undefined8 *)(param_1 + 0x10d8));
      }
      else {
        uVar22 = uVar22 + 0x110;
        uVar25 = uVar25 + 0x110;
      }
      uVar26 = *(ulonglong *)(param_1 + 0x10d8);
    } while (uVar22 != uVar26);
  }
  uVar26 = _UNK_140aac918;
  uVar22 = _DAT_140aac910;
  lVar27 = *(longlong *)(param_1 + 0x10d0);
  lVar20 = *(longlong *)(param_1 + 0x10d8);
  local_538 = lVar20;
  if (lVar27 != lVar20) {
    do {
      FUN_1402e0b70(local_178,lVar27);
      cVar13 = '\0';
      if ((char)local_68 != '\0') {
        local_4d8[0] = local_178[0];
        FUN_140019ca0(local_4d0,local_170);
        uStack_4a8 = 0;
        local_4a0 = 7;
        local_498 = 0xf;
        local_4b0 = (ulonglong)
                    CONCAT16(DAT_140a59828._6_1_,
                             CONCAT24(DAT_140a59828._4_2_,(undefined4)DAT_140a59828));
        local_490 = 0;
        lVar15 = FUN_1402dcd00(local_4d8,local_298);
        if (*(longlong *)(lVar15 + 0x10) == 0) {
          local_510 = '\0';
        }
        else {
          uStack_4f0 = 0;
          local_4e8 = uVar22;
          uStack_4e0 = uVar26;
          local_4f8 = (void *)0x0;
          FUN_140277450(local_3a8,lVar15);
          bVar14 = std::ios_base::good((ios_base *)
                                       ((longlong)local_3a8 + (longlong)*(int *)(local_3a8[0] + 4)))
          ;
          if (bVar14) {
            FUN_1402786e0(local_278);
            std::basic_ostream<char,std::char_traits<char>_>::operator<<(local_268,local_398);
            FUN_140279c10(&local_260,&local_5a8);
            local_530 = (undefined4)local_5a8;
            uStack_52c = local_5a8._4_4_;
            uStack_528 = (undefined4)uStack_5a0;
            uStack_524 = uStack_5a0._4_4_;
            local_520 = (undefined4)local_598;
            uStack_51c = local_598._4_4_;
            uStack_518 = (undefined4)uStack_590;
            uStack_514 = uStack_590._4_4_;
            local_510 = '\x01';
            local_598 = uVar22;
            uStack_590 = uVar26;
            local_5a8 = (void *)((ulonglong)local_5a8 & 0xffffffffffffff00);
            *(undefined ***)((longlong)local_278 + (longlong)*(int *)(local_278[0] + 4)) =
                 std::basic_stringstream<char,std::char_traits<char>,std::allocator<char>_>::vftable
            ;
            *(int *)((longlong)&iStack_27c + (longlong)*(int *)(local_278[0] + 4)) =
                 *(int *)(local_278[0] + 4) + -0x98;
            local_260 = std::basic_stringbuf<char,std::char_traits<char>,std::allocator<char>_>::
                        vftable;
            FUN_140279b20(&local_260);
            std::basic_streambuf<char,std::char_traits<char>_>::
            ~basic_streambuf<char,std::char_traits<char>_>
                      ((basic_streambuf<char,std::char_traits<char>_> *)&local_260);
            std::basic_iostream<char,std::char_traits<char>_>::
            ~basic_iostream<char,std::char_traits<char>_>(local_258);
            std::basic_ios<char,std::char_traits<char>_>::~basic_ios<char,std::char_traits<char>_>
                      (local_1e0);
          }
          else {
            local_510 = '\0';
          }
          *(undefined ***)((longlong)local_3a8 + (longlong)*(int *)(local_3a8[0] + 4)) =
               std::basic_ifstream<char,std::char_traits<char>_>::vftable;
          *(int *)((longlong)local_3a8 + (longlong)*(int *)(local_3a8[0] + 4) + -4) =
               *(int *)(local_3a8[0] + 4) + -0xb0;
          FUN_140278670(local_398);
          std::basic_istream<char,std::char_traits<char>_>::
          ~basic_istream<char,std::char_traits<char>_>(local_390);
          std::basic_ios<char,std::char_traits<char>_>::~basic_ios<char,std::char_traits<char>_>
                    (local_2f8);
        }
        FUN_140025470(local_298);
        if (local_510 != '\0') {
          local_5a8 = (void *)0x0;
          uStack_5a0 = 0;
          local_598 = 0;
          uStack_590 = 0;
          uVar25 = CONCAT44(uStack_51c,local_520);
          puVar18 = (undefined8 *)&local_530;
          if (0xf < CONCAT44(uStack_514,uStack_518)) {
            puVar18 = (undefined8 *)CONCAT44(uStack_52c,local_530);
          }
          if (0x7fffffffffffffff < uVar25) {
                    /* WARNING: Subroutine does not return */
            FUN_140001c70();
          }
          if (uVar25 < 0x10) {
            uStack_590 = 0xf;
            local_5a8 = (void *)*puVar18;
            uStack_5a0 = puVar18[1];
            local_598 = uVar25;
          }
          else {
            uVar23 = uVar25 | 0xf;
            if (uVar23 < 0x8000000000000000) {
              if (uVar23 < 0x16) {
                uVar23 = 0x16;
              }
            }
            else {
              uVar23 = 0x7fffffffffffffff;
            }
            local_5a8 = (void *)FUN_140003270(uVar23 + 1);
            local_598 = uVar25;
            uStack_590 = uVar23;
            memcpy(local_5a8,puVar18,uVar25 + 1);
          }
          local_508 = &local_5a8;
          uStack_580 = 0;
          local_578 = uVar22;
          uStack_570 = uVar26;
          local_588 = 0;
          local_568 = (undefined8 *)0x0;
          puStack_560 = (undefined8 *)0x0;
          local_558 = 0;
          local_550 = (void *)0x0;
          local_548 = 0;
          local_550 = (void *)FUN_140983da8(0x48);
          *(void **)local_550 = local_550;
          *(void **)((longlong)local_550 + 8) = local_550;
          *(void **)((longlong)local_550 + 0x10) = local_550;
          *(undefined2 *)((longlong)local_550 + 0x18) = 0x101;
          local_4f8 = local_5a8;
          uStack_4f0 = uStack_5a0;
          local_4e8 = local_598;
          uStack_4e0 = uStack_590;
          local_5a8 = (void *)((ulonglong)local_5a8 & 0xffffffffffffff00);
          local_598 = 0;
          uStack_590 = 0xf;
          FUN_140271bd0(&local_588,&local_4f8);
          if (0xf < uStack_590) {
            pvVar17 = local_5a8;
            if ((0xfff < uStack_590 + 1) &&
               (pvVar17 = *(void **)((longlong)local_5a8 + -8),
               0x1f < (ulonglong)((longlong)local_5a8 + (-8 - (longlong)pvVar17)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(pvVar17);
          }
          local_598 = 0;
          uStack_590 = 0xf;
          local_5a8 = (void *)((ulonglong)local_5a8 & 0xffffffffffffff00);
          bVar14 = false;
          bVar6 = false;
          bVar7 = false;
          bVar8 = false;
          bVar12 = false;
          bVar9 = false;
          bVar11 = false;
          bVar10 = false;
          puVar18 = local_568;
          if (local_568 == puStack_560) {
LAB_14048a3ba:
            *(undefined1 *)(lVar27 + 0x10a) = 0;
          }
          else {
            do {
              plVar21 = (longlong *)*puVar18;
              bVar5 = true;
              if ((((puVar18[1] != 0x11) || (*plVar21 != 0x6c754d6e69617254)) ||
                  (plVar21[1] != 0x696e55656c706974)) || ((char)plVar21[2] != 't')) {
                if (puVar18[1] == 9) {
                  if ((*plVar21 != 0x696e556e69617254) || ((char)plVar21[1] != 't')) {
                    if ((puVar18[1] != 9) ||
                       ((*plVar21 != 0x6e694b6b63617254 || ((char)plVar21[1] != 'd'))))
                    goto LAB_14048a1c2;
                    bVar6 = true;
                    bVar5 = bVar14;
                  }
                }
                else {
LAB_14048a1c2:
                  if ((puVar18[1] == 0xc) &&
                     ((*plVar21 == 0x676e69646c697542 && ((int)plVar21[1] == 0x646e694b)))) {
                    bVar7 = true;
                    bVar5 = bVar14;
                  }
                  else if ((puVar18[1] == 8) && (*plVar21 == 0x726579614c494f50)) {
                    bVar8 = true;
                    bVar5 = bVar14;
                  }
                  else if ((((puVar18[1] == 0xb) && (*plVar21 == 0x7543646e616d6544)) &&
                           ((short)plVar21[1] == 0x7672)) &&
                          (*(char *)((longlong)plVar21 + 10) == 'e')) {
                    bVar12 = true;
                    bVar5 = bVar14;
                  }
                  else if (((puVar18[1] == 6) && ((int)*plVar21 == 0x69726353)) &&
                          (*(short *)((longlong)plVar21 + 4) == 0x7470)) {
                    bVar9 = true;
                    bVar5 = bVar14;
                  }
                  else if (((puVar18[1] == 0xe) && (*plVar21 == 0x65546c616e676953)) &&
                          (((int)plVar21[1] == 0x72757478 &&
                           (*(short *)((longlong)plVar21 + 0xc) == 0x7365)))) {
                    bVar11 = true;
                    bVar5 = bVar14;
                    bVar10 = true;
                  }
                  else {
                    bVar5 = bVar14;
                    if (puVar18[1] == 0xe) {
                      lVar20 = *plVar21 + -0x65546c616e676953;
                      if ((lVar20 == 0) &&
                         (lVar20 = (ulonglong)*(uint *)(plVar21 + 1) - 0x616c706d, lVar20 == 0)) {
                        lVar20 = (ulonglong)*(ushort *)((longlong)plVar21 + 0xc) - 0x6574;
                      }
                      bVar10 = bVar11;
                      if (lVar20 == 0) {
                        bVar11 = true;
                        bVar10 = true;
                      }
                    }
                  }
                }
              }
              puVar18 = puVar18 + 7;
              bVar14 = bVar5;
            } while (puVar18 != puStack_560);
            if (bVar5) {
              FUN_14040ad90(lVar2,local_178,&local_588);
            }
            if (bVar6) {
              FUN_14040cb70(lVar2,local_178,&local_588);
            }
            if (bVar7) {
              FUN_14040e310(lVar2,local_178,&local_588);
            }
            if ((bVar8) || (bVar12)) {
              FUN_14040fff0(lVar2,local_178,&local_588);
            }
            if (bVar9) {
              FUN_140411e70(lVar2,local_178,&local_588);
            }
            if (bVar10) {
              FUN_14040dcc0(lVar2,local_178,&local_588);
            }
            if ((((!bVar5) && (!bVar6)) && (!bVar7)) &&
               (((!bVar8 && (!bVar12)) && ((!bVar9 && (!bVar10)))))) goto LAB_14048a3ba;
            *(undefined1 *)(lVar27 + 0x10a) = 1;
            FUN_1402e40e0(lVar27,local_178);
          }
          FUN_140279620(&local_550,&local_550);
          free(local_550);
          puVar18 = puStack_560;
          if (local_568 != (undefined8 *)0x0) {
            if (local_568 != puStack_560) {
              puVar24 = local_568 + 5;
              do {
                FUN_140279620(puVar24,puVar24);
                free((void *)*puVar24);
                FUN_140015e70(puVar24 + -3);
                puVar1 = puVar24 + 2;
                puVar24 = puVar24 + 7;
              } while (puVar1 != puVar18);
            }
            puVar18 = local_568;
            if ((0xfff < (ulonglong)((local_558 - (longlong)local_568 >> 3) * 8)) &&
               (puVar18 = (undefined8 *)local_568[-1],
               0x1f < (ulonglong)((longlong)local_568 + (-8 - (longlong)puVar18)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(puVar18);
            local_568 = (undefined8 *)0x0;
            puStack_560 = (undefined8 *)0x0;
            local_558 = 0;
          }
          lVar20 = local_538;
          if (0xf < uStack_570) {
            FUN_140003040(&local_588,local_588);
            lVar20 = local_538;
          }
        }
        if ((local_510 != '\0') && (0xf < CONCAT44(uStack_514,uStack_518))) {
          pvVar4 = (void *)CONCAT44(uStack_52c,local_530);
          pvVar17 = pvVar4;
          if ((0xfff < CONCAT44(uStack_514,uStack_518) + 1) &&
             (pvVar17 = *(void **)((longlong)pvVar4 + -8),
             0x1f < (ulonglong)((longlong)pvVar4 + (-8 - (longlong)pvVar17)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar17);
        }
        FUN_140002d30(&local_4b0);
        FUN_140002d30(local_4d0);
        cVar13 = (char)local_68;
      }
      if (cVar13 != '\0') {
        FUN_1402dfc60(local_178);
      }
      lVar27 = lVar27 + 0x110;
    } while (lVar27 != lVar20);
  }
  plVar21 = *(longlong **)(param_1 + 0xb00);
  while (plVar21 != (longlong *)(param_1 + 0xaf8)) {
    FUN_1404128c0(lVar2,plVar21 + 4);
    plVar16 = (longlong *)*plVar21;
    if (plVar16 == (longlong *)0x0) {
      plVar16 = (longlong *)plVar21[2];
      plVar19 = (longlong *)0x0;
      if (plVar21 == (longlong *)*plVar16) {
        do {
          plVar21 = plVar16;
          plVar16 = (longlong *)plVar21[2];
        } while (plVar21 == (longlong *)*plVar16);
        plVar19 = (longlong *)*plVar21;
      }
      if (plVar19 != plVar16) {
        plVar21 = plVar16;
      }
    }
    else {
      for (plVar19 = (longlong *)plVar16[1]; plVar21 = plVar16, plVar19 != (longlong *)0x0;
          plVar19 = (longlong *)plVar19[1]) {
        plVar16 = plVar19;
      }
    }
  }
  FUN_140489780(param_1);
  *(int *)(param_1 + 0xfe0) = *(int *)(param_1 + 0xfe0) + 1;
  return;
}
