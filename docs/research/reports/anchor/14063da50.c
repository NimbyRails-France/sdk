
void FUN_14063da50(longlong param_1,longlong *param_2,undefined8 param_3,longlong *param_4,
                  undefined8 param_5,longlong *param_6,undefined8 param_7,longlong *param_8)

{
  int *piVar1;
  float fVar2;
  int iVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  longlong lVar11;
  longlong *plVar12;
  longlong *plVar13;
  undefined8 uVar14;
  void *pvVar15;
  longlong *plVar16;
  void *_Memory;
  float fVar17;
  float fVar18;
  undefined4 uVar19;
  ulonglong *_Memory_00;
  ulonglong uVar20;
  undefined4 uVar21;
  undefined8 *puVar22;
  longlong lVar23;
  undefined8 *puVar24;
  undefined8 *puVar25;
  longlong lVar26;
  longlong *plVar27;
  undefined1 *_Memory_01;
  ulonglong *puVar28;
  ulonglong *puVar29;
  ulonglong uVar30;
  longlong lVar31;
  ulonglong *puVar32;
  longlong extraout_XMM0_Qb;
  longlong extraout_XMM0_Qb_00;
  ulonglong *local_228;
  ulonglong *puStack_220;
  undefined8 local_218;
  undefined8 local_208;
  undefined8 uStack_200;
  undefined8 local_1f8;
  undefined2 local_1f0;
  void *local_1e8;
  undefined8 uStack_1e0;
  undefined1 local_1d8 [16];
  undefined8 local_1c8;
  undefined8 uStack_1c0;
  void *local_1b8;
  undefined8 uStack_1b0;
  undefined8 local_1a8;
  undefined1 *local_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  longlong local_188;
  undefined8 local_170;
  undefined1 local_168 [8];
  undefined8 *local_160;
  ulonglong local_158;
  undefined8 local_150;
  undefined4 local_148;
  undefined8 local_144;
  void *local_138;
  undefined8 uStack_130;
  undefined8 local_128;
  char local_120;
  undefined8 local_118;
  undefined8 local_110;
  longlong local_108;
  longlong *local_100;
  longlong local_f8 [2];
  undefined8 local_e8;
  undefined8 uStack_e0;
  
  lVar11 = *param_8;
  if (param_8[1] != 0) {
    LOCK();
    piVar1 = (int *)(param_8[1] + 8);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  uVar19 = DAT_140aac0a0;
  fVar18 = DAT_140aabb98;
  fVar17 = DAT_140aabae4;
  plVar12 = (longlong *)param_8[1];
  local_108 = lVar11;
  local_100 = plVar12;
  if (lVar11 != 0) {
    plVar13 = *(longlong **)(param_1 + 0x180);
    for (plVar27 = *(longlong **)(param_1 + 0x178); plVar27 != plVar13; plVar27 = plVar27 + 2) {
      puVar22 = (undefined8 *)0x0;
      lVar31 = *plVar27;
      lVar26 = plVar27[1];
      if (lVar31 != 0) {
        puVar22 = (undefined8 *)FUN_14032c3b0(*param_4 + 0x80,lVar31);
        lVar26 = extraout_XMM0_Qb;
      }
      lVar23 = 0;
      if (lVar31 != 0) {
        lVar23 = FUN_1403ca140(param_4 + 1,lVar31);
        lVar26 = extraout_XMM0_Qb_00;
      }
      puVar25 = (undefined8 *)0x0;
      puVar24 = puVar25;
      if (lVar26 != 0) {
        puVar24 = (undefined8 *)FUN_14032c3b0(*param_4 + 0x80,lVar26);
        puVar25 = (undefined8 *)FUN_1403ca140(param_4 + 1,lVar26);
      }
      if ((((puVar22 != (undefined8 *)0x0) && (lVar23 != 0)) && (puVar24 != (undefined8 *)0x0)) &&
         (puVar25 != (undefined8 *)0x0)) {
        local_208 = 0;
        uStack_200 = 0;
        local_1f8 = 0;
        local_1f0 = 0;
        local_1e8 = (void *)0x0;
        uStack_1e0 = 0;
        local_1d8 = ZEXT816(0);
        local_1c8 = 0;
        uStack_1c0 = 0;
        local_1b8 = (void *)0x0;
        uStack_1b0 = 0;
        local_1a8 = 0;
        local_1a0 = &DAT_140a6d430;
        local_198 = 0;
        uStack_190 = 0;
        local_188 = 0;
        local_170 = 0;
        local_148 = 0x3f800000;
        local_144 = 0x40000000;
        local_158 = 1;
        local_160 = &DAT_140b5abc0;
        local_150 = 0;
        local_138 = (void *)0x0;
        uStack_130 = 0;
        local_128 = 0;
        local_120 = '\0';
        local_118 = 0;
        local_110 = 0;
        uVar14 = *puVar24;
        uVar21 = FUN_1404775e0(param_7);
        FUN_14035ea20(lVar11,&local_208,*puVar22,uVar21,uVar14,1,0x7fffffffffffffff);
        if (local_120 == '\0') {
          if (local_138 != (void *)0x0) {
            free(local_138);
          }
          uVar20 = local_158;
          puVar22 = local_160;
          uVar30 = 0;
          if (local_158 != 0) {
            do {
              _Memory = (void *)puVar22[uVar30];
              while (_Memory != (void *)0x0) {
                pvVar15 = *(void **)((longlong)_Memory + 0x68);
                free(_Memory);
                _Memory = pvVar15;
              }
              puVar22[uVar30] = 0;
              uVar30 = uVar30 + 1;
            } while (uVar30 < uVar20);
          }
          local_150 = 0;
          if (1 < local_158) {
            free(local_160);
          }
          if (local_188 != 0) {
            _Memory_01 = local_1a0;
            if ((0xfff < (local_188 + 0x18U & 0xfffffffffffffff8) + local_188 * 8) &&
               (_Memory_01 = *(undefined1 **)(local_1a0 + -8),
               (undefined1 *)0x1f < local_1a0 + (-8 - (longlong)_Memory_01))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(_Memory_01);
            local_1a0 = &DAT_140a6d430;
            local_198 = 0;
            uStack_190 = 0;
            local_188 = 0;
            local_170 = 0;
          }
        }
        else {
          local_228 = (ulonglong *)0x0;
          puStack_220 = (ulonglong *)0x0;
          local_218 = 0;
          param_8 = (longlong *)*puVar24;
          if (param_8 != (longlong *)*puVar22) {
            do {
              puVar24 = (undefined8 *)local_160[(ulonglong)param_8 % (local_158 & 0xffffffff)];
              lVar31 = 0;
              if (puVar24 == (undefined8 *)0x0) break;
              do {
                if (param_8 == (longlong *)*puVar24) {
                  lVar31 = lVar31 + 1;
                }
                puVar24 = (undefined8 *)puVar24[0xd];
              } while (puVar24 != (undefined8 *)0x0);
              if (lVar31 == 0) break;
              FUN_140364ee0(local_168,local_f8,lVar31,&param_8,param_8);
              lVar31 = local_f8[0];
              FUN_140643060(&local_228);
              param_8 = *(longlong **)(lVar31 + 8);
            } while (param_8 != (longlong *)*puVar22);
          }
          _Memory_00 = local_228;
          if (puStack_220 != local_228) {
            puVar29 = puStack_220 + -6;
            do {
              lVar31 = 0;
              if (puVar29[-1] == 0) {
                if (puVar29[-6] != 0) {
                  lVar31 = FUN_14032c3b0(*param_4 + 0x80);
                }
                lVar26 = 0;
                if (puVar29[2] != 0) {
                  lVar26 = FUN_14032c3b0(*param_4 + 0x80);
                }
                if ((lVar31 != 0) && (lVar26 != 0)) {
                  dVar4 = *(double *)(lVar31 + 0x10);
                  dVar5 = *(double *)(lVar31 + 0x18);
                  dVar6 = (double)param_2[0x32];
                  dVar7 = (double)param_2[0x2f];
                  dVar8 = (double)param_2[0x2c];
                  dVar9 = *(double *)(lVar26 + 0x10);
                  dVar10 = *(double *)(lVar26 + 0x18);
                  plVar16 = *(longlong **)(*param_2 + 0x1580);
                  fVar2 = *(float *)(*param_2 + 0x1550);
                  if (fVar2 < 0.0) {
                    fVar2 = fVar17;
                  }
                  FUN_1401b2200(plVar16,0xc,(short)param_2[0x14],
                                *(undefined2 *)((longlong)param_2 + 0xa4),fVar2 * fVar18);
                  (**(code **)*plVar16)(plVar16);
                  (**(code **)(*plVar16 + 8))
                            (plVar16,*(code **)(*plVar16 + 8),
                             (float)(dVar4 * dVar8 + dVar5 * dVar7 + dVar6));
                  (**(code **)(*plVar16 + 0x10))
                            (plVar16,*(code **)(*plVar16 + 0x10),
                             (float)(dVar9 * dVar8 + dVar10 * dVar7 + dVar6));
                  (**(code **)(*plVar16 + 0x88))(plVar16,0xff4f4f4f,uVar19,10);
                  FUN_1401b2400(plVar16);
                }
              }
              else {
                lVar31 = FUN_14033f710(*param_6 + 0x280);
                if (lVar31 != 0) {
                  lVar26 = *(longlong *)(lVar31 + 0x1b8);
                  for (puVar28 = *(ulonglong **)
                                  (lVar26 + (*puVar29 % (ulonglong)*(uint *)(lVar31 + 0x1c0)) * 8);
                      puVar28 != (ulonglong *)0x0; puVar28 = (ulonglong *)puVar28[0x3b]) {
                    if (*puVar29 == *puVar28) goto LAB_14063dece;
                  }
                  puVar28 = *(ulonglong **)(lVar26 + *(longlong *)(lVar31 + 0x1c0) * 8);
LAB_14063dece:
                  puVar32 = puVar28 + 1;
                  if (puVar28 == *(ulonglong **)(lVar26 + *(longlong *)(lVar31 + 0x1c0) * 8)) {
                    puVar32 = (ulonglong *)0x0;
                  }
                  if (puVar32 != (ulonglong *)0x0) {
                    iVar3 = (int)puVar29[-4];
                    if ((-1 < iVar3) && (iVar3 < (int)((longlong)(puVar32[0xb] - puVar32[10]) >> 3))
                       ) {
                      lVar31 = (longlong)*(int *)(puVar32[10] + 4 + (longlong)iVar3 * 8) * 0x20;
                      puVar22 = (undefined8 *)(lVar31 + 0x10 + puVar32[0xd]);
                      local_e8 = *puVar22;
                      uStack_e0 = puVar22[1];
                      if (puVar29[-3] == puVar29[4]) {
                        FUN_140639c10(param_1,param_2,param_3,param_4,param_5,
                                      SUB168(*(undefined1 (*) [16])(lVar31 + puVar32[0xd]),8),
                                      0xffffffff,(longlong)(int)puVar29[-2],
                                      (longlong)(int)puVar29[5],0,0);
                      }
                    }
                  }
                }
              }
              puVar28 = puVar29 + -6;
              puVar29 = puVar29 + -0xc;
            } while (puVar28 != _Memory_00);
          }
          if (_Memory_00 != (ulonglong *)0x0) {
            free(_Memory_00);
          }
          if (local_138 != (void *)0x0) {
            free(local_138);
          }
          FUN_140354a90(local_168);
          if (1 < local_158) {
            free(local_160);
          }
          FUN_140338520(&local_1a0);
        }
        if (local_1b8 != (void *)0x0) {
          free(local_1b8);
        }
        if ((void *)local_1d8._8_8_ != (void *)0x0) {
          free((void *)local_1d8._8_8_);
        }
        if (local_1e8 != (void *)0x0) {
          free(local_1e8);
        }
      }
    }
  }
  if (plVar12 != (longlong *)0x0) {
    LOCK();
    plVar27 = plVar12 + 1;
    lVar11 = *plVar27;
    *(int *)plVar27 = (int)*plVar27 + -1;
    UNLOCK();
    if ((int)lVar11 == 1) {
      (**(code **)*plVar12)(plVar12);
      LOCK();
      piVar1 = (int *)((longlong)plVar12 + 0xc);
      iVar3 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar3 == 1) {
        (**(code **)(*plVar12 + 8))(plVar12);
      }
    }
  }
  return;
}

