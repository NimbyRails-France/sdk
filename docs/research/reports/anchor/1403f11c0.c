
void FUN_1403f11c0(longlong *param_1,undefined8 param_2,undefined8 param_3,undefined8 *param_4)

{
  void **ppvVar1;
  void **ppvVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  undefined8 *puVar6;
  undefined1 *puVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  char cVar10;
  undefined4 uVar11;
  longlong *plVar12;
  longlong lVar13;
  longlong lVar14;
  longlong *plVar15;
  void *pvVar16;
  ulonglong uVar17;
  longlong *plVar18;
  ulonglong uVar19;
  ulonglong uVar20;
  int iVar21;
  int iVar22;
  longlong lVar23;
  void *pvVar24;
  ulonglong uVar25;
  longlong *plVar26;
  longlong *plVar27;
  longlong *plVar28;
  longlong *plVar29;
  longlong *plVar30;
  undefined4 uVar31;
  longlong *local_3f8;
  longlong *plStack_3f0;
  longlong *local_3e8;
  longlong *local_3e0;
  longlong *local_3d8;
  void *local_3c8;
  void *local_3c0;
  undefined1 local_3b0 [16];
  undefined8 *local_3a0;
  void *local_380;
  undefined8 local_378;
  undefined8 local_370;
  void *local_368;
  undefined8 local_360;
  undefined8 local_358;
  undefined1 *local_350;
  undefined8 local_348;
  undefined8 local_340;
  undefined8 local_338;
  undefined8 local_320;
  undefined4 local_318;
  void *local_310;
  undefined1 local_2f8 [16];
  undefined8 *local_2e8;
  undefined1 local_2c8 [648];
  
  if (param_4 != (undefined8 *)0x0) {
    FUN_1403e7940(param_1,param_4);
    plVar28 = (longlong *)0x0;
    plVar12 = plVar28;
    if (param_4[0xf] != 0) {
      plVar12 = (longlong *)FUN_14033f7f0(*(longlong *)(*param_1 + 0x780) + 0x180);
    }
    local_3e0 = plVar12;
    if (plVar12 != (longlong *)0x0) {
      plVar18 = plVar12 + 0xb;
      if (param_4 + 2 != plVar18) {
        if (0xf < (ulonglong)plVar12[0xe]) {
          plVar18 = (longlong *)*plVar18;
        }
        FUN_140030630(param_4 + 2,plVar18,plVar12[0xd]);
      }
      plVar18 = plVar12 + 0xf;
      if (param_4 + 6 != plVar18) {
        if (0xf < (ulonglong)plVar12[0x12]) {
          plVar18 = (longlong *)*plVar18;
        }
        FUN_140030630(param_4 + 6,plVar18,plVar12[0x11]);
      }
      *(int *)(param_4 + 10) = (int)plVar12[0x17];
      if (param_4[0x15] != 1) {
        FUN_14033b450(param_4 + 0x12);
        lVar13 = FUN_1403e4580(param_4,10000);
        lVar14 = FUN_1402d82e0("sched_order_list_first_name","Default");
        lVar23 = -1;
        do {
          lVar23 = lVar23 + 1;
        } while (*(char *)(lVar14 + lVar23) != '\0');
        FUN_140030630(lVar13 + 8,lVar14);
      }
      lVar13 = *(longlong *)param_4[0x13];
      if (lVar13 == 0) {
        plVar18 = (longlong *)param_4[0x13] + 1;
        lVar13 = *plVar18;
        while (lVar13 == 0) {
          plVar18 = plVar18 + 1;
          lVar13 = *plVar18;
        }
      }
      FUN_140345640(lVar13 + 0x30);
      iVar21 = 7;
      if (0x5460 < *(int *)((longlong)plVar12 + 0x19c)) {
        iVar21 = 1;
      }
      if (*(int *)((longlong)plVar12 + 0x104) == 1) {
        if ((int)plVar12[0x21] == 1) {
          iVar21 = 1;
        }
        else if ((int)plVar12[0x21] == 0) {
          iVar21 = 7;
        }
      }
      lVar13 = FUN_1403f3540(lVar13 + 0x30);
      *(undefined8 *)(lVar13 + 0x28) = 100000;
      *(undefined4 *)(lVar13 + 0x20) = 0;
      *(longlong *)(lVar13 + 0x30) = *plVar12;
      iVar22 = *(int *)((longlong)plVar12 + 0x204) % 0x15180;
      if (iVar22 < 0) {
        iVar22 = iVar22 + 0x15180;
      }
      *(int *)(lVar13 + 0x18) = iVar22;
      uVar11 = 1;
      if (iVar21 != 1) {
        uVar11 = 0x7f;
      }
      *(undefined4 *)(lVar13 + 0x1c) = uVar11;
      if ((int)plVar12[0x42] == 0) {
        *(undefined4 *)(param_4 + 0x18) = 2;
        param_4[0x1a] = param_4[0xf];
      }
      else {
        *(undefined4 *)(param_4 + 0x18) = 0;
        *(undefined4 *)((longlong)param_4 + 0xc4) = *(undefined4 *)((longlong)plVar12 + 0x214);
      }
      local_3f8 = (longlong *)0x0;
      plStack_3f0 = (longlong *)0x0;
      local_3e8 = (longlong *)0x0;
      plVar12 = (longlong *)param_4[0x37];
      plVar18 = (longlong *)*plVar12;
      if (plVar18 == (longlong *)0x0) {
        plVar12 = plVar12 + 1;
        plVar18 = (longlong *)*plVar12;
        while (plVar18 == (longlong *)0x0) {
          plVar12 = plVar12 + 1;
          plVar18 = (longlong *)*plVar12;
        }
      }
      plVar26 = *(longlong **)(param_4[0x37] + param_4[0x38] * 8);
      plVar30 = (longlong *)0x0;
      plVar15 = (longlong *)0x0;
      plVar27 = plVar15;
      local_3d8 = plVar26;
      if (plVar18 != plVar26) {
        do {
          lVar13 = *plVar18;
          uVar25 = param_4[0x48];
          uVar17 = (longlong)(param_4[0x49] - uVar25) >> 4;
          plVar15 = plVar27;
          if ((longlong)uVar17 < 1) {
LAB_1403f1524:
            if (plVar30 < plVar28) {
              *plVar30 = *plVar18;
              plVar29 = plVar30;
            }
            else {
              lVar13 = (longlong)plVar30 - (longlong)plVar27 >> 3;
              if (lVar13 == 0) {
                lVar13 = 1;
LAB_1403f155c:
                plVar15 = (longlong *)thunk_FUN_140983da8(lVar13 * 8);
              }
              else {
                lVar13 = lVar13 * 2;
                if (lVar13 != 0) goto LAB_1403f155c;
                plVar15 = (longlong *)0x0;
              }
              plVar29 = plVar15;
              if (plVar27 != plVar30) {
                pvVar16 = memmove(plVar15,plVar27,(longlong)plVar30 - (longlong)plVar27);
                plVar29 = (longlong *)
                          ((longlong)pvVar16 + ((longlong)plVar30 - (longlong)plVar27 >> 3) * 8);
              }
              *plVar29 = *plVar18;
              if (plVar27 != (longlong *)0x0) {
                free(plVar27);
              }
              plVar28 = plVar15 + lVar13;
              plVar26 = local_3d8;
              local_3f8 = plVar15;
              local_3e8 = plVar28;
            }
            plVar30 = plVar29 + 1;
            plStack_3f0 = plVar30;
          }
          else {
            do {
              uVar19 = uVar17 >> 1;
              lVar14 = *(longlong *)(uVar25 + uVar19 * 0x10);
              if ((lVar14 < lVar13) ||
                 ((lVar14 <= lVar13 && (*(longlong *)(uVar25 + 8 + uVar19 * 0x10) < 0)))) {
                uVar25 = uVar25 + uVar19 * 0x10 + 0x10;
                uVar19 = uVar17 + (-1 - uVar19);
              }
              uVar17 = uVar19;
            } while (0 < (longlong)uVar19);
            uVar17 = param_4[0x48];
            uVar19 = (longlong)(param_4[0x49] - uVar17) >> 4;
            do {
              uVar20 = uVar19 >> 1;
              lVar14 = *(longlong *)(uVar17 + uVar20 * 0x10);
              if ((lVar14 <= lVar13 + 1) &&
                 ((lVar14 < lVar13 + 1 || (*(longlong *)(uVar17 + 8 + uVar20 * 0x10) < 1)))) {
                uVar17 = uVar17 + uVar20 * 0x10 + 0x10;
                uVar20 = uVar19 + (-1 - uVar20);
              }
              uVar19 = uVar20;
            } while (0 < (longlong)uVar20);
            if (uVar17 <= uVar25) goto LAB_1403f1524;
          }
          plVar18 = (longlong *)plVar18[0x3b];
          while (plVar18 == (longlong *)0x0) {
            plVar12 = plVar12 + 1;
            plVar18 = (longlong *)*plVar12;
          }
          plVar27 = plVar15;
        } while (plVar18 != plVar26);
      }
      for (; plVar15 != plVar30; plVar15 = plVar15 + 1) {
        FUN_1403e3c30(param_4,*plVar15);
      }
      if (plVar27 != (longlong *)0x0) {
        free(plVar27);
      }
    }
    plVar12 = local_3e0;
    FUN_1403f0a20(param_1,local_2c8,param_4);
    *(undefined1 *)(param_4 + 0x11) = 0;
    plVar28 = (longlong *)param_4[0x37];
    lVar13 = *plVar28;
    if (lVar13 == 0) {
      plVar28 = plVar28 + 1;
      lVar13 = *plVar28;
      while (lVar13 == 0) {
        plVar28 = plVar28 + 1;
        lVar13 = *plVar28;
      }
    }
    local_3d8 = *(longlong **)(param_4[0x37] + param_4[0x38] * 8);
    if ((longlong *)lVar13 != local_3d8) {
      uVar11 = 10;
      if (plVar12 != (longlong *)0x0) {
        uVar11 = 300;
      }
      do {
        ppvVar1 = (void **)(lVar13 + 0x58);
        *(void **)(lVar13 + 0x60) = *ppvVar1;
        ppvVar2 = (void **)(lVar13 + 0x70);
        *(void **)(lVar13 + 0x78) = *ppvVar2;
        uVar31 = FUN_1403f0da0();
        FUN_1403f0fb0(uVar31,param_4,lVar13 + 8,local_2c8,&local_3f8);
        uVar31 = FUN_1403e7c10(param_1,*param_4);
        FUN_1403edf30(param_1,&local_3c8,param_2,param_3,&local_3f8,uVar31,uVar11);
        if (ppvVar1 != &local_380) {
          pvVar16 = *ppvVar1;
          *ppvVar1 = (void *)0x0;
          *(undefined8 *)(lVar13 + 0x60) = 0;
          uVar3 = *(undefined8 *)(lVar13 + 0x68);
          *(undefined8 *)(lVar13 + 0x68) = 0;
          pvVar24 = *ppvVar1;
          *ppvVar1 = pvVar16;
          *(void **)(lVar13 + 0x60) = pvVar16;
          *(undefined8 *)(lVar13 + 0x68) = uVar3;
          if (pvVar24 != (void *)0x0) {
            free(pvVar24);
          }
          pvVar16 = *ppvVar1;
          *ppvVar1 = local_380;
          uVar3 = *(undefined8 *)(lVar13 + 0x60);
          *(undefined8 *)(lVar13 + 0x60) = local_378;
          uVar4 = *(undefined8 *)(lVar13 + 0x68);
          *(undefined8 *)(lVar13 + 0x68) = local_370;
          local_380 = pvVar16;
          local_378 = uVar3;
          local_370 = uVar4;
        }
        if (ppvVar2 != &local_368) {
          pvVar16 = *ppvVar2;
          *ppvVar2 = (void *)0x0;
          *(undefined8 *)(lVar13 + 0x78) = 0;
          uVar3 = *(undefined8 *)(lVar13 + 0x80);
          *(undefined8 *)(lVar13 + 0x80) = 0;
          pvVar24 = *ppvVar2;
          *ppvVar2 = pvVar16;
          *(void **)(lVar13 + 0x78) = pvVar16;
          *(undefined8 *)(lVar13 + 0x80) = uVar3;
          if (pvVar24 != (void *)0x0) {
            free(pvVar24);
          }
          pvVar16 = *ppvVar2;
          *ppvVar2 = local_368;
          uVar3 = *(undefined8 *)(lVar13 + 0x78);
          *(undefined8 *)(lVar13 + 0x78) = local_360;
          uVar4 = *(undefined8 *)(lVar13 + 0x80);
          *(undefined8 *)(lVar13 + 0x80) = local_358;
          local_368 = pvVar16;
          local_360 = uVar3;
          local_358 = uVar4;
        }
        uVar9 = local_320;
        uVar8 = local_338;
        uVar4 = local_340;
        uVar3 = local_348;
        puVar7 = local_350;
        local_350 = &DAT_140a6d430;
        local_348 = 0;
        local_340 = 0;
        local_338 = 0;
        local_320 = 0;
        pvVar16 = *(void **)(lVar13 + 0x88);
        *(undefined1 **)(lVar13 + 0x88) = puVar7;
        *(undefined8 *)(lVar13 + 0x90) = uVar3;
        *(undefined8 *)(lVar13 + 0x98) = uVar4;
        lVar14 = *(longlong *)(lVar13 + 0xa0);
        *(undefined8 *)(lVar13 + 0xa0) = uVar8;
        *(undefined8 *)(lVar13 + 0xb8) = uVar9;
        if (lVar14 != 0) {
          pvVar24 = pvVar16;
          if ((0xfff < (lVar14 + 0x18U & 0xfffffffffffffff8) + lVar14 * 8) &&
             (pvVar24 = *(void **)((longlong)pvVar16 + -8),
             0x1f < (ulonglong)((longlong)pvVar16 + (-8 - (longlong)pvVar24)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar24);
        }
        *(undefined4 *)(lVar13 + 0xc0) = local_318;
        FUN_1403e3a70(lVar13 + 0x58);
        if ((*(char *)(param_4 + 0x11) == '\0') && (*(int *)(lVar13 + 0xc0) == 0)) {
          cVar10 = '\0';
        }
        else {
          cVar10 = '\x01';
        }
        *(char *)(param_4 + 0x11) = cVar10;
        puVar6 = local_2e8;
        while (puVar6 != (undefined8 *)0x0) {
          FUN_1402450d0(local_2f8,*puVar6);
          puVar5 = (undefined8 *)puVar6[1];
          free(puVar6);
          puVar6 = puVar5;
        }
        if (local_310 != (void *)0x0) {
          free(local_310);
        }
        FUN_140338520(&local_350);
        if (local_368 != (void *)0x0) {
          free(local_368);
        }
        pvVar16 = local_3c0;
        puVar6 = local_3a0;
        if (local_380 != (void *)0x0) {
          free(local_380);
          pvVar16 = local_3c0;
          puVar6 = local_3a0;
        }
        while (pvVar24 = local_3c8, local_3c0 = pvVar16, puVar6 != (undefined8 *)0x0) {
          FUN_1402450d0(local_3b0,*puVar6);
          puVar5 = (undefined8 *)puVar6[1];
          free(puVar6);
          pvVar16 = local_3c0;
          puVar6 = puVar5;
        }
        for (; pvVar24 != pvVar16; pvVar24 = (void *)((longlong)pvVar24 + 0x78)) {
          FUN_1403203e0((longlong)pvVar24 + 0x60);
        }
        plVar12 = local_3f8;
        plVar18 = plStack_3f0;
        if (local_3c8 != (void *)0x0) {
          free(local_3c8);
          plVar12 = local_3f8;
          plVar18 = plStack_3f0;
        }
        for (; plVar26 = plStack_3f0, plVar12 != plStack_3f0; plVar12 = plVar12 + 0xf) {
          plStack_3f0 = plVar18;
          FUN_1403203e0(plVar12 + 0xc);
          plVar18 = plStack_3f0;
          plStack_3f0 = plVar26;
        }
        plStack_3f0 = plVar18;
        if (local_3f8 != (longlong *)0x0) {
          free(local_3f8);
        }
        lVar13 = *(longlong *)(lVar13 + 0x1d8);
        while (lVar13 == 0) {
          plVar28 = plVar28 + 1;
          lVar13 = *plVar28;
        }
      } while ((longlong *)lVar13 != local_3d8);
    }
    _eh_vector_destructor_iterator_(local_2c8,0x40,10,FUN_1402f3f10);
  }
  return;
}

