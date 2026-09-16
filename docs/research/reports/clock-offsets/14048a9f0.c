
/* WARNING: Function: __chkstk replaced with injection: alloca_probe */

void FUN_14048a9f0(longlong param_1,longlong *param_2,longlong *param_3,longlong *param_4,
                  longlong *param_5)

{
  int iVar1;
  undefined8 *puVar2;
  undefined4 *puVar3;
  int *piVar4;
  ulonglong uVar5;
  longlong lVar6;
  ulonglong *puVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  bool bVar10;
  longlong lVar11;
  byte bVar12;
  byte bVar13;
  char cVar14;
  void *pvVar15;
  undefined8 ****ppppuVar16;
  longlong lVar17;
  longlong lVar18;
  longlong *plVar19;
  ulonglong *puVar20;
  undefined8 ****ppppuVar21;
  ulonglong *puVar22;
  ulonglong *puVar23;
  longlong lVar24;
  longlong lVar25;
  undefined8 *puVar26;
  ulonglong *puVar27;
  undefined1 *puVar28;
  ulonglong *puVar29;
  undefined8 *puVar30;
  undefined8 ****ppppuVar31;
  longlong lVar32;
  ulonglong *puVar33;
  undefined8 uVar34;
  int *piVar35;
  int *piVar36;
  undefined8 *puVar37;
  undefined4 *puVar38;
  undefined8 ***local_3200;
  undefined8 ***pppuStack_31f8;
  undefined8 ***local_31f0;
  undefined8 uStack_31e8;
  longlong local_31e0;
  undefined8 ***local_31d0;
  undefined8 ***pppuStack_31c8;
  undefined8 ***local_31c0;
  undefined8 uStack_31b8;
  longlong local_31b0;
  undefined8 ***local_31a0;
  undefined8 ***pppuStack_3198;
  undefined8 ***local_3190;
  undefined8 uStack_3188;
  longlong local_3180;
  longlong local_3170;
  undefined1 local_3168 [24];
  undefined8 local_3150;
  undefined8 uStack_3148;
  undefined8 local_3140;
  undefined8 uStack_3138;
  undefined1 local_3130;
  undefined4 local_312c;
  undefined8 local_3128;
  undefined8 uStack_3120;
  undefined8 local_3118;
  undefined8 uStack_3110;
  undefined8 local_3108;
  undefined8 uStack_3100;
  undefined8 local_30f8;
  undefined8 uStack_30f0;
  undefined1 local_30e8 [2056];
  undefined1 local_28e0 [2056];
  undefined1 local_20d8 [2056];
  undefined1 local_18d0 [6280];
  undefined8 uStack_48;

  uStack_48 = 0x14048aa22;
  local_31a0 = &local_31a0;
  pppuStack_3198 = &local_31a0;
  local_3190 = (undefined8 ****)0x0;
  uStack_3188 = 0;
  local_3180 = 0;
  local_31d0 = &local_31d0;
  pppuStack_31c8 = &local_31d0;
  local_31c0 = (undefined8 ****)0x0;
  uStack_31b8 = 0;
  local_31b0 = 0;
  bVar10 = false;
  lVar24 = *(longlong *)(param_1 + 0xb78);
  for (lVar25 = *(longlong *)(param_1 + 0xb70); lVar25 != lVar24; lVar25 = lVar25 + 0x1880) {
    if (*(char *)(lVar25 + 0x20) != '\0') {
      pvVar15 = (void *)thunk_FUN_140983da8(0x40);
      FUN_140019ca0((longlong)pvVar15 + 0x20,lVar25);
      bVar13 = 1;
      ppppuVar21 = &local_31a0;
      ppppuVar16 = (undefined8 ****)local_3190;
      while (ppppuVar16 != (undefined8 ****)0x0) {
        bVar12 = FUN_14023b690((longlong)pvVar15 + 0x20,ppppuVar16 + 4);
        bVar13 = bVar12 >> 7;
        ppppuVar21 = ppppuVar16;
        if ((char)bVar12 < '\0') {
          ppppuVar16 = (undefined8 ****)ppppuVar16[1];
        }
        else {
          ppppuVar16 = (undefined8 ****)*ppppuVar16;
        }
      }
      ppppuVar16 = ppppuVar21;
      if (bVar13 == 0) {
LAB_14048ab1a:
        cVar14 = FUN_14023b690(ppppuVar16 + 4);
        if (-1 < cVar14) {
          FUN_140002d30((longlong)pvVar15 + 0x20);
          free(pvVar15);
          goto LAB_14048ab73;
        }
      }
      else if (ppppuVar21 != (undefined8 ****)pppuStack_3198) {
        ppppuVar16 = (undefined8 ****)FUN_14001dcf0();
        goto LAB_14048ab1a;
      }
      if ((ppppuVar21 == &local_31a0) ||
         (cVar14 = FUN_14023b690((longlong)pvVar15 + 0x20,ppppuVar21 + 4), cVar14 < '\0')) {
        uVar34 = 0;
      }
      else {
        uVar34 = 1;
      }
      FUN_14001de10(pvVar15,ppppuVar21,&local_31a0,uVar34);
      local_3180 = local_3180 + 1;
    }
LAB_14048ab73:
  }
  lVar24 = *(longlong *)(param_1 + 0xb90);
  for (lVar25 = *(longlong *)(param_1 + 0xb88); lVar25 != lVar24; lVar25 = lVar25 + 0x128) {
    if (*(char *)(lVar25 + 0x20) != '\0') {
      pvVar15 = (void *)thunk_FUN_140983da8(0x40);
      FUN_140019ca0((longlong)pvVar15 + 0x20,lVar25);
      bVar13 = 1;
      ppppuVar21 = &local_31d0;
      ppppuVar16 = (undefined8 ****)local_31c0;
      while (ppppuVar16 != (undefined8 ****)0x0) {
        bVar12 = FUN_14023b690((longlong)pvVar15 + 0x20,ppppuVar16 + 4);
        bVar13 = bVar12 >> 7;
        ppppuVar21 = ppppuVar16;
        if ((char)bVar12 < '\0') {
          ppppuVar16 = (undefined8 ****)ppppuVar16[1];
        }
        else {
          ppppuVar16 = (undefined8 ****)*ppppuVar16;
        }
      }
      ppppuVar16 = ppppuVar21;
      if (bVar13 == 0) {
LAB_14048ac2a:
        cVar14 = FUN_14023b690(ppppuVar16 + 4);
        if (-1 < cVar14) {
          FUN_140002d30((longlong)pvVar15 + 0x20);
          free(pvVar15);
          goto LAB_14048ac85;
        }
      }
      else if (ppppuVar21 != (undefined8 ****)pppuStack_31c8) {
        ppppuVar16 = (undefined8 ****)FUN_14001dcf0();
        goto LAB_14048ac2a;
      }
      if ((ppppuVar21 == &local_31d0) ||
         (cVar14 = FUN_14023b690((longlong)pvVar15 + 0x20,ppppuVar21 + 4), cVar14 < '\0')) {
        uVar34 = 0;
      }
      else {
        uVar34 = 1;
      }
      FUN_14001de10(pvVar15,ppppuVar21,&local_31d0,uVar34);
      local_31b0 = local_31b0 + 1;
    }
LAB_14048ac85:
  }
  puVar26 = (undefined8 *)*param_2;
  puVar2 = (undefined8 *)param_2[1];
  if (puVar26 != puVar2) {
    puVar37 = puVar26 + 8;
    do {
      ppppuVar21 = &local_31a0;
      ppppuVar16 = (undefined8 ****)local_3190;
      if ((undefined8 ****)local_3190 == (undefined8 ****)0x0) {
LAB_14048ad0a:
        *(undefined4 *)((longlong)puVar37 + -0x1c) = 0xffffffff;
        puVar28 = local_18d0;
        puVar30 = &local_3150;
        for (lVar25 = 0x1880; lVar25 != 0; lVar25 = lVar25 + -1) {
          *(undefined1 *)puVar30 = *puVar28;
          puVar28 = puVar28 + 1;
          puVar30 = (undefined8 *)((longlong)puVar30 + 1);
        }
        local_3150 = *puVar26;
        uStack_3148 = puVar26[1];
        local_3140 = puVar26[2];
        uStack_3138 = puVar26[3];
        puVar37[-6] = 0;
        puVar37[-5] = 0xf;
        *(undefined1 *)puVar26 = 0;
        local_3130 = *(undefined1 *)(puVar37 + -4);
        local_312c = *(undefined4 *)((longlong)puVar37 + -0x1c);
        local_3128 = puVar37[-3];
        uStack_3120 = puVar37[-2];
        local_3118 = puVar37[-1];
        uStack_3110 = *puVar37;
        puVar37[-1] = 0;
        *puVar37 = 0xf;
        *(undefined1 *)(puVar37 + -3) = 0;
        local_3108 = puVar37[1];
        uStack_3100 = puVar37[2];
        local_30f8 = puVar37[3];
        uStack_30f0 = puVar37[4];
        puVar37[3] = 0;
        puVar37[4] = 0xf;
        *(undefined1 *)(puVar37 + 1) = 0;
        FUN_14033bbc0(local_30e8,puVar37 + 5);
        FUN_14033bbc0(local_28e0,puVar37 + 0x106);
        FUN_14033bbc0(local_20d8,puVar37 + 0x207);
        local_312c = 0xffffffff;
        FUN_1404171d0(param_1 + 0xb70,&local_3150,1);
        FUN_14032c6e0(&local_3150);
        bVar10 = true;
      }
      else {
        do {
          cVar14 = FUN_14023b690(ppppuVar16 + 4,puVar26);
          if (cVar14 < '\0') {
            ppppuVar16 = (undefined8 ****)*ppppuVar16;
          }
          else {
            ppppuVar21 = ppppuVar16;
            ppppuVar16 = (undefined8 ****)ppppuVar16[1];
          }
        } while (ppppuVar16 != (undefined8 ****)0x0);
        if ((ppppuVar21 == &local_31a0) || (cVar14 = FUN_14023b690(puVar26), cVar14 < '\0'))
        goto LAB_14048ad0a;
      }
      puVar26 = puVar26 + 0x310;
      puVar37 = puVar37 + 0x310;
    } while (puVar26 != puVar2);
  }
  lVar24 = param_3[1];
  ppppuVar21 = (undefined8 ****)local_31c0;
  for (lVar25 = *param_3; local_31c0 = ppppuVar21, lVar25 != lVar24; lVar25 = lVar25 + 0x128) {
    ppppuVar16 = &local_31d0;
    if (ppppuVar21 == (undefined8 ****)0x0) {
LAB_14048aeb7:
      *(undefined4 *)(lVar25 + 0x24) = 0xffffffff;
      lVar17 = FUN_14033bc80(&local_3150,lVar25);
      *(undefined4 *)(lVar17 + 0x24) = 0xffffffff;
      FUN_140416780(param_1 + 0xb88);
      FUN_14032c7d0(lVar17);
      bVar10 = true;
    }
    else {
      do {
        cVar14 = FUN_14023b690(ppppuVar21 + 4,lVar25);
        if (cVar14 < '\0') {
          ppppuVar21 = (undefined8 ****)*ppppuVar21;
        }
        else {
          ppppuVar16 = ppppuVar21;
          ppppuVar21 = (undefined8 ****)ppppuVar21[1];
        }
      } while (ppppuVar21 != (undefined8 ****)0x0);
      if ((ppppuVar16 == &local_31d0) || (cVar14 = FUN_14023b690(lVar25), cVar14 < '\0'))
      goto LAB_14048aeb7;
    }
    ppppuVar21 = (undefined8 ****)local_31c0;
  }
  local_3200 = &local_3200;
  pppuStack_31f8 = &local_3200;
  local_31f0 = (undefined8 ****)0x0;
  uStack_31e8 = 0;
  local_31e0 = 0;
  puVar3 = *(undefined4 **)(param_1 + 0x10d8);
  for (puVar38 = *(undefined4 **)(param_1 + 0x10d0); puVar38 != puVar3; puVar38 = puVar38 + 0x44) {
    pvVar15 = (void *)thunk_FUN_140983da8(0x48);
    *(undefined4 *)((longlong)pvVar15 + 0x20) = *puVar38;
    FUN_140019ca0((longlong)pvVar15 + 0x28,puVar38 + 2);
    ppppuVar21 = &local_3200;
    bVar13 = 1;
    if ((undefined8 ****)local_31f0 != (undefined8 ****)0x0) {
      iVar1 = *(int *)((longlong)pvVar15 + 0x20);
      ppppuVar16 = (undefined8 ****)local_31f0;
      do {
        ppppuVar21 = ppppuVar16;
        if (iVar1 == *(int *)(ppppuVar21 + 4)) {
          bVar12 = FUN_14023b690((longlong)pvVar15 + 0x28,ppppuVar21 + 5);
        }
        else {
          bVar12 = 0xff;
          if (*(int *)(ppppuVar21 + 4) <= iVar1) {
            bVar12 = 1;
          }
        }
        bVar13 = bVar12 >> 7;
        if ((char)bVar12 < '\0') {
          ppppuVar16 = (undefined8 ****)ppppuVar21[1];
        }
        else {
          ppppuVar16 = (undefined8 ****)*ppppuVar21;
        }
      } while (ppppuVar16 != (undefined8 ****)0x0);
    }
    ppppuVar16 = ppppuVar21;
    if (bVar13 == 0) {
LAB_14048afdf:
      if (*(int *)(ppppuVar16 + 4) == *(int *)((longlong)pvVar15 + 0x20)) {
        cVar14 = FUN_14023b690(ppppuVar16 + 5);
      }
      else {
        cVar14 = -1;
        if (*(int *)((longlong)pvVar15 + 0x20) <= *(int *)(ppppuVar16 + 4)) {
          cVar14 = '\x01';
        }
      }
      if (cVar14 < '\0') goto LAB_14048b003;
      FUN_140002d30((longlong)pvVar15 + 0x28);
      free(pvVar15);
    }
    else {
      if (ppppuVar21 != (undefined8 ****)pppuStack_31f8) {
        ppppuVar16 = (undefined8 ****)FUN_14001dcf0();
        goto LAB_14048afdf;
      }
LAB_14048b003:
      if (ppppuVar21 == &local_3200) {
LAB_14048b045:
        uVar34 = 0;
      }
      else {
        if (*(int *)((longlong)pvVar15 + 0x20) == *(int *)(ppppuVar21 + 4)) {
          cVar14 = FUN_14023b690((longlong)pvVar15 + 0x28,ppppuVar21 + 5);
        }
        else {
          cVar14 = -1;
          if (*(int *)(ppppuVar21 + 4) <= *(int *)((longlong)pvVar15 + 0x20)) {
            cVar14 = '\x01';
          }
        }
        if (cVar14 < '\0') goto LAB_14048b045;
        uVar34 = 1;
      }
      FUN_14001de10(pvVar15,ppppuVar21,&local_3200,uVar34);
      local_31e0 = local_31e0 + 1;
    }
  }
  piVar36 = (int *)*param_5;
  piVar4 = (int *)param_5[1];
  if (piVar36 != piVar4) {
    piVar35 = piVar36 + 2;
    do {
      ppppuVar21 = &local_3200;
      if ((undefined8 ****)local_31f0 == (undefined8 ****)0x0) {
LAB_14048b142:
        uVar5 = *(ulonglong *)(param_1 + 0x10d8);
        if (uVar5 < *(ulonglong *)(param_1 + 0x10e0)) {
          FUN_1402e6140(uVar5);
          *(longlong *)(param_1 + 0x10d8) = *(longlong *)(param_1 + 0x10d8) + 0x110;
        }
        else {
          lVar25 = (longlong)(uVar5 - *(longlong *)(param_1 + 0x10d0)) >> 4;
          if (lVar25 * -0xf0f0f0f0f0f0f0f == 0) {
            lVar24 = 1;
LAB_14048b1a9:
            lVar25 = lVar24 * 0x110;
            lVar17 = thunk_FUN_140983da8(lVar25);
          }
          else {
            lVar24 = lVar25 * -0x1e1e1e1e1e1e1e1e;
            if (lVar24 != 0) goto LAB_14048b1a9;
            lVar17 = 0;
            lVar25 = lVar25 * 0x20;
          }
          FUN_14033a680(&local_3170,*(undefined8 *)(param_1 + 0x10d0),
                        *(undefined8 *)(param_1 + 0x10d8),lVar17);
          lVar11 = local_3170;
          FUN_1402e6140(local_3170);
          lVar6 = *(longlong *)(param_1 + 0x10d8);
          lVar32 = *(longlong *)(param_1 + 0x10d0);
          lVar18 = lVar25;
          if (lVar32 != lVar6) {
            do {
              FUN_1402dfc60(lVar32);
              lVar32 = lVar32 + 0x110;
            } while (lVar32 != lVar6);
            lVar18 = lVar24 * 0x110;
          }
          if (*(void **)(param_1 + 0x10d0) != (void *)0x0) {
            free(*(void **)(param_1 + 0x10d0));
            lVar18 = lVar25;
          }
          *(longlong *)(param_1 + 0x10d0) = lVar17;
          *(longlong *)(param_1 + 0x10d8) = lVar11 + 0x110;
          *(longlong *)(param_1 + 0x10e0) = lVar18 + lVar17;
        }
      }
      else {
        iVar1 = *piVar36;
        ppppuVar16 = (undefined8 ****)local_31f0;
        do {
          if (*(int *)(ppppuVar16 + 4) == iVar1) {
            cVar14 = FUN_14023b690(ppppuVar16 + 5);
          }
          else {
            cVar14 = -1;
            if (iVar1 <= *(int *)(ppppuVar16 + 4)) {
              cVar14 = '\x01';
            }
          }
          if (cVar14 < '\0') {
            ppppuVar31 = (undefined8 ****)*ppppuVar16;
          }
          else {
            ppppuVar31 = (undefined8 ****)ppppuVar16[1];
            ppppuVar21 = ppppuVar16;
          }
          ppppuVar16 = ppppuVar31;
        } while (ppppuVar31 != (undefined8 ****)0x0);
        if (ppppuVar21 == &local_3200) goto LAB_14048b142;
        if (iVar1 == *(int *)(ppppuVar21 + 4)) {
          cVar14 = FUN_14023b690(piVar35);
        }
        else {
          cVar14 = -1;
          if (*(int *)(ppppuVar21 + 4) <= iVar1) {
            cVar14 = '\x01';
          }
        }
        if (cVar14 < '\0') goto LAB_14048b142;
      }
      piVar36 = piVar36 + 0x44;
      piVar35 = piVar35 + 0x44;
    } while (piVar36 != piVar4);
  }
  puVar7 = (ulonglong *)param_4[1];
  puVar27 = (ulonglong *)*param_4;
  if (puVar27 != puVar7) {
    puVar29 = puVar27 + 0xb6;
    do {
      uVar5 = *puVar27;
      lVar25 = 0;
      puVar23 = *(ulonglong **)
                 (*(longlong *)(param_1 + 0x1170) +
                 (uVar5 % (ulonglong)*(uint *)(param_1 + 0x1178)) * 8);
      if (puVar23 == (ulonglong *)0x0) {
LAB_14048b302:
        plVar19 = (longlong *)FUN_140494160(param_1 + 0x1168,local_3168,uVar5,puVar27);
        lVar25 = *plVar19;
        *(ulonglong *)(lVar25 + 8) = *puVar27;
        *(char *)(lVar25 + 0x10) = (char)puVar29[-0xb5];
        FUN_14032a1c0(lVar25 + 0x18,puVar29 + -0xb4);
        FUN_140025630(lVar25 + 0x48,puVar29 + -0xae);
        lVar24 = 5;
        puVar23 = (ulonglong *)(lVar25 + 0x68);
        puVar33 = puVar29 + -0xaa;
        do {
          puVar22 = puVar33;
          puVar20 = puVar23;
          uVar5 = puVar22[1];
          *puVar20 = *puVar22;
          puVar20[1] = uVar5;
          uVar5 = puVar22[3];
          puVar20[2] = puVar22[2];
          puVar20[3] = uVar5;
          uVar5 = puVar22[5];
          puVar20[4] = puVar22[4];
          puVar20[5] = uVar5;
          uVar5 = puVar22[7];
          puVar20[6] = puVar22[6];
          puVar20[7] = uVar5;
          uVar5 = puVar22[9];
          puVar20[8] = puVar22[8];
          puVar20[9] = uVar5;
          uVar5 = puVar22[0xb];
          puVar20[10] = puVar22[10];
          puVar20[0xb] = uVar5;
          uVar5 = puVar22[0xd];
          puVar20[0xc] = puVar22[0xc];
          puVar20[0xd] = uVar5;
          uVar5 = puVar22[0xf];
          puVar20[0xe] = puVar22[0xe];
          puVar20[0xf] = uVar5;
          lVar24 = lVar24 + -1;
          puVar23 = puVar20 + 0x10;
          puVar33 = puVar22 + 0x10;
        } while (lVar24 != 0);
        uVar5 = puVar22[0x11];
        puVar20[0x10] = puVar22[0x10];
        puVar20[0x11] = uVar5;
        uVar5 = puVar22[0x13];
        puVar20[0x12] = puVar22[0x12];
        puVar20[0x13] = uVar5;
        lVar24 = 5;
        puVar23 = puVar29 + -0x56;
        puVar33 = (ulonglong *)(lVar25 + 0x308);
        do {
          puVar22 = puVar33;
          puVar20 = puVar23;
          uVar5 = puVar20[1];
          *puVar22 = *puVar20;
          puVar22[1] = uVar5;
          uVar5 = puVar20[3];
          puVar22[2] = puVar20[2];
          puVar22[3] = uVar5;
          uVar5 = puVar20[5];
          puVar22[4] = puVar20[4];
          puVar22[5] = uVar5;
          uVar5 = puVar20[7];
          puVar22[6] = puVar20[6];
          puVar22[7] = uVar5;
          uVar5 = puVar20[9];
          puVar22[8] = puVar20[8];
          puVar22[9] = uVar5;
          uVar5 = puVar20[0xb];
          puVar22[10] = puVar20[10];
          puVar22[0xb] = uVar5;
          uVar5 = puVar20[0xd];
          puVar22[0xc] = puVar20[0xc];
          puVar22[0xd] = uVar5;
          uVar5 = puVar20[0xf];
          puVar22[0xe] = puVar20[0xe];
          puVar22[0xf] = uVar5;
          lVar24 = lVar24 + -1;
          puVar23 = puVar20 + 0x10;
          puVar33 = puVar22 + 0x10;
        } while (lVar24 != 0);
        uVar8 = *(undefined4 *)((longlong)puVar20 + 0x84);
        uVar5 = puVar20[0x11];
        uVar9 = *(undefined4 *)((longlong)puVar20 + 0x8c);
        *(int *)(puVar22 + 0x10) = (int)puVar20[0x10];
        *(undefined4 *)((longlong)puVar22 + 0x84) = uVar8;
        *(int *)(puVar22 + 0x11) = (int)uVar5;
        *(undefined4 *)((longlong)puVar22 + 0x8c) = uVar9;
        uVar8 = *(undefined4 *)((longlong)puVar20 + 0x94);
        uVar5 = puVar20[0x13];
        uVar9 = *(undefined4 *)((longlong)puVar20 + 0x9c);
        *(int *)(puVar22 + 0x12) = (int)puVar20[0x12];
        *(undefined4 *)((longlong)puVar22 + 0x94) = uVar8;
        *(int *)(puVar22 + 0x13) = (int)uVar5;
        *(undefined4 *)((longlong)puVar22 + 0x9c) = uVar9;
        puVar33 = (ulonglong *)(lVar25 + 0x5a8);
        puVar23 = puVar29 + -2;
        if (puVar33 != puVar23) {
          FUN_1403375b0(puVar33);
          uVar5 = *puVar33;
          *puVar33 = *puVar23;
          *puVar23 = uVar5;
          uVar5 = *(ulonglong *)(lVar25 + 0x5b0);
          *(ulonglong *)(lVar25 + 0x5b0) = puVar29[-1];
          puVar29[-1] = uVar5;
          uVar5 = *(ulonglong *)(lVar25 + 0x5b8);
          *(ulonglong *)(lVar25 + 0x5b8) = *puVar29;
          *puVar29 = uVar5;
        }
      }
      else {
        do {
          lVar24 = lVar25 + 1;
          if (uVar5 != *puVar23) {
            lVar24 = lVar25;
          }
          puVar23 = (ulonglong *)puVar23[0xb8];
          lVar25 = lVar24;
        } while (puVar23 != (ulonglong *)0x0);
        if (lVar24 == 0) goto LAB_14048b302;
      }
      puVar27 = puVar27 + 0xb7;
      puVar29 = puVar29 + 0xb7;
    } while (puVar27 != puVar7);
  }
  ppppuVar21 = (undefined8 ****)local_31f0;
  if (bVar10) {
    *(int *)(param_1 + 0xfe0) = *(int *)(param_1 + 0xfe0) + 1;
  }
  while (ppppuVar16 = (undefined8 ****)local_31c0, ppppuVar21 != (undefined8 ****)0x0) {
    FUN_14028ba30(&local_3200,*ppppuVar21);
    ppppuVar16 = (undefined8 ****)ppppuVar21[1];
    FUN_140002d30(ppppuVar21 + 5);
    free(ppppuVar21);
    ppppuVar21 = ppppuVar16;
  }
  while (ppppuVar21 = (undefined8 ****)local_3190, ppppuVar16 != (undefined8 ****)0x0) {
    FUN_14029c2b0(&local_31d0,*ppppuVar16);
    ppppuVar21 = (undefined8 ****)ppppuVar16[1];
    FUN_140002d30(ppppuVar16 + 4);
    free(ppppuVar16);
    ppppuVar16 = ppppuVar21;
  }
  while (ppppuVar21 != (undefined8 ****)0x0) {
    FUN_14029c2b0(&local_31a0,*ppppuVar21);
    ppppuVar16 = (undefined8 ****)ppppuVar21[1];
    FUN_140002d30(ppppuVar21 + 4);
    free(ppppuVar21);
    ppppuVar21 = ppppuVar16;
  }
  lVar25 = param_2[1];
  for (lVar24 = *param_2; lVar24 != lVar25; lVar24 = lVar24 + 0x1880) {
    FUN_14032c6e0(lVar24);
  }
  if ((void *)*param_2 != (void *)0x0) {
    free((void *)*param_2);
  }
  lVar25 = param_3[1];
  for (lVar24 = *param_3; lVar24 != lVar25; lVar24 = lVar24 + 0x128) {
    FUN_14032c7d0(lVar24);
  }
  if ((void *)*param_3 != (void *)0x0) {
    free((void *)*param_3);
  }
  FUN_140324f50(*param_4,param_4[1]);
  if ((void *)*param_4 != (void *)0x0) {
    free((void *)*param_4);
  }
  lVar25 = param_5[1];
  for (lVar24 = *param_5; lVar24 != lVar25; lVar24 = lVar24 + 0x110) {
    FUN_1402dfc60(lVar24);
  }
  if ((void *)*param_5 != (void *)0x0) {
    free((void *)*param_5);
  }
  return;
}
