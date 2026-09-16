// Candidate VA 14045c0d0; RVA 0x45c0d0
// Ghidra inferred prototype: undefined FUN_14045c0d0()

void FUN_14045c0d0(longlong param_1,undefined4 *param_2,longlong param_3,undefined8 param_4,
                  longlong *param_5,longlong param_6,longlong param_7,undefined8 param_8,
                  char param_9,longlong *param_10)

{
  double dVar1;
  double dVar2;
  double dVar3;
  double dVar4;
  undefined8 uVar5;
  char cVar6;
  undefined1 uVar7;
  int iVar8;
  undefined8 *puVar9;
  longlong lVar10;
  undefined8 *puVar11;
  longlong *plVar12;
  longlong lVar13;
  int iVar14;
  int iVar15;
  ulonglong uVar16;
  undefined8 *puVar17;
  undefined4 uVar18;
  undefined4 extraout_XMM0_Da;
  undefined4 uVar19;
  undefined4 uVar21;
  undefined4 extraout_XMM0_Db;
  double dVar20;
  uint uVar22;
  undefined4 uVar23;
  undefined4 uVar24;
  double local_res8;
  undefined8 *local_res10;
  longlong *local_res18;
  undefined4 local_328;
  undefined4 uStack_324;
  undefined8 uStack_320;
  undefined8 local_318;
  char local_310;
  undefined8 *local_308;
  undefined8 local_2f8;
  undefined8 uStack_2f0;
  int local_2e8 [2];
  undefined8 local_2e0;
  undefined8 uStack_2d8;
  undefined8 local_2d0;
  undefined8 local_2c8;
  undefined4 local_2b8;
  undefined4 uStack_2b4;
  undefined8 uStack_2b0;
  undefined8 local_2a8;
  undefined8 local_2a0;
  undefined8 uStack_298;
  undefined8 local_290;
  undefined8 local_288;
  undefined8 uStack_280;
  undefined8 local_278;
  undefined8 uStack_270;
  undefined8 local_268;
  undefined4 uStack_260;
  undefined4 uStack_25c;
  undefined8 local_258;
  undefined8 uStack_250;
  undefined2 local_248;
  undefined6 uStack_246;
  longlong local_240;
  double dStack_238;
  char local_230;
  char cStack_22f;
  undefined6 uStack_22e;
  char local_228;
  undefined4 local_224;
  undefined2 local_220;
  undefined8 local_218;
  void *local_210;
  longlong local_208;
  undefined8 uStack_200;
  void *local_1f8;
  undefined8 uStack_1f0;
  undefined8 local_1e8;
  undefined8 local_1e0;
  undefined4 local_1d8;
  undefined8 local_1d0;
  undefined8 local_1c8;
  undefined8 uStack_1c0;
  undefined8 local_1b8;
  undefined8 uStack_1b0;
  undefined8 local_198;
  double *local_190;
  undefined8 *local_188;
  undefined8 *local_180;
  longlong *local_178;
  void *local_170;
  void *local_168;
  void *local_150;
  undefined8 local_100;
  undefined8 uStack_f8;
  undefined8 local_f0;
  double dStack_e8;
  int local_e0;
  undefined4 uStack_dc;
  undefined4 uStack_d8;
  undefined4 uStack_d4;
  undefined1 local_d0 [152];

  dVar4 = DAT_140aac188;
  puVar17 = (undefined8 *)(param_1 + 0x428);
  local_res18 = (longlong *)(param_1 + 0x780);
  local_2f8 = 0;
  uStack_2f0 = (undefined8 *)0x0;
  if (param_6 != 0) {
    local_2f8 = param_6 + 0x80;
  }
  dVar2 = DAT_140aabfa0;
  if (param_7 != 0) {
    dVar2 = (double)*(float *)(param_7 + 0x38);
  }
  local_res8 = 0.0;
  local_190 = &local_res8;
  local_180 = &local_2f8;
  local_178 = param_10;
  uVar23 = *param_2;
  uVar24 = param_2[1];
  dVar20 = *(double *)(param_2 + 2);
  local_198 = *(undefined8 *)(param_2 + 4);
  uVar18 = (undefined4)local_198;
  uVar21 = (undefined4)((ulonglong)local_198 >> 0x20);
  local_res10 = puVar17;
  local_188 = puVar17;
  dVar3 = DAT_140aabd08;
  if (param_3 == 0) {
    do {
      dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
      if (((((1 < *(int *)((longlong)param_5 + 0x3c)) ||
            (dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320),
            *(char *)((longlong)param_5 + 0x44) != '\0')) ||
           (dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320),
           (char)param_5[5] != '\0')) ||
          ((lVar10 = *param_5, lVar10 != 0 &&
           (dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320),
           *(longlong *)(lVar10 + 0x50) == *(longlong *)(lVar10 + 0x58))))) ||
         (lVar10 = param_5[2], dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320),
         lVar10 == 0)) break;
      local_328 = (undefined4)lVar10;
      uStack_324 = (undefined4)((ulonglong)lVar10 >> 0x20);
      uStack_320._0_4_ = *(undefined4 *)((longlong)param_5 + 0x2c);
      uVar22 = *(uint *)(param_5 + 6);
      local_318 = CONCAT44(local_318._4_4_,(int)param_5[8]);
      dVar1 = *(double *)((longlong)param_5 + 0x2c);
      if (lVar10 == 0) break;
      lVar13 = *local_res18;
      uStack_320._4_4_ = uVar22;
      puVar9 = (undefined8 *)FUN_14033f7f0(lVar13 + 0x180,lVar10);
      local_308 = puVar9;
      dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
      if (puVar9 == (undefined8 *)0x0) break;
      lVar10 = FUN_14033f7f0(lVar13 + 0x180,lVar10);
      dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
      if (((lVar10 == 0) ||
          (dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320), (int)uVar22 < 0)) ||
         ((dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320),
          (int)(*(longlong *)(lVar10 + 0x120) - *(longlong *)(lVar10 + 0x118) >> 3) * 0x2fa0be83 <=
          (int)uVar22 ||
          (lVar10 = (longlong)(int)uVar22 * 0x158 + *(longlong *)(lVar10 + 0x118),
          dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320), lVar10 == 0)))) break;
      local_318._1_7_ = (undefined7)(CONCAT44(uVar21,uVar18) >> 8);
      local_318 = CONCAT71(local_318._1_7_,-(char)local_198);
      local_328 = uVar23;
      uStack_324 = uVar24;
      uStack_320 = dVar20;
      FUN_140377010(&local_170,puVar17,&local_328,SUB84(dVar2,0));
      cVar6 = FUN_140385760(lVar10 + 8,&local_170);
      if (cVar6 == '\0') {
        iVar8 = 1;
        lVar13 = *(longlong *)(lVar10 + 0xf0);
        iVar14 = (int)(*(longlong *)(lVar10 + 0xf8) - lVar13 >> 3) * 0x3cf3cf3d;
        if (0 < iVar14) {
          do {
            cVar6 = FUN_140385760((longlong)(iVar8 + -1) * 0xa8 + lVar13,&local_170);
            puVar9 = local_308;
            if (cVar6 != '\0') goto LAB_14045c422;
            iVar8 = iVar8 + 1;
          } while (iVar8 <= iVar14);
        }
        iVar8 = -1;
        puVar9 = local_308;
      }
      else {
        iVar8 = 0;
      }
