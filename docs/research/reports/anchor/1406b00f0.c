
void FUN_1406b00f0(longlong param_1,longlong param_2,longlong param_3,longlong param_4,
                  longlong param_5)

{
  int *piVar1;
  longlong *plVar2;
  longlong *plVar3;
  uint uVar4;
  void *pvVar5;
  undefined4 *puVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined8 *puVar9;
  char cVar10;
  ulonglong *puVar11;
  ulonglong *puVar12;
  void *pvVar13;
  ulonglong *puVar14;
  longlong lVar15;
  longlong lVar16;
  int iVar17;
  int iVar18;
  undefined4 uVar19;
  undefined1 *puVar20;
  longlong lVar21;
  ulonglong *puVar22;
  ulonglong uVar23;
  ulonglong *puVar24;
  ulonglong *puVar25;
  ulonglong uVar26;
  ulonglong *puVar27;
  int local_res8;
  ulonglong local_res18;
  longlong local_res20;
  undefined8 local_1e0;
  ulonglong *local_1d8;
  ulonglong *puStack_1d0;
  ulonglong *local_1c8;
  ulonglong local_1b8;
  ulonglong uStack_1b0;
  undefined8 local_1a8;
  undefined2 local_1a0;
  void *local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  void *pvStack_180;
  undefined8 local_178;
  undefined8 uStack_170;
  void *local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined1 *local_150;
  undefined8 local_148;
  undefined8 local_140;
  longlong local_138;
  undefined8 local_120;
  undefined1 local_118 [8];
  undefined8 *local_110;
  ulonglong local_108;
  undefined8 local_100;
  undefined4 local_f8;
  undefined8 local_f4;
  void *local_e8;
  undefined8 uStack_e0;
  undefined8 local_d8;
  char local_d0;
  undefined8 local_c8;
  undefined8 local_c0;
  longlong *local_b8;
  ulonglong *local_b0;
  ulonglong *puStack_a8;
  ulonglong *local_a0;
  ulonglong *local_98;
  longlong local_90;
  longlong *local_88;
  longlong local_80;
  longlong *local_78;
  longlong local_70 [6];
  
  if ((param_4 != *(longlong *)(param_1 + 0xd8)) || (*(longlong *)(param_1 + 0xe0) != param_5)) {
    *(undefined8 *)(param_1 + 200) = 0;
    *(undefined8 *)(param_1 + 0xd0) = 0;
  }
  lVar15 = *(longlong *)(param_3 + 0x1658);
  if (*(longlong *)(param_3 + 0x1660) != 0) {
    LOCK();
    piVar1 = (int *)(*(longlong *)(param_3 + 0x1660) + 8);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  local_88 = *(longlong **)(param_3 + 0x1660);
  local_res20 = param_4;
  local_90 = lVar15;
  local_80 = lVar15;
  local_78 = local_88;
  if ((lVar15 != 0) &&
     ((*(longlong *)(lVar15 + 0x2a0) != *(longlong *)(param_1 + 200) ||
      (*(longlong *)(lVar15 + 0x2a8) != *(longlong *)(param_1 + 0xd0))))) {
    *(longlong *)(param_1 + 200) = *(longlong *)(lVar15 + 0x2a0);
    *(undefined8 *)(param_1 + 0xd0) = *(undefined8 *)(lVar15 + 0x2a8);
    plVar3 = (longlong *)(param_1 + 0x108);
    lVar16 = *(longlong *)(param_1 + 0x110);
    local_b8 = plVar3;
    for (lVar21 = *plVar3; lVar21 != lVar16; lVar21 = lVar21 + 0x20) {
      if (*(void **)(lVar21 + 8) != (void *)0x0) {
        free(*(void **)(lVar21 + 8));
      }
    }
    local_b8 = (longlong *)(param_1 + 0x108);
    *(longlong *)(param_1 + 0x110) = *plVar3;
    *(longlong *)(param_1 + 0xd8) = local_res20;
    *(longlong *)(param_1 + 0xe0) = param_5;
    local_res8 = 0;
    do {
      local_1e0 = 0;
      local_1d8 = (ulonglong *)0x0;
      puStack_1d0 = (ulonglong *)0x0;
      local_98 = (ulonglong *)0x0;
      local_1c8 = (ulonglong *)0x0;
      local_1b8 = 0;
      uStack_1b0 = 0;
      puVar22 = (ulonglong *)0x0;
      local_1a8 = 0;
      local_1a0 = 0;
      local_198 = (void *)0x0;
      uStack_190 = 0;
      local_188 = 0;
      pvStack_180 = (void *)0x0;
      local_178 = 0;
      uStack_170 = 0;
      local_168 = (void *)0x0;
      uStack_160 = 0;
      local_158 = 0;
      local_150 = &DAT_140a6d430;
      local_148 = 0;
      local_140 = 0;
      local_138 = 0;
      local_120 = 0;
      local_f8 = 0x3f800000;
      local_f4 = 0x40000000;
      local_108 = 1;
      local_110 = &DAT_140b5abc0;
      local_100 = 0;
      local_e8 = (void *)0x0;
      uStack_e0 = 0;
      local_d8 = 0;
      local_d0 = '\0';
      local_c8 = 0;
      local_c0 = 0;
      cVar10 = FUN_14035ea20(lVar15,&local_1b8,local_res20,local_res8,param_5,1,0x7fffffffffffffff);
      if (cVar10 == '\0') {
        if (local_e8 != (void *)0x0) {
          free(local_e8);
        }
        uVar26 = local_108;
        puVar9 = local_110;
        uVar23 = 0;
        if (local_108 != 0) {
          do {
            pvVar13 = (void *)puVar9[uVar23];
            while (pvVar13 != (void *)0x0) {
              pvVar5 = *(void **)((longlong)pvVar13 + 0x68);
              free(pvVar13);
              pvVar13 = pvVar5;
            }
            puVar9[uVar23] = 0;
            uVar23 = uVar23 + 1;
          } while (uVar23 < uVar26);
        }
joined_r0x0001406b0bbc:
        local_100 = 0;
        if (1 < local_108) {
          local_100 = 0;
          free(local_110);
        }
        local_res8 = local_res8 + 600;
        if (local_138 != 0) {
          puVar20 = local_150;
          if ((0xfff < local_138 * 8 + (local_138 + 0x18U & 0xfffffffffffffff8)) &&
             (puVar20 = *(undefined1 **)(local_150 + -8),
             (undefined1 *)0x1f < local_150 + (-8 - (longlong)puVar20))) {
LAB_1406b0bcb:
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar20);
          local_150 = &DAT_140a6d430;
          local_120 = 0;
          local_138 = 0;
          local_140 = 0;
          local_148 = 0;
        }
        if (local_168 != (void *)0x0) {
          free(local_168);
        }
        if (pvStack_180 != (void *)0x0) {
          free(pvStack_180);
        }
        if (local_198 != (void *)0x0) {
          free(local_198);
        }
      }
      else {
        uVar26 = local_108 & 0xffffffff;
        puVar11 = puVar22;
        if (local_d0 != '\0') {
          puVar24 = puVar22;
          for (uVar4 = local_1a8._4_4_; uVar4 != (uint)local_1a8;
              uVar4 = *(uint *)((longlong)local_168 + (ulonglong)uVar4 * 4)) {
            puVar24 = (ulonglong *)(ulonglong)uVar4;
          }
          if (puVar24 < (ulonglong *)
                        ((*(longlong *)(lVar15 + 0x168) - *(longlong *)(lVar15 + 0x160) >> 3) *
                        0x1cac083126e978d5)) {
            uVar23 = *(ulonglong *)((longlong)puVar24 * 1000 + *(longlong *)(lVar15 + 0x160));
            for (puVar24 = (ulonglong *)local_110[uVar23 % uVar26]; puVar24 != (ulonglong *)0x0;
                puVar24 = (ulonglong *)puVar24[0xd]) {
              if (uVar23 == *puVar24) {
                if ((puVar24 != (ulonglong *)0x0) && (puVar24 != (ulonglong *)local_110[local_108]))
                {
                  puVar11 = puVar24 + 1;
                }
                break;
              }
            }
          }
        }
        if (puVar11 == (ulonglong *)0x0) {
          if (local_e8 != (void *)0x0) {
            free(local_e8);
          }
          uVar26 = local_108;
          puVar9 = local_110;
          uVar23 = 0;
          if (local_108 != 0) {
            do {
              pvVar13 = (void *)puVar9[uVar23];
              while (pvVar13 != (void *)0x0) {
                pvVar5 = *(void **)((longlong)pvVar13 + 0x68);
                free(pvVar13);
                pvVar13 = pvVar5;
              }
              puVar9[uVar23] = 0;
              uVar23 = uVar23 + 1;
            } while (uVar23 < uVar26);
          }
          goto joined_r0x0001406b0bbc;
        }
        iVar17 = (*(int *)((longlong)puVar11 + 0x24) - local_res8) % 0x93a80;
        if (iVar17 < 0) {
          iVar17 = iVar17 + 0x93a80;
        }
        local_1e0 = CONCAT44(iVar17,local_res8);
        local_b0 = (ulonglong *)0x0;
        puStack_a8 = (ulonglong *)0x0;
        local_a0 = (ulonglong *)0x0;
        local_res18 = uStack_1b0;
        puVar11 = (ulonglong *)0x0;
        if (uStack_1b0 == local_1b8) {
LAB_1406b0a3a:
          local_res8 = local_res8 + 600;
          if (puVar11 != (ulonglong *)0x0) {
            free(puVar11);
          }
          if (local_e8 != (void *)0x0) {
            free(local_e8);
          }
          uVar26 = local_108;
          puVar9 = local_110;
          uVar23 = 0;
          if (local_108 != 0) {
            do {
              pvVar13 = (void *)puVar9[uVar23];
              while (pvVar13 != (void *)0x0) {
                pvVar5 = *(void **)((longlong)pvVar13 + 0x68);
                free(pvVar13);
                pvVar13 = pvVar5;
              }
              puVar9[uVar23] = 0;
              uVar23 = uVar23 + 1;
            } while (uVar23 < uVar26);
          }
          local_100 = 0;
          if (1 < local_108) {
            free(local_110);
          }
          if (local_138 != 0) {
            puVar20 = local_150;
            if ((0xfff < local_138 * 8 + (local_138 + 0x18U & 0xfffffffffffffff8)) &&
               (puVar20 = *(undefined1 **)(local_150 + -8),
               (undefined1 *)0x1f < local_150 + (-8 - (longlong)puVar20))) goto LAB_1406b0bcb;
            free(puVar20);
            local_150 = &DAT_140a6d430;
            local_148 = 0;
            local_140 = 0;
            local_138 = 0;
            local_120 = 0;
          }
          if (local_168 != (void *)0x0) {
            free(local_168);
          }
          if (pvStack_180 != (void *)0x0) {
            free(pvStack_180);
          }
          if (local_198 != (void *)0x0) {
            free(local_198);
          }
        }
        else {
          puVar24 = puVar11;
          while (puVar27 = puStack_a8, puVar12 = (ulonglong *)local_110[local_res18 % uVar26],
                puVar25 = (ulonglong *)0x0, puVar11 = puVar24, puVar12 != (ulonglong *)0x0) {
            do {
              if (local_res18 == *puVar12) {
                puVar25 = (ulonglong *)((longlong)puVar25 + 1);
              }
              puVar12 = (ulonglong *)puVar12[0xd];
            } while (puVar12 != (ulonglong *)0x0);
            if (puVar25 == (ulonglong *)0x0) break;
            FUN_140364ee0(local_118,local_70,puVar25,&local_res18,local_res18);
            lVar15 = local_70[0];
            if (puVar27 < puVar22) {
              uVar26 = *(ulonglong *)(local_70[0] + 0x10);
              *puVar27 = *(ulonglong *)(local_70[0] + 8);
              puVar27[1] = uVar26;
              uVar26 = *(ulonglong *)(local_70[0] + 0x20);
              puVar27[2] = *(ulonglong *)(local_70[0] + 0x18);
              puVar27[3] = uVar26;
              uVar26 = *(ulonglong *)(local_70[0] + 0x30);
              puVar27[4] = *(ulonglong *)(local_70[0] + 0x28);
              puVar27[5] = uVar26;
              uVar26 = *(ulonglong *)(local_70[0] + 0x40);
              puVar27[6] = *(ulonglong *)(local_70[0] + 0x38);
              puVar27[7] = uVar26;
              uVar19 = *(undefined4 *)(local_70[0] + 0x4c);
              uVar7 = *(undefined4 *)(local_70[0] + 0x50);
              uVar8 = *(undefined4 *)(local_70[0] + 0x54);
              *(undefined4 *)(puVar27 + 8) = *(undefined4 *)(local_70[0] + 0x48);
              *(undefined4 *)((longlong)puVar27 + 0x44) = uVar19;
              *(undefined4 *)(puVar27 + 9) = uVar7;
              *(undefined4 *)((longlong)puVar27 + 0x4c) = uVar8;
              uVar19 = *(undefined4 *)(local_70[0] + 0x5c);
              uVar7 = *(undefined4 *)(local_70[0] + 0x60);
              uVar8 = *(undefined4 *)(local_70[0] + 100);
              *(undefined4 *)(puVar27 + 10) = *(undefined4 *)(local_70[0] + 0x58);
              *(undefined4 *)((longlong)puVar27 + 0x54) = uVar19;
              *(undefined4 *)(puVar27 + 0xb) = uVar7;
              *(undefined4 *)((longlong)puVar27 + 0x5c) = uVar8;
              puVar12 = puVar27;
            }
            else {
              lVar16 = (longlong)puVar27 - (longlong)puVar24 >> 5;
              if (lVar16 * -0x5555555555555555 == 0) {
                lVar16 = 1;
LAB_1406b0606:
                puVar11 = (ulonglong *)thunk_FUN_140983da8(lVar16 * 0x60);
              }
              else {
                lVar16 = lVar16 * 0x5555555555555556;
                puVar11 = (ulonglong *)0x0;
                if (lVar16 != 0) goto LAB_1406b0606;
              }
              puVar12 = puVar11;
              if (puVar24 != puVar27) {
                pvVar13 = memmove(puVar11,puVar24,(longlong)puVar27 - (longlong)puVar24);
                puVar12 = (ulonglong *)
                          ((longlong)pvVar13 + ((longlong)puVar27 - (longlong)puVar24 >> 5) * 0x20);
              }
              uVar26 = *(ulonglong *)(lVar15 + 0x10);
              *puVar12 = *(ulonglong *)(lVar15 + 8);
              puVar12[1] = uVar26;
              uVar26 = *(ulonglong *)(lVar15 + 0x20);
              puVar12[2] = *(ulonglong *)(lVar15 + 0x18);
              puVar12[3] = uVar26;
              uVar26 = *(ulonglong *)(lVar15 + 0x30);
              puVar12[4] = *(ulonglong *)(lVar15 + 0x28);
              puVar12[5] = uVar26;
              uVar26 = *(ulonglong *)(lVar15 + 0x40);
              puVar12[6] = *(ulonglong *)(lVar15 + 0x38);
              puVar12[7] = uVar26;
              uVar19 = *(undefined4 *)(lVar15 + 0x4c);
              uVar7 = *(undefined4 *)(lVar15 + 0x50);
              uVar8 = *(undefined4 *)(lVar15 + 0x54);
              *(undefined4 *)(puVar12 + 8) = *(undefined4 *)(lVar15 + 0x48);
              *(undefined4 *)((longlong)puVar12 + 0x44) = uVar19;
              *(undefined4 *)(puVar12 + 9) = uVar7;
              *(undefined4 *)((longlong)puVar12 + 0x4c) = uVar8;
              uVar19 = *(undefined4 *)(lVar15 + 0x5c);
              uVar7 = *(undefined4 *)(lVar15 + 0x60);
              uVar8 = *(undefined4 *)(lVar15 + 100);
              *(undefined4 *)(puVar12 + 10) = *(undefined4 *)(lVar15 + 0x58);
              *(undefined4 *)((longlong)puVar12 + 0x54) = uVar19;
              *(undefined4 *)(puVar12 + 0xb) = uVar7;
              *(undefined4 *)((longlong)puVar12 + 0x5c) = uVar8;
              if (puVar24 != (ulonglong *)0x0) {
                free(puVar24);
              }
              puVar22 = puVar11 + lVar16 * 0xc;
              local_b0 = puVar11;
              local_a0 = puVar22;
            }
            puStack_a8 = puVar12 + 0xc;
            local_res18 = *(ulonglong *)(lVar15 + 8);
            if (local_res18 == local_1b8) break;
            puVar24 = puVar11;
            uVar26 = local_108 & 0xffffffff;
          }
          if (puVar11 == puStack_a8) goto LAB_1406b0a3a;
          local_res18 = CONCAT44(local_res18._4_4_,*(int *)((longlong)puStack_a8 + -0x3c));
          puVar22 = puStack_1d0;
          puVar24 = local_98;
          puVar12 = local_1d8;
          puVar27 = puStack_a8 + -6;
          iVar18 = *(int *)((longlong)puStack_a8 + -0x3c);
          do {
            if (puVar22 < puVar24) {
              *puVar22 = 0;
              puVar22[1] = 0;
              puVar22[2] = 0;
              puVar22[3] = 0;
              puVar22[4] = 0;
              puVar22[5] = 0;
              puVar22[6] = 0;
              puVar22[7] = 0;
              puVar25 = puVar12;
              puVar14 = puVar22;
            }
            else {
              lVar15 = (longlong)puVar22 - (longlong)puVar12 >> 6;
              if (lVar15 == 0) {
                lVar15 = 1;
LAB_1406b0745:
                puVar25 = (ulonglong *)thunk_FUN_140983da8(lVar15 << 6);
              }
              else {
                lVar15 = lVar15 * 2;
                if (lVar15 != 0) goto LAB_1406b0745;
                puVar25 = (ulonglong *)0x0;
              }
              puVar14 = puVar25;
              if (puVar12 != puVar22) {
                pvVar13 = memmove(puVar25,puVar12,(longlong)puVar22 - (longlong)puVar12);
                puVar14 = (ulonglong *)
                          ((longlong)pvVar13 +
                          ((longlong)puVar22 - (longlong)puVar12 & 0xffffffffffffffc0U));
              }
              *puVar14 = 0;
              puVar14[1] = 0;
              puVar14[2] = 0;
              puVar14[3] = 0;
              puVar14[4] = 0;
              puVar14[5] = 0;
              puVar14[6] = 0;
              puVar14[7] = 0;
              if (puVar12 != (ulonglong *)0x0) {
                free(puVar12);
              }
              puVar24 = puVar25 + lVar15 * 8;
              iVar18 = (int)local_res18;
              local_1d8 = puVar25;
              local_1c8 = puVar24;
            }
            puVar22 = puVar14 + 8;
            *puVar14 = puVar27[-1];
            puVar14[1] = *puVar27;
            puVar14[2] = puVar27[-3];
            *(int *)(puVar14 + 5) = (int)puVar27[-2];
            *(int *)(puVar14 + 6) = (int)puVar27[-4];
            puVar14[3] = puVar27[4];
            *(int *)((longlong)puVar14 + 0x2c) = (int)puVar27[5];
            *(int *)((longlong)puVar14 + 0x34) = (int)puVar27[3];
            *(int *)(puVar14 + 7) = *(int *)((longlong)puVar27 + -0xc) - iVar18;
            *(int *)((longlong)puVar14 + 0x3c) = *(int *)((longlong)puVar27 + 0x2c) - iVar18;
            puStack_1d0 = puVar22;
            if (puVar27[-3] == 0) {
              lVar15 = 0;
            }
            else {
              lVar15 = FUN_14033f7f0(*(longlong *)(param_2 + 0x780) + 0x180);
            }
            if (puVar27[4] == 0) {
              lVar16 = 0;
            }
            else {
              lVar16 = FUN_14033f7f0(*(longlong *)(param_2 + 0x780) + 0x180);
            }
            if (lVar15 == 0) {
              uVar19 = 0xff000000;
            }
            else {
              uVar19 = *(undefined4 *)(lVar15 + 0xb8);
            }
            *(undefined4 *)(puVar14 + 4) = uVar19;
            if (lVar16 == 0) {
              uVar19 = 0xff000000;
            }
            else {
              uVar19 = *(undefined4 *)(lVar16 + 0xb8);
            }
            *(undefined4 *)((longlong)puVar14 + 0x24) = uVar19;
            puVar14 = puVar27 + -6;
            puVar12 = puVar25;
            puVar27 = puVar27 + -0xc;
            iVar18 = (int)local_res18;
          } while (puVar14 != puVar11);
          if (iVar17 < 2) {
            local_res8 = local_res8 + 300;
          }
          else {
            local_res8 = local_res8 + 1 + iVar17;
          }
          puVar6 = *(undefined4 **)((longlong)local_b8 + 8);
          if (puVar6 < *(undefined4 **)((longlong)local_b8 + 0x10)) {
            *puVar6 = (undefined4)local_1e0;
            puVar6[1] = iVar17;
            *(ulonglong **)(puVar6 + 2) = puVar25;
            *(ulonglong **)(puVar6 + 4) = puVar22;
            *(ulonglong **)(puVar6 + 6) = puVar24;
            *(longlong *)((longlong)local_b8 + 8) = *(longlong *)((longlong)local_b8 + 8) + 0x20;
            puVar22 = (ulonglong *)0x0;
          }
          else {
            FUN_1406278b0(local_b8,&local_1e0);
            puVar22 = local_1d8;
          }
          if (puVar11 != (ulonglong *)0x0) {
            free(puVar11);
          }
          if (local_e8 != (void *)0x0) {
            free(local_e8);
          }
          uVar26 = local_108;
          puVar9 = local_110;
          uVar23 = 0;
          if (local_108 != 0) {
            do {
              pvVar13 = (void *)puVar9[uVar23];
              while (pvVar13 != (void *)0x0) {
                pvVar5 = *(void **)((longlong)pvVar13 + 0x68);
                free(pvVar13);
                pvVar13 = pvVar5;
              }
              puVar9[uVar23] = 0;
              uVar23 = uVar23 + 1;
            } while (uVar23 < uVar26);
          }
          local_100 = 0;
          if (1 < local_108) {
            free(local_110);
          }
          if (local_138 != 0) {
            puVar20 = local_150;
            if ((0xfff < local_138 * 8 + (local_138 + 0x18U & 0xfffffffffffffff8)) &&
               (puVar20 = *(undefined1 **)(local_150 + -8),
               (undefined1 *)0x1f < local_150 + (-8 - (longlong)puVar20))) goto LAB_1406b0bcb;
            free(puVar20);
            local_150 = &DAT_140a6d430;
            local_148 = 0;
            local_140 = 0;
            local_138 = 0;
            local_120 = 0;
          }
          if (local_168 != (void *)0x0) {
            free(local_168);
          }
          if (pvStack_180 != (void *)0x0) {
            free(pvStack_180);
          }
          if (local_198 != (void *)0x0) {
            free(local_198);
          }
          if (puVar22 != (ulonglong *)0x0) {
            free(puVar22);
          }
        }
      }
      lVar15 = local_90;
    } while (local_res8 < 0x93a80);
  }
  plVar3 = local_88;
  if (local_88 != (longlong *)0x0) {
    LOCK();
    plVar2 = local_88 + 1;
    lVar15 = *plVar2;
    *(int *)plVar2 = (int)*plVar2 + -1;
    UNLOCK();
    if ((int)lVar15 == 1) {
      (**(code **)*local_88)(local_88);
      LOCK();
      piVar1 = (int *)((longlong)plVar3 + 0xc);
      iVar17 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar17 == 1) {
        (**(code **)(*plVar3 + 8))(plVar3);
      }
    }
  }
  return;
}

