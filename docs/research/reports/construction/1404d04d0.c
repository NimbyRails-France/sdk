
void FUN_1404d04d0(undefined8 param_1,longlong param_2)

{
  longlong lVar1;
  void *pvVar2;
  void *pvVar3;
  undefined8 *puVar4;
  ulonglong *puVar5;
  longlong *plVar6;
  longlong lVar7;
  ulonglong uVar8;
  ulonglong *puVar9;
  longlong lVar10;
  longlong lVar11;
  ulonglong *puVar12;
  ulonglong uVar13;
  ulonglong uVar14;
  undefined8 *puVar15;
  longlong lVar16;
  ulonglong *puVar17;
  ulonglong *local_res8;
  longlong local_res10;
  ulonglong local_res18;
  char local_res20 [4];
  uint local_res24;
  undefined1 local_108 [8];
  undefined8 *local_100;
  ulonglong local_f8;
  ulonglong local_f0;
  undefined4 local_e8;
  undefined8 local_e4;
  undefined1 local_d8 [8];
  undefined8 *local_d0;
  ulonglong local_c8;
  ulonglong local_c0;
  undefined4 local_b8;
  undefined8 local_b4;
  undefined1 local_a8 [8];
  undefined8 *local_a0;
  ulonglong local_98;
  ulonglong local_90;
  undefined4 local_88;
  undefined8 local_84;
  longlong *local_78;
  longlong *local_70;
  undefined1 local_68 [40];
  
  local_88 = 0x3f800000;
  local_84 = 0x40000000;
  puVar17 = (ulonglong *)0x1;
  local_98 = 1;
  puVar15 = &DAT_140b5abc0;
  local_a0 = &DAT_140b5abc0;
  local_90 = 0;
  local_e8 = 0x3f800000;
  local_e4 = 0x40000000;
  local_f8 = 1;
  local_100 = &DAT_140b5abc0;
  local_f0 = 0;
  local_b8 = 0x3f800000;
  local_b4 = 0x40000000;
  uVar8 = 1;
  local_c8 = 1;
  local_d0 = &DAT_140b5abc0;
  local_c0 = 0;
  local_70 = (longlong *)(param_2 + 0x210);
  puVar12 = (ulonglong *)*local_70;
  puVar9 = *(ulonglong **)(param_2 + 0x218);
  local_res10 = param_2;
  if (puVar12 != puVar9) {
    local_res8 = (ulonglong *)0x1;
    do {
      uVar14 = *puVar12;
      uVar13 = uVar14 % (ulonglong)puVar17;
      for (puVar5 = (ulonglong *)puVar15[uVar13]; puVar5 != (ulonglong *)0x0;
          puVar5 = (ulonglong *)puVar5[1]) {
        if (uVar14 == *puVar5) goto LAB_1404d061c;
      }
      puVar5 = (ulonglong *)thunk_FUN_140983da8(0x10);
      *puVar5 = *puVar12;
      puVar5[1] = 0;
      FUN_14001db20(&local_88,&local_res8,puVar17,local_90 & 0xffffffff,1);
      if ((char)local_res8 != '\0') {
        uVar13 = uVar14 % ((ulonglong)local_res8 >> 0x20);
        FUN_140342e50(local_a8);
      }
      puVar5[1] = local_a0[uVar13];
      local_a0[uVar13] = puVar5;
      local_90 = local_90 + 1;
      puVar17 = (ulonglong *)(local_98 & 0xffffffff);
      puVar15 = local_a0;
LAB_1404d061c:
      puVar12 = puVar12 + 199;
    } while (puVar12 != puVar9);
  }
  puVar12 = *(ulonglong **)(local_res10 + 0x228);
  puVar9 = *(ulonglong **)(local_res10 + 0x230);
  local_res8 = puVar17;
  if (puVar12 != puVar9) {
    uVar8 = local_c8 & 0xffffffff;
    puVar15 = local_d0;
    local_res18 = uVar8;
    do {
      uVar14 = *puVar12;
      uVar13 = uVar14 % uVar8;
      for (puVar5 = (ulonglong *)puVar15[uVar13]; puVar5 != (ulonglong *)0x0;
          puVar5 = (ulonglong *)puVar5[1]) {
        if (uVar14 == *puVar5) goto LAB_1404d06fc;
      }
      puVar5 = (ulonglong *)thunk_FUN_140983da8(0x10);
      *puVar5 = *puVar12;
      puVar5[1] = 0;
      FUN_14001db20(&local_b8,&local_res18,uVar8,local_c0 & 0xffffffff,1);
      if ((char)local_res18 != '\0') {
        uVar13 = uVar14 % (local_res18 >> 0x20);
        FUN_140342e50(local_d8);
      }
      puVar5[1] = local_d0[uVar13];
      local_d0[uVar13] = puVar5;
      local_c0 = local_c0 + 1;
      uVar8 = local_c8 & 0xffffffff;
      puVar15 = local_d0;
LAB_1404d06fc:
      puVar12 = puVar12 + 0x2f;
    } while (puVar12 != puVar9);
  }
  local_78 = (longlong *)(local_res10 + 0x240);
  puVar12 = (ulonglong *)*local_78;
  puVar9 = *(ulonglong **)(local_res10 + 0x248);
  uVar14 = local_f8;
  local_res18 = uVar8;
  if (puVar12 != puVar9) {
    uVar14 = local_f8 & 0xffffffff;
    puVar15 = local_100;
    do {
      uVar8 = *puVar12;
      uVar13 = uVar8 % uVar14;
      for (puVar17 = (ulonglong *)puVar15[uVar13]; puVar17 != (ulonglong *)0x0;
          puVar17 = (ulonglong *)puVar17[1]) {
        if (uVar8 == *puVar17) goto LAB_1404d07eb;
      }
      puVar17 = (ulonglong *)thunk_FUN_140983da8(0x10);
      *puVar17 = *puVar12;
      puVar17[1] = 0;
      FUN_14001db20(&local_e8,local_res20,uVar14,local_f0 & 0xffffffff,1);
      if (local_res20[0] != '\0') {
        uVar13 = uVar8 % (ulonglong)local_res24;
        FUN_140342e50(local_108);
      }
      puVar17[1] = local_100[uVar13];
      local_100[uVar13] = puVar17;
      local_f0 = local_f0 + 1;
      uVar14 = local_f8 & 0xffffffff;
      puVar15 = local_100;
LAB_1404d07eb:
      puVar12 = puVar12 + 8;
      puVar17 = local_res8;
    } while (puVar12 != puVar9);
  }
  plVar6 = local_78;
  puVar4 = local_a0;
  puVar15 = local_100;
  puVar12 = *(ulonglong **)(local_res10 + 0x170);
  puVar9 = *(ulonglong **)(local_res10 + 0x168);
  if (puVar9 != puVar12) {
    do {
      lVar11 = 0;
      puVar5 = (ulonglong *)puVar4[*puVar9 % ((ulonglong)puVar17 & 0xffffffff)];
      if (puVar5 == (ulonglong *)0x0) {
LAB_1404d086b:
        puVar5 = (ulonglong *)FUN_1404d7a70(local_70);
        *puVar5 = *puVar9;
      }
      else {
        do {
          lVar7 = lVar11 + 1;
          if (*puVar9 != *puVar5) {
            lVar7 = lVar11;
          }
          puVar5 = (ulonglong *)puVar5[1];
          lVar11 = lVar7;
        } while (puVar5 != (ulonglong *)0x0);
        if (lVar7 == 0) goto LAB_1404d086b;
      }
      lVar11 = 0;
      puVar5 = (ulonglong *)puVar15[*puVar9 % (uVar14 & 0xffffffff)];
      if (puVar5 == (ulonglong *)0x0) {
LAB_1404d08bb:
        puVar5 = (ulonglong *)FUN_1404d7bd0(plVar6);
        *puVar5 = *puVar9;
      }
      else {
        do {
          lVar7 = lVar11 + 1;
          if (*puVar9 != *puVar5) {
            lVar7 = lVar11;
          }
          puVar5 = (ulonglong *)puVar5[1];
          lVar11 = lVar7;
        } while (puVar5 != (ulonglong *)0x0);
        if (lVar7 == 0) goto LAB_1404d08bb;
      }
      puVar9 = puVar9 + 0x2f;
    } while (puVar9 != puVar12);
  }
  puVar12 = *(ulonglong **)(local_res10 + 0xd8);
  puVar9 = *(ulonglong **)(local_res10 + 0xe0);
  lVar11 = local_res10;
  if (puVar12 != puVar9) {
    local_res18 = local_res18 & 0xffffffff;
    do {
      puVar17 = (ulonglong *)local_d0[*puVar12 % local_res18 & 0xffffffff];
      lVar7 = 0;
      if (puVar17 == (ulonglong *)0x0) {
LAB_1404d094f:
        uVar8 = *(ulonglong *)(lVar11 + 0x230);
        if (uVar8 < *(ulonglong *)(lVar11 + 0x238)) {
          FUN_1404774b0(uVar8);
          puVar17 = *(ulonglong **)(lVar11 + 0x230);
        }
        else {
          lVar7 = (longlong)(uVar8 - *(longlong *)(lVar11 + 0x228)) >> 3;
          if (lVar7 * 0x51b3bea3677d46cf == 0) {
            lVar7 = 1;
LAB_1404d09a1:
            lVar16 = thunk_FUN_140983da8(lVar7 * 0x178);
          }
          else {
            lVar7 = lVar7 * -0x5c9882b931057262;
            if (lVar7 != 0) goto LAB_1404d09a1;
            lVar16 = 0;
          }
          FUN_140486e10(&local_res8,*(undefined8 *)(lVar11 + 0x228),*(undefined8 *)(lVar11 + 0x230),
                        lVar16);
          puVar17 = local_res8;
          FUN_1404774b0(local_res8);
          lVar1 = *(longlong *)(lVar11 + 0x230);
          lVar11 = local_res10;
          for (lVar10 = *(longlong *)(local_res10 + 0x228); local_res10 = lVar11, lVar10 != lVar1;
              lVar10 = lVar10 + 0x178) {
            FUN_140351180(lVar10);
            lVar11 = local_res10;
          }
          if (*(void **)(lVar11 + 0x228) != (void *)0x0) {
            free(*(void **)(lVar11 + 0x228));
          }
          *(longlong *)(lVar11 + 0x228) = lVar16;
          *(longlong *)(lVar11 + 0x238) = lVar7 * 0x178 + lVar16;
        }
        *(ulonglong **)(lVar11 + 0x230) = puVar17 + 0x2f;
        *puVar17 = *puVar12;
      }
      else {
        do {
          lVar16 = lVar7 + 1;
          if (*puVar12 != *puVar17) {
            lVar16 = lVar7;
          }
          puVar17 = (ulonglong *)puVar17[1];
          lVar7 = lVar16;
        } while (puVar17 != (ulonglong *)0x0);
        if (lVar16 == 0) goto LAB_1404d094f;
      }
      puVar12 = puVar12 + 0x7d;
    } while (puVar12 != puVar9);
  }
  uVar14 = 0;
  plVar6 = (longlong *)**(longlong **)(lVar11 + 0x468);
  uVar8 = uVar14;
  if (plVar6 != (longlong *)0x0) {
    do {
      uVar13 = uVar8 + 1;
      if (*plVar6 != 0) {
        uVar13 = uVar8;
      }
      plVar6 = (longlong *)plVar6[0xb8];
      uVar8 = uVar13;
    } while (plVar6 != (longlong *)0x0);
    if (uVar13 != 0) goto LAB_1404d0af5;
  }
  local_res10 = 0;
  plVar6 = (longlong *)FUN_140494160(lVar11 + 0x460,local_68,lVar11 + 0x460,&local_res10);
  lVar11 = *plVar6;
  lVar7 = FUN_1402d82e0("demand_default_name","Population");
  lVar16 = -1;
  do {
    lVar16 = lVar16 + 1;
  } while (*(char *)(lVar7 + lVar16) != '\0');
  FUN_140030630(lVar11 + 0x48,lVar7);
  FUN_1404913c0(lVar11 + 8);
LAB_1404d0af5:
  uVar8 = local_c8;
  puVar15 = local_d0;
  uVar13 = uVar14;
  if (local_c8 != 0) {
    do {
      pvVar3 = (void *)puVar15[uVar13];
      while (pvVar3 != (void *)0x0) {
        pvVar2 = *(void **)((longlong)pvVar3 + 8);
        free(pvVar3);
        pvVar3 = pvVar2;
      }
      puVar15[uVar13] = 0;
      uVar13 = uVar13 + 1;
    } while (uVar13 < uVar8);
  }
  if (1 < uVar8) {
    free(puVar15);
  }
  uVar8 = local_f8;
  puVar15 = local_100;
  uVar13 = uVar14;
  if (local_f8 != 0) {
    do {
      pvVar3 = (void *)puVar15[uVar13];
      while (pvVar3 != (void *)0x0) {
        pvVar2 = *(void **)((longlong)pvVar3 + 8);
        free(pvVar3);
        pvVar3 = pvVar2;
      }
      puVar15[uVar13] = 0;
      uVar13 = uVar13 + 1;
    } while (uVar13 < uVar8);
  }
  if (1 < uVar8) {
    free(puVar15);
  }
  uVar8 = local_98;
  puVar15 = local_a0;
  if (local_98 != 0) {
    do {
      pvVar3 = (void *)puVar15[uVar14];
      while (pvVar3 != (void *)0x0) {
        pvVar2 = *(void **)((longlong)pvVar3 + 8);
        free(pvVar3);
        pvVar3 = pvVar2;
      }
      puVar15[uVar14] = 0;
      uVar14 = uVar14 + 1;
    } while (uVar14 < uVar8);
  }
  if (1 < uVar8) {
    free(puVar15);
  }
  return;
}

