// Candidate VA 140653e30; RVA 0x653e30
// Ghidra inferred prototype: undefined FUN_140653e30()

void FUN_140653e30(longlong param_1,undefined8 param_2,longlong param_3)

{
  longlong *plVar1;
  char cVar2;
  undefined4 uVar3;
  int *piVar4;
  bool bVar5;
  uint uVar6;
  longlong lVar7;
  undefined8 *puVar9;
  undefined8 *puVar10;
  undefined4 *puVar11;
  void *pvVar12;
  undefined4 *puVar13;
  ulonglong uVar14;
  longlong lVar15;
  undefined8 *puVar16;
  int iVar17;
  int iVar18;
  ulonglong *puVar19;
  size_t sVar20;
  ulonglong *puVar21;
  ulonglong *_Src;
  longlong lVar22;
  int *piVar23;
  undefined4 *puVar24;
  undefined8 *puVar25;
  undefined4 *local_78;
  longlong local_70;
  undefined4 local_68;
  uint uStack_64;
  uint local_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  ulonglong uStack_40;
  ulonglong uVar8;
  
  uVar8 = 0;
  local_78 = (undefined4 *)((ulonglong)local_78 & 0xffffffff00000000);
  if (*(longlong *)(param_3 + 600) != 0) {
    lVar15 = *(longlong *)(*(longlong *)(param_3 + 600) + 0x408);
    local_70 = lVar15;
    FUN_140417b50(param_1 + 0x48);
    *(undefined8 *)(param_1 + 0x68) = *(undefined8 *)(param_1 + 0x60);
    lVar7 = FUN_1402d82e0("asset_stations","Stations");
    local_58 = (void *)0x0;
    uStack_50 = 0;
    local_48 = 0;
    uStack_40 = 0;
    lVar22 = -1;
    do {
      lVar22 = lVar22 + 1;
    } while (*(char *)(lVar7 + lVar22) != '\0');
    FUN_140002c00(&local_58,lVar7);
    puVar24 = *(undefined4 **)(param_1 + 0x50);
    if (puVar24 < *(undefined4 **)(param_1 + 0x58)) {
      *(undefined4 **)(param_1 + 0x50) = puVar24 + 8;
      *puVar24 = (undefined4)local_58;
      puVar24[1] = local_58._4_4_;
      puVar24[2] = (undefined4)uStack_50;
      puVar24[3] = uStack_50._4_4_;
      *(undefined8 *)(puVar24 + 4) = local_48;
      *(ulonglong *)(puVar24 + 6) = uStack_40;
      local_48 = 0;
      uStack_40 = 0xf;
      local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
    }
    else {
      FUN_1404ba220(param_1 + 0x48,&local_58);
    }
    FUN_140002d30(&local_58);
    lVar7 = FUN_1402d82e0("asset_trains","Trains");
    local_58 = (void *)0x0;
    uStack_50 = 0;
    local_48 = 0;
    uStack_40 = 0;
    lVar22 = -1;
    do {
      lVar22 = lVar22 + 1;
    } while (*(char *)(lVar7 + lVar22) != '\0');
    FUN_140002c00(&local_58,lVar7);
    puVar24 = *(undefined4 **)(param_1 + 0x50);
    if (puVar24 < *(undefined4 **)(param_1 + 0x58)) {
      *(undefined4 **)(param_1 + 0x50) = puVar24 + 8;
      *puVar24 = (undefined4)local_58;
      puVar24[1] = local_58._4_4_;
      puVar24[2] = (undefined4)uStack_50;
      puVar24[3] = uStack_50._4_4_;
      *(undefined8 *)(puVar24 + 4) = local_48;
      *(ulonglong *)(puVar24 + 6) = uStack_40;
      local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
      uVar14 = 0xf;
    }
    else {
      FUN_1404ba220(param_1 + 0x48,&local_58);
      uVar14 = uStack_40;
    }
    if (0xf < uVar14) {
      FUN_140003040(&local_58,local_58);
    }
    lVar7 = FUN_1402d82e0("asset_lines","Lines");
    local_58 = (void *)0x0;
    uStack_50 = 0;
    local_48 = 0;
    uStack_40 = 0;
    lVar22 = -1;
    do {
      lVar22 = lVar22 + 1;
    } while (*(char *)(lVar7 + lVar22) != '\0');
    FUN_140002c00(&local_58,lVar7);
    puVar24 = *(undefined4 **)(param_1 + 0x50);
    if (puVar24 < *(undefined4 **)(param_1 + 0x58)) {
      *(undefined4 **)(param_1 + 0x50) = puVar24 + 8;
      *puVar24 = (undefined4)local_58;
      puVar24[1] = local_58._4_4_;
      puVar24[2] = (undefined4)uStack_50;
      puVar24[3] = uStack_50._4_4_;
      *(undefined8 *)(puVar24 + 4) = local_48;
      *(ulonglong *)(puVar24 + 6) = uStack_40;
      local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
      uVar14 = 0xf;
    }
    else {
      FUN_1404ba220(param_1 + 0x48,&local_58);
      uVar14 = uStack_40;
    }
    if (0xf < uVar14) {
      FUN_140003040(&local_58,local_58);
    }
    local_78._0_4_ = 1;
    FUN_140290a60(param_1 + 0x60,&local_78);
    local_78._0_4_ = 2;
    FUN_140290a60(param_1 + 0x60,&local_78);
    local_78 = (undefined4 *)CONCAT44(local_78._4_4_,3);
    FUN_140290a60(param_1 + 0x60,&local_78);
    *(undefined4 *)(param_1 + 0x24) = 0;
    iVar17 = (int)(*(longlong *)(param_1 + 0x68) - *(longlong *)(param_1 + 0x60) >> 2);
    if (0 < iVar17) {
      uVar14 = uVar8;
      do {
        if (*(int *)(param_1 + 8) == *(int *)(*(longlong *)(param_1 + 0x60) + uVar14)) {
          *(int *)(param_1 + 0x24) = (int)uVar8;
          break;
        }
        uVar6 = (int)uVar8 + 1;
        uVar8 = (ulonglong)uVar6;
        uVar14 = uVar14 + 4;
      } while ((int)uVar6 < iVar17);
    }
    FUN_140417b50(param_1 + 0x78);
    plVar1 = (longlong *)(param_1 + 0x90);
    *(longlong *)(param_1 + 0x98) = *plVar1;
    lVar7 = FUN_1402d82e0("sort_sep_accounting","--- Accounting ---");
    local_58 = (void *)0x0;
    uStack_50 = 0;
    local_48 = 0;
    uStack_40 = 0;
    lVar22 = -1;
    do {
      lVar22 = lVar22 + 1;
    } while (*(char *)(lVar7 + lVar22) != '\0');
    FUN_140002c00(&local_58,lVar7);
    puVar24 = *(undefined4 **)(param_1 + 0x80);
    if (puVar24 < *(undefined4 **)(param_1 + 0x88)) {
      *(undefined4 **)(param_1 + 0x80) = puVar24 + 8;
      *puVar24 = (undefined4)local_58;
      puVar24[1] = local_58._4_4_;
      puVar24[2] = (undefined4)uStack_50;
      puVar24[3] = uStack_50._4_4_;
      *(undefined8 *)(puVar24 + 4) = local_48;
      *(ulonglong *)(puVar24 + 6) = uStack_40;
      local_48 = 0;
      uStack_40 = 0xf;
      local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
    }
    else {
      FUN_1404ba220(param_1 + 0x78,&local_58);
    }
    FUN_140002d30(&local_58);
    local_60 = local_60 & 0xffffff00;
    FUN_1404b78d0(plVar1,&local_68);
    puVar24 = *(undefined4 **)(lVar15 + 0x480);
    local_78 = *(undefined4 **)(lVar15 + 0x488);
    if (puVar24 != local_78) {
      puVar25 = (undefined8 *)(puVar24 + 0x14);
      do {
        if ((undefined8 *)puVar25[2] != (undefined8 *)0x0) {
          puVar10 = (undefined8 *)puVar25[2];
          puVar16 = puVar25;
          do {
            if (*(int *)(puVar10 + 4) < *(int *)(param_1 + 8)) {
              puVar9 = (undefined8 *)*puVar10;
            }
            else {
              puVar9 = (undefined8 *)puVar10[1];
              puVar16 = puVar10;
            }
            puVar10 = puVar9;
          } while (puVar9 != (undefined8 *)0x0);
          if ((puVar16 != puVar25) && (*(int *)(puVar16 + 4) <= *(int *)(param_1 + 8))) {
            FUN_1403bbc40(param_1 + 0x78,puVar25 + -9);
            uVar6 = uStack_64;
            local_68 = *puVar24;
            uStack_64 = uStack_64 & 0xffffff00;
            local_60 = CONCAT31(local_60._1_3_,1);
            puVar21 = *(ulonglong **)(param_1 + 0x98);
            if (puVar21 < *(ulonglong **)(param_1 + 0xa0)) {
              *(ulonglong *)(param_1 + 0x98) = (longlong)puVar21 + 0xc;
              *puVar21 = CONCAT44(uVar6,local_68) & 0xffffff00ffffffff;
              *(uint *)(puVar21 + 1) = local_60;
            }
            else {
              lVar15 = (longlong)puVar21 - *plVar1 >> 2;
              if (lVar15 * -0x5555555555555555 == 0) {
                lVar7 = 1;
LAB_140654233:
                lVar15 = lVar7 * 0xc;
                puVar10 = (undefined8 *)thunk_FUN_140983da8(lVar15);
              }
              else {
                lVar7 = lVar15 * 0x5555555555555556;
                if (lVar7 != 0) goto LAB_140654233;
                puVar10 = (undefined8 *)0x0;
                lVar15 = lVar15 << 3;
              }
              pvVar12 = (void *)*plVar1;
              puVar16 = puVar10;
              if (pvVar12 != *(void **)(param_1 + 0x98)) {
                sVar20 = (longlong)*(void **)(param_1 + 0x98) - (longlong)pvVar12;
                pvVar12 = memmove(puVar10,pvVar12,sVar20);
                puVar16 = (undefined8 *)((longlong)pvVar12 + ((longlong)sVar20 >> 2) * 4);
              }
              *puVar16 = CONCAT44(uStack_64,local_68);
              *(uint *)(puVar16 + 1) = local_60;
              if ((void *)*plVar1 != (void *)0x0) {
                free((void *)*plVar1);
              }
              *plVar1 = (longlong)puVar10;
              *(longlong *)(param_1 + 0x98) = (longlong)puVar16 + 0xc;
              *(longlong *)(param_1 + 0xa0) = (longlong)puVar10 + lVar15;
            }
          }
        }
        puVar24 = puVar24 + 0x22;
        puVar25 = puVar25 + 0x11;
        lVar15 = local_70;
      } while (puVar24 != local_78);
    }
    lVar7 = FUN_1402d82e0("sort_sep_real_time","--- Realtime ---");
    local_58 = (void *)0x0;
    uStack_50 = 0;
    local_48 = 0;
    uStack_40 = 0;
    lVar22 = -1;
    do {
      lVar22 = lVar22 + 1;
    } while (*(char *)(lVar7 + lVar22) != '\0');
    FUN_140002c00(&local_58,lVar7);
    puVar24 = *(undefined4 **)(param_1 + 0x80);
    if (puVar24 < *(undefined4 **)(param_1 + 0x88)) {
      *(undefined4 **)(param_1 + 0x80) = puVar24 + 8;
      *puVar24 = (undefined4)local_58;
      puVar24[1] = local_58._4_4_;
      puVar24[2] = (undefined4)uStack_50;
      puVar24[3] = uStack_50._4_4_;
      *(undefined8 *)(puVar24 + 4) = local_48;
      *(ulonglong *)(puVar24 + 6) = uStack_40;
      local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
      uVar8 = 0xf;
    }
    else {
      FUN_1404ba220(param_1 + 0x78,&local_58);
      uVar8 = uStack_40;
    }
    if (0xf < uVar8) {
      FUN_140003040(&local_58,local_58);
    }
    local_60 = local_60 & 0xffffff00;
    FUN_1404b78d0(plVar1,&local_68);
    puVar24 = *(undefined4 **)(lVar15 + 0x4f8);
    puVar11 = *(undefined4 **)(lVar15 + 0x500);
    if (puVar24 != puVar11) {
      puVar25 = (undefined8 *)(puVar24 + 0xc);
      do {
        if ((undefined8 *)puVar25[2] != (undefined8 *)0x0) {
          puVar10 = (undefined8 *)puVar25[2];
          puVar16 = puVar25;
          do {
            if (*(int *)(puVar10 + 4) < *(int *)(param_1 + 8)) {
              puVar9 = (undefined8 *)*puVar10;
            }
            else {
              puVar9 = (undefined8 *)puVar10[1];
              puVar16 = puVar10;
            }
            puVar10 = puVar9;
          } while (puVar9 != (undefined8 *)0x0);
          if ((puVar16 != puVar25) && (*(int *)(puVar16 + 4) <= *(int *)(param_1 + 8))) {
            FUN_1403bbc40(param_1 + 0x78,puVar25 + -5);
            local_68 = *puVar24;
            uStack_64 = CONCAT31(uStack_64._1_3_,1);
            local_60 = CONCAT31(local_60._1_3_,1);
            puVar10 = *(undefined8 **)(param_1 + 0x98);
            if (puVar10 < *(undefined8 **)(param_1 + 0xa0)) {
              *(ulonglong *)(param_1 + 0x98) = (longlong)puVar10 + 0xc;
              *puVar10 = CONCAT44(uStack_64,local_68);
              *(uint *)(puVar10 + 1) = local_60;
            }
            else {
              lVar15 = (longlong)puVar10 - *(longlong *)(param_1 + 0x90) >> 2;
              if (lVar15 * -0x5555555555555555 == 0) {
                lVar15 = 1;
LAB_14065445c:
                puVar10 = (undefined8 *)thunk_FUN_140983da8(lVar15 * 0xc);
              }
              else {
                lVar15 = lVar15 * 0x5555555555555556;
                if (lVar15 != 0) goto LAB_14065445c;
                puVar10 = (undefined8 *)0x0;
              }
              pvVar12 = *(void **)(param_1 + 0x90);
              puVar16 = puVar10;
              if (pvVar12 != *(void **)(param_1 + 0x98)) {
                sVar20 = (longlong)*(void **)(param_1 + 0x98) - (longlong)pvVar12;
                pvVar12 = memmove(puVar10,pvVar12,sVar20);
                puVar16 = (undefined8 *)((longlong)pvVar12 + ((longlong)sVar20 >> 2) * 4);
              }
              *puVar16 = CONCAT44(uStack_64,local_68);
              *(uint *)(puVar16 + 1) = local_60;
              if (*(void **)(param_1 + 0x90) != (void *)0x0) {
                free(*(void **)(param_1 + 0x90));
              }
              *(undefined8 **)(param_1 + 0x90) = puVar10;
              *(longlong *)(param_1 + 0x98) = (longlong)puVar16 + 0xc;
              *(longlong *)(param_1 + 0xa0) = (longlong)puVar10 + lVar15 * 0xc;
            }
          }
        }
        puVar24 = puVar24 + 0x18;
        puVar25 = puVar25 + 0xc;
      } while (puVar24 != puVar11);
    }
    uVar14 = 0;
    *(undefined4 *)(param_1 + 0x20) = 0;
    lVar15 = *(longlong *)(param_1 + 0x90);
    uVar8 = uVar14;
    if (0 < (int)(*(longlong *)(param_1 + 0x98) - lVar15 >> 2) * -0x55555555) {
      do {
        if (*(char *)(uVar8 + 8 + lVar15) != '\0') {
          cVar2 = *(char *)(uVar8 + 4 + lVar15);
          if ((*(char *)(param_1 + 0x10) == cVar2) &&
             ((cVar2 == -1 || (*(int *)(param_1 + 0xc) == *(int *)(uVar8 + lVar15))))) {
            bVar5 = true;
          }
          else {
            bVar5 = false;
          }
          if (bVar5) {
            *(int *)(param_1 + 0x20) = (int)uVar14;
            break;
          }
        }
        uVar6 = (int)uVar14 + 1;
        uVar14 = (ulonglong)uVar6;
        uVar8 = uVar8 + 0xc;
      } while ((int)uVar6 <
               (int)(*(longlong *)(param_1 + 0x98) - *(longlong *)(param_1 + 0x90) >> 2) *
               -0x55555555);
    }
    lVar15 = param_1 + 0xa8;
    FUN_140417b50(lVar15);
    *(undefined8 *)(param_1 + 200) = *(undefined8 *)(param_1 + 0xc0);
    if (*(int *)(param_1 + 8) == 1) {
      puVar24 = &DAT_140a2ce28;
      do {
        puVar11 = (undefined4 *)0x0;
        uVar3 = *puVar24;
        uStack_50 = 0;
        local_48 = 0;
        uStack_40 = 0xf;
        local_58 = (void *)0x0;
        local_78 = (undefined4 *)((ulonglong)local_78 & 0xffffffff00000000 | 1);
        switch(uVar3) {
        case 0:
          lVar7 = FUN_1402d82e0("accounting_period_lifetime","Lifetime");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 1:
          lVar7 = FUN_1402d82e0("accounting_period_year",&DAT_140a84ab8);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 2:
          lVar7 = FUN_1402d82e0("accounting_period_quarter","Quarter");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 3:
          lVar7 = FUN_1402d82e0("accounting_period_month","Month");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 4:
          lVar7 = FUN_1402d82e0("accounting_period_week",&DAT_140a84b50);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 5:
          lVar7 = FUN_1402d82e0("accounting_period_day",&DAT_140a84b10);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        default:
          goto switchD_140654602_default;
        }
        FUN_140030630(&local_58,lVar7);
switchD_140654602_default:
        puVar25 = *(undefined8 **)(param_1 + 0xb0);
        if (puVar25 < *(undefined8 **)(param_1 + 0xb8)) {
          *(undefined8 **)(param_1 + 0xb0) = puVar25 + 4;
          *puVar25 = 0;
          puVar25[1] = 0;
          puVar25[2] = 0;
          puVar25[3] = 0;
          *puVar25 = local_58;
          puVar25[1] = uStack_50;
          puVar25[2] = local_48;
          puVar25[3] = uStack_40;
          local_48 = 0;
          uStack_40 = 0xf;
          local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
        }
        else {
          FUN_1404ba220(lVar15,&local_58);
        }
        if (0xf < uStack_40) {
          pvVar12 = local_58;
          if ((0xfff < uStack_40 + 1) &&
             (pvVar12 = *(void **)((longlong)local_58 + -8),
             0x1f < (ulonglong)((longlong)local_58 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        puVar13 = *(undefined4 **)(param_1 + 200);
        if (puVar13 < *(undefined4 **)(param_1 + 0xd0)) {
          *(undefined4 **)(param_1 + 200) = puVar13 + 1;
          *puVar13 = uVar3;
        }
        else {
          lVar7 = (longlong)puVar13 - *(longlong *)(param_1 + 0xc0) >> 2;
          if (lVar7 == 0) {
            lVar7 = 1;
LAB_1406547df:
            puVar11 = (undefined4 *)thunk_FUN_140983da8(lVar7 * 4);
          }
          else {
            lVar7 = lVar7 * 2;
            if (lVar7 != 0) goto LAB_1406547df;
          }
          pvVar12 = *(void **)(param_1 + 0xc0);
          puVar13 = puVar11;
          if (pvVar12 != *(void **)(param_1 + 200)) {
            sVar20 = (longlong)*(void **)(param_1 + 200) - (longlong)pvVar12;
            pvVar12 = memmove(puVar11,pvVar12,sVar20);
            puVar13 = (undefined4 *)((longlong)pvVar12 + ((longlong)sVar20 >> 2) * 4);
          }
          *puVar13 = uVar3;
          if (*(void **)(param_1 + 0xc0) != (void *)0x0) {
            free(*(void **)(param_1 + 0xc0));
          }
          *(undefined4 **)(param_1 + 0xc0) = puVar11;
          *(undefined4 **)(param_1 + 200) = puVar13 + 1;
          *(undefined4 **)(param_1 + 0xd0) = puVar11 + lVar7;
        }
        puVar24 = puVar24 + 1;
      } while (puVar24 != (undefined4 *)&DAT_140a2ce30);
    }
    else if (*(int *)(param_1 + 8) == 2) {
      puVar24 = &DAT_140a2ce20;
      do {
        puVar11 = (undefined4 *)0x0;
        uVar3 = *puVar24;
        uStack_50 = 0;
        local_48 = 0;
        uStack_40 = 0xf;
        local_58 = (void *)0x0;
        local_78 = (undefined4 *)((ulonglong)local_78 & 0xffffffff00000000 | 2);
        switch(uVar3) {
        case 0:
          lVar7 = FUN_1402d82e0("accounting_period_lifetime","Lifetime");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 1:
          lVar7 = FUN_1402d82e0("accounting_period_year",&DAT_140a84ab8);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 2:
          lVar7 = FUN_1402d82e0("accounting_period_quarter","Quarter");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 3:
          lVar7 = FUN_1402d82e0("accounting_period_month","Month");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 4:
          lVar7 = FUN_1402d82e0("accounting_period_week",&DAT_140a84b50);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 5:
          lVar7 = FUN_1402d82e0("accounting_period_day",&DAT_140a84b10);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        default:
          goto switchD_140654936_default;
        }
        FUN_140030630(&local_58,lVar7);
switchD_140654936_default:
        puVar25 = *(undefined8 **)(param_1 + 0xb0);
        if (puVar25 < *(undefined8 **)(param_1 + 0xb8)) {
          *(undefined8 **)(param_1 + 0xb0) = puVar25 + 4;
          *puVar25 = 0;
          puVar25[1] = 0;
          puVar25[2] = 0;
          puVar25[3] = 0;
          *puVar25 = local_58;
          puVar25[1] = uStack_50;
          puVar25[2] = local_48;
          puVar25[3] = uStack_40;
          local_48 = 0;
          uStack_40 = 0xf;
          local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
        }
        else {
          FUN_1404ba220(lVar15,&local_58);
        }
        if (0xf < uStack_40) {
          pvVar12 = local_58;
          if ((0xfff < uStack_40 + 1) &&
             (pvVar12 = *(void **)((longlong)local_58 + -8),
             0x1f < (ulonglong)((longlong)local_58 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        puVar13 = *(undefined4 **)(param_1 + 200);
        if (puVar13 < *(undefined4 **)(param_1 + 0xd0)) {
          *(undefined4 **)(param_1 + 200) = puVar13 + 1;
          *puVar13 = uVar3;
        }
        else {
          lVar7 = (longlong)puVar13 - *(longlong *)(param_1 + 0xc0) >> 2;
          if (lVar7 == 0) {
            lVar7 = 1;
LAB_140654b0f:
            puVar11 = (undefined4 *)thunk_FUN_140983da8(lVar7 * 4);
          }
          else {
            lVar7 = lVar7 * 2;
            if (lVar7 != 0) goto LAB_140654b0f;
          }
          pvVar12 = *(void **)(param_1 + 0xc0);
          puVar13 = puVar11;
          if (pvVar12 != *(void **)(param_1 + 200)) {
            sVar20 = (longlong)*(void **)(param_1 + 200) - (longlong)pvVar12;
            pvVar12 = memmove(puVar11,pvVar12,sVar20);
            puVar13 = (undefined4 *)((longlong)pvVar12 + ((longlong)sVar20 >> 2) * 4);
          }
          *puVar13 = uVar3;
          if (*(void **)(param_1 + 0xc0) != (void *)0x0) {
            free(*(void **)(param_1 + 0xc0));
          }
          *(undefined4 **)(param_1 + 0xc0) = puVar11;
          *(undefined4 **)(param_1 + 200) = puVar13 + 1;
          *(undefined4 **)(param_1 + 0xd0) = puVar11 + lVar7;
        }
        puVar24 = puVar24 + 1;
      } while (puVar24 != &DAT_140a2ce28);
    }
    else {
      puVar24 = &DAT_140a2ce40;
      do {
        puVar11 = (undefined4 *)0x0;
        uVar3 = *puVar24;
        uStack_50 = 0;
        local_48 = 0;
        uStack_40 = 0xf;
        local_58 = (void *)0x0;
        local_78 = (undefined4 *)((ulonglong)local_78 & 0xffffffff00000000 | 4);
        switch(uVar3) {
        case 0:
          lVar7 = FUN_1402d82e0("accounting_period_lifetime","Lifetime");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 1:
          lVar7 = FUN_1402d82e0("accounting_period_year",&DAT_140a84ab8);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 2:
          lVar7 = FUN_1402d82e0("accounting_period_quarter","Quarter");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 3:
          lVar7 = FUN_1402d82e0("accounting_period_month","Month");
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 4:
          lVar7 = FUN_1402d82e0("accounting_period_week",&DAT_140a84b50);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        case 5:
          lVar7 = FUN_1402d82e0("accounting_period_day",&DAT_140a84b10);
          lVar22 = -1;
          do {
            lVar22 = lVar22 + 1;
          } while (*(char *)(lVar7 + lVar22) != '\0');
          break;
        default:
          goto switchD_140654bf7_default;
        }
        FUN_140030630(&local_58,lVar7);
switchD_140654bf7_default:
        puVar25 = *(undefined8 **)(param_1 + 0xb0);
        if (puVar25 < *(undefined8 **)(param_1 + 0xb8)) {
          *(undefined8 **)(param_1 + 0xb0) = puVar25 + 4;
          *puVar25 = 0;
          puVar25[1] = 0;
          puVar25[2] = 0;
          puVar25[3] = 0;
          *puVar25 = local_58;
          puVar25[1] = uStack_50;
          puVar25[2] = local_48;
          puVar25[3] = uStack_40;
          local_48 = 0;
          uStack_40 = 0xf;
          local_58 = (void *)((ulonglong)local_58 & 0xffffffffffffff00);
        }
        else {
          FUN_1404ba220(lVar15,&local_58);
        }
        if (0xf < uStack_40) {
          pvVar12 = local_58;
          if ((0xfff < uStack_40 + 1) &&
             (pvVar12 = *(void **)((longlong)local_58 + -8),
             0x1f < (ulonglong)((longlong)local_58 + (-8 - (longlong)pvVar12)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar12);
        }
        puVar13 = *(undefined4 **)(param_1 + 200);
        if (puVar13 < *(undefined4 **)(param_1 + 0xd0)) {
          *(undefined4 **)(param_1 + 200) = puVar13 + 1;
          *puVar13 = uVar3;
        }
        else {
          lVar7 = (longlong)puVar13 - *(longlong *)(param_1 + 0xc0) >> 2;
          if (lVar7 == 0) {
            lVar7 = 1;
LAB_140654dcf:
            puVar11 = (undefined4 *)thunk_FUN_140983da8(lVar7 * 4);
          }
          else {
            lVar7 = lVar7 * 2;
            if (lVar7 != 0) goto LAB_140654dcf;
          }
          pvVar12 = *(void **)(param_1 + 0xc0);
          puVar13 = puVar11;
          if (pvVar12 != *(void **)(param_1 + 200)) {
            sVar20 = (longlong)*(void **)(param_1 + 200) - (longlong)pvVar12;
            pvVar12 = memmove(puVar11,pvVar12,sVar20);
            puVar13 = (undefined4 *)((longlong)pvVar12 + ((longlong)sVar20 >> 2) * 4);
          }
          *puVar13 = uVar3;
          if (*(void **)(param_1 + 0xc0) != (void *)0x0) {
            free(*(void **)(param_1 + 0xc0));
          }
          *(undefined4 **)(param_1 + 0xc0) = puVar11;
          *(undefined4 **)(param_1 + 200) = puVar13 + 1;
          *(undefined4 **)(param_1 + 0xd0) = puVar11 + lVar7;
        }
        puVar24 = puVar24 + 1;
      } while (puVar24 != (undefined4 *)&DAT_140a2ce58);
    }
    *(undefined4 *)(param_1 + 0x28) = 0;
    piVar4 = *(int **)(param_1 + 0xc0);
    iVar17 = 0;
    iVar18 = 0;
    if (0 < (int)(*(longlong *)(param_1 + 200) - (longlong)piVar4 >> 2)) {
      piVar23 = piVar4;
      iVar17 = 0;
      do {
        if (*(int *)(param_1 + 0x14) == *piVar23) {
          *(int *)(param_1 + 0x28) = iVar17;
          iVar18 = iVar17;
          goto LAB_140654e7d;
        }
        iVar18 = iVar17 + 1;
        piVar23 = piVar23 + 1;
        iVar17 = iVar18;
      } while (iVar18 < (int)(*(longlong *)(param_1 + 200) - (longlong)piVar4 >> 2));
      iVar17 = 0;
    }
LAB_140654e7d:
    if ((int)(*(longlong *)(param_1 + 200) - (longlong)piVar4 >> 2) <= iVar18) {
      *(undefined4 *)(param_1 + 0x28) = 0;
      *(int *)(param_1 + 0x14) = *piVar4;
      iVar17 = 0;
    }
    *(undefined4 *)(param_1 + 0x30) = *(undefined4 *)(param_1 + 0x24);
    *(undefined4 *)(param_1 + 0x2c) = *(undefined4 *)(param_1 + 0x20);
    *(int *)(param_1 + 0x34) = iVar17;
    *(undefined4 *)(param_1 + 0x40) = *(undefined4 *)(param_1 + 0x18);
    *(undefined4 *)(param_1 + 0x38) = *(undefined4 *)(param_1 + 0x1c);
    if ((*(char *)(param_1 + 0x259) != '\0') || (*(char *)(param_1 + 0x25a) != '\0')) {
      FUN_1406533c0(param_1);
      *(undefined1 *)(param_1 + 0x25a) = 0;
      lVar15 = *(longlong *)(param_3 + 0x260);
      if (lVar15 == 0) goto LAB_140654fd9;
      *(longlong *)(param_1 + 0x260) =
           (*(longlong *)(lVar15 + 0x28) + *(longlong *)(lVar15 + 0x20) * 100) * 10000;
    }
    if (*(longlong *)(param_3 + 600) != 0) {
      lVar15 = *(longlong *)(*(longlong *)(param_3 + 600) + 0x420);
      puVar21 = *(ulonglong **)(param_1 + 0x220);
      if (puVar21 != *(ulonglong **)(param_1 + 0x228)) {
        _Src = puVar21 + 1;
        do {
          puVar19 = *(ulonglong **)
                     (*(longlong *)(lVar15 + 0x10) +
                     (*puVar21 % (ulonglong)*(uint *)(lVar15 + 0x18)) * 8);
          lVar7 = 0;
          if (puVar19 == (ulonglong *)0x0) {
LAB_140654f89:
            if (_Src < *(ulonglong **)(param_1 + 0x228)) {
              memmove(puVar21,_Src,(longlong)*(ulonglong **)(param_1 + 0x228) - (longlong)_Src);
            }
            *(longlong *)(param_1 + 0x228) = *(longlong *)(param_1 + 0x228) + -8;
          }
          else {
            do {
              lVar22 = lVar7 + 1;
              if (*puVar21 != *puVar19) {
                lVar22 = lVar7;
              }
              puVar19 = (ulonglong *)puVar19[0x10];
              lVar7 = lVar22;
            } while (puVar19 != (ulonglong *)0x0);
            if (lVar22 == 0) goto LAB_140654f89;
            puVar21 = puVar21 + 1;
            _Src = _Src + 1;
          }
        } while (puVar21 != *(ulonglong **)(param_1 + 0x228));
      }
      *(undefined1 *)(param_1 + 0x269) = *(undefined1 *)(param_1 + 0x268);
      return;
    }
  }
LAB_140654fd9:
                    /* WARNING: Subroutine does not return */
  abort();
}


// Incoming references
// 0xc32510 DATA caller none
// 0x656fe6 UNCONDITIONAL_CALL caller 140656fb0
