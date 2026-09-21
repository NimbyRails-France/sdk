// Candidate VA 1404abd50; RVA 0x4abd50
// Ghidra inferred prototype: undefined FUN_1404abd50()

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Type propagation algorithm not settling */

undefined8 FUN_1404abd50(longlong param_1,undefined8 param_2,undefined4 *param_3,int param_4)

{
  ulonglong uVar1;
  ulonglong *puVar2;
  undefined8 *puVar3;
  size_t sVar4;
  size_t _Size;
  uint uVar5;
  int iVar6;
  undefined8 *******pppppppuVar7;
  undefined8 *puVar8;
  ulonglong *_Memory;
  longlong lVar9;
  undefined8 uVar10;
  char cVar11;
  char cVar12;
  undefined1 *puVar13;
  undefined2 *puVar14;
  undefined2 *puVar15;
  void *_Memory_00;
  ulonglong *puVar16;
  uint uVar17;
  undefined8 *******pppppppuVar18;
  longlong *plVar19;
  ulonglong uVar20;
  ulonglong *puVar21;
  ulonglong *puVar22;
  undefined8 *puVar23;
  undefined8 *puVar24;
  longlong lVar25;
  undefined8 *puVar26;
  ulonglong *puVar27;
  ulonglong local_1148;
  undefined8 *******local_1140;
  undefined8 uStack_1138;
  ulonglong local_1130;
  ulonglong local_1128;
  undefined2 local_1120;
  void *local_1108;
  undefined8 uStack_1100;
  undefined8 local_10f8;
  ulonglong local_10f0;
  ulonglong *local_10e8;
  ulonglong *puStack_10e0;
  ulonglong *local_10d8;
  undefined8 *******local_10d0;
  undefined8 uStack_10c8;
  size_t local_10c0;
  ulonglong uStack_10b8;
  undefined8 *local_10b0;
  undefined8 *local_10a8;
  undefined1 local_1098 [32];
  char local_1078;
  undefined1 local_1070 [40];
  undefined8 *******local_1048 [3];
  ulonglong local_1030;
  ulonglong local_1028 [4];
  ulonglong local_1008 [4];
  ulonglong local_fe8;
  ulonglong local_fe0;
  undefined1 local_fd8;
  undefined1 local_fd7;
  undefined1 local_fd0 [88];
  undefined1 local_f78 [32];
  undefined1 local_f58 [168];
  char local_eb0;
  undefined1 local_ea8 [848];
  undefined4 local_b58;
  undefined4 uStack_b54;
  undefined4 uStack_b50;
  undefined4 uStack_b4c;
  undefined4 local_b48;
  undefined4 uStack_b44;
  undefined4 uStack_b40;
  undefined4 uStack_b3c;
  undefined1 local_b38 [2800];
  undefined8 uStack_48;

  uStack_48 = 0x1404abd82;
  puVar22 = (ulonglong *)0x0;
  FUN_14049cb20(&local_10b0,&DAT_140b77d10);
  local_10e8 = (ulonglong *)0x0;
  puStack_10e0 = (ulonglong *)0x0;
  local_10d8 = (ulonglong *)0x0;
  lVar25 = *(longlong *)(param_1 + 0x410);
  puVar23 = (undefined8 *)(lVar25 + 0x20);
  sVar4 = *(size_t *)(lVar25 + 0x30);
  if (0x7fffffffffffffff - sVar4 < 10) {
                    /* WARNING: Subroutine does not return */
    FUN_140001c70();
  }
  if (0xf < *(ulonglong *)(lVar25 + 0x38)) {
    puVar23 = (undefined8 *)*puVar23;
  }
  local_1140 = (undefined8 *******)0x0;
  uStack_1138 = 0;
  local_1130 = 0;
  local_1128 = 0;
  uVar1 = sVar4 + 10;
  uVar20 = 0xf;
  pppppppuVar7 = &local_1140;
  if (0xf < uVar1) {
    uVar20 = uVar1 | 0xf;
    if (uVar20 < 0x8000000000000000) {
      if (uVar20 < 0x16) {
        uVar20 = 0x16;
      }
    }
    else {
      uVar20 = 0x7fffffffffffffff;
    }
    pppppppuVar7 = (undefined8 *******)FUN_140003270(uVar20 + 1);
    local_1140 = pppppppuVar7;
  }
  local_1130 = uVar1;
  local_1128 = uVar20;
  memcpy(pppppppuVar7,puVar23,sVar4);
  *(undefined8 *)((longlong)pppppppuVar7 + sVar4) = s_Autosave_140a76098._0_8_;
  *(undefined2 *)((longlong)pppppppuVar7 + sVar4 + 8) = s_Autosave_140a76098._8_2_;
  *(undefined1 *)((longlong)pppppppuVar7 + uVar1) = 0;
  puVar23 = local_10a8;
  local_1148 = 1;
  puVar27 = puStack_10e0;
  _Memory = local_10e8;
  if (local_10b0 != local_10a8) {
    puVar16 = puStack_10e0;
    puVar24 = local_10b0 + 4;
    puVar21 = local_10e8;
    do {
      pppppppuVar7 = &local_1140;
      if (0xf < local_1128) {
        pppppppuVar7 = local_1140;
      }
      puVar26 = puVar24;
      if (0xf < (ulonglong)puVar24[3]) {
        puVar26 = (undefined8 *)*puVar24;
      }
      puVar27 = puVar16;
      _Memory = puVar21;
      if ((local_1130 <= (ulonglong)puVar24[2]) &&
         ((local_1130 == 0 ||
          ((puVar3 = (undefined8 *)(puVar24[2] + (longlong)puVar26),
           puVar8 = (undefined8 *)thunk_FUN_1409827a0(puVar26,puVar3,pppppppuVar7), puVar8 != puVar3
           && (puVar8 == puVar26)))))) {
        if (puVar16 < puVar22) {
          puVar27 = puVar16 + 0x33;
          puStack_10e0 = puVar27;
          FUN_1404ad0e0(puVar16,puVar24 + -4);
        }
        else {
          lVar25 = (longlong)puVar16 - (longlong)puVar21 >> 3;
          if (lVar25 * -0x505050505050505 == 0) {
            lVar9 = 1;
LAB_1404abf4d:
            lVar25 = lVar9 * 0x198;
            _Memory = (ulonglong *)thunk_FUN_140983da8(lVar25);
          }
          else {
            lVar9 = lVar25 * -0xa0a0a0a0a0a0a0a;
            if (lVar9 != 0) goto LAB_1404abf4d;
            _Memory = (ulonglong *)0x0;
            lVar25 = lVar25 * 0x10;
          }
          lVar9 = FUN_1404c3390(puVar21,puVar16,_Memory);
          FUN_1404ad0e0(lVar9,puVar24 + -4);
          puVar27 = (ulonglong *)(lVar9 + 0x198);
          FUN_1404ba350(puVar21,puVar16);
          if (puVar21 != (ulonglong *)0x0) {
            free(puVar21);
          }
          puVar22 = (ulonglong *)((longlong)_Memory + lVar25);
          local_10e8 = _Memory;
          puStack_10e0 = puVar27;
          local_10d8 = puVar22;
        }
      }
      puVar26 = puVar24 + 0x2f;
      puVar16 = puVar27;
      puVar24 = puVar24 + 0x33;
      puVar21 = _Memory;
    } while (puVar26 != puVar23);
  }
  if (0 < param_4) {
    while (param_4 <= (int)((longlong)puVar27 - (longlong)_Memory >> 3) * -0x5050505) {
      cVar11 = '\0';
      local_eb0 = '\0';
      puVar22 = puVar27;
      cVar12 = cVar11;
      if (_Memory != puVar27) {
        puVar16 = puVar27;
        puVar21 = _Memory + 7;
        do {
          if (cVar11 == '\0') {
            FUN_1404ad0e0(local_1048,puVar21 + -7);
            local_eb0 = '\x01';
LAB_1404ac0e5:
            puVar16 = puVar21 + -7;
            cVar11 = local_eb0;
          }
          else if ((longlong)puVar21[5] < (longlong)local_fe8) {
            FUN_140246270(local_1048,puVar21 + -7);
            puVar16 = puVar21 + -3;
            if (local_1028 != puVar16) {
              if (0xf < *puVar21) {
                puVar16 = (ulonglong *)*puVar16;
              }
              FUN_140030630(local_1028,puVar16,puVar21[-1]);
            }
            puVar16 = puVar21 + 1;
            if (local_1008 != puVar16) {
              if (0xf < puVar21[4]) {
                puVar16 = (ulonglong *)*puVar16;
              }
              FUN_140030630(local_1008,puVar16,puVar21[3]);
            }
            local_fe8 = puVar21[5];
            local_fe0 = puVar21[6];
            local_fd8 = (undefined1)puVar21[7];
            local_fd7 = *(undefined1 *)((longlong)puVar21 + 0x39);
            FUN_1404ba0b0(local_fd0,puVar21 + 8);
            goto LAB_1404ac0e5;
          }
          puVar2 = puVar21 + 0x2c;
          puVar21 = puVar21 + 0x33;
        } while (puVar2 != puVar27);
        cVar12 = '\0';
        if (cVar11 != '\0') {
          if ((puVar16 + 0x33 < puVar27) &&
             (lVar25 = ((longlong)puVar27 - (longlong)(puVar16 + 0x33) >> 3) * -0x505050505050505,
             0 < lVar25)) {
            puVar16 = puVar16 + 0x3b;
            do {
              FUN_1402522e0(puVar16 + -0x3b,puVar16 + -8);
              FUN_140025630(puVar16 + -0x37,puVar16 + -4);
              FUN_140025630(puVar16 + -0x33,puVar16);
              puVar16[-0x2f] = puVar16[4];
              puVar16[-0x2e] = puVar16[5];
              *(char *)(puVar16 + -0x2d) = (char)puVar16[6];
              *(undefined1 *)((longlong)puVar16 + -0x167) =
                   *(undefined1 *)((longlong)puVar16 + 0x31);
              FUN_1404c44a0(puVar16 + -0x2c,puVar16 + 7);
              lVar25 = lVar25 + -1;
              puVar16 = puVar16 + 0x33;
            } while (0 < lVar25);
          }
          puVar22 = puVar27 + -0x33;
          puStack_10e0 = puVar22;
          FUN_140002d30(puVar27 + -0x15);
          FUN_140002d30(puVar27 + -0x19);
          FUN_140002d30(puVar27 + -0x2b);
          FUN_140002d30(puVar27 + -0x2f);
          FUN_140025470(puVar22);
          pppppppuVar7 = local_1048;
          if (7 < local_1030) {
            pppppppuVar7 = local_1048[0];
          }
          FUN_140983920(pppppppuVar7);
          cVar12 = local_eb0;
        }
      }
      puVar27 = puVar22;
      if (cVar12 != '\0') {
        FUN_140002d30(local_f58);
        FUN_140002d30(local_f78);
        FUN_140002d30(local_1008);
        FUN_140002d30(local_1028);
        FUN_140025470(local_1048);
      }
    }
  }
  do {
    uVar5 = (uint)local_1148;
    uVar17 = -uVar5;
    if (local_1148 >> 0x1f == 0) {
      uVar17 = uVar5;
    }
    iVar6 = 0x1f;
    if ((uVar17 | 1) != 0) {
      for (; (uVar17 | 1) >> iVar6 == 0; iVar6 = iVar6 + -1) {
      }
    }
    puVar13 = (undefined1 *)&local_1120;
    if (local_1148 >> 0x1f != 0) {
      local_1120._0_1_ = 0x2d;
      puVar13 = (undefined1 *)((longlong)&local_1120 + 1);
    }
    puVar14 = (undefined2 *)
              (puVar13 +
              (int)(*(longlong *)(&DAT_140a49580 + (longlong)iVar6 * 8) + (ulonglong)uVar17 >> 0x20)
              );
    puVar15 = puVar14;
    uVar1 = (ulonglong)uVar17;
    while (99 < uVar17) {
      puVar15 = puVar15 + -1;
      uVar17 = (uint)(uVar1 / 100);
      *puVar15 = *(undefined2 *)(&DAT_140a496e0 + (ulonglong)((int)uVar1 + uVar17 * -100) * 2);
      uVar1 = uVar1 / 100;
    }
    if (uVar17 < 10) {
      *(char *)((longlong)puVar15 + -1) = (char)uVar17 + '0';
    }
    else {
      puVar15[-1] = *(undefined2 *)(&DAT_140a496e0 + (ulonglong)uVar17 * 2);
    }
    local_1108 = (void *)0x0;
    uStack_1100 = 0;
    local_10f8 = 0;
    local_10f0 = 0;
    if (&local_1120 == puVar14) {
      local_10f8 = 0;
      local_10f0 = 0xf;
      local_1108 = (void *)0x0;
    }
    else {
      FUN_140002c00(&local_1108,&local_1120,(longlong)puVar14 - (longlong)&local_1120);
    }
    puVar23 = (undefined8 *)FUN_14029cb50(&local_1108,0);
    local_10d0 = (undefined8 *******)*puVar23;
    uStack_10c8 = puVar23[1];
    local_10c0 = puVar23[2];
    uStack_10b8 = puVar23[3];
    puVar23[2] = 0;
    puVar23[3] = 0xf;
    *(undefined1 *)puVar23 = 0;
    if (0xf < local_10f0) {
      _Memory_00 = local_1108;
      if ((0xfff < local_10f0 + 1) &&
         (_Memory_00 = *(void **)((longlong)local_1108 + -8),
         0x1f < (ulonglong)((longlong)local_1108 + (-8 - (longlong)_Memory_00)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(_Memory_00);
    }
    uVar1 = uStack_10b8;
    sVar4 = local_10c0;
    pppppppuVar7 = local_10d0;
    local_10f8 = 0;
    local_10f0 = 0xf;
    local_1108 = (void *)((ulonglong)local_1108 & 0xffffffffffffff00);
    if (_Memory == puVar27) {
LAB_1404ac478:
      uVar10 = FUN_14049f830(local_1070,&local_10d0,&DAT_140b77d10);
      FUN_1404aafd0(local_ea8,param_1,param_2);
      local_b58 = *param_3;
      uStack_b54 = param_3[1];
      uStack_b50 = param_3[2];
      uStack_b4c = param_3[3];
      local_b48 = param_3[4];
      uStack_b44 = param_3[5];
      uStack_b40 = param_3[6];
      uStack_b3c = param_3[7];
      plVar19 = (longlong *)(param_3 + 8);
      if ((longlong *)local_b38 != plVar19) {
        if (0xf < *(ulonglong *)(param_3 + 0xe)) {
          plVar19 = (longlong *)*plVar19;
        }
        FUN_140030630(local_b38,plVar19,*(undefined8 *)(param_3 + 0xc));
      }
      FUN_140247b90(&local_1108,uVar10);
      FUN_14049fbd0(local_1098,&local_1108);
      FUN_1404aa510(local_ea8);
      FUN_140025470(uVar10);
      if (local_1078 != '\0') {
        FUN_140002d30(local_1098);
      }
      if (0xf < uStack_10b8) {
        pppppppuVar7 = local_10d0;
        if ((0xfff < uStack_10b8 + 1) &&
           (pppppppuVar7 = (undefined8 *******)local_10d0[-1],
           0x1f < (ulonglong)((longlong)local_10d0 + (-8 - (longlong)pppppppuVar7)))) {
LAB_1404ac577:
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppuVar7);
      }
      uVar10 = 1;
      goto LAB_1404ac5fc;
    }
    puVar22 = _Memory + 4;
    while( true ) {
      pppppppuVar18 = &local_10d0;
      if (0xf < uVar1) {
        pppppppuVar18 = pppppppuVar7;
      }
      _Size = puVar22[2];
      puVar16 = puVar22;
      if (0xf < puVar22[3]) {
        puVar16 = (ulonglong *)*puVar22;
      }
      if ((_Size == sVar4) &&
         ((_Size == 0 || (iVar6 = memcmp(puVar16,pppppppuVar18,_Size), iVar6 == 0)))) break;
      puVar16 = puVar22 + 0x2f;
      puVar22 = puVar22 + 0x33;
      if (puVar16 == puVar27) goto LAB_1404ac478;
    }
    if (0xf < uVar1) {
      pppppppuVar18 = pppppppuVar7;
      if ((0xfff < uVar1 + 1) &&
         (pppppppuVar18 = (undefined8 *******)pppppppuVar7[-1],
         0x1f < (ulonglong)((longlong)pppppppuVar7 + (-8 - (longlong)pppppppuVar18))))
      goto LAB_1404ac577;
      free(pppppppuVar18);
    }
    local_1148 = (ulonglong)(uVar5 + 1);
  } while ((int)(uVar5 + 1) < 100);
  uVar10 = 0;
LAB_1404ac5fc:
  if (0xf < local_1128) {
    pppppppuVar7 = local_1140;
    if ((0xfff < local_1128 + 1) &&
       (pppppppuVar7 = (undefined8 *******)local_1140[-1],
       0x1f < (ulonglong)((longlong)local_1140 + (-8 - (longlong)pppppppuVar7)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppppuVar7);
  }
  local_1130 = 0;
  local_1128 = 0xf;
  local_1140 = (undefined8 *******)((ulonglong)local_1140 & 0xffffffffffffff00);
  FUN_1404ba350(_Memory,puVar27);
  if (_Memory != (ulonglong *)0x0) {
    free(_Memory);
  }
  FUN_1404ba350(local_10b0,local_10a8);
  if (local_10b0 != (undefined8 *)0x0) {
    free(local_10b0);
  }
  return uVar10;
}


// Incoming references
// 0xc269a8 DATA caller none
// 0x742353 UNCONDITIONAL_CALL caller 1407422c0
// 0x74e102 UNCONDITIONAL_CALL caller 14074e0b0