LAB_14045c422:
      if (-1 < iVar8) {
        puVar17 = local_res10;
        if ((((iVar8 != 0) || (*(int *)((longlong)puVar9 + 0xfc) != 1)) ||
            (*(int *)(puVar9 + 0x20) != 1)) ||
           (dVar1 = uStack_320, *(int *)(lVar10 + 0xe0) + *(int *)(lVar10 + 0xc0) == 0))
        goto LAB_14045c826;
LAB_14045ca79:
        if ((local_170 != (void *)0x0) && (local_170 != local_150)) {
          uStack_320 = dVar1;
          free(local_170);
          dVar1 = uStack_320;
        }
        break;
      }
      local_2a0 = 0;
      uStack_298 = 0.0;
      local_290 = CONCAT62(local_290._2_6_,0x101);
      local_288 = 0;
      uStack_280 = 0;
      local_278 = CONCAT62(local_278._2_6_,0x101);
      uStack_270 = 0;
      local_268 = 0;
      uStack_260 = CONCAT22(uStack_260._2_2_,0x101);
      local_258 = 0;
      uStack_250 = 0;
      local_248 = 0x101;
      local_240 = 0;
      dStack_238 = 0.0;
      local_230 = '\x01';
      cStack_22f = '\x01';
      local_228 = '\0';
      local_224 = 0;
      local_220 = 0;
      local_218 = 0;
      local_210 = (void *)0x0;
      local_208 = 0;
      uStack_200 = 0;
      local_1f8 = (void *)0x0;
      uStack_1f0 = 0;
      local_1e8 = 0;
      local_1e0 = 0;
      local_1d8 = 0;
      local_1d0 = 0;
      local_1c8 = 0;
      uStack_1c0 = 0;
      local_1b8 = 0;
      uStack_1b0 = 0;
      local_2a8 = CONCAT44(uVar21,uVar18);
      if (param_9 != '\0') {
        local_328 = 0;
        uStack_324 = 0;
        uStack_320._0_4_ = 0;
        uStack_320._4_4_ = 0;
        local_318 = CONCAT62(local_318._2_6_,0x101);
        if (local_170 != local_168) {
          local_328 = (undefined4)*(undefined8 *)((longlong)local_168 + -0x20);
          uStack_324 = (undefined4)((ulonglong)*(undefined8 *)((longlong)local_168 + -0x20) >> 0x20)
          ;
          dVar1 = *(double *)((longlong)local_168 + -0x10);
          uStack_320._0_4_ = SUB84(dVar1,0);
          uStack_320._4_4_ = (uint)((ulonglong)dVar1 >> 0x20);
          uVar7 = 0xff;
          if (*(double *)((longlong)local_168 + -0x18) <= dVar1 &&
              dVar1 != *(double *)((longlong)local_168 + -0x18)) {
            uVar7 = 1;
          }
          local_318._0_2_ = CONCAT11(*(undefined1 *)((longlong)local_168 + -8),uVar7);
        }
        local_2a0 = CONCAT44(uStack_324,local_328);
        uStack_298 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
        local_290 = local_318;
        uStack_320 = uStack_298;
      }
      local_2b8 = uVar23;
      uStack_2b4 = uVar24;
      uStack_2b0 = dVar20;
      puVar11 = (undefined8 *)
                FUN_14036c5f0(local_res18,&local_100,lVar10 + 8,(float)dVar2,
                              *(int *)(lVar10 + 0xe0) + *(int *)(lVar10 + 0xc0) != 0);
      puVar17 = local_res10;
      local_288 = *puVar11;
      uStack_280 = puVar11[1];
      local_278 = puVar11[2];
      uStack_270 = puVar11[3];
      local_268 = puVar11[4];
      uStack_260 = *(undefined4 *)(puVar11 + 5);
      uStack_25c = *(undefined4 *)((longlong)puVar11 + 0x2c);
      local_220 = CONCAT11(local_220._1_1_,*(int *)(lVar10 + 0xe0) + *(int *)(lVar10 + 0xc0) != 0);
      puVar9 = puVar9 + 0x19;
      FUN_140376940(local_res10,puVar9,param_6,param_7,&local_2b8);
      local_2e8[0] = -1;
      local_2e0 = local_258;
      uStack_2d8 = uStack_250;
      local_2d0 = CONCAT62(uStack_246,local_248);
      local_2c8 = 0;
      uStack_2f0 = puVar9;
      if (local_228 == '\0') {
LAB_14045ca5c:
        if (local_1f8 != (void *)0x0) {
          free(local_1f8);
        }
        dVar1 = uStack_320;
        if (local_210 != (void *)0x0) {
          free(local_210);
          dVar1 = uStack_320;
        }
        goto LAB_14045ca79;
      }
      if (-1 < (int)(local_208 - (longlong)local_210 >> 3)) {
        do {
          if (dVar4 <= local_res8) break;
          local_328 = (undefined4)local_2f8;
          uStack_324 = local_2f8._4_4_;
          uStack_320._0_4_ = (undefined4)uStack_2f0;
          uStack_320._4_4_ = uStack_2f0._4_4_;
          puVar9 = (undefined8 *)
                   FUN_140377dd0(local_d0,puVar17,&local_2b8,local_2e8,dVar4,&local_328);
          local_100 = *puVar9;
          uStack_f8 = puVar9[1];
          local_f0 = puVar9[2];
          dStack_e8 = (double)puVar9[3];
          local_e0 = *(int *)(puVar9 + 4);
          uStack_dc = *(undefined4 *)((longlong)puVar9 + 0x24);
          uStack_d8 = *(undefined4 *)(puVar9 + 5);
          uStack_d4 = *(undefined4 *)((longlong)puVar9 + 0x2c);
          if (local_e0 == 0) goto LAB_14045ca5c;
          local_res8 = local_res8 + dStack_e8;
          plVar12 = (longlong *)param_10[7];
          if (plVar12 == (longlong *)0x0) {
                    /* WARNING: Subroutine does not return */
            std::_Xbad_function_call();
          }
          cVar6 = (**(code **)(*plVar12 + 0x10))(plVar12,&local_100);
          if (cVar6 == '\0') goto LAB_14045ca5c;
        } while (local_2e8[0] < (int)(local_208 - (longlong)local_210 >> 3));
      }
      dVar20 = dStack_238;
      lVar10 = local_240;
      uVar23 = (undefined4)local_240;
      uVar24 = (undefined4)((ulonglong)local_240 >> 0x20);
      uVar19 = SUB84(dStack_238,0);
      uVar18 = (undefined4)CONCAT62(uStack_22e,CONCAT11(cStack_22f,local_230));
      uVar21 = (undefined4)((uint6)uStack_22e >> 0x10);
      if (local_240 != 0) {
        plVar12 = (longlong *)FUN_14032c420(*puVar17,local_240);
        uVar18 = extraout_XMM0_Da;
        uVar21 = extraout_XMM0_Db;
        if (plVar12 != (longlong *)0x0) {
          cVar6 = local_230;
          if (((cStack_22f != '\0') && (*plVar12 == lVar10)) &&
             (*(char *)((longlong)plVar12 + 0x2c) != cStack_22f)) {
            cVar6 = -local_230;
          }
          if (cVar6 == '\x01') {
            if (((cStack_22f != '\0') && (*plVar12 == lVar10)) &&
               (*(char *)((longlong)plVar12 + 0x2c) != cStack_22f)) {
              uVar19 = SUB84(dVar3 - dVar20,0);
            }
            dVar20 = (double)nextafter(uVar19);
            local_318 = CONCAT71(local_318._1_7_,1);
          }
          else {
            if (((cStack_22f != '\0') && (*plVar12 == lVar10)) &&
               (*(char *)((longlong)plVar12 + 0x2c) != cStack_22f)) {
              uVar19 = SUB84(dVar3 - dVar20,0);
            }
            dVar20 = (double)nextafter(uVar19);
            local_318 = CONCAT71(local_318._1_7_,0xff);
          }
          uVar5 = local_318;
          local_328 = (undefined4)*plVar12;
          uStack_324 = (undefined4)((ulonglong)*plVar12 >> 0x20);
          local_318._0_2_ =
               CONCAT11(*(undefined1 *)((longlong)plVar12 + 0x2c),(undefined1)local_318);
          uVar18 = (undefined4)local_318;
          uVar21 = (undefined4)((ulonglong)uVar5 >> 0x20);
          uVar23 = local_328;
          uVar24 = uStack_324;
          uStack_320 = dVar20;
        }
      }
      local_198 = CONCAT44(uVar21,uVar18);
      param_9 = (char)local_220;
      if (local_1f8 != (void *)0x0) {
        free(local_1f8);
      }
      if (local_210 != (void *)0x0) {
        free(local_210);
      }
