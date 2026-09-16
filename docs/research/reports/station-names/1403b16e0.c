// Candidate VA 1403b16e0; RVA 0x3b16e0
// Ghidra inferred prototype: undefined FUN_1403b16e0()

void FUN_1403b16e0(longlong *param_1,longlong *param_2,longlong *param_3,longlong *param_4)

{
  undefined8 *puVar1;
  bool bVar2;
  undefined1 auVar3 [16];
  undefined1 auVar4 [16];
  undefined1 auVar5 [16];
  undefined1 auVar6 [16];
  double dVar7;
  double dVar8;
  double dVar9;
  longlong lVar10;
  void *pvVar11;
  longlong *plVar12;
  undefined1 (*_Dst) [16];
  undefined1 (*pauVar13) [16];
  longlong lVar14;
  longlong *plVar15;
  undefined8 *puVar16;
  undefined8 *puVar17;
  undefined8 *puVar18;
  longlong *plVar19;
  undefined8 *puVar20;
  undefined8 *puVar21;
  undefined8 uVar22;
  longlong *plVar23;
  undefined1 (*pauVar24) [16];
  undefined1 (*_Src) [16];
  undefined1 (*pauVar25) [16];
  double dVar26;
  undefined4 uVar27;
  undefined4 uVar28;
  undefined4 uVar29;
  undefined1 local_108 [48];
  undefined1 local_d8 [152];

  param_1[0x62] = param_1[0x62] + 1;
  if (param_3[4] != 0) {
    param_1[99] = param_1[99] + 1;
  }
  pauVar25 = (undefined1 (*) [16])0x0;
  plVar19 = (longlong *)param_3[1];
  pauVar24 = (undefined1 (*) [16])0x0;
  _Dst = (undefined1 (*) [16])0x0;
  if (plVar19 != param_3) {
    uVar29 = (undefined4)DAT_140aac0c0;
    _Src = _Dst;
    do {
      lVar14 = plVar19[4];
      lVar10 = FUN_14032c3b0(*param_1 + 0x80,lVar14);
      _Dst = _Src;
      if (lVar10 != 0) {
        FUN_1403bd470(param_1 + 1,lVar14);
        plVar15 = param_1 + 0x56;
        pvVar11 = (void *)thunk_FUN_140983da8(0x28);
        *(longlong *)((longlong)pvVar11 + 0x20) = lVar14;
        bVar2 = true;
        plVar12 = plVar15;
        plVar23 = (longlong *)param_1[0x58];
        while (plVar23 != (longlong *)0x0) {
          bVar2 = lVar14 < plVar23[4];
          plVar12 = plVar23;
          if (lVar14 < plVar23[4]) {
            plVar23 = (longlong *)plVar23[1];
          }
          else {
            plVar23 = (longlong *)*plVar23;
          }
        }
        plVar23 = plVar12;
        if (bVar2) {
          if (plVar12 != (longlong *)param_1[0x57]) {
            plVar12 = (longlong *)FUN_14001dcf0(plVar12);
            goto LAB_1403b1806;
          }
LAB_1403b180c:
          if ((plVar23 == plVar15) || (lVar14 < plVar23[4])) {
            uVar22 = 0;
          }
          else {
            uVar22 = 1;
          }
          FUN_14001de10(pvVar11,plVar23,plVar15,uVar22);
          param_1[0x5a] = param_1[0x5a] + 1;
        }
        else {
LAB_1403b1806:
          if (plVar12[4] < lVar14) goto LAB_1403b180c;
          free(pvVar11);
        }
        plVar15 = param_1 + 0x50;
        pvVar11 = (void *)thunk_FUN_140983da8(0x28);
        *(longlong *)((longlong)pvVar11 + 0x20) = lVar14;
        bVar2 = true;
        plVar12 = plVar15;
        plVar23 = (longlong *)param_1[0x52];
        while (plVar23 != (longlong *)0x0) {
          bVar2 = lVar14 < plVar23[4];
          plVar12 = plVar23;
          if (lVar14 < plVar23[4]) {
            plVar23 = (longlong *)plVar23[1];
          }
          else {
            plVar23 = (longlong *)*plVar23;
          }
        }
        plVar23 = plVar12;
        if (bVar2) {
          if (plVar12 != (longlong *)param_1[0x51]) {
            plVar12 = (longlong *)FUN_14001dcf0(plVar12);
            goto LAB_1403b18a4;
          }
LAB_1403b18aa:
          if ((plVar23 == plVar15) || (lVar14 < plVar23[4])) {
            uVar22 = 0;
          }
          else {
            uVar22 = 1;
          }
          FUN_14001de10(pvVar11,plVar23,plVar15,uVar22);
          param_1[0x54] = param_1[0x54] + 1;
        }
        else {
LAB_1403b18a4:
          if (plVar12[4] < lVar14) goto LAB_1403b18aa;
          free(pvVar11);
        }
        dVar26 = (double)FUN_140249120(uVar29,lVar10 + 0x10);
        dVar8 = *(double *)(lVar10 + 0x10) - dVar26;
        dVar9 = *(double *)(lVar10 + 0x18) - dVar26;
        uVar28 = (undefined4)((ulonglong)dVar9 >> 0x20);
        dVar7 = (dVar26 + *(double *)(lVar10 + 0x10)) - dVar8;
        dVar26 = (dVar26 + *(double *)(lVar10 + 0x18)) - dVar9;
        uVar27 = (undefined4)((ulonglong)dVar26 >> 0x20);
        if ((0.0 < dVar7) && (0.0 < dVar26)) {
          if (pauVar24 < pauVar25) {
            auVar3._8_4_ = SUB84(dVar9,0);
            auVar3._0_8_ = dVar8;
            auVar3._12_4_ = uVar28;
            *pauVar24 = auVar3;
            auVar5._8_4_ = SUB84(dVar26,0);
            auVar5._0_8_ = dVar7;
            auVar5._12_4_ = uVar27;
            pauVar24[1] = auVar5;
            pauVar13 = pauVar24;
          }
          else {
            lVar14 = (longlong)pauVar24 - (longlong)_Src >> 5;
            if (lVar14 == 0) {
              lVar10 = 1;
LAB_1403b197b:
              lVar14 = lVar10 << 5;
              _Dst = (undefined1 (*) [16])thunk_FUN_140983da8(lVar14);
            }
            else {
              lVar10 = lVar14 * 2;
              if (lVar10 != 0) goto LAB_1403b197b;
              _Dst = (undefined1 (*) [16])0x0;
              lVar14 = lVar14 << 6;
            }
            pauVar13 = _Dst;
            if (_Src != pauVar24) {
              pvVar11 = memmove(_Dst,_Src,(longlong)pauVar24 - (longlong)_Src);
              pauVar13 = (undefined1 (*) [16])
                         ((longlong)pvVar11 +
                         ((longlong)pauVar24 - (longlong)_Src & 0xffffffffffffffe0U));
            }
            auVar4._8_4_ = SUB84(dVar9,0);
            auVar4._0_8_ = dVar8;
            auVar4._12_4_ = uVar28;
            *pauVar13 = auVar4;
            auVar6._8_4_ = SUB84(dVar26,0);
            auVar6._0_8_ = dVar7;
            auVar6._12_4_ = uVar27;
            pauVar13[1] = auVar6;
            if (_Src != (undefined1 (*) [16])0x0) {
              free(_Src);
            }
            pauVar25 = (undefined1 (*) [16])(*_Dst + lVar14);
          }
          pauVar24 = pauVar13 + 2;
        }
      }
      plVar15 = (longlong *)*plVar19;
      if (plVar15 == (longlong *)0x0) {
        plVar15 = (longlong *)plVar19[2];
        plVar12 = (longlong *)0x0;
        if (plVar19 == (longlong *)*plVar15) {
          do {
            plVar19 = plVar15;
            plVar15 = (longlong *)plVar19[2];
          } while (plVar19 == (longlong *)*plVar15);
          plVar12 = (longlong *)*plVar19;
        }
        if (plVar12 != plVar15) {
          plVar19 = plVar15;
        }
      }
      else {
        for (plVar12 = (longlong *)plVar15[1]; plVar19 = plVar15, plVar12 != (longlong *)0x0;
            plVar12 = (longlong *)plVar12[1]) {
          plVar15 = plVar12;
        }
      }
      _Src = _Dst;
    } while (plVar19 != param_3);
  }
  lVar14 = FUN_14031ff10(local_d8,param_2);
  uVar22 = FUN_14031ff10(local_108,lVar14);
  FUN_1403b05e0(param_1,uVar22,0);
  puVar1 = *(undefined8 **)(lVar14 + 0x10);
  while (puVar1 != (undefined8 *)0x0) {
    FUN_1402450d0(lVar14,*puVar1);
    puVar21 = (undefined8 *)puVar1[1];
    free(puVar1);
    puVar1 = puVar21;
  }
  plVar19 = (longlong *)param_4[1];
  do {
    if (plVar19 == param_4) {
      uVar22 = FUN_14031ff10(local_108,param_3);
      FUN_1403942e0(param_1,uVar22);
      plVar19 = (longlong *)param_2[1];
      while (plVar19 != param_2) {
        FUN_1403aaa20(param_1,plVar19[4]);
        plVar15 = (longlong *)*plVar19;
        if (plVar15 == (longlong *)0x0) {
          plVar15 = (longlong *)plVar19[2];
          plVar12 = (longlong *)0x0;
          if (plVar19 == (longlong *)*plVar15) {
            do {
              plVar19 = plVar15;
              plVar15 = (longlong *)plVar19[2];
            } while (plVar19 == (longlong *)*plVar15);
            plVar12 = (longlong *)*plVar19;
          }
          if (plVar12 != plVar15) {
            plVar19 = plVar15;
          }
        }
        else {
          for (plVar12 = (longlong *)plVar15[1]; plVar19 = plVar15, plVar12 != (longlong *)0x0;
              plVar12 = (longlong *)plVar12[1]) {
            plVar15 = plVar12;
          }
        }
      }
      plVar19 = (longlong *)param_3[1];
      while (pauVar25 = _Dst, plVar19 != param_3) {
        FUN_1403914d0(param_1,plVar19[4]);
        plVar15 = (longlong *)*plVar19;
        if (plVar15 == (longlong *)0x0) {
          plVar15 = (longlong *)plVar19[2];
          plVar12 = (longlong *)0x0;
          if (plVar19 == (longlong *)*plVar15) {
            do {
              plVar19 = plVar15;
              plVar15 = (longlong *)plVar19[2];
            } while (plVar19 == (longlong *)*plVar15);
            plVar12 = (longlong *)*plVar19;
          }
          if (plVar12 != plVar15) {
            plVar19 = plVar15;
          }
        }
        else {
          for (plVar12 = (longlong *)plVar15[1]; plVar19 = plVar15, plVar12 != (longlong *)0x0;
              plVar12 = (longlong *)plVar12[1]) {
            plVar15 = plVar12;
          }
        }
      }
      for (; pauVar25 != pauVar24; pauVar25 = pauVar25 + 2) {
        FUN_140390ea0(param_1,pauVar25);
      }
      if (_Dst != (undefined1 (*) [16])0x0) {
        free(_Dst);
      }
      return;
    }
    lVar14 = *param_1;
    plVar15 = (longlong *)FUN_14033f860(lVar14 + 0x100,plVar19[4]);
    if (plVar15 != (longlong *)0x0) {
      puVar1 = *(undefined8 **)(lVar14 + 0x170);
      puVar21 = *(undefined8 **)(lVar14 + 0x168);
      if (puVar21 != puVar1) {
        puVar20 = puVar21 + 0xc;
        do {
          if ((undefined8 *)puVar20[2] == (undefined8 *)0x0) {
LAB_1403b1b42:
            if ((undefined8 *)puVar20[-10] != (undefined8 *)0x0) {
              puVar16 = (undefined8 *)puVar20[-10];
              puVar18 = puVar21;
              do {
                if ((longlong)puVar16[4] < *plVar15) {
                  puVar17 = (undefined8 *)*puVar16;
                }
                else {
                  puVar17 = (undefined8 *)puVar16[1];
                  puVar18 = puVar16;
                }
                puVar16 = puVar17;
              } while (puVar17 != (undefined8 *)0x0);
              if ((puVar18 != puVar21) && ((longlong)puVar18[4] <= *plVar15)) goto LAB_1403b1b89;
            }
            uVar22 = FUN_14033b730(puVar21,plVar15);
            FUN_140339e30(uVar22,plVar15);
          }
          else {
            puVar16 = (undefined8 *)puVar20[2];
            puVar18 = puVar20;
            do {
              if ((longlong)puVar16[4] < *plVar15) {
                puVar17 = (undefined8 *)*puVar16;
              }
              else {
                puVar17 = (undefined8 *)puVar16[1];
                puVar18 = puVar16;
              }
              puVar16 = puVar17;
            } while (puVar17 != (undefined8 *)0x0);
            if ((puVar18 == puVar20) || (*plVar15 < (longlong)puVar18[4])) goto LAB_1403b1b42;
          }
LAB_1403b1b89:
          puVar21 = puVar21 + 0x18;
          puVar20 = puVar20 + 0x18;
        } while (puVar21 != puVar1);
      }
      if (plVar15[0x15] == 0) {
        uVar22 = 0;
      }
      else {
        uVar22 = FUN_14032c420(*param_1);
      }
      FUN_1403882d0(plVar15,uVar22);
      FUN_140380510(param_1 + 0x1d,*plVar15,plVar15 + 0x19,param_1[0x62]);
      FUN_1403aaa20(param_1,*plVar15);
    }
    plVar15 = (longlong *)*plVar19;
    if (plVar15 == (longlong *)0x0) {
      plVar15 = (longlong *)plVar19[2];
      plVar12 = (longlong *)0x0;
      if (plVar19 == (longlong *)*plVar15) {
        do {
          plVar19 = plVar15;
          plVar15 = (longlong *)plVar19[2];
        } while (plVar19 == (longlong *)*plVar15);
        plVar12 = (longlong *)*plVar19;
      }
      if (plVar12 != plVar15) {
        plVar19 = plVar15;
      }
    }
    else {
      for (plVar12 = (longlong *)plVar15[1]; plVar19 = plVar15, plVar12 != (longlong *)0x0;
          plVar12 = (longlong *)plVar12[1]) {
        plVar15 = plVar12;
      }
    }
  } while( true );
}


// Incoming references
// 0xc1ee54 DATA caller none
// 0x3aeb92 UNCONDITIONAL_CALL caller 1403ad950
// 0x3b36c8 UNCONDITIONAL_CALL caller 1403b2480
// 0x3b3cce UNCONDITIONAL_CALL caller 1403b3970
