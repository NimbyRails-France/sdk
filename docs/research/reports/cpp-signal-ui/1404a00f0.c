// Candidate VA 1404a00f0; RVA 0x4a00f0
// Ghidra inferred prototype: undefined FUN_1404a00f0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 * FUN_1404a00f0(undefined8 *param_1,undefined8 *param_2,undefined8 *param_3)

{
  ulonglong uVar1;
  ulonglong uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  void *pvVar5;
  undefined8 ****ppppuVar6;
  ulonglong uVar7;
  undefined8 *puVar8;
  uint uVar9;
  undefined8 *****pppppuVar10;
  longlong lVar11;
  undefined8 uVar12;
  undefined8 *puVar13;
  int extraout_var;
  undefined8 uVar14;
  undefined8 *puVar15;
  longlong lVar16;
  void *pvVar17;
  undefined8 *****pppppuVar18;
  undefined1 *puVar19;
  ulonglong uVar20;
  undefined1 *puVar21;
  int iVar22;
  undefined1 uVar23;
  ulonglong uVar24;
  int local_res20 [2];
  undefined8 ****local_288;
  undefined8 uStack_280;
  ulonglong local_278;
  ulonglong local_270;
  undefined1 local_268;
  undefined7 uStack_267;
  undefined8 *local_258;
  ulonglong uStack_250;
  undefined1 local_248;
  undefined7 uStack_247;
  undefined8 uStack_240;
  undefined8 *local_238;
  ulonglong uStack_230;
  char local_228;
  undefined8 *local_218;
  undefined8 **ppuStack_210;
  undefined8 *local_208;
  ulonglong uStack_200;
  undefined4 local_1f8;
  undefined8 ****local_1f0 [3];
  ulonglong local_1d8;
  undefined8 *local_1c8;
  undefined8 **ppuStack_1c0;
  undefined8 *local_1b8;
  ulonglong uStack_1b0;
  undefined1 local_1a8;
  undefined1 uStack_1a7;
  undefined6 uStack_1a6;
  undefined8 uStack_1a0;
  undefined8 *local_198;
  ulonglong uStack_190;
  undefined8 *local_188;
  undefined8 local_180;
  undefined8 *local_178;
  ulonglong uStack_170;
  undefined1 local_168 [32];
  undefined1 local_148 [32];
  undefined1 local_128 [32];
  undefined1 local_108 [32];
  undefined1 local_e8 [32];
  basic_ostream<char,std::char_traits<char>_> local_c8 [136];

  local_1f8 = 0;
  uStack_280 = 0;
  local_278 = 0;
  local_270 = 0xf;
  local_288 = (undefined8 *****)0x0;
  local_res20[0] = 0;
  local_1c8 = param_3;
  do {
    uVar9 = std::_Random_device();
    iVar22 = 0;
    do {
      uVar7 = local_270;
      uVar24 = local_278;
      uVar23 = (undefined1)uVar9;
      if (local_278 < local_270) {
        pppppuVar10 = &local_288;
        if (0xf < local_270) {
          pppppuVar10 = (undefined8 *****)local_288;
        }
        puVar21 = (undefined1 *)((longlong)pppppuVar10 + local_278);
        local_278 = local_278 + 1;
        *puVar21 = uVar23;
        *(undefined1 *)((longlong)pppppuVar10 + uVar24 + 1) = 0;
      }
      else {
        if (local_278 == 0x7fffffffffffffff) {
                    /* WARNING: Subroutine does not return */
          FUN_140001c70();
        }
        uVar1 = local_278 + 1;
        uVar20 = uVar1 | 0xf;
        if (uVar20 < 0x8000000000000000) {
          if (0x7fffffffffffffff - (local_270 >> 1) < local_270) {
            uVar20 = 0x7fffffffffffffff;
          }
          else {
            uVar2 = local_270 + (local_270 >> 1);
            if (uVar20 < uVar2) {
              uVar20 = uVar2;
            }
          }
        }
        else {
          uVar20 = 0x7fffffffffffffff;
        }
        pppppuVar10 = (undefined8 *****)FUN_140003270(uVar20 + 1);
        ppppuVar6 = local_288;
        local_278 = uVar1;
        local_270 = uVar20;
        if (uVar7 < 0x10) {
          memcpy(pppppuVar10,&local_288,uVar24);
          *(undefined1 *)((longlong)pppppuVar10 + uVar24) = uVar23;
          *(undefined1 *)((longlong)pppppuVar10 + uVar24 + 1) = 0;
          local_288 = pppppuVar10;
        }
        else {
          memcpy(pppppuVar10,local_288,uVar24);
          *(undefined1 *)((longlong)pppppuVar10 + uVar24) = uVar23;
          *(undefined1 *)((longlong)pppppuVar10 + uVar24 + 1) = 0;
          FUN_140003040(&local_288,ppppuVar6,uVar7);
          local_288 = pppppuVar10;
        }
      }
      uVar9 = uVar9 >> 8;
      iVar22 = iVar22 + 1;
    } while (iVar22 < 4);
    local_res20[0] = local_res20[0] + 1;
  } while (local_res20[0] < 4);
  pppppuVar10 = &local_288;
  if (0xf < local_270) {
    pppppuVar10 = (undefined8 *****)local_288;
  }
  pppppuVar18 = &local_288;
  if (0xf < local_270) {
    pppppuVar18 = (undefined8 *****)local_288;
  }
  FUN_1404ae2a0(&local_268,pppppuVar18,(longlong)pppppuVar10 + local_278);
  puVar8 = local_1c8;
  FUN_1402dc2a0(local_1c8,local_168);
  ppuStack_210 = (undefined8 **)0x0;
  local_208 = _DAT_140aac900;
  uStack_200 = _UNK_140aac908;
  local_218 = (undefined8 *)0x0;
  local_1f8 = 6;
  puVar21 = &local_268;
  if (0xf < uStack_250) {
    puVar21 = (undefined1 *)CONCAT71(uStack_267,local_268);
  }
  puVar19 = &local_268;
  if (0xf < uStack_250) {
    puVar19 = (undefined1 *)CONCAT71(uStack_267,local_268);
  }
  FUN_14020e5c0(local_res20,puVar19,puVar21 + (longlong)local_258);
  uVar24 = _UNK_140aac908;
  puVar13 = _DAT_140aac900;
  local_1c8 = local_218;
  ppuStack_1c0 = ppuStack_210;
  local_1b8 = local_208;
  uStack_1b0 = uStack_200;
  local_208 = _DAT_140aac900;
  uStack_200 = _UNK_140aac908;
  local_218 = (undefined8 *)((ulonglong)local_218 & 0xffffffffffff0000);
  FUN_14026ce20(local_1f0,local_168);
  if (7 < uStack_1b0) {
    puVar13 = local_1c8;
    if ((0xfff < uStack_1b0 * 2 + 2) &&
       (puVar13 = (undefined8 *)local_1c8[-1],
       0x1f < (ulonglong)((longlong)local_1c8 + (-8 - (longlong)puVar13)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar13);
    puVar13 = _DAT_140aac900;
    uVar24 = _UNK_140aac908;
  }
  local_1c8 = (undefined8 *)((ulonglong)local_1c8 & 0xffffffffffff0000);
  local_1b8 = puVar13;
  uStack_1b0 = uVar24;
  if (7 < uStack_200) {
    puVar13 = local_218;
    if ((0xfff < uStack_200 * 2 + 2) &&
       (puVar13 = (undefined8 *)local_218[-1],
       0x1f < (ulonglong)((longlong)local_218 + (-8 - (longlong)puVar13)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar13);
    puVar13 = _DAT_140aac900;
    uVar24 = _UNK_140aac908;
  }
  local_218 = (undefined8 *)((ulonglong)local_218 & 0xffffffffffff0000);
  pppppuVar10 = local_1f0;
  if (7 < local_1d8) {
    pppppuVar10 = (undefined8 *****)local_1f0[0];
  }
  local_208 = puVar13;
  uStack_200 = uVar24;
  lVar11 = FUN_14027c000(*param_1,pppppuVar10);
  if (lVar11 == 0) {
    FUN_140247b90(&local_1a8,local_1f0);
    uVar12 = FUN_140498e70(&local_188,&local_1a8);
    lVar11 = FUN_1402d82e0("error_cannot_open_temp_file","Cannot open temporary file: ");
    lVar16 = -1;
    do {
      lVar16 = lVar16 + 1;
    } while (*(char *)(lVar11 + lVar16) != '\0');
    puVar13 = (undefined8 *)FUN_14029cb50(uVar12,0);
    uVar12 = *puVar13;
    uVar14 = puVar13[1];
    uVar3 = puVar13[2];
    uVar4 = puVar13[3];
    *(undefined1 *)puVar13 = 0;
    puVar13[2] = 0;
    puVar13[3] = 0xf;
    *param_2 = uVar12;
    param_2[1] = uVar14;
    param_2[2] = uVar3;
    param_2[3] = uVar4;
    *(undefined1 *)(param_2 + 4) = 1;
    if (0xf < uStack_170) {
      puVar13 = local_188;
      if ((0xfff < uStack_170 + 1) &&
         (puVar13 = (undefined8 *)local_188[-1],
         0x1f < (ulonglong)((longlong)local_188 + (-8 - (longlong)puVar13)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar13);
    }
    uVar24 = _UNK_140aac918;
    puVar13 = _DAT_140aac910;
    local_178 = _DAT_140aac910;
    uStack_170 = _UNK_140aac918;
    local_188 = (undefined8 *)((ulonglong)local_188 & 0xffffffffffffff00);
    if (7 < uStack_190) {
      pvVar5 = (void *)CONCAT62(uStack_1a6,CONCAT11(uStack_1a7,local_1a8));
      pvVar17 = pvVar5;
      if ((0xfff < uStack_190 * 2 + 2) &&
         (pvVar17 = *(void **)((longlong)pvVar5 + -8),
         0x1f < (ulonglong)((longlong)pvVar5 + (-8 - (longlong)pvVar17)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar17);
    }
    local_198 = _DAT_140aac900;
    uStack_190 = _UNK_140aac908;
    local_1a8 = 0;
    uStack_1a7 = 0;
    FUN_140025470(local_1f0);
    FUN_140025470(local_168);
    if (0xf < uStack_250) {
      pvVar5 = (void *)CONCAT71(uStack_267,local_268);
      pvVar17 = pvVar5;
      if ((0xfff < uStack_250 + 1) &&
         (pvVar17 = *(void **)((longlong)pvVar5 + -8),
         0x1f < (ulonglong)((longlong)pvVar5 + (-8 - (longlong)pvVar17)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar17);
    }
    local_258 = puVar13;
    uStack_250 = uVar24;
    local_268 = 0;
    if (local_270 < 0x10) goto LAB_1404a0640;
    pppppuVar10 = (undefined8 *****)local_288;
    if ((0xfff < local_270 + 1) &&
       (pppppuVar10 = (undefined8 *****)local_288[-1],
       0x1f < (ulonglong)((longlong)local_288 + (-8 - (longlong)local_288[-1])))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else {
    std::basic_ostream<char,std::char_traits<char>_>::basic_ostream<char,std::char_traits<char>_>
              (local_c8,(basic_streambuf<char,std::char_traits<char>_> *)*param_1,false);
    FUN_14049f050(&local_248,local_c8);
    FUN_140279a60(*param_1);
    if (local_228 == '\0') {
      puVar13 = puVar8;
      if (7 < (ulonglong)puVar8[3]) {
        puVar13 = (undefined8 *)*puVar8;
      }
      FUN_140983920(puVar13);
      if (extraout_var == 0) {
        puVar13 = puVar8;
        if (7 < (ulonglong)puVar8[3]) {
          puVar13 = (undefined8 *)*puVar8;
        }
        pppppuVar10 = local_1f0;
        if (7 < local_1d8) {
          pppppuVar10 = (undefined8 *****)local_1f0[0];
        }
        iVar22 = FUN_140983b4c(pppppuVar10,puVar13);
        if (iVar22 != 0) {
          FUN_140247b90(local_128,puVar8);
          puVar13 = (undefined8 *)FUN_140498e70(local_e8,local_128);
          FUN_140247b90(local_148,local_1f0);
          puVar15 = (undefined8 *)FUN_140498e70(local_108,local_148);
          local_218 = (undefined8 *)
                      FUN_1402d82e0("error_cannot_rename_save",
                                    "Cannot rename temporary file: {} into {} ");
          ppuStack_210 = (undefined8 **)0xffffffffffffffff;
          do {
            ppuStack_210 = (undefined8 **)((longlong)ppuStack_210 + 1);
          } while (*(char *)((longlong)local_218 + (longlong)ppuStack_210) != '\0');
          local_188 = puVar15;
          if (0xf < (ulonglong)puVar15[3]) {
            local_188 = (undefined8 *)*puVar15;
          }
          local_180 = puVar15[2];
          local_178 = puVar13;
          if (0xf < (ulonglong)puVar13[3]) {
            local_178 = (undefined8 *)*puVar13;
          }
          uStack_170 = puVar13[2];
          local_1c8 = (undefined8 *)0xdd;
          ppuStack_1c0 = &local_188;
          FUN_140021e90(&local_1a8,&local_218,&local_1c8);
          *param_2 = CONCAT62(uStack_1a6,CONCAT11(uStack_1a7,local_1a8));
          param_2[1] = uStack_1a0;
          param_2[2] = local_198;
          param_2[3] = uStack_190;
          uVar24 = _UNK_140aac918;
          puVar13 = _DAT_140aac910;
          local_198 = _DAT_140aac910;
          uStack_190 = _UNK_140aac918;
          local_1a8 = 0;
          *(undefined1 *)(param_2 + 4) = 1;
          FUN_140002d30(&local_1a8);
          FUN_140002d30(local_108);
          FUN_140025470(local_148);
          FUN_140002d30(local_e8);
          FUN_140025470(local_128);
          goto LAB_1404a087a;
        }
        *(undefined1 *)(param_2 + 4) = 0;
        uVar24 = _UNK_140aac918;
        puVar13 = _DAT_140aac910;
        if (local_228 != '\0') {
          *param_2 = CONCAT71(uStack_247,local_248);
          param_2[1] = uStack_240;
          param_2[2] = local_238;
          param_2[3] = uStack_230;
          local_238 = puVar13;
          uStack_230 = uVar24;
          local_248 = 0;
          *(undefined1 *)(param_2 + 4) = 1;
          FUN_140002d30(&local_248);
        }
      }
      else {
        FUN_140247b90(&local_218);
        uVar12 = FUN_140498e70(local_148,&local_218);
        uVar14 = FUN_1402d82e0("error_cannot_delete_save","Cannot delete old save for overwriting: "
                              );
        puVar13 = (undefined8 *)FUN_14029a380(local_128,uVar14,uVar12);
        *param_2 = 0;
        param_2[1] = 0;
        param_2[2] = 0;
        param_2[3] = 0;
        uVar12 = puVar13[1];
        *param_2 = *puVar13;
        param_2[1] = uVar12;
        uVar12 = puVar13[3];
        param_2[2] = puVar13[2];
        param_2[3] = uVar12;
        *(undefined1 *)puVar13 = 0;
        puVar13[2] = 0;
        puVar13[3] = 0xf;
        *(undefined1 *)(param_2 + 4) = 1;
        FUN_140002d30(local_128);
        FUN_140002d30(local_148);
        FUN_140025470(&local_218);
        puVar13 = _DAT_140aac910;
        uVar24 = _UNK_140aac918;
LAB_1404a087a:
        if (local_228 != '\0') {
          if (0xf < uStack_230) {
            FUN_140003040(&local_248,CONCAT71(uStack_247,local_248));
          }
          local_248 = 0;
          local_238 = puVar13;
          uStack_230 = uVar24;
        }
      }
      std::basic_ostream<char,std::char_traits<char>_>::_vbase_destructor_(local_c8);
      FUN_140025470(local_1f0);
      FUN_140025470(local_168);
      if (0xf < uStack_250) {
        FUN_140003040(&local_268,CONCAT71(uStack_267,local_268));
      }
      local_268 = 0;
      local_258 = puVar13;
      uStack_250 = uVar24;
      if (0xf < local_270) {
        FUN_140003040(&local_288,local_288);
      }
      goto LAB_1404a0640;
    }
    *param_2 = CONCAT71(uStack_247,local_248);
    param_2[1] = uStack_240;
    param_2[2] = local_238;
    param_2[3] = uStack_230;
    *(undefined1 *)(param_2 + 4) = 1;
    uVar24 = _UNK_140aac918;
    puVar13 = _DAT_140aac910;
    local_238 = _DAT_140aac910;
    uStack_230 = _UNK_140aac918;
    local_248 = 0;
    std::basic_ostream<char,std::char_traits<char>_>::_vbase_destructor_(local_c8);
    FUN_140025470(local_1f0);
    FUN_140025470(local_168);
    if (0xf < uStack_250) {
      pvVar5 = (void *)CONCAT71(uStack_267,local_268);
      pvVar17 = pvVar5;
      if ((0xfff < uStack_250 + 1) &&
         (pvVar17 = *(void **)((longlong)pvVar5 + -8),
         0x1f < (ulonglong)((longlong)pvVar5 + (-8 - (longlong)pvVar17)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar17);
    }
    local_258 = puVar13;
    uStack_250 = uVar24;
    local_268 = 0;
    if (local_270 < 0x10) goto LAB_1404a0640;
    pppppuVar10 = (undefined8 *****)local_288;
    if ((0xfff < local_270 + 1) &&
       (pppppuVar10 = (undefined8 *****)local_288[-1],
       0x1f < (ulonglong)((longlong)local_288 + (-8 - (longlong)pppppuVar10)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  local_268 = 0;
  free(pppppuVar10);
LAB_1404a0640:
  local_278 = 0;
  local_270 = 0xf;
  local_288 = (undefined8 ****)((ulonglong)local_288 & 0xffffffffffffff00);
  FUN_140025470(puVar8);
  return param_2;
}


// Incoming references
// 0xc267b0 DATA caller none
// 0x49fd52 UNCONDITIONAL_CALL caller 14049fbd0
// 0x49fe8e UNCONDITIONAL_CALL caller 14049fbd0