LAB_14045c826:
      plVar12 = local_res18;
      FUN_14043ef00(param_5,local_res18);
      dVar1 = uStack_320;
      if ((((char)param_5[5] != '\0') ||
          ((lVar10 = *param_5, lVar10 != 0 &&
           (*(longlong *)(lVar10 + 0x50) == *(longlong *)(lVar10 + 0x58))))) ||
         (lVar10 = param_5[2], lVar10 == 0)) goto LAB_14045ca79;
      local_328 = (undefined4)lVar10;
      uStack_324 = (undefined4)((ulonglong)lVar10 >> 0x20);
      uStack_320._0_4_ = *(undefined4 *)((longlong)param_5 + 0x2c);
      iVar14 = (int)param_5[6];
      local_318 = CONCAT44(local_318._4_4_,(int)param_5[8]);
      dVar1 = *(double *)((longlong)param_5 + 0x2c);
      if (lVar10 == 0) goto LAB_14045ca79;
      lVar13 = *plVar12;
      uStack_320._4_4_ = iVar14;
      puVar9 = (undefined8 *)FUN_14033f7f0(lVar13 + 0x180,lVar10);
      dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
      if (puVar9 == (undefined8 *)0x0) goto LAB_14045ca79;
      lVar10 = FUN_14033f7f0(lVar13 + 0x180,lVar10);
      dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
      if (((lVar10 == 0) ||
          (dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320), iVar14 < 0)) ||
         ((dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320),
          (int)(*(longlong *)(lVar10 + 0x120) - *(longlong *)(lVar10 + 0x118) >> 3) * 0x2fa0be83 <=
          iVar14 || (lVar10 = (longlong)iVar14 * 0x158 + *(longlong *)(lVar10 + 0x118),
                    dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320), lVar10 == 0))
         )) goto LAB_14045ca79;
      if ((iVar8 == 0) && (puVar9 == local_308)) {
        iVar8 = (int)((longlong)(local_308[0x24] - local_308[0x23]) >> 3);
        iVar15 = iVar8 * 0x2fa0be83;
        uVar16 = (ulonglong)uVar22;
        if (iVar15 < 0) {
          iVar8 = FUN_140360e20(-(uVar22 + 1),iVar8 * -0x2fa0be83);
        }
        else {
          iVar8 = (int)(uVar22 + 1) % iVar15;
          if (iVar8 < 0) {
            iVar8 = iVar8 + iVar15;
          }
        }
        if (iVar14 == iVar8) {
          lVar13 = FUN_14036d030(local_res18,*puVar9,uVar16 & 0xffffffff);
          if (lVar13 != 0) {
            local_2e8[0] = -1;
            local_2e0 = *(undefined8 *)(lVar13 + 0x60);
            uStack_2d8 = *(undefined8 *)(lVar13 + 0x68);
            local_2d0 = *(undefined8 *)(lVar13 + 0x70);
            local_2c8 = 0;
            uStack_2f0 = puVar9 + 0x19;
            FUN_14045cb00(&local_190,&local_328,lVar13,local_2e8);
            dVar20 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
            dVar1 = dVar20;
            if (local_310 == '\0') goto LAB_14045ca79;
            uVar18 = (undefined4)local_318;
            uVar21 = (undefined4)((ulonglong)local_318 >> 0x20);
            local_198 = local_318;
            param_9 = *(int *)(lVar10 + 0xe0) + *(int *)(lVar10 + 0xc0) != 0;
            uVar23 = local_328;
            uVar24 = uStack_324;
          }
        }
      }
      if ((local_170 != (void *)0x0) && (local_170 != local_150)) {
        free(local_170);
      }
      dVar1 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
joined_r0x00014045c23a:
    } while (local_res8 < dVar4);
    plVar12 = (longlong *)param_10[7];
    if (plVar12 != (longlong *)0x0) {
      uStack_320 = dVar1;
      (**(code **)(*plVar12 + 0x20))(plVar12,plVar12 != param_10);
      param_10[7] = 0;
    }
  }
  else {
    FUN_14045cb00(&local_190,&local_328,param_3);
    dVar20 = (double)CONCAT44(uStack_320._4_4_,(undefined4)uStack_320);
    if (local_310 != '\0') {
      uVar18 = (undefined4)local_318;
      uVar21 = (undefined4)((ulonglong)local_318 >> 0x20);
      param_9 = *(char *)(param_3 + 0x98);
      local_198 = local_318;
      dVar3 = DAT_140aabd08;
      uVar23 = local_328;
      uVar24 = uStack_324;
      dVar1 = dVar20;
      goto joined_r0x00014045c23a;
    }
    plVar12 = (longlong *)param_10[7];
    if (plVar12 != (longlong *)0x0) {
      (**(code **)(*plVar12 + 0x20))(plVar12,plVar12 != param_10);
      param_10[7] = 0;
    }
  }
  return;
}


// Incoming references
// 0xc23e70 DATA caller none
// 0x4575aa UNCONDITIONAL_CALL caller 140457340
